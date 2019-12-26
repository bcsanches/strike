#include "ST_Client.h"

#include <Phobos/Engine/Console.h>
#include <Phobos/Engine/Session.h>

#include <Phobos/OgreEngine/Render.h>

#include <Phobos/Game/Level/MapObject.h>

#include <Phobos/Game/Gui/LevelSelector.h>

#include <Phobos/Shell/Utils.h>

#include <Phobos/Error.h>
#include <Phobos/Exception.h>

#include <Phobos/System/MouseInputDevice.h>
#include <Phobos/System/Window.h>

#include <Phobos/OgreEngine/TransformProperty.h>

#include "ST_GameDefs.h"

PH_PLUGIN_ENTRY_POINT("ST_Client", "strike.cfg");

namespace
{
	static Strike::Client g_clClient;
}

namespace Strike
{
	//PH_DEFINE_DEFAULT_SINGLETON(Client);

	//PH_PLUGIN_REGISTER_MODULE(Client);

	Client::Client():					
		m_varMouseSensitivity("dvMouseSensitivity", "0.1"),
		m_varSpectatorMoveSpeed("dvSpectatorMoveSpeed", "2.0"),
		m_varSpectatorTurnSpeed("dvSpectatorTurnSpeed", "45.0"),
		m_varIsometricCameraHeight(ST_ISOMETRIC_CAMERA_HEIGHT_VAR, "80"),
		m_fMapLoaded(false),
		m_eCurrentControlMode(ControlModes::SPECTATOR),
		m_cmdToggleSpectatorMode("toggleSpectatorMode"),
		m_pclCurrentController(NULL)
	{				
		m_cmdToggleSpectatorMode.SetProc(PH_CONTEXT_CMD_BIND(&Client::CmdToggleSpectatorMode, this));

		m_varSpectatorMoveSpeed.SetCallback(PH_CONTEXT_VAR_BIND(&Client::VarSpectatorMoveSpeedChanged, this));
		m_varSpectatorTurnSpeed.SetCallback(PH_CONTEXT_VAR_BIND(&Client::VarSpectatorTurnSpeedChanged, this));
		m_varMouseSensitivity.SetCallback(PH_CONTEXT_VAR_BIND(&Client::VarMouseSensitivityChanged, this));		

		m_clSpectatorCamera.Disable();
		m_clPlayerController.Disable();

		auto &console = Phobos::Engine::Console::GetInstance();

		console.AddContextCommand(m_cmdToggleSpectatorMode);

		console.AddContextVariable(m_varMouseSensitivity);
		console.AddContextVariable(m_varSpectatorMoveSpeed);
		console.AddContextVariable(m_varSpectatorTurnSpeed);

		console.AddContextVariable(m_varIsometricCameraHeight);

		using namespace Phobos;

		Game::WorldManager::GetInstance().AddListener(*this);

		auto &levelSelector = Game::Gui::LevelSelector::GetInstance();
		levelSelector.Open();

		auto &session = Engine::Session::GetInstance();
		session.SetGuiForm(&levelSelector);
		session.CloseConsole();
	}
	
	void Client::OnFixedUpdate()
	{
		if(!m_fMapLoaded)
			return;

		if(m_pclCurrentController)
			m_pclCurrentController->FixedUpdate(m_ipLastCmd);
	}

	void Client::OnUpdate()
	{
		if(!m_fMapLoaded)
			return;

		if(m_pclCurrentController)
			m_pclCurrentController->Update();		
	}	
	
	void Client::OnMapUnloaded()
	{
		if(m_fMapLoaded)
		{
			m_fMapLoaded = false;

			m_ipLastCmd.reset();
		}
	}

	void Client::OnMapLoaded()
	{
		using namespace Phobos;

		auto &worldManager = Game::WorldManager::GetInstance();
		auto &player = worldManager.GetMapObject("Player");
		
		m_clSpectatorCamera.SetTransform(player.MakeWorldTransform());
		m_clPlayerController.SetTarget(&player);
		
		m_fMapLoaded = true;		

		auto &session = Engine::Session::GetInstance();

		if(m_eCurrentControlMode == ControlModes::SPECTATOR)		
			this->ToggleSpectatorMode();
		else
		{
			session.SetPlayerCommandProducer(&m_clPlayerCommandProducer);
		}

		session.SetGuiForm(nullptr);
		session.SetClient(this);
	}    

	void Client::DispatchCommand(Phobos::Engine::IPlayerCmdPtr_t cmd)
	{
		m_ipLastCmd = cmd;
	}

	Phobos::Engine::EscAction Client::HandleEsc(Phobos::Engine::Gui::Form *&outForm)
	{
		using namespace Phobos;

		auto &levelSelector = Game::Gui::LevelSelector::GetInstance();
		levelSelector.Open();

		outForm = &levelSelector;

		return Engine::EscAction::SET_GUI;
	}

	void Client::ToggleSpectatorMode()
	{
		if(m_eCurrentControlMode == ControlModes::PLAYER)
		{			
			m_clPlayerController.Disable();
			m_clSpectatorCamera.Enable();

			m_pclCurrentController = &m_clSpectatorCamera;

			m_eCurrentControlMode = ControlModes::SPECTATOR;
			Phobos::Engine::Session::GetInstance().SetPlayerCommandProducer(&m_clSpectatorCameraCommandProducer);
		}
		else
		{			
			m_clSpectatorCamera.Disable();
			m_clPlayerController.Enable();
			
			m_pclCurrentController = &m_clPlayerController;					

			m_eCurrentControlMode = ControlModes::PLAYER;
			Phobos::Engine::Session::GetInstance().SetPlayerCommandProducer(&m_clPlayerCommandProducer);
		}		
	}

	void Client::CmdToggleSpectatorMode(const Phobos::Shell::StringVector_t &args, Phobos::Shell::Context &)
	{
		this->ToggleSpectatorMode();
	}

	void Client::VarSpectatorMoveSpeedChanged(const Phobos::Shell::Variable &var, const Phobos::String_t &oldValue, const Phobos::String_t &newValue)
	{		
		//m_clSpectatorCamera.SetMoveSpeed(Phobos::StringToFloat(newValue));
	}

	void Client::VarSpectatorTurnSpeedChanged(const Phobos::Shell::Variable &var, const Phobos::String_t &oldValue, const Phobos::String_t &newValue)
	{
		//m_clSpectatorCamera.SetTurnSpeed(Phobos::StringToFloat(newValue));
	}

	void Client::VarMouseSensitivityChanged(const Phobos::Shell::Variable &var, const Phobos::String_t &oldValue, const Phobos::String_t &newValue)
	{
		//m_clSpectatorCamera.SetMouseSensitivity(Phobos::StringToFloat(newValue));
	}
}

