#ifndef ST_HELICOPTER_COMMAND_PRODUCER_H
#define ST_HELICOPTER_COMMAND_PRODUCER_H

#include <memory>

#include <Phobos/System/AxisButton.h>
#include <Phobos/System/Thumb.h>

#include <Phobos/Shell/CommandFwd.h>
#include <Phobos/Shell/ContextFwd.h>
#include <Phobos/Shell/Variable.h>

#include <Phobos/Engine/IPlayerCommandProducer.h>

#include <Phobos/Types.h>

namespace Strike
{
	class HelicopterCmd;
	typedef std::shared_ptr<HelicopterCmd> HelicopterCmdPtr_t;

	class HelicopterCmd: public Phobos::Engine::IPlayerCmd
	{
		public:
			inline HelicopterCmd(Phobos::Float_t move, Phobos::Float_t turn, Phobos::Float_t strafe):
				m_fpMove(move),
				m_fpTurn(turn),
				m_fpStrafe(strafe)
			{
				//empty
			}					

			inline Phobos::Float_t GetMove() const;			
			inline Phobos::Float_t GetTurn() const;			
			inline Phobos::Float_t GetStrafe() const;

		private:
			Phobos::Float_t m_fpMove;			
			Phobos::Float_t m_fpTurn;			
			Phobos::Float_t m_fpStrafe;
	};

	inline Phobos::Float_t HelicopterCmd::GetMove() const
	{
		return m_fpMove;
	}
	
	inline Phobos::Float_t HelicopterCmd::GetTurn() const
	{
		return m_fpTurn;
	}

	inline Phobos::Float_t HelicopterCmd::GetStrafe() const
	{
		return m_fpStrafe;
	}

	#define PH_MOUSE_THUMB_CMD_NAME "mouseThumb"

	class HelicopterCommandProducer: public Phobos::Engine::IPlayerCommandProducer
	{
		public:
			HelicopterCommandProducer(Phobos::Shell::Context *context = NULL);

			virtual Phobos::Engine::IPlayerCmdPtr_t CreateCmd();

			virtual void Enable() override;
			virtual void Disable() override;

			virtual bool IsMouseClipped() const override;
			
			void SetMouseSensitivity(Phobos::Float_t v);
		
		private:
			Phobos::System::Button		m_clStrafeModifier;
			Phobos::System::AxisButton	m_clMoveButton;
			Phobos::System::AxisButton	m_clTurnButton;
			Phobos::System::AxisButton	m_clLookButton;		
			Phobos::System::Thumb		m_clMouseThumb;			
					
			Phobos::Float_t				m_fpMouseSensitivity;
	};
}

#endif
