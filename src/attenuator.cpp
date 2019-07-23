#include "common.hpp"
#include "attenuator.hpp"

void Attenuator::process(const ProcessArgs &args)
{
	for(int k = 0; k < NUM_ATTENUATORS; k++)
	{
		if(outputs[OUT_1 + k].isConnected())
			outputs[OUT_1 + k].value = inputs[IN_1 + k].value * params[ATT_1 + k].value;
	}
}

AttenuatorWidget::AttenuatorWidget(Attenuator *module) : ModuleWidget()
{
	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/attenuator.svg")));		
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	float in_x = mm2px(2.490);
	float pot_x = mm2px(16.320);
	float out_x = mm2px(29.894);
	float y = yncscape(107.460, 8.255);
	float ypot = yncscape(107.588, 8.0);
	float delta_y = mm2px(19.0);
	
	for(int k = 0; k < NUM_ATTENUATORS; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(in_x, y), module, Attenuator::IN_1 + k));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(pot_x, ypot), module, Attenuator::OUT_1+k));
		addOutput(createOutput<PJ301GPort>(Vec(out_x, y), module, Attenuator::OUT_1+k));
		y += delta_y;
		ypot += delta_y;
	}
}

