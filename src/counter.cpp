#include "Counter.hpp"

void Counter::on_loaded()
{
	load();
}

void Counter::load()
{
	curCounter = 0;
	countDown=0;
}

void Counter::process_keys()
{
	if(pWidget != NULL)
	{
		if(btnup.process(params[COUNTER_INC].value))
		{
			int n = roundf(counter_f);
			if(n < COUNTER_MAXVALUE)
			{
				n += 1;
				pWidget->SetCounter(n);
			}
		} else if(btndwn.process(params[COUNTER_DEC].value))
		{
			int n = roundf(counter_f);
			if(n > COUNTER_MINVALUE)
			{
				n -= 1;
				pWidget->SetCounter(n);
			}
		}
	}
}

void Counter::process(const ProcessArgs &args)
{
	process_keys();
	counter_f = params[COUNTER].value;
	int n = roundf(counter_f);
	countDown = counter_f - curCounter;

	if(resetTrigger.process(inputs[RESET].value))
	{
		curCounter = 0;
	} else if(counterTigger.process(inputs[IN_1].value))
	{
		++curCounter;
		if(curCounter >= n)
		{
			lights[ACTIVE].value = LVL_ON;
			outputs[OUT_1].value = LVL_ON;
			curCounter = 0;
		} else
		{
			lights[ACTIVE].value = LVL_OFF;
			outputs[OUT_1].value = LVL_OFF;
		}
	} 
}

CounterWidget::CounterWidget(Counter *module) : SequencerWidget(module)
{
	if(module != NULL)
		module->setWidget(this);
	box.size = Vec(12* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;

		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/Counter.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(15, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 30, 0)));
	addChild(createWidget<ScrewBlack>(Vec(15, 365)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 30, 365)));

	addParam(createParam<UPSWITCH>(Vec(mm2px(2.281), yncscape(104.588,4.115)), module, Counter::COUNTER_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(2.281), yncscape(99.788, 4.115)), module, Counter::COUNTER_DEC));

	SigDisplayWidget *display = new SigDisplayWidget(3, 0);
	display->box.pos = Vec(mm2px(9), RACK_GRID_HEIGHT-mm2px(108));
	display->box.size = Vec(30+16, 24);
	if(module != NULL)
		display->value = &module->counter_f;
	addChild(display);

	SigDisplayWidget *displayCtr = new SigDisplayWidget(3, 0);
	displayCtr->box.pos = Vec(mm2px(9), RACK_GRID_HEIGHT-mm2px(98));
	displayCtr->box.size = Vec(30+16, 24);
	if(module != NULL)
		displayCtr->value = &module->countDown;
	addChild(displayCtr);

	ParamWidget *pw = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(39.018), yncscape(99.483, 9.525)), module, Counter::COUNTER);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	addInput(createInput<PJ301BPort>(Vec(mm2px(3.238), yncscape(12.664, 8.255)), module, Counter::IN_1));
	addInput(createInput<PJ301YPort>(Vec(mm2px(20.972), yncscape(27.229, 8.255)), module, Counter::RESET));
	
	addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(34.767), yncscape(15.703, 2.176)), module, Counter::ACTIVE));

	addOutput(createOutput<PJ301OPort>(Vec(mm2px(38.789), yncscape(12.664, 8.255)), module, Counter::OUT_1));
}

void CounterWidget::SetCounter(int n)
{
	int index = getParamIndex(Counter::COUNTER);
	if(index >= 0)
	{
		// VCV interface update is ahem.... migliorabile....
		Davies1900hFixWhiteKnob *pKnob = (Davies1900hFixWhiteKnob *)params[index];
		bool smooth = pKnob->smooth;
		pKnob->smooth = false;
		params[index]->paramQuantity->setValue((float)n);
		pKnob->smooth = smooth;
	}
}