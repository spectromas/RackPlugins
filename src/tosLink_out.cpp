#include "../include/tosLink.hpp"


void TOSLinkOutput::create_signature()
{
	uint64_t ptr = (uint64_t)& digitalData;
	for(int k = 0; k < ADDRESS_SIGNATURE_LEN; k++)
	{
		tosAddressSignature[k] = tosAddrSignTempl[k];
		unsigned short b = (ptr & 0xff) << 8;
		tosAddressSignature[k] |= b;
		ptr >>= 8;
	}
}

void TOSLinkOutput::_process()
{
	if(downlink != NULL)
	{
		if(!downlink->isConnected())
			downlink_announce();
		else
		{
			switch(downlinkStatus)
			{
				case NO_DATA:
					break;

				case SEND_FRAME:
					send_signature();
					break;

				case SEND_ADDRESS:
					send_address();
					break;

				case WAIT_LINK:
					wait_link();
					break;

				case LINKED:
					break;
			}
		}
	}
}

void TOSLinkOutput::downlink_announce()
{
	connected = false;
	digitalData.linked = false;
	downlinkStatus = digitalData.type != Undefined ? SEND_FRAME : NO_DATA;
	downlink_frame_n = downlink_frames_count = 0;
}

void TOSLinkOutput::send_signature()
{
	float n;
	memcpy(&n, &tosLinkSignature[downlink_frame_n], sizeof(n));
	downlink->value = n;
	if(++downlink_frame_n == SIGNATURE_LEN)
	{
		downlink_frame_n = 0;
		if(++downlink_frames_count == FRAME_IS_VALID)
		{
			downlinkStatus = SEND_ADDRESS;
		}
	}
}

void TOSLinkOutput::send_address()
{
	float n;
	memcpy(&n, &tosAddressSignature[downlink_frame_n], sizeof(n));
	downlink->value = n;
	if(++downlink_frame_n == ADDRESS_SIGNATURE_LEN)
	{
		downlink_frame_n = 0;
		downlinkStatus = WAIT_LINK;
	}
}

void TOSLinkOutput::wait_link()
{
	if(digitalData.linked)
	{
		downlinkStatus = LINKED;
		connected = true;
	} else if(++downlink_frame_n > MAX_WAIT_CYCLES)
		downlink_announce();
}

void TOSLinkOutput::Create(ModuleWidget *pWidget, float x, float y, int port)
{
	if(!permanently_disabled && pWidget->module != NULL) 
	{
		downlink = &pWidget->module->outputs[port];

		pWidget->addOutput(createOutput<tosPort>(Vec(mm2px(x), yncscape(y, 6.923)), pWidget->module, port));
		pWidget->addChild(new tosLinkLed(mm2px(x + 1.755f), yncscape(y +7.034f, 2.373), &connected));
	}
}

void TOSLinkOutput::Create(ModuleWidget *pWidget, TOSLinkInput *pThru, float x, float y, int port)
{
	if(!permanently_disabled && pWidget->module != NULL)
	{
		Create(pWidget, x, y, port);
		pThru->setThruPort(&digitalData);
		downlink_announce();
	}
}
