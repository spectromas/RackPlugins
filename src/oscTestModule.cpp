#include "oscTestModule.hpp"
#ifdef OSCTEST_MODULE

void OscTest::process(const ProcessArgs &args)
{
	drv->ProcessOSC();
	connected = drv->Connected() ? 1.0 : 0.0;
	if(clock() - lasttime > 1000)
	{
		lasttime = clock();
		lights[LED1].value = lights[LED1].value > 0 ? 0 : 10;
	}
	outputs[OUT_1].value = params[BTN1].value > 0 ? LVL_ON : LVL_OFF;
}

OscTestWidget::OscTestWidget(OscTest *module) : ModuleWidget()
{
	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		addChild(panel);
	}

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	if(module != NULL)
		addChild(new DigitalLed(60, 20, &module->connected));

	ParamWidget *pctrl = createParam<Davies1900hBlackKnob>(Vec(20, 70), module, OscTest::POT1);
	if(module != NULL)
	{
		module->drv->Add(new oscControl("/Knob1"), pctrl);
	}
	addParam(pctrl);     // rnd threshold
	ModuleLightWidget *plight = createLight<MediumLight<RedLight>>(Vec(60, 70), module, OscTest::LED1);
	if(module != NULL)
	{
		module->drv->Add(new oscControl("/Led1"), plight);
	}
	addChild(plight);
	
	addOutput(createOutput<PJ301MPort>(Vec(50, 100), module, OscTest::OUT_1 ));
	pctrl = createParam<CKSS>(Vec(20, 20), module, OscTest::BTN1);
	if(module != NULL)
	{
		module->drv->Add(new oscControl("/Switch1"), pctrl);
	}
	addParam(pctrl);
}
#endif
