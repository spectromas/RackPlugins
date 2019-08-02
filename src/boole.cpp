#include "common.hpp"
#include "boole.hpp"

void Boole::process(const ProcessArgs &args)
{
	bool hiz = params[HIZ].value > 0.1;
	lights[LED_HIZ].value = hiz ? 5.0 : 0.0;

	for(int k = 0; k < NUM_BOOL_OP; k++)
	{
		int index = 2 * k;
		if(outputs[OUT_1 + k].isConnected())
		{
			bool o = process(k, index, hiz);
			if(params[INVERT_1 + k].value > 0.1)
				o = !o;
			lights[LED_1+k+ 2 * NUM_BOOL_OP-1].value = o ? 5.0 : 0.0;
			outputs[OUT_1 + k].value = o ? LVL_ON : LVL_OFF;
		} else
			lights[LED_1+k+ 2 * NUM_BOOL_OP-1].value = 0.0;
	}
}

float Boole::getVoltage(int index, bool hiz)
{
	if(hiz && !inputs[index].isConnected())
		return random::uniform() * 2.5;
	else
		return inputs[index].getNormalVoltage(0.0);
}

bool Boole::process(int num_op, int index, bool hiz)
{
	bool x;
	if(num_op == 0)	// not?
	{
		x = getVoltage(IN_1, hiz) > params[THRESH_1 ].value;
		lights[LED_1].value = x ? 5.0 : 0.0;
		return !x;
	} else
	{
		x = getVoltage(IN_1 + index-1, hiz) > params[THRESH_1 + index-1].value;
		lights[LED_1 + index - 1].value = x ? 5.0 : 0.0;
	}
	bool y = getVoltage(IN_1 + index, hiz) > params[THRESH_1 + index].value;
	lights[LED_1 + index].value = y ? 5.0 : 0.0;
		
	switch(num_op)
	{	
		case 1: return x && y;	//and
		case 2: return x || y;	//or
		case 3: return x ^ y;	//the xor
		case 4: return !x || y;	// implication
	}

	return false;
}

BooleWidget::BooleWidget(Boole *module) : ModuleWidget()
{
	setModule(module);
	box.size = Vec(14* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/boole.svg")));		
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	float in_x = mm2px(5.170);
	float in_led_x = mm2px(15.778);
	float out_led_x = mm2px(53.878);
	float pot_x = mm2px(20.561);
	float out_x = mm2px(58.091);
	float y = 112.349;
	float yout = 112.349;
	float ypot = 112.477;
	float yled = 115.389;
	float yinv = 97.892;
	float yled_out = 115.389;
	float delta_y =- 14.771;
	float sub_dy = -11.92;
	float out_dy = -26.691;
	
	for(int k = 0; k < NUM_BOOL_OP; k++)
	{
		int index = 2 * k;
		if(k > 0)
			index--;

		// X
		addInput(createInput<PJ301GRPort>(Vec(in_x, yncscape(y, 8.255)), module, Boole::IN_1 + index));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(pot_x, yncscape(ypot, 8.0)), module, Boole::THRESH_1 + index));
		addChild(createLight<SmallLight<RedLight>>(Vec(in_led_x, yncscape(yled, 2.176)), module, Boole::LED_1+index));

		// Y
		if(k > 0)
		{
			index++;
			y += sub_dy;
			ypot += sub_dy;
			yled += sub_dy;
			addInput(createInput<PJ301GRPort>(Vec(in_x, yncscape(y, 8.255)), module, Boole::IN_1 + index));
			addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(pot_x, yncscape(ypot, 8.0) ), module, Boole::THRESH_1 + index));
			addChild(createLight<SmallLight<RedLight>>(Vec(in_led_x, yncscape(yled, 2.176)), module, Boole::LED_1 + index ));
		}
		
		// OUT
		addOutput(createOutput<PJ301WPort>(Vec(out_x, yncscape(yout, 8.255)), module, Boole::OUT_1+k));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(out_led_x, yncscape(yled_out, 2.176)), module, Boole::LED_1 + k+ 2 * NUM_BOOL_OP-1));
		if(k == 0)
		{
			addParam(createParam<CKSSFix>(Vec(mm2px(53.116), yncscape(118.714, 5.460)), module, Boole::INVERT_1 + k));
			yled_out -= 20.731;
			yout -= 20.731;
		} else
		{
			addParam(createParam<CKSSFix>(Vec(mm2px(53.116), yncscape(yinv, 5.460)), module, Boole::INVERT_1 + k));
			yled_out += out_dy;
			yout += out_dy;
			yinv += out_dy;
		}

		y += delta_y;
		ypot += delta_y;
		yled += delta_y;
	}

	addParam(createParam<CKSSFixH>(Vec(mm2px(47.847), yncscape(2.136, 3.704)), module, Boole::HIZ));
	addChild(createLight<SmallLight<YellowLight>>(Vec(mm2px(54.166), yncscape(2.7, 2.176)), module, Boole::LED_HIZ ));
}

