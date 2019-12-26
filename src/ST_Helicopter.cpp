#include "ST_Helicopter.h"

#include <OgreEntity.h>

#include <Phobos/Engine/Clocks.h>
#include <Phobos/Game/Level/MapObjectComponentFactory.h>

#include <Phobos/Game/Level/ComponentPool.h>
#include <Phobos/Game/Level/MapObject.h>
#include <Phobos/Game/Level/MapObjectComponentAccess.h>
#include <Phobos/Game/Level/MeshComponent.h>

#include <Phobos/Register/Table.h>

#include <Phobos/OgreEngine/Render.h>
#include <Phobos/OgreEngine/TransformProperty.h>

#include <boost/pool/object_pool.hpp>

#include <OgreBone.h>

#include "ST_EntityKeys.h"

namespace Strike
{	
	PH_GAME_DEFINE_COMPONENT_POOL(Strike::Helicopter, g_poolHelicopters);

	PH_MAP_COMPONENT_FULL_CREATOR(ST_HELICOPTER_COMPONENT_NAME, Strike::Helicopter);

	static const Phobos::String_t g_strComponentTypeName(ST_HELICOPTER_COMPONENT_NAME);	

	Helicopter::Helicopter(Phobos::Game::MapObject &owner, const Phobos::Register::Table &table) :
		MapObjectComponent(owner),
		m_pclRotorBone(nullptr),		
		m_pclRotorObject(nullptr),
		m_pclRootBone(nullptr),
		m_fpMaxPitch(0),
		m_fpMaxRoll(0),
		m_eCurrentState(States::STARTING),
		//m_clAltitudeInterpolator(0, 0, 0, 0),
		m_clMoveAccelerometer(Phobos::System::Seconds(table.GetFloat("moveAcceleration"))),
		m_clStrafeAccelerometer(Phobos::System::Seconds(table.GetFloat("moveAcceleration"))),
		m_clTurnAccelerometer(Phobos::System::Seconds(table.GetFloat("turnAcceleration"))),
		m_secRotorWarmupTime(table.GetFloat("rotorWarmupTime")),
		m_secTakeOffTime(table.GetFloat("takeOffTime")),
		m_ipLastCmd(std::make_shared<HelicopterCmd>(0.0f, 0.0f, 0.0f))
	{
		using namespace Phobos;

		auto &render = OgreEngine::Render::GetInstance();

		m_pclRotorObject = render.CreateEntity(table.GetString(ST_HELICOPTER_ROTOR_MODEL));
		m_pclTailRotorObject = render.CreateEntity(table.GetString(ST_HELICOPTER_TAIL_ROTOR_MODEL));

		m_radRotorSpeed = Ogre::Degree(table.GetFloat("rotorRPS") * 360.0f);		
		
		m_fpCruiseAltitude = table.GetFloat("cruiseAltitude");		

		m_fpMaxPitch = table.GetFloat("maxPitch");
		m_fpMaxRoll = table.GetFloat("maxRoll");
	}
	
	Helicopter::~Helicopter()
	{
		using namespace Phobos;

		auto &render = OgreEngine::Render::GetInstance();	
		
		render.DestroyEntity(m_pclRotorObject);		
		render.DestroyEntity(m_pclTailRotorObject);
	}
	
	void Helicopter::OnLoadFinished(const Phobos::Register::Table &table)
	{
		using namespace Phobos;
		
		auto &meshComponent = this->GetOwnerComponent<Game::MeshComponent>();		

		auto &rotorBoneName = table.GetString("rotorTag");
		auto &tailRotorBoneName = table.GetString("tailRotorTag");
		auto &rootBoneName = table.GetString("rootTag");
		
		meshComponent.AttachObjectToBone(rotorBoneName, *m_pclRotorObject, Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_Z));
		meshComponent.AttachObjectToBone(tailRotorBoneName, *m_pclTailRotorObject);
		
		m_pclRotorBone = &meshComponent.GetBone(rotorBoneName);
		m_pclRotorBone->setManuallyControlled(true);

		m_pclTailRotorBone = &meshComponent.GetBone(tailRotorBoneName);
		m_pclTailRotorBone->setManuallyControlled(true);

		m_pclRootBone = &meshComponent.GetBone(rootBoneName);
		m_pclRootBone->setManuallyControlled(true);
		m_qOriginalRotation = m_pclRootBone->getOrientation();

