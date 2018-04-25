#include "pwmClock.hpp"

#define PWM_MINVALUE (0.05)
#define PWM_MAXVALUE (0.95)
#define SWING_MINVALUE (0.0)
#define SWING_MAXVALUE (0.5)
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

void PwmClock::process_keys()
{
	if(btnup.process(params[BPM_INC].value))
	{
		if(bpm_integer < 220.0)
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

void PwmClock::step()
{
	process_keys();
	bpm_integer = roundf(params[BPM].value);
	updateBpm();

	if((params[OFFON].value + inputs[OFFON_IN].value) > 0.5)
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
	float offs = inputs[PWM_IN].active ? rescale(inputs[PWM_IN].value, 0.0, 5.0, PWM_MINVALUE, PWM_MAXVALUE) : 0.0;
	return clamp(offs + params[PWM].value, PWM_MINVALUE, PWM_MAXVALUE);
}

float PwmClock::getSwing()
{
	float offs = inputs[SWING_IN].active ? rescale(inputs[SWING_IN].value, 0.0, 5.0, SWING_MINVALUE, SWING_MAXVALUE) : 0.0;
	return clamp(offs + params[SWING].value, SWING_MINVALUE, SWING_MAXVALUE);
}

PwmClockWidget::PwmClockWidget(PwmClock *module) : SequencerWidget(module)
{
	module->setWidget(this);
	box.size = Vec(15 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;

		panel->setBackground(SVG::load(assetPlugin(plugin, "res/PwmClock.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

	int pos_y = 55;
	SigDisplayWidget *display = new SigDisplayWidget(4, 1);
	display->box.pos = Vec(30, pos_y);
	display->box.size = Vec(30+53, 24);
	display->value = &module->bpm;
	addChild(display);
	
	addParam(ParamWidget::create<UPSWITCH>(Vec(8, pos_y ), module, PwmClock::BPM_INC, 0.0, 1.0, 0.0));
	addParam(ParamWidget::create<DNSWITCH>(Vec(8, pos_y + 12), module, PwmClock::BPM_DEC, 0.0, 1.0, 0.0));

	int row = 0;
	int col = 0;
	pos_y += 45;	
	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		int x = col * 40 + 20;
		int y = row * 37 + pos_y;
		addOutput(Port::create<PJ301MPort>(Vec(x, y), Port::OUTPUT, module, PwmClock::OUT_1 + k));
		if(++col >= 3)
		{
			col = 0;
			row++;
		}
	}
	int pos_x = 186;
	addInput(Port::create<PJ301YPort>(Vec(pos_x, pos_y), Port::INPUT, module, PwmClock::RESET));

	pos_x = 132;
	pos_y -= 60;
	addParam(ParamWidget::create<Rogan1PSWhiteSnappedSmall>(Vec(pos_x+2, pos_y+11), module, PwmClock::BPMDEC, 0.0, 9.0, 0.0));
	addParam(ParamWidget::create<Rogan1PSWhiteSnapped>(Vec(pos_x + 44, pos_y+5), module, PwmClock::BPM, 20.0, 220.0, 120.0));

	pos_y = 100+ 2*37;
	pos_x = 186;
	addInput(Port::create<PJ301GPort>(Vec(pos_x, pos_y), Port::INPUT, module, PwmClock::OFFON_IN));
	addParam(ParamWidget::create<NKK2>(Vec(pos_x - 46, pos_y - 8), module, PwmClock::OFFON, 0.0, 1.0, 0.0));
	addChild(ModuleLightWidget::create<SmallLight<RedLight>>(Vec(pos_x - 20, pos_y-20), module, PwmClock::ACTIVE));

	pos_y += 2 * 37;
	addParam(ParamWidget::create<Rogan1PSRed>(Vec(pos_x - 50, pos_y-11), module, PwmClock::SWING, SWING_MINVALUE, SWING_MAXVALUE, SWING_MINVALUE));
	addInput(Port::create<PJ301GPort>(Vec(pos_x, pos_y), Port::INPUT, module, PwmClock::SWING_IN));

	pos_y += 2 * 37;
	addParam(ParamWidget::create<Rogan1PSGreen>(Vec(pos_x-50, pos_y-11 ), module, PwmClock::PWM, PWM_MINVALUE, PWM_MAXVALUE, 0.5));
	addInput(Port::create<PJ301GPort>(Vec(pos_x, pos_y), Port::INPUT, module, PwmClock::PWM_IN));
}

void PwmClockWidget::SetBpm(float bpm_integer)
{
	int index = getParamIndex(PwmClock::BPM);
	if(index >= 0)
	{
		// VCV interface update is ahem.... migliorabile....
		bool smooth = params[index]->smooth;
		params[index]->smooth = false;
		params[index]->setValue((double)bpm_integer);
		params[index]->smooth = smooth;
	}
}