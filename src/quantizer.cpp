#include "common.hpp"
#include "quantizer.hpp"

void Quantizer::step()
{
	for(int k = 0; k < NUM_QUANTIZERS; k++)
	{
		if(outputs[OUT_1+k].active) 
			outputs[OUT_1 + k].value = quantize_out(inputs[IN_1+k], getQuantize(k));
	}
}

float Quantizer::getQuantize(int n)
{
	return inputs[TRNSPIN_1 + n].normalize(0.0) + params[TRANSP_1 + n].value;
}

float Quantizer::quantize_out(Input &in, float transpose)
{
	float v = in.normalize(0.0) + transpose;
	float octave = round(v);
	float rest = v - octave;
	float semi = round(rest*12.0);
	return octave + semi / 12.0;
}

QuantizerWidget::QuantizerWidget(Quantizer *module) : ModuleWidget(module)
{
	box.size = Vec(10 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;

		panel->setBackground(SVG::load(assetPlugin(plugin, "res/quantizer.svg")));
		
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	int dist_v = (RACK_GRID_HEIGHT / (2 + NUM_QUANTIZERS));
	int y = 75;
	int x = 2+RACK_GRID_WIDTH/2;
	for(int k = 0; k < NUM_QUANTIZERS; k++)
	{
		addInput(Port::create<PJ301GPort>(Vec(x, y), Port::INPUT, module, Quantizer::IN_1 + k));
		addParam(ParamWidget::create<Rogan1PSWhiteSnappedSmall>(Vec(x+38, y-1), module, Quantizer::TRANSP_1+k, 0.0, 5.0, 0.0));
		addInput(Port::create<PJ301WPort>(Vec(x+69, y+14), Port::INPUT, module, Quantizer::TRNSPIN_1 + k));
		addOutput(Port::create<PJ301RPort>(Vec(x+106, y), Port::OUTPUT, module, Quantizer::OUT_1+k));
		y += dist_v;
	}
}

