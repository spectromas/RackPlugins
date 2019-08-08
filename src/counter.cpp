#include "counter.hpp"

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

CounterWidget::CounterWidget(Counter *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);
	CREATE_PANEL(module, this, 8, "res/modules/Counter.svg");

	addParam(createParam<UPSWITCH>(Vec(mm2px(2.281), yncscape(104.588,4.115)), module, Counter::COUNTER_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(2.281), yncscape(99.788, 4.115)), module, Counter::COUNTER_DEC));

	SigDisplayWidget *display = new SigDisplayWidget(3, 0);
	display->box.size = Vec(30+16, 24);
	display->box.pos = Vec(mm2px(7.934), yncscape(100.07, px2mm(display->box.size.y)));

	if(module != NULL)
		display->value = &module->counter_f;
	addChild(display);

	SigDisplayWidget *displayCtr = new SigDisplayWidget(3, 0);
	displayCtr->box.size = Vec(30+16, 24);
	displayCtr->box.pos = Vec(mm2px(12.418), yncscape(83.887, px2mm(display->box.size.y)));
	if(module != NULL)
		displayCtr->value = &module->countDown;
	addChild(displayCtr);

	ParamWidget *pw = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(28.0435), yncscape(99.483, 9.525)), module, Counter::COUNTER);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	addInput(createInput<PJ301BPort>(Vec(mm2px(3.238), yncscape(12.664, 8.255)), module, Counter::IN_1));
	addInput(createInput<PJ301YPort>(Vec(mm2px(16.516), yncscape(28.287, 8.255)), module, Counter::RESET));
	
	addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(25.242), yncscape(15.703, 2.176)), module, Counter::ACTIVE));

	addOutput(createOutput<PJ301OPort>(Vec(mm2px(29.793), yncscape(12.664, 8.255)), module, Counter::OUT_1));
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