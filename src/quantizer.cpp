#include "common.hpp"
#include "quantizer.hpp"

void Quantizer::process(const ProcessArgs &args)
{
	if(outputs[IN_1].isConnected())
	{
		int channels = inputs[IN_1].getChannels();
		for(int k = 0; k < channels; k++)
		{
			float v = inputs[IN_1].getVoltage(k);
			int octave = int(v);	// 1v/octave
			float semitone = quantize(v, octave);
			outputs[OUT_1].setVoltage(semitone+octave, k);
			outputs[OUT_NOTRANSPOSE].setVoltage(semitone, k);
		}
		outputs[OUT_1].setChannels(channels);
		outputs[OUT_NOTRANSPOSE].setChannels(channels);
	}
}

void Quantizer::on_loaded()
{
	load();
	calcScale();
}

QuantizerWidget::QuantizerWidget(Quantizer *module) : ModuleWidget()
{
	display = NULL;
	if(module != NULL)
		module->setWidget(this);

	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
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

	display = createWidget<qtzrDisplay>(mm2px(Vec(3.41891, 15.8373)));
	display->box.size = mm2px(Vec(33.840, 19));
	if(module != NULL)
		display->CreateInterface(module);
	addChild(display);

	addInput(createInput<PJ301BPort>(Vec(mm2px(16.192), yncscape(32.580, 8.255)), module, Quantizer::IN_1));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(4.486), yncscape(14.195, 8.255)), module, Quantizer::OUT_1));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(27.899), yncscape(14.195, 8.255)), module, Quantizer::OUT_NOTRANSPOSE));
}

