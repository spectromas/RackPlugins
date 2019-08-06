#include "common.hpp"
#include "switch.hpp"

void XSwitch::process(const ProcessArgs &args)
{
	const float NOT_FOUND = -1000.0;
	float last_value = NOT_FOUND;
	for(int k = 0; k < NUM_SWITCHES; k++)
	{
		if(inputs[IN_1 + k].isConnected())
			last_value = inputs[IN_1 + k].value;

		if(last_value != NOT_FOUND)
		{
			if(outputs[OUT_1 + k].isConnected() && getSwitch(k))
			{
				lights[LED_1 + k].value = LED_ON;
				outputs[OUT_1 + k].value = last_value;
				continue;
			}
		}

		lights[LED_1 + k].value = LED_OFF;
		outputs[OUT_1 + k].value = LVL_OFF;
	}
}

SwitchWidget::SwitchWidget(XSwitch *module) : ModuleWidget()
{
	setModule(module);
	box.size = Vec(10 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/Switch.svg")));		
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	float in_x = mm2px(2.500);
	float mod_x = mm2px(17.306);
	float sw_x = mm2px(25.027);
	float led_x = mm2px(28.173);
	float out_x = mm2px(40.045);
	float y = 101.567;
	float y1 = 98.387;
	float yled = 114.949;
	float ysw = 105.667;
	float yinv = 99.785;
	float delta_y = 79.394 - 101.567;
	
	for(int k = 0; k < NUM_SWITCHES; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(in_x, yncscape(y, 8.255)), module, XSwitch::IN_1 + k));
		addInput(createInput<PJ301BPort>(Vec(mod_x, yncscape(y1, 8.255)), module, XSwitch::MOD_1 + k));
		addParam(createParam<NKK1>(Vec(sw_x, yncscape(ysw, 7.336)), module, XSwitch::SW_1+k));
		addParam(createParam<CKSSFix>(Vec(mm2px(34.098), yncscape(yinv, 5.460)), module, XSwitch::INV_1+k));
		addChild(createLight<SmallLight<RedLight>>(Vec(led_x, yncscape(yled, 2.176)), module, XSwitch::LED_1 + k ));
		addOutput(createOutput<PJ301GPort>(Vec(out_x, yncscape(y, 8.255)), module, XSwitch::OUT_1+k));
		y += delta_y;
		y1 += delta_y;
		ysw += delta_y;
		yled += delta_y;
		yinv += delta_y;
	}
}

