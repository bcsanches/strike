#ifndef ST_ISOMETRIC_CAMERA_H
#define ST_ISOMETRIC_CAMERA_H

#include <OgreCamera.h>

#include <Phobos/Game/Camera.h>
#include <Phobos/Game/IController.h>
#include <Phobos/Game/Level/MapObjectFwd.h>
#include <Phobos/OgreEngine/TransformPropertyFwd.h>

namespace Strike
{
	class Helicopter;

	class IsometricController: public Phobos::Game::IController, private Ogre::Camera::Listener
	{
		public:			
			IsometricController();

			inline void Enable() override
			{				
				m_clCamera.Enable();				
			}

			inline void Disable() override
			{
				m_clCamera.Disable();				
			}

			void SetTarget(Phobos::Game::MapObject *object);

			void FixedUpdate(Phobos::Engine::IPlayerCmdPtr_t cmd) override;

			virtual void cameraPreRenderScene(Ogre::Camera* cam);	

		private:
			Phobos::Game::Camera m_clCamera;			

			Helicopter	*m_pclTarget;
	};
}

#endif
