#include "common.hpp"
#ifdef OSC_ENABLE
#include "../digitalExt/osc/oscCommunicator.hpp"
#include "../digitalExt/osc/oscControl.hpp"
#include "../digitalExt/osc/oscDriver.hpp"
#include "../digitalExt/osc/oscSkt.hpp"

void oscControl::Draw(OSCDriver *drv, bool force)
{
	if((force || m_dirty))
	{
		drv->sendMsg(m_address.c_str(), getValue());
	}
	m_dirty = false;
	m_lastDrawnValue = getValue();
}

void oscControl::ChangeFromGUI(OSCDriver *drv)  // gui updated: the new value is already in the binded parameter
{
	m_dirty = true;
	Draw(drv);
}

uint32_t OSCBundle::WriteNextMessage(OSC *osc, const char *address, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	if(bundleLen >= bufLen) return 0;
	const uint32_t i = osc->Write(marker + 4, bufLen - bundleLen - 4, address, format, ap);

	va_end(ap);
	*((uint32_t *)marker) = htonl(i); // write the length of the message
	marker += (4 + i);
	bundleLen += (4 + i);
	return i;
}

#endif //OSC
