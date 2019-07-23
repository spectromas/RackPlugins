#include "common.hpp"
#include "mplex.hpp"

void Mplex::on_loaded()
{
	load();
}

void Mplex::load()
{
	set_output(0);
}

void Mplex::set_output(int n)
{
	cur_sel = n;
	for(int k = 0; k < NUM_MPLEX_INPUTS; k++)
	{
		lights[LED_1 + k].value = k == cur_sel ? LVL_ON : LVL_OFF;
	}
}

void Mplex::process(const ProcessArgs &args)
{
	if(upTrigger.process(params[BTDN].value + inputs[INDN].value))
	{
		if(++cur_sel >= NUM_MPLEX_INPUTS)
			cur_sel = 0;
		set_output(cur_sel);
	} else if(dnTrigger.process(params[BTUP].value + inputs[INUP].value))
	{
		if(--cur_sel < 0)
			cur_sel = NUM_MPLEX_INPUTS-1;
		set_output(cur_sel);
	}

	outputs[OUT_1].value = inputs[IN_1 + cur_sel].value;
}

MplexWidget::MplexWidget(Mplex *module) : ModuleWidget()
{
	setModule(module);
	box.size = Vec(10 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/mplex.svg")));		
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addParam(createParam<BefacoPushBig>(Vec(mm2px(25.322), yncscape(85.436, 8.999)), module, Mplex::BTUP));
	addParam(createParam<BefacoPushBig>(Vec(mm2px(25.322), yncscape(33.452, 8.999)), module, Mplex::BTDN));
	addInput(createInput<PJ301BPort>(Vec(mm2px(25.694), yncscape(71.230, 8.255)), module, Mplex::INUP));
	addInput(createInput<PJ301BPort>(Vec(mm2px(25.694), yncscape(49.014, 8.255)), module, Mplex::INDN));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(40.045), yncscape(60.122, 8.255)), module, Mplex::OUT_1));

	float y = 105.068;
	float x = 3.558;
	float led_x = 13.843;
	float y_offs = y - 108.108;
	float delta_y = 92.529 - 105.068;
	for(int k = 0; k < NUM_MPLEX_INPUTS; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(mm2px(x), yncscape(y, 8.255)), module, Mplex::IN_1 + k));
		addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(led_x), yncscape(y-y_offs, 2.176)), module, Mplex::LED_1 + k));
		y += delta_y;
		if(k == 3)
			y -= 2.117;
	}
}
