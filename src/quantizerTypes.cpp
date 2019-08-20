#include "../include/common.hpp"
#include "../include/quantizer.hpp"

struct scaleRootChoiceItem : ui::MenuItem 
{
	quantizeModule *pquantizer;
	int root_n;
	scaleRootChoiceItem(quantizeModule *p, int n)
	{
		pquantizer = p;
		root_n = n;
		text = QuantizerRootNotes.at(root_n);
		rightText = CHECKMARK(root_n == pquantizer->getRoot());
	}
	void onAction(const event::Action &e) override 
	{
		pquantizer->SetRoot(root_n);
	}
};

struct scaleRootChoice : LedDisplayChoice 
{
	quantizeModule *pquantizer;
	void onAction(const event::Action &e) override 
	{
		if (pquantizer != NULL)
		{
			ui::Menu *menu = createMenu();
			menu->addChild(createMenuLabel("Note Root"));
			for (int k=0; k<int(QuantizerRootNotes.size()); k++)
			{
				menu->addChild(new scaleRootChoiceItem(pquantizer, k));
			}
		}
	}

	void step() override 
	{
		int n = pquantizer->getRoot();
		text = n >= 0 ? QuantizerRootNotes.at(n) : "(No root)";
		color.a = n >= 0 ? 1.0 : 0.5f;
	}
};

struct scaleChoiceItem : ui::MenuItem 
{
	quantizeModule *pquantizer;
	int scale_n;
	scaleChoiceItem(quantizeModule *p, int n,const std::vector<QScale> &scaleList)
	{
		pquantizer = p;
		scale_n = n;
		text = scaleList.at(scale_n).name;
		rightText = CHECKMARK(scale_n == pquantizer->getScale());
	}
	void onAction(const event::Action &e) override 
	{
		pquantizer->SetScale(scale_n);
	}
};

struct ScaleChoice : LedDisplayChoice 
{
	quantizeModule *pquantizer;
	void onAction(const event::Action &e) override 
	{
		if (pquantizer != NULL)
		{
			ui::Menu *menu = createMenu();
			menu->addChild(createMenuLabel("Scale"));
			const std::vector<QScale> &scaleList = pquantizer->getAvailableScales();
			for (int k=0; k<int(scaleList.size()); k++)
			{
				menu->addChild(new scaleChoiceItem(pquantizer, k, scaleList));
			}
		}
	}
	
	void step() override 
	{
		int n = pquantizer->getScale();
		text = n >= 0 ? pquantizer->getAvailableScales().at(n).name : "(No scale)";
		color.a = n >= 0 ? 1.0 : 0.5f;
	}
};

qtzrDisplay::qtzrDisplay()
{
	scaleChoice = NULL;
	rootChoice = NULL;
	separator = NULL;
}

void qtzrDisplay::CreateInterface(quantizeModule *module)
{
	clearChildren();

	math::Vec pos;
	scaleChoice = createWidget<ScaleChoice>(pos);
	scaleChoice->box.size.x = box.size.x;
	scaleChoice->pquantizer = module;
	addChild(scaleChoice);
	pos = scaleChoice->box.getBottomLeft();

	separator = createWidget<LedDisplaySeparator>(pos);
	separator->box.size.x = box.size.x;
	addChild(separator);

	rootChoice = createWidget<scaleRootChoice>(pos);
	rootChoice->box.size.x = box.size.x;
	rootChoice->pquantizer = module;
	addChild(rootChoice);
	scaleChoice->color = rootChoice->color = nvgRGB(0xff, 0xff, 0xff);
}

struct midiQtzDriverItem : ui::MenuItem 
{
	midi::Port *port = NULL;
	int driverId;
	void onAction(const event::Action &e) override 
	{
		port->setDriverId(driverId);
	}
};

struct midiQtzDriverChoice : LedDisplayChoice 
{
	midi::Port *port = NULL;
	void onAction(const event::Action &e) override 
	{
		if (port != NULL)
		{
			ui::Menu *menu = createMenu();
			menu->addChild(createMenuLabel("MIDI driver"));
			for (int driverId : port->getDriverIds()) 
			{
				midiQtzDriverItem *item = new midiQtzDriverItem;
				item->port = port;
				item->driverId = driverId;
				item->text = port->getDriverName(driverId);
				item->rightText = CHECKMARK(item->driverId == port->driverId);
				menu->addChild(item);
			}
		}
	}

	void step() override 
	{
		if(port == NULL)
		{
			color.a = 0.5f;
			text = "(No driver)";
		} else
		{
			text = port->getDriverName(port->driverId);
			color.a = 1.f;
		}
	}
};

struct midiQtzDeviceItem : ui::MenuItem 
{
	midi::Port *port = NULL;
	int deviceId;
	void onAction(const event::Action &e) override 
	{
		port->setDeviceId(deviceId);
	}
};

struct midiQtzDeviceChoice : LedDisplayChoice
{
	midi::Port *port = NULL;
	void onAction(const event::Action &e) override 
	{
		if (port != NULL)
		{
			ui::Menu *menu = createMenu();
			menu->addChild(createMenuLabel("MIDI device"));
			{
				midiQtzDeviceItem *item = new midiQtzDeviceItem;
				item->port = port;
				item->deviceId = -1;
				item->text = "(No device)";
				item->rightText = CHECKMARK(item->deviceId == port->deviceId);
				menu->addChild(item);
			}
			for (int deviceId : port->getDeviceIds()) 
			{
				midiQtzDeviceItem *item = new midiQtzDeviceItem;
				item->port = port;
				item->deviceId = deviceId;
				item->text = port->getDeviceName(deviceId);
				item->rightText = CHECKMARK(item->deviceId == port->deviceId);
				menu->addChild(item);
			}
		}
	}

	void step() override 
	{
		if(port ==NULL)
		{
			text = "(No device)";
			color.a = 0.5f;
		} else 
		{
			color.a = 1.f;
			text = port->getDeviceName(port->deviceId);
		}
	}
};

struct midiQtzChannelItem : ui::MenuItem 
{
	midi::Port *port = NULL;
	int channel;
	void onAction(const event::Action &e) override 
	{
		port->channel = channel;
	}
};

struct midiQtzChannelChoice : LedDisplayChoice 
{
	midi::Port *port = NULL;
	void onAction(const event::Action &e) override 
	{
		if (port != NULL)
		{
			ui::Menu *menu = createMenu();
			menu->addChild(createMenuLabel("MIDI channel"));
			for (int channel : port->getChannels()) 
			{
				midiQtzChannelItem *item = new midiQtzChannelItem;
				item->port = port;
				item->channel = channel;
				item->text = port->getChannelName(channel);
				item->rightText = CHECKMARK(item->channel == port->channel);
				menu->addChild(item);
			}
		}
	}
	void step() override 
	{
		text = port ? port->getChannelName(port->channel) : "Channel 1";
	}
};

qtzrMidiDisplay::qtzrMidiDisplay()
{
	driverChoice = NULL;
	deviceChoice = NULL;
	driverSeparator = NULL;
	deviceSeparator = NULL;
	channelChoice = NULL;
}

void qtzrMidiDisplay::CreateInterface(midyQuant *module)
{
	clearChildren();
	math::Vec pos;

	driverChoice = createWidget<midiQtzDriverChoice>(pos);
	driverChoice->box.size.x = box.size.x;
	driverChoice->port = &module->midiOutput;
	addChild(driverChoice);
	pos = driverChoice->box.getBottomLeft();
	
	driverSeparator = createWidget<LedDisplaySeparator>(pos);
	driverSeparator->box.size.x = box.size.x;
	addChild(driverSeparator);

	deviceChoice = createWidget<midiQtzDeviceChoice>(pos);
	deviceChoice->box.size.x = box.size.x;
	deviceChoice->port = &module->midiOutput;
	addChild(deviceChoice);
	pos = deviceChoice->box.getBottomLeft();

	deviceSeparator = createWidget<LedDisplaySeparator>(pos);
	deviceSeparator->box.size.x = box.size.x;
	addChild(deviceSeparator);

	channelChoice = createWidget<midiQtzChannelChoice>(pos);
	channelChoice->box.size.x = box.size.x;
	channelChoice->port = &module->midiOutput;
	addChild(channelChoice);
}