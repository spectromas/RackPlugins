#include "Z8K.hpp"
#include <sstream>

void Z8K::on_loaded()
{
	#ifdef DIGITAL_EXT
	connected = 0;
	#endif
	load();
}

void Z8K::load()
{
	// sequencer 1-4
	for(int k = 0; k < 4; k++)
	{
		int base = VOLTAGE_1 + 4 * k;
		std::vector<int> steps = {base, base + 1, base + 2, base + 3};
		seq[SEQ_1 + k].Init(&inputs[RESET_1 + k], &inputs[DIR_1 + k], &inputs[CLOCK_1 + k], &outputs[CV_1 + k], &lights[LED_ROW], params, steps);
	}
	// sequencer A-D
	for(int k = 0; k < 4; k++)
	{
		std::vector<int> steps = {k, k + 4, k + 8, k + 12};
		seq[SEQ_A + k].Init(&inputs[RESET_A + k], &inputs[DIR_A + k], &inputs[CLOCK_A + k], &outputs[CV_A + k], &lights[LED_COL], params, steps);
	}
	// horiz
	std::vector<int> steps_h = {0,1,2,3,7,6,5,4,8,9,10,11,15,14,13,12};
	seq[SEQ_HORIZ].Init(&inputs[RESET_HORIZ], &inputs[DIR_HORIZ], &inputs[CLOCK_HORIZ], &outputs[CV_HORIZ], &lights[LED_HORIZ], params, steps_h);
	//vert
	std::vector<int> steps_v = {0,4,8,12,13,9,5,1,2,6,10,14,15,11,7,3};
	seq[SEQ_VERT].Init(&inputs[RESET_VERT], &inputs[DIR_VERT], &inputs[CLOCK_VERT], &outputs[CV_VERT], &lights[LED_VERT], params, steps_v);
}

void Z8K::process(const ProcessArgs &args)
{
	bool activeSteps[16];
	for(int k = 0; k < 16; k++)
		activeSteps[k] = false;
	if (masterReset.process(params[M_RESET].value))
	{
		for (int k = 0; k < NUM_SEQUENCERS; k++)
			seq[k].Reset();
	} else
	{
		if (randomizeTrigger.process(inputs[RANDOMIZE].value))
			pWidget->std_randomize(VOLTAGE_1, VOLTAGE_1 + 16);

		float transpose = inputs[TRANSPOSER].isConnected() ? inputs[TRANSPOSER].value : 0;
		for (int k = 0; k < NUM_SEQUENCERS; k++)
			activeSteps[seq[k].Step(transpose)] = true;

		for (int k = 0; k < 16; k++)
		{
			outputs[ACTIVE_STEP + k].value = activeSteps[k] ? LVL_ON : LVL_OFF;
		}
	}
	#ifdef DIGITAL_EXT
	bool dig_connected = false;

	#if defined(OSCTEST_MODULE)
	if(oscDrv->Connected())
		dig_connected = true;
	oscDrv->ProcessOSC();
	#endif	
	connected = dig_connected ? 1.0 : 0.0;
	#endif
}

