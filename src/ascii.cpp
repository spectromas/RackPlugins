#include "ascii.hpp"

void ascii::process(const ProcessArgs &args)
{
	if (textField != NULL)
	{
		if (resetTrigger.process(inputs[RESET].value) || masterReset.process(params[M_RESET].value))
		{
			textField->cursor = textField->selection = 0;
			outputs[OUT].value = LVL_OFF;
		} else
		{
			int clk = clockTrig.process(inputs[CLK].value); // 1=rise, -1=fall
			if (clk == 1)
			{
				std::string safecopy = textField->text;
				int len = safecopy.length();
				if (len > 0)
				{
					if (textField->cursor >= len)
						textField->cursor = 0;
					char c = safecopy.at(textField->cursor++);
					textField->selection = textField->cursor;
					outputs[OUT].value = getValue(c);
				} else
					outputs[OUT].value = LVL_OFF;

			}
		}
	}
}

asciiWidget::asciiWidget(ascii *module)
{
	setModule(module);
	box.size = Vec(16 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/ascii.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	textField = createWidget<LedDisplayTextField>(mm2px(Vec(3.39962, 14.8373)));
	textField->box.size = mm2px(Vec(74.480, 98.753));
	((LedDisplayTextField *)textField)->color = nvgRGB(0xff, 0xff, 0xff);

	textField->multiline = true;
	addChild(textField);
	if (module != NULL)
		module->setField(textField);

	addInput(createInput<PJ301RPort>(Vec(mm2px(10.932), yncscape(4.233, 8.255)), module, ascii::CLK));
	addInput(createInput<PJ301YPort>(Vec(mm2px(36.512),  yncscape(4.233, 8.255)), module, ascii::RESET));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(62.092), yncscape(4.233, 8.255)), module, ascii::OUT));
	addChild(createParam<BefacoPushBig>(Vec(mm2px(3.4), yncscape(116.611, 8.999)), module, ascii::M_RESET));
}
