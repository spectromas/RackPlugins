#include "pwmClock.hpp"

void PwmClock::on_loaded()
{
	bpm = 0;
	swing = 0;
	_reset();
	load();
}

void PwmClock::_reset()
{
	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		sa_timer[k].Reset();
		odd_beat[k] = false;
	}
}

void PwmClock::load()
{
	updateBpm();
}

void PwmClock::updateBpm()
{
	bool updated = false;
	float new_bpm;
	if(inputs[EXT_BPM].isConnected())
		new_bpm = rescale(inputs[EXT_BPM].value, LVL_OFF, LVL_ON, BPM_MINVALUE, BPM_MAXVALUE);
	else
		new_bpm = (roundf(params[BPMDEC].value) + 10 * bpm_integer) / 10.0;

	if(bpm != new_bpm)
	{
		updated = true;
		bpm = new_bpm;
		duration[0] = 240.0 / bpm;	// 1/1
		duration[1] = duration[0] + duration[0] / 2.0;
		duration[2] = 2.0* duration[0] / 3.0;

		for(int k = 1; k < 7; k++)
		{
			duration[3 * k] = duration[3 * (k - 1)] / 2.0;
			duration[3 * k + 1] = duration[3 * (k - 1) + 1] / 2.0;
			duration[3 * k + 2] = duration[3 * (k - 1) + 2] / 2.0;
		}
	}
	float new_swing = getSwing();
	if(updated || new_swing != swing)
	{
		swing = new_swing;
		for(int k = 0; k < OUT_SOCKETS; k++)
			swingAmt[k] = duration[k] + duration[k] * swing;
	}
}

void PwmClock::process_keys()
{
	if(pWidget != NULL)
	{
		if(btnup.process(params[BPM_INC].value))
		{
			if(bpm_integer < BPM_MAXVALUE)
				bpm_integer += 1;
			pWidget->SetBpm(bpm_integer);
		}

		if(btndwn.process(params[BPM_DEC].value))
		{
			if(bpm_integer > 0)
				bpm_integer -= 1;
			pWidget->SetBpm(bpm_integer);
		}
	}
}

void PwmClock::process(const ProcessArgs &args)
{
	process_keys();
	bpm_integer = roundf(params[BPM].value);
	updateBpm();

	double offonin = (inputs[OFF_IN].isConnected() || inputs[ON_IN].isConnected()) ? 0.0 : inputs[OFFON_IN].value;
	
	if(pWidget != NULL)
	{
		if (offTrigger.process(inputs[OFF_IN].value))
		{
			pWidget->params[OFFON]->dirtyValue = params[OFFON].value = 0.0;
		} else if (onTrigger.process(inputs[ON_IN].value))
		{
			pWidget->params[OFFON]->dirtyValue = params[OFFON].value = 1.0;
		}
	}

	if((params[OFFON].value + offonin) > 0.5)
	{
		lights[ACTIVE].value = LVL_ON;
		if(resetTrigger.process(inputs[RESET].value))
		{
			_reset();
		} else
		{
			for(int k = 0; k < OUT_SOCKETS; k++)
			{
				float gate_len = getDuration(k) * getPwm();
				sa_timer[k].Step();
				float elps = sa_timer[k].Elapsed();
				if(elps >= getDuration(k))
				{
					elps = sa_timer[k].Reset();
					odd_beat[k] = !odd_beat[k];
				}
				if(elps <= gate_len)
					outputs[OUT_1 + k].value = LVL_ON;
				else
					outputs[OUT_1 + k].value = LVL_OFF;
			}
		}
	} else
	{
		for(int k = 0; k < OUT_SOCKETS; k++)
		{
			outputs[OUT_1 + k].value = LVL_OFF;
		}
		lights[ACTIVE].value = LVL_OFF;
	}
}

float PwmClock::getPwm()
{
	float offs = inputs[PWM_IN].isConnected() ? rescale(inputs[PWM_IN].value, 0.0, 5.0, PWM_MINVALUE, PWM_MAXVALUE) : 0.0;
	return clamp(offs + params[PWM].value, PWM_MINVALUE, PWM_MAXVALUE);
}