Z8KWidget::Z8KWidget(Z8K *module) : SequencerWidget(module)
{	
	if(module != NULL)
		module->setWidget(this);

	#ifdef OSCTEST_MODULE
	char name[60];
	#endif

	box.size = Vec(34 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	SvgPanel *panel = new SvgPanel();
	panel->box.size = box.size;
	panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/Z8KModule.svg")));
	addChild(panel);
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2*RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2*RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));
	float dist_h = 22.225;
	float dist_v = -18.697;

	for(int k = 0; k < 4; k++)
	{
		addInput(createInput<PJ301YPort>(Vec(mm2px(5.738), yncscape(82.210+k*dist_v,8.255)), module, Z8K::RESET_1 + k));
		addInput(createInput<PJ301BPort>(Vec(mm2px(16.544), yncscape(82.210+k*dist_v,8.255)), module, Z8K::DIR_1 + k));
		addInput(createInput<PJ301RPort>(Vec(mm2px(27.349), yncscape(82.210+k*dist_v,8.255)), module, Z8K::CLOCK_1 + k));
	}

	for(int k = 0; k < 4; k++)
	{
		addInput(createInput<PJ301YPort>(Vec(mm2px(52.168+k*dist_h), yncscape(115.442,8.255)), module, Z8K::RESET_A + k));
		addInput(createInput<PJ301BPort>(Vec(mm2px(52.168+k*dist_h), yncscape(105.695,8.255)), module, Z8K::DIR_A + k));
		addInput(createInput<PJ301RPort>(Vec(mm2px(52.168+k*dist_h), yncscape(95.948,8.255)), module, Z8K::CLOCK_A + k));
	}

	addInput(createInput<PJ301BPort>( Vec(mm2px(161.154), yncscape(5.860,8.255)), module, Z8K::TRANSPOSER));

	addInput(createInput<PJ301YPort>( Vec(mm2px(135.416), yncscape(111.040,8.255)), module, Z8K::RESET_VERT ));
	addInput(createInput<PJ301BPort>( Vec(mm2px(143.995), yncscape(102.785,8.255)), module, Z8K::DIR_VERT));
	addInput(createInput<PJ301RPort>( Vec(mm2px(152.575), yncscape(111.040,8.255)), module, Z8K::CLOCK_VERT ));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(161.154), yncscape(102.785,8.255)), module, Z8K::CV_VERT) );

	addInput(createInput<PJ301YPort> (Vec(mm2px(5.738), yncscape(10.941, 8.255)), module, Z8K::RESET_HORIZ));
	addInput(createInput<PJ301BPort> (Vec(mm2px(14.318), yncscape(2.685, 8.255)), module, Z8K::DIR_HORIZ ));
	addInput(createInput<PJ301RPort> (Vec(mm2px(22.897), yncscape(10.941, 8.255)), module, Z8K::CLOCK_HORIZ));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(31.477), yncscape(2.685, 8.255)), module, Z8K::CV_HORIZ));

	addInput(createInput<PJ301BPort> (Vec(mm2px(24.183), yncscape(115.442, 8.255)), module, Z8K::RANDOMIZE));

	for(int r = 0; r < 4; r++)
	{
		for(int c = 0; c < 4; c++)
		{
			int n = c + r * 4;
			ParamWidget *pctrl = createParam<Davies1900hFixRedKnob>(Vec(mm2px(51.533 + dist_h * c), yncscape(81.575+ dist_v * r,9.525)), module, Z8K::VOLTAGE_1 + n);
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/Knob%i", n + 1);
				oscControl *oc = new oscControl(name);
				module->oscDrv->Add(oc, pctrl);
			}
			#endif
			addParam(pctrl);

			ModuleLightWidget *plight = createLight<SmallLight<RedLight>>(Vec(mm2px(62.116 + dist_h * c), yncscape(85.272 + dist_v * r, 2.132)), module, Z8K::LED_ROW + n);
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/LedR%i", n + 1);
				module->oscDrv->Add(new oscControl(name), plight);
			}
			#endif
			addChild(plight);

			plight = createLight<SmallLight<GreenLight>>(Vec(mm2px(55.230 + dist_h * c), yncscape(78.385 + dist_v * r, 2.132)), module, Z8K::LED_COL + n);
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/LedC%i", n + 1);
				module->oscDrv->Add(new oscControl(name), plight);
			}
			#endif
			addChild(plight);

			plight = createLight<SmallLight<YellowLight>>(Vec(mm2px(51.533 + dist_h * c), yncscape(78.385 + dist_v * r, 2.132)), module, Z8K::LED_VERT + n);
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/LedV%i", n + 1);
				module->oscDrv->Add(new oscControl(name), plight);
			}
			#endif
			addChild(plight);

			plight = createLight<SmallLight<BlueLight>>(Vec(mm2px(62.116 + dist_h * c), yncscape(81.575 +  dist_v * r, 2.132)), module, Z8K::LED_HORIZ + n);
			#ifdef OSCTEST_MODULE
			if(module != NULL)
			{
				sprintf(name, "/LedH%i", n + 1);
				module->oscDrv->Add(new oscControl(name), plight);
			}
			#endif
			addChild(plight);

			if(r == 3)
				addOutput(createOutput<PJ301GPort>(Vec(mm2px(52.168+ dist_h * c), yncscape(2.685, 8.255)), module, Z8K::CV_A + c));

			addOutput(createOutput<PJ301WPort>(Vec(mm2px(57.362 + dist_h * c), yncscape(73.320 + dist_v * r, 8.255)), module, Z8K::ACTIVE_STEP + n));
		}
		addOutput(createOutput<PJ301GPort>(Vec(mm2px(161.154), yncscape(82.210+r*dist_v, 8.255)), module, Z8K::CV_1 + r));
	}

	addChild(createParam<BefacoPushBig>(Vec(mm2px(5.366), yncscape(115.070, 9.001)), module, Z8K::M_RESET));
	#ifdef DIGITAL_EXT
	if(module != NULL)
		addChild(new DigitalLed(mm2px(147.350), yncscape(92.799, 7.074), &module->connected));
	#endif
}
