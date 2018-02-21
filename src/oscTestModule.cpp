#include "oscTestModule.hpp"
#ifdef OSCTEST_MODULE
#include "../digitalExt/osc/oscDriver.hpp"

#define MY_SCENE    8
struct OscTest : Module
{
	enum ParamIds {
		BTN1,
	
		NUM_PARAMS
	};
	enum InputIds {

		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	OscTest() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		connected = 0;
		drv = new OSCDriver(MY_SCENE);
	}
	~OscTest()
	{
		delete drv;
	}
	void step() override;

	OSCDriver *drv;
	float connected;

};

void OscTest::step()
{
	drv->ProcessOSC();
	connected = drv->Connected() ? 1.0 : 0.0;
}

OscTestWidget::OscTestWidget()
{
	OscTest *module = new OscTest();
	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		LightPanel *panel = new LightPanel();
		panel->box.size = box.size;
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(new DigitalLed(60, 20, &module->connected));

	ParamWidget * pctrl = createParam<CKSS>(Vec(20, 20), module, OscTest::BTN1, 0.0, 1.0, 0.0);
	oscControl *sw1 = new oscControl("/switch1");
	module->drv->Add(sw1, pctrl);
	addParam(pctrl);

#ifdef DEBUG
	info("RDY");
#endif
}
#endif
