#include "../include/common.hpp"
#ifdef OSC_ENABLE
#include "../include/../digitalExt/osc/oscCommunicator.hpp"
#include "../include/../digitalExt/osc/oscControl.hpp"
#include "../include/../digitalExt/osc/oscDriver.hpp"

void oscControl::Draw(OSCDriver *drv, bool force)
{
	if((force || m_dirty))
	{
		float v;
		if(is_light)
			v = getValue();
		else
			v=  rescale(getValue(), pBindedParam->paramQuantity->getMinValue(), pBindedParam->paramQuantity->getMaxValue(), 0.0, 1.0);
		drv->sendMsg(m_address.c_str(), v);
	}
	m_dirty = false;
	m_lastDrawnValue = getValue();
}

void oscControl::ChangeFromGUI(OSCDriver *drv)  // gui updated: the new value is already in the binded parameter
{
	m_dirty = true;
	Draw(drv);
}

#endif //OSC