		this->EnableUpdate(&Helicopter::OnUpdate);

		auto ticks = Engine::GameClock::Now();

		m_clRotorInterpolator.Start(ticks.time_since_epoch(), m_secRotorWarmupTime, Ogre::Radian(0), m_radRotorSpeed);
		
		this->AddReminder(&Helicopter::OnStartFinished, m_secRotorWarmupTime);		
	}

	void Helicopter::OnFixedUpdate()
	{		
		auto frameDuration = Phobos::Engine::GameClock::GetFrameDuration();

		m_clMoveAccelerometer.SetButtonState(m_ipLastCmd->GetMove());
		m_clMoveAccelerometer.Update(frameDuration);

		m_clStrafeAccelerometer.SetButtonState(m_ipLastCmd->GetStrafe());
		m_clStrafeAccelerometer.Update(frameDuration);

		m_clTurnAccelerometer.SetButtonState(m_ipLastCmd->GetTurn());
		m_clTurnAccelerometer.Update(frameDuration);

		Phobos::Float_t speed = m_clMoveAccelerometer.GetValue() * 0.5f;
		Phobos::Float_t strafeSpeed = m_clStrafeAccelerometer.GetValue() * 0.5f;
		m_degTurnSpeed = m_clTurnAccelerometer.GetValue()  * 2;

		auto transform = this->GetOwner().MakeWorldTransform();
		
		m_v3Velocity += transform.GetRotation().zAxis() * speed;
		m_v3Velocity += transform.GetRotation().xAxis() * strafeSpeed;
		m_v3Velocity -= m_v3Velocity * (m_v3Velocity.length() * 0.3f);
		if(m_v3Velocity.length() < 0.02f)
			m_v3Velocity = Ogre::Vector3::ZERO;

		transform.Translate(m_v3Velocity);
		transform.RotateY(m_degTurnSpeed);

		this->AccessMapObject().SetTransform(transform);
	}

	void Helicopter::OnUpdate()
	{
		auto now = Phobos::Engine::RenderClock::Now().time_since_epoch();
		auto frameTime = Phobos::Engine::RenderClock::GetFrameDuration();

		Ogre::Radian speed = m_clRotorInterpolator.GetValue(now);
		Phobos::Float_t altitude = m_clAltitudeInterpolator.GetValue(now);

		Phobos::Float_t rollValue = (m_clTurnAccelerometer.GetValue() + m_clStrafeAccelerometer.GetValue()) / 2.0f;

		Ogre::Quaternion rootRotation(Ogre::Degree(m_fpMaxPitch * m_clMoveAccelerometer.GetValue()), Ogre::Vector3::UNIT_Z);		
		rootRotation = rootRotation * Ogre::Quaternion(Ogre::Degree(-m_fpMaxRoll * rollValue), Ogre::Vector3::UNIT_Y);
		m_pclRootBone->setOrientation(m_qOriginalRotation * rootRotation);
		
		m_pclRotorBone->pitch(speed * frameTime.count());
		m_pclTailRotorBone->pitch(speed * frameTime.count());
		
		auto transform = this->GetOwner().MakeWorldTransform();

		Ogre::Vector3 currentPosition = transform.GetOrigin();
		currentPosition.y = altitude;
		
		transform.SetOrigin(currentPosition);

		this->AccessMapObject().SetTransform(transform);
	}

	void Helicopter::OnStartFinished()
	{
		m_eCurrentState = States::TAKE_OFF;
		auto ticks = Phobos::Engine::GameClock::Now().time_since_epoch();

		m_clAltitudeInterpolator.Start(ticks, m_secTakeOffTime, 0, m_fpCruiseAltitude);	

		this->AddReminder(&Helicopter::OnTakeoffFinished, m_secTakeOffTime);		
	}

	void Helicopter::OnTakeoffFinished()
	{
		m_eCurrentState = States::PLAY;		

		m_clMoveAccelerometer.Reset();
		m_clStrafeAccelerometer.Reset();
		m_clTurnAccelerometer.Reset();		

		m_v3Velocity = Ogre::Vector3::ZERO;

		this->EnableFixedUpdate(&Helicopter::OnFixedUpdate);		
	}
}
