#include "common.hpp"
#include "attenuator.hpp"

void Attenuator::process(const ProcessArgs &args)
{
	bool am = params[ATTMODE].value > 0.1;

	for(int k = 0; k < NUM_ATTENUATORS; k++)
	{
		if(outputs[OUT_1 + k].isConnected())
			outputs[OUT_1 + k].value = (inputs[IN_1 + k].getVoltage() * params[ATT_1 + k].value)+params[OFFS_1 + k].value;
	}

	for(int k = 0; k < NUM_VLIMITERS; k++)
	{
		if (outputs[OUT_1 + k + NUM_ATTENUATORS].isConnected())
		{
			float mi = std::min(params[LIM1_MIN + k].value, params[LIM1_MAX + k].value);
			float ma = std::max(params[LIM1_MIN + k].value, params[LIM1_MAX + k].value);
			if(am)
				outputs[OUT_1 + k + NUM_ATTENUATORS].value = rescale(inputs[IN_1 + k + NUM_ATTENUATORS].getVoltage(), LVL_MIN, LVL_MAX, mi, ma);
			else
				outputs[OUT_1 + k + NUM_ATTENUATORS].value = clamp(inputs[IN_1 + k + NUM_ATTENUATORS].getVoltage(), mi, ma);
		}
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
	float y = yncscape(104.285, 8.255);
	float ypot = yncscape(104.418, 8.0);
	float delta_y = mm2px(14.301);
	
	for(int k = 0; k < NUM_ATTENUATORS; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(mm2px(1.432), y), module, Attenuator::IN_1 + k));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(11.558), ypot), module, Attenuator::OFFS_1+k));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(21.612), ypot), module, Attenuator::ATT_1+k));
		addOutput(createOutput<PJ301GPort>(Vec(mm2px(30.953), y), module, Attenuator::OUT_1+k));
		y += delta_y;
		ypot += delta_y;
	}

	y = yncscape(45.170, 8.255);
	ypot = yncscape(45.298, 8.0);

	for(int k = 0; k < NUM_VLIMITERS; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(mm2px(1.432), y), module, Attenuator::IN_1 + k+NUM_ATTENUATORS));
		addParam(createParam<Davies1900hFixRedKnobSmall>(Vec(mm2px(11.558), ypot), module, Attenuator::LIM1_MIN+k));
		addParam(createParam<Davies1900hFixRedKnobSmall>(Vec(mm2px(21.612), ypot), module, Attenuator::LIM1_MAX+k));
		addOutput(createOutput<PJ301GPort>(Vec(mm2px(30.953), y), module, Attenuator::OUT_1+k+NUM_ATTENUATORS));
		y += delta_y;
		ypot += delta_y;
	}

	addParam(createParam<CKSSFixH>(Vec(mm2px(17.561), yncscape(60.170, 3.704)), module, Attenuator::ATTMODE));

}

