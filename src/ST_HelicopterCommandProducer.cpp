#include "ST_HelicopterCommandProducer.h"

#include <memory>

#include <Phobos/Engine/Console.h>
#include <Phobos/Shell/Utils.h>
#include <Phobos/Engine/Core.h>
#include <Phobos/Engine/CoreTimer.h>
#include <Phobos/Exception.h>

namespace Strike
{
	HelicopterCommandProducer::HelicopterCommandProducer(Phobos::Shell::Context *context):
		m_clStrafeModifier("-strafe", "+strafe", "=strafe", context),
		m_clMoveButton("-moveBwd", "+moveBwd", "=moveBwd", "-moveFwd", "+moveFwd", "=moveFwd", context),		
		m_clTurnButton( "-turnRight", "+turnRight", "=turnRight", "-turnLeft", "+turnLeft", "=turnLeft", context),
		m_clLookButton("-lookUp", "+lookUp", "=lookUp", "-lookDown", "+lookDown", "=lookDown", context),
		m_clMouseThumb(PH_MOUSE_THUMB_CMD_NAME, context),			
		m_fpMouseSensitivity(0.3f)
	{		
		m_clMouseThumb.Disable();
	}

	Phobos::Engine::IPlayerCmdPtr_t HelicopterCommandProducer::CreateCmd()
	{
		using namespace Phobos;
		

		const Float_t fwd = m_clMoveButton.GetValue();
		const Float_t *thumb = m_clMouseThumb.GetPoint();
		Float_t turnAngle(m_clTurnButton.GetValue());
		
		const bool strafeOn = m_clStrafeModifier.GetValue() >= 0.5f;

		Float_t strafe = strafeOn ? turnAngle : 0;
		turnAngle = !strafeOn ? turnAngle : 0;

		return std::make_shared<HelicopterCmd>(
			fwd,			
			turnAngle,
			strafe
		);
	}

	bool HelicopterCommandProducer::IsMouseClipped() const
	{
		return true;
	}

	void HelicopterCommandProducer::Enable()
	{
		using namespace Phobos;

		auto &console = Engine::Console::GetInstance();

		m_clStrafeModifier.Enable(console);
		m_clMoveButton.Enable(console);		
		m_clTurnButton.Enable(console);
		m_clLookButton.Enable(console);			
	}

	void HelicopterCommandProducer::Disable()
	{
		m_clStrafeModifier.Disable();
		m_clMoveButton.Disable();
		m_clTurnButton.Disable();
		m_clLookButton.Disable();		
	}
	
	void HelicopterCommandProducer::SetMouseSensitivity(Phobos::Float_t v)
	{
		if(v <= 0)
			PH_RAISE(Phobos::INVALID_PARAMETER_EXCEPTION, "HelicopterCommandProducer::SetMouseSensitivity", "Mouse sensitivity must be greater than zero");

		m_fpMouseSensitivity = v;
	}
}
