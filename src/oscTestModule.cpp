#include "oscTestModule.hpp"
#ifdef OSCTEST_MODULE

#define MY_SCENE    8
struct OscTest : Module
{
	enum ParamIds {
		BTN1,
		POT1,
		NUM_PARAMS
	};
	enum InputIds {

		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		LED1,
		NUM_LIGHTS
	};
	OscTest() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		connected = 0;
		drv = new OSCDriver(MY_SCENE);
		lasttime = clock();
	}
	~OscTest()
	{
		delete drv;
	}
	void step() override;

	OSCDriver *drv;
	float connected;
	clock_t lasttime;
};

void OscTest::step()
{
	drv->ProcessOSC();
	connected = drv->Connected() ? 1.0 : 0.0;
	if(clock() - lasttime > 1000)
	{
		lasttime = clock();
		lights[LED1].value = lights[LED1].value > 0 ? 0 : 10;
	}
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

	ParamWidget *pctrl = createParam<Davies1900hBlackKnob>(Vec(20, 70), module, OscTest::POT1, 0.0, 1.0, 0.0);
	oscControl *oc = new oscControl("/Knob1");
	module->drv->Add(oc, pctrl);
	addParam(pctrl);     // rnd threshold
	
	ModuleLightWidget *plight = createLight<MediumLight<RedLight>>(Vec(60, 70), module, OscTest::LED1);
	oc = new oscControl("/Led1");
	module->drv->Add(oc, plight);
	addChild(plight);
	
	pctrl = createParam<CKSS>(Vec(20, 20), module, OscTest::BTN1, 0.0, 1.0, 0.0);
	oc = new oscControl("/Switch1");
	module->drv->Add(oc, pctrl);
	addParam(pctrl);
}
#endif
