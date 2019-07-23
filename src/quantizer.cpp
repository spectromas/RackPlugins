#include "common.hpp"
#include "quantizer.hpp"

void Quantizer::process(const ProcessArgs &args)
{
	for(int k = 0; k < NUM_QUANTIZERS; k++)
	{
		if(outputs[OUT_1+k].isConnected()) 
			outputs[OUT_1 + k].value = quantize_out(inputs[IN_1+k], getQuantize(k));
	}
}

float Quantizer::getQuantize(int n)
{
	return inputs[TRNSPIN_1 + n].getNormalVoltage(0.0) + params[TRANSP_1 + n].value;
}

float Quantizer::quantize_out(Input &in, float transpose)
{
	float v = in.getNormalVoltage(0.0) + transpose;
	float octave = round(v);
	float rest = v - octave;
	float semi = round(rest*12.0);
	return octave + semi / 12.0;
}

QuantizerWidget::QuantizerWidget(Quantizer *module) : ModuleWidget()
{
	setModule(module);
	box.size = Vec(10 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/quantizer.svg")));		
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	float in_x = mm2px(1.749);
	float pot_x = mm2px(15.352);
	float mod_x = mm2px(25.192);
	float out_x = mm2px(40.795);
	float y = yncscape(107.0, 8.255);
	float mod_y = yncscape(105.0, 8.255);
	float ypot = yncscape(109.0, 9.175);
	float delta_y = mm2px(19.0);
	
	for(int k = 0; k < NUM_QUANTIZERS; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(in_x, y), module, Quantizer::IN_1 + k));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(pot_x, ypot+1), module, Quantizer::TRANSP_1+k));
		addInput(createInput<PJ301BPort>(Vec(mod_x, mod_y), module, Quantizer::TRNSPIN_1 + k));
		addOutput(createOutput<PJ301GPort>(Vec(out_x, y), module, Quantizer::OUT_1+k));
		y += delta_y;
		ypot += delta_y;
		mod_y += delta_y;
	}
}

