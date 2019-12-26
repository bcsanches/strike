#ifndef ST_CLIENT_H
#define ST_CLIENT_H

#include <Phobos/Engine/Client.h>

#include <Phobos/Game/IController.h>

#include <Phobos/Shell/ContextFwd.h>
#include <Phobos/Shell/Command.h>
#include <Phobos/Shell/Variable.h>
#include <Phobos/Engine/Plugin.h>
#include <Phobos/Singleton.h>
#include <Phobos/Game/WorldManager.h>

#include <Phobos/Game/SpectatorCamera.h>
#include <Phobos/Game/SpectatorCameraCommandProducer.h>

#include "ST_IsometricController.h"
#include "ST_HelicopterCommandProducer.h"

namespace Strike
{
	PH_DECLARE_NODE_PTR(Client);

	class Client: public Phobos::Engine::Client, private Phobos::Game::WorldManagerListener
	{
		//PH_DECLARE_SINGLETON_METHODS(Client);

		public:
			PH_PLUGIN_CREATE_MODULE_PROC_DECL;

			Client();

			enum class ControlModes
			{
				PLAYER,
				SPECTATOR
			};

			virtual void DispatchCommand(Phobos::Engine::IPlayerCmdPtr_t cmd) override;
			virtual Phobos::Engine::EscAction HandleEsc(Phobos::Engine::Gui::Form *&outForm) override;

		protected:
			virtual void OnFixedUpdate() override;
			virtual void OnUpdate() override;			

		private:			
			virtual void OnMapUnloaded() override;
			virtual void OnMapLoaded() override;			
									
			void CmdToggleSpectatorMode(const Phobos::Shell::StringVector_t &args, Phobos::Shell::Context&);

			void ToggleSpectatorMode();

			void VarSpectatorMoveSpeedChanged(const class Phobos::Shell::Variable &var, const Phobos::String_t &oldValue, const Phobos::String_t &newValue);
			void VarSpectatorTurnSpeedChanged(const class Phobos::Shell::Variable &var, const Phobos::String_t &oldValue, const Phobos::String_t &newValue);
			void VarMouseSensitivityChanged(const class Phobos::Shell::Variable &var, const Phobos::String_t &oldValue, const Phobos::String_t &newValue);			

		private:											
			Phobos::Shell::Variable			m_varMouseSensitivity;

			Phobos::Shell::Variable			m_varSpectatorMoveSpeed;
			Phobos::Shell::Variable			m_varSpectatorTurnSpeed;

			Phobos::Shell::Variable			m_varIsometricCameraHeight;			

			Phobos::Shell::Command			m_cmdToggleSpectatorMode;

			Phobos::Game::SpectatorCamera					m_clSpectatorCamera;
			Phobos::Game::SpectatorCameraCommandProducer_c	m_clSpectatorCameraCommandProducer;

			IsometricController				m_clPlayerController;	
			HelicopterCommandProducer		m_clPlayerCommandProducer;
			
			bool m_fMapLoaded;

			Phobos::Game::IController		*m_pclCurrentController;
			ControlModes					m_eCurrentControlMode;

			Phobos::Engine::IPlayerCmdPtr_t m_ipLastCmd;
	};
}

#endif
