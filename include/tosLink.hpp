#pragma once
#include "common.hpp"

struct TOSLink
{
public:
	enum DIGITAL_TYPE
	{
		Undefined,
		Z8KMatrix
	};
	void process()
	{
		if(!permanently_disabled)
			_process();
	}

protected:
	struct DigitalData
	{
		DIGITAL_TYPE type = Undefined;
		bool linked = false;
		void *data = NULL;
	};
	
	struct tosLinkLed : SvgWidget
	{
		bool *value;
		std::vector<std::shared_ptr<Svg>> frames;

		tosLinkLed(float x, float y, bool *pVal)
		{
			frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/tosLinkLed_off.svg")));
			frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/tosLinkLed_on.svg")));
			setSvg(frames[0]);
			box.pos = Vec(x, y);
			wrap();
			value = pVal;
		}

		void draw(const DrawArgs &args) override
		{
			int index = *value ? 1 : 0;
			setSvg(frames[index]);
			SvgWidget::draw(args);
		}
	};

	struct tosPort : _ioPort
	{
		tosPort() : _ioPort("res/tosPort.svg")
		{
			fb->removeChild(shadow);
		}
	};

	TOSLink()
	{
		connected = false;
		permanently_disabled = (sizeof(float) != sizeof(unsigned long));
		if(permanently_disabled)
			INFO("TOSLink cannot work on this machine");
	}
	bool permanently_disabled = true;
	bool connected = false;

protected:
	virtual void _process() = 0;

	static const int SIGNATURE_LEN = 16;
	static const int FRAME_IS_VALID = 5;
	static const unsigned long tosLinkSignature[SIGNATURE_LEN];
	static const int ADDRESS_SIGNATURE_LEN = sizeof(uint64_t);
	static const unsigned long tosAddrSignTempl[ADDRESS_SIGNATURE_LEN];
};

struct TOSLinkInput : TOSLink
{
	TOSLinkInput() : TOSLink()
	{
		uplink = NULL;
		pPublishedData = pThruPort = NULL;
		uplink_down();
	}

	void *GetPublishedData()
	{
		if(pPublishedData != NULL && uplinkStatus == LINKED)
			return pPublishedData->data;

		return NULL;
	}

	void Create(ModuleWidget *pWidget, DIGITAL_TYPE type, float x, float y, int port);
	void setThruPort(DigitalData *pThru) { pThruPort = pThru; enableThru(); }

private:
	void wait_uplink_address();
	virtual void _process() override;
	void wait_uplink();
	void uplink_down();
	bool validSignature();
	void enableThru();
	DIGITAL_TYPE type_accepted;

	Input *uplink;
	DigitalData *pPublishedData;
	DigitalData *pThruPort;
	enum FSMUPL
	{
		UNLINKED,
		WAIT_ADDRESS,
		LINKED
	};

	FSMUPL uplinkStatus;
	int uplink_frame_n;
	int uplink_frames_count;
	unsigned long uplink_address_buffer[ADDRESS_SIGNATURE_LEN];
};

struct TOSLinkOutput : TOSLink
{
	TOSLinkOutput() : TOSLink()
	{
		downlink = NULL;
		digitalData.data = NULL;
		digitalData.linked = false;
	
		if(!permanently_disabled)
			create_signature();
	}

	virtual ~TOSLinkOutput()
	{
		digitalData.data = NULL;
		digitalData.type = Undefined;
	}

	void Publish(void *publishedData, DIGITAL_TYPE type)
	{
		if(!permanently_disabled)
		{
			digitalData.data = publishedData;
			digitalData.type = type;
			downlink_announce();
		} else
			INFO("TOSLink cannot work on this machine");
	}

	void Create(ModuleWidget *pWidget, float x, float y, int port);
	void Create(ModuleWidget *pWidget, TOSLinkInput *pThru, float x, float y, int port);

private:
	virtual void _process() override;
	void create_signature();
	void downlink_announce();
	void send_signature();
	void send_address();
	void wait_link();
	const int MAX_WAIT_CYCLES = 10;
	DigitalData digitalData;
	unsigned long tosAddressSignature[ADDRESS_SIGNATURE_LEN];
	Output *downlink;
	enum FSMDNL
	{
		NO_DATA,
		SEND_FRAME,
		SEND_ADDRESS,
		WAIT_LINK,
		LINKED
	};
	FSMDNL downlinkStatus;
	int downlink_frame_n;
	int downlink_frames_count;
};