float PwmClock::getSwing()
{
	float offs = inputs[SWING_IN].isConnected() ? rescale(inputs[SWING_IN].value, 0.0, 5.0, SWING_MINVALUE, SWING_MAXVALUE) : 0.0;
	return clamp(offs + params[SWING].value, SWING_MINVALUE, SWING_MAXVALUE);
}

PwmClockWidget::PwmClockWidget(PwmClock *module) : SequencerWidget(module)
{
	if(module != NULL)
		module->setWidget(this);
	box.size = Vec(15 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;

		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/PwmClock.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(15, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 30, 0)));
	addChild(createWidget<ScrewBlack>(Vec(15, 365)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 30, 365)));

	addParam(createParam<UPSWITCH>(Vec(mm2px(7.572), yncscape(104.588,4.115)), module, PwmClock::BPM_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(7.572), yncscape(99.788, 4.115)), module, PwmClock::BPM_DEC));

	SigDisplayWidget *display = new SigDisplayWidget(4, 1);
	display->box.pos = Vec(mm2px(20), RACK_GRID_HEIGHT-mm2px(108));
	display->box.size = Vec(30+53, 24);
	if(module != NULL)
		display->value = &module->bpm;
	addChild(display);
	
	ParamWidget *pw = createParam<Davies1900hFixWhiteKnobSmall>(Vec(mm2px(50.364), yncscape(100.245, 8)), module, PwmClock::BPMDEC);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	pw = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(62.528), yncscape(99.483, 9.525)), module, PwmClock::BPM);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	addInput(createInput<PJ301BPort>(Vec(mm2px(50.326), yncscape(86.857, 8.255)), module, PwmClock::EXT_BPM));
	addInput(createInput<PJ301YPort>(Vec(mm2px(63.162), yncscape(86.857, 8.255)), module, PwmClock::RESET));

	addParam(createParam<NKK1>(Vec(mm2px(49.040), yncscape(64.997, 9.488)), module, PwmClock::OFFON));
	addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(59.141), yncscape(67.715, 2.176)), module, PwmClock::ACTIVE));
	
	addInput(createInput<PJ301BPort>(Vec(mm2px(63.162), yncscape(64.675, 8.255)), module, PwmClock::OFFON_IN));
	addInput(createInput<PJ301BPort>(Vec(mm2px(21.633), yncscape(86.857, 8.255)), module, PwmClock::OFF_IN));
	addInput(createInput<PJ301BPort>(Vec(mm2px(35.392), yncscape(86.857, 8.255)), module, PwmClock::ON_IN));

	addParam(createParam<Davies1900hFixRedKnob>(Vec(mm2px(48.511), yncscape(42.040, 9.525)), module, PwmClock::SWING));
	addInput(createInput<PJ301BPort>(Vec(mm2px(63.162), yncscape(42.675, 8.255)), module, PwmClock::SWING_IN));

	addParam(createParam<Davies1900hFixBlackKnob>(Vec(mm2px(48.511), yncscape(20.040, 9.525)), module, PwmClock::PWM));
	addInput(createInput<PJ301BPort>(Vec(mm2px(63.162), yncscape(20.675, 8.255)), module, PwmClock::PWM_IN));

	float col_x[3] = {7.875, 21.633, 35.392};
	float pos_y = yncscape(70.175, 8.255);
	int col = 0;
	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		addOutput(createOutput<PJ301OPort>(Vec(mm2px(col_x[col]), pos_y), module, PwmClock::OUT_1 + k));
		if(++col >= 3)
		{
			col = 0;
			pos_y += mm2px(11);
		}
	}
}

void PwmClockWidget::SetBpm(float bpm_integer)
{
	int index = getParamIndex(PwmClock::BPM);
	if(index >= 0)
	{
		// VCV interface update is ahem.... migliorabile....
		Davies1900hFixWhiteKnob *pKnob = (Davies1900hFixWhiteKnob *)params[index];
		bool smooth = pKnob->smooth;
		pKnob->smooth = false;
		params[index]->paramQuantity->setValue((double)bpm_integer);
		pKnob->smooth = smooth;
	}
}