#include "ST_IsometricController.h"

#include <Phobos/Engine/Console.h>

#include <Phobos/Game/Level/MapObject.h>

#include <Phobos/OgreEngine/TransformProperty.h>

#include "ST_GameDefs.h"
#include "ST_Helicopter.h"

namespace Strike
{
	IsometricController::IsometricController():
		m_pclTarget(nullptr)
	{		
		m_clCamera.SetNearClipDistance(0.1f);
		m_clCamera.EnableViewport(1);
		m_clCamera.AddCameraListener(*this);
	}

	void IsometricController::SetTarget(Phobos::Game::MapObject *target)
	{		
		if (!target)
			m_pclTarget = nullptr;
		else
		{
			m_pclTarget = &target->GetComponent<Helicopter>();
		}		
	}

	void IsometricController::FixedUpdate(Phobos::Engine::IPlayerCmdPtr_t cmd)
	{
		using namespace Phobos;
		
		HelicopterCmdPtr_t cmdx = std::static_pointer_cast<HelicopterCmd>(cmd);
		static_cast<Helicopter*>(m_pclTarget)->SetCurrentCmd(cmdx);
	}

	void IsometricController::cameraPreRenderScene(Ogre::Camera* cam)
	{
		using namespace Ogre;
		using namespace Phobos;		

		Vector3 position = m_pclTarget->GetOwner().MakeWorldTransform().GetOrigin();
		position.y = Engine::Console::GetInstance().GetContextVariable(ST_ISOMETRIC_CAMERA_HEIGHT_VAR).GetFloat();
		position.z -= 50;

		m_clCamera.SetTransform(Phobos::Engine::Math::Transform(position, Quaternion(Degree(55), Vector3(1, 0, 0))));
	}
}


