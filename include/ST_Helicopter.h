#ifndef ST_HELICOPTER
#define ST_HELICOPTER

#include <Phobos/Engine/Math/Accelerometer.h>
#include <Phobos/Engine/Math/Interpolator.h>

#include <Phobos/Game/Level/MapObjectComponent.h>

#include <Phobos/Memory.h>

#include <Phobos/OgreEngine/TransformPropertyFwd.h>

#include <Phobos/Register/TableFwd.h>

#include <OgreMath.h>
#include <OgrePrerequisites.h>
#include <OgreVector3.h>

#include "ST_HelicopterCommandProducer.h"

#define ST_HELICOPTER_COMPONENT_NAME "Helicopter"

namespace Strike
{
	class Helicopter : public Phobos::Game::MapObjectComponent
	{
		public:
			PH_DECLARE_MEMORY_OPERATORS;

			static MapObjectComponent::UniquePtr_t Create(Phobos::Game::MapObject &owner, const Phobos::Register::Table &table);

			inline void SetCurrentCmd(HelicopterCmdPtr_t cmd);

			Helicopter(Phobos::Game::MapObject &owner, const Phobos::Register::Table &table);
			~Helicopter();

		protected:			
			virtual void OnLoadFinished(const Phobos::Register::Table &table) override;						

		private:
			void OnStartFinished();
			void OnTakeoffFinished();

			void OnUpdate();
			void OnFixedUpdate();

		private:							
			Ogre::Entity    *m_pclRotorObject;
			Ogre::Entity    *m_pclTailRotorObject;
			Ogre::Bone		*m_pclRotorBone;
			Ogre::Bone		*m_pclTailRotorBone;
			Ogre::Bone      *m_pclRootBone;

			Phobos::Engine::Math::LinearInterpolator<Ogre::Radian> m_clRotorInterpolator;
			Phobos::Engine::Math::LinearInterpolator<Phobos::Float_t> m_clAltitudeInterpolator;

			Ogre::Radian	m_radRotorSpeed;			

			Phobos::System::Seconds m_secRotorWarmupTime;	
			Phobos::System::Seconds m_secTakeOffTime;

			Phobos::Float_t m_fpCruiseAltitude;

			Phobos::Float_t m_fpMaxPitch;
			Phobos::Float_t m_fpMaxRoll;

			enum class States
			{
				STARTING,
				TAKE_OFF,
				PLAY
			};

			States m_eCurrentState;

			HelicopterCmdPtr_t m_ipLastCmd;
			
			Ogre::Degree m_degTurnSpeed;

			Phobos::Engine::Math::Accelerometer m_clMoveAccelerometer;
			Phobos::Engine::Math::Accelerometer m_clStrafeAccelerometer;
			Phobos::Engine::Math::Accelerometer m_clTurnAccelerometer;

			Ogre::Vector3 m_v3Velocity;
			Ogre::Quaternion m_qOriginalRotation;
	};

	inline void Helicopter::SetCurrentCmd(HelicopterCmdPtr_t cmd)
	{
		m_ipLastCmd = cmd;
	}
}

#endif

