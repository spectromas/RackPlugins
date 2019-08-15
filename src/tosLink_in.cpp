#include "../include/tosLink.hpp"

const unsigned long TOSLink::tosLinkSignature[SIGNATURE_LEN] =
{
	0x00f1f100, 0x0000f100, 0x00f10000,
	0x001f0000, 0x00001f00, 0x001f1f00,
	0x000f0f00, 0x3f000001, 0x40200001,
	0xc02000f1, 0xc020f100, 0x4048f5f1,
	0x4048f1f5, 0xc048f1f5, 0xc048f0f1,
	0x3d8f5c29
};

const unsigned long TOSLink::tosAddrSignTempl[ADDRESS_SIGNATURE_LEN] =
{
	0xc04800f5, 0x001f0000,
	0xc02000f1, 0xc04800f1,
	0x3d8f0029, 0x00000000,
	0x3d8f0029, 0x3f000001
};

void TOSLinkInput::_process()
{
	if(uplink != NULL)
	{
		if(!uplink->isConnected())
			uplink_down();
		else
		{
			switch(uplinkStatus)
			{
				case UNLINKED:
					wait_uplink();
					break;

				case WAIT_ADDRESS:
					wait_uplink_address();
					break;

				case LINKED:
					break;
			}
		}
	}
}

void TOSLinkInput::wait_uplink()
{
	float n = uplink->value;
	if(memcmp(&n, &tosLinkSignature[uplink_frame_n], sizeof(unsigned long)) == 0)
	{
		if(++uplink_frame_n == SIGNATURE_LEN)
		{
			uplink_frame_n = 0;
			if(++uplink_frames_count == FRAME_IS_VALID)
			{
				uplinkStatus = WAIT_ADDRESS;
			}
		}
	} else
		uplink_down();
}

bool TOSLinkInput::validSignature()
{
	uint64_t ptr = 0;
	for(int k = 0; k < ADDRESS_SIGNATURE_LEN; k++)
	{
		if((uplink_address_buffer[k] & 0xffff00ff) != tosAddrSignTempl[k])
			return false;

		ptr >>= 8;
		uint64_t b = (uplink_address_buffer[k] >> 8) & 0xff;
		ptr |= (b << 56);
	}
	pPublishedData = (DigitalData *)ptr;
	return (pPublishedData->type == type_accepted);
}

void TOSLinkInput::wait_uplink_address()
{
	float n = uplink->value;
	memcpy(&uplink_address_buffer[uplink_frame_n], &n, sizeof(unsigned long));
	if(++uplink_frame_n == ADDRESS_SIGNATURE_LEN)
	{
		if(validSignature())
		{
			pPublishedData->linked = true;
			uplinkStatus = LINKED;
			connected = true;
			enableThru();
		} else
			uplink_down();
	}
}

void TOSLinkInput::enableThru()
{
	if(pThruPort != NULL && pPublishedData != NULL)
	{
		pThruPort->type = pPublishedData->type;
		pThruPort->data = pPublishedData->data;
	}
}

void TOSLinkInput::uplink_down()
{
	connected = false;
	pPublishedData = NULL;
	uplinkStatus = UNLINKED;
	if(pThruPort != NULL)
	{
		pThruPort->type = Undefined;
		pThruPort->data = NULL;
	}
	uplink_frame_n = uplink_frames_count = 0;
	memset(uplink_address_buffer, 0, sizeof(uplink_address_buffer));
}

void TOSLinkInput::Create(ModuleWidget *pWidget, DIGITAL_TYPE type, float x, float y, int port)
{
	if(!permanently_disabled)
	{
		if(pWidget->module != NULL)
		{
			uplink = &pWidget->module->inputs[port];
			type_accepted = type;
			pWidget->addInput(createInput<tosPort>(Vec(mm2px(x), yncscape(y, 6.923)), pWidget->module, port));
			pWidget->addChild(new tosLinkLed(mm2px(x + 1.755f), yncscape(y + 7.034f, 2.373), &connected));
		}
	}
}
