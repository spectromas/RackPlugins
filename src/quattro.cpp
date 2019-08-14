#include "../include/common.hpp"
#include "../include/quattro.hpp"

int quattro::ledStrips[4] = {LEDSTRIP1, LEDSTRIP2, LEDSTRIP3, LEDSTRIP4};

void quattro::process(const ProcessArgs &args)
{
	float deltaTime = 1.0 / args.sampleRate;
	if(masterReset.process(params[M_RESET].value + inputs[MRESET_IN].value))
	{
		reset(deltaTime);
	} else
	{
		if(pWidget != NULL && rndTrigger.process(inputs[RANDOMIZONE].value))
			randrandrand();

		int forceStep = -1;
		for(int k = 0; k < QUATTRO_NUM_STEPS && forceStep == -1; k++)
		{
			if(setStepTrig[k].process(inputs[quattro::SETSTEP1 + k].value))
			{
				forceStep = k;
				break;
			}
		}

		for(int k = 0; k < NUM_STRIPS; k++)
			strip[k].process(forceStep, deltaTime);
	}
}

void quattro::reset(float deltaTime)
{
	for(int k = 0; k < NUM_STRIPS; k++)
		strip[k].reset(deltaTime);
}

void quattro::on_loaded()
{
	#ifdef DIGITAL_EXT
	connected = 0;
	#endif
	load();
}

void quattro::load()
{
	reset(0.f);
}

void quattro::randrandrand()
{
	if(theRandomizer & quattroWidget::RANDOMIZE_PITCH)
		randrandrand(0);

	if(theRandomizer & quattroWidget::RANDOMIZE_MODE)
		randrandrand(1);

	if(theRandomizer & quattroWidget::RANDOMIZE_DIRECTION)
		randrandrand(2);

	if(theRandomizer & quattroWidget::RANDOMIZE_LAQUALUNQUE)
	{
		randrandrand(int(random::uniform() * 3));
	}
}

void quattro::randrandrand(int action)
{
	switch(action)
	{
		case 0:
			pWidget->std_randomize(quattro::VOLTAGE_1, quattro::VOLTAGE_1 + QUATTRO_NUM_STEPS);
			break;

		case 1:
			pWidget->std_randomize(quattro::MODE, quattro::MODE + QUATTRO_NUM_STEPS);
			break;

		case 2:
			pWidget->std_randomize(quattro::BACKWARD, quattro::BACKWARD + NUM_STRIPS);
			break;
	}
}


quattroWidget::quattroWidget(quattro *module) : SequencerWidget()
{
	#ifdef OSCTEST_MODULE
	char name[60];
	#endif
	if(module != NULL)
		module->setWidget(this);

	CREATE_PANEL(module, this, 50, "res/modules/quattro.svg");

	for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
	{
		create_strip(module, k);
	}

	addInput(createInput<PJ301HPort>(Vec(mm2px(231.288), yncscape(90.423, 8.255)), module, quattro::RANDOMIZONE));

	for(int k = 0; k < NUM_STRIPS; k++)
	{
		addInput(createInput<PJ301RPort>(Vec(mm2px(4.036), yncscape(72.184f - k * 14.798f, 8.255)), module, quattro::CLOCK1 + k));
		addInput(createInput<PJ301BPort>(Vec(mm2px(15.07), yncscape(72.184f - k * 14.798f, 8.255)), module, quattro::DIRECTION1 + k));
		addInput(createInput<PJ301YPort>(Vec(mm2px(30.867), yncscape(72.184f - k * 14.798f, 8.255)), module, quattro::RESET1 + k));
		addParam(createParam<TL1105Sw>(Vec(mm2px(24.336), yncscape(73.009 - k * 14.798f, 6.607)), module, quattro::BACKWARD + k));

		addOutput(createOutput<PJ301GPort>(Vec(mm2px(223.548), yncscape(72.184f - k * 14.798f, 8.255)), module, quattro::CV1 + k));
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(239.028), yncscape(72.184f - k * 14.798f, 8.255)), module, quattro::GATE1 + k));
	}
	ParamWidget *pwdg = createParam<BefacoPushBig>(Vec(mm2px(21.577), yncscape(110.950, 8.999)), module, quattro::M_RESET);
	addChild(pwdg);
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		module->oscDrv->Add(new oscControl("reset"), pwdg);
	}
	#endif

	addInput(createInput<PJ301BPort>(Vec(mm2px(6.682), yncscape(111.322, 8.255)), module, quattro::MRESET_IN));

	if(module != NULL)
		module->orng.Create(this, 224.846f, 108.829f, quattro::RANGE_IN, quattro::RANGE);

	#ifdef DIGITAL_EXT
	if(module != NULL)
		addChild(new DigitalLed(mm2px(15.118), yncscape(99.290, 3.867), &module->connected));
	#endif
}

void quattroWidget::create_strip(quattro *module, int n)
{
	#ifdef OSCTEST_MODULE
	char name[60];
	#endif
	int xleft = 38.588f + 23.651f * n;

	ParamWidget *pwdg = createParam<Davies1900hLargeFixRedKnob>(Vec(mm2px(xleft - 1), yncscape(108.306f, 14.288)), module, quattro::VOLTAGE_1 + n);
	addParam(pwdg);
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		sprintf(name, "/Step%i", n + 1);
		module->oscDrv->Add(new oscControl(name), pwdg);
	}
	#endif
	pwdg = createParam<NKK2>(Vec(mm2px(xleft + 4.022f), yncscape(92.204f, 7.336)), module, quattro::MODE + n);
	addParam(pwdg);
	#ifdef OSCTEST_MODULE
	if(module != NULL)
	{
		sprintf(name, "/Mode%i", n + 1);
		module->oscDrv->Add(new oscControl(name), pwdg);
	}
	#endif

	for(int k = 0; k < NUM_STRIPS; k++)
	{
		int ybase = 73.723f - 14.799f * k;
		ModuleLightWidget *plight = createLight<LargeLight<RedLight>>(Vec(mm2px(xleft + 5.666f), yncscape(ybase, 5.179)), module, quattro::ledStrips[k] + n);
		addChild(plight);
		#ifdef OSCTEST_MODULE
		if(module != NULL)
		{
			sprintf(name, "/Strip%i_%i", k + 1, n + 1);
			module->oscDrv->Add(new oscControl(name), plight);
		}
		#endif
	}

	addInput(createInput<PJ301BPort>(Vec(mm2px(xleft), yncscape(13.878, 8.255)), module, quattro::SETSTEP1 + n));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(xleft + 7.895f), yncscape(4.882, 8.255)), module, quattro::CURSTEP1 + n));
}


Menu *quattroWidget::addContextMenu(Menu *menu)
{
	menu->addChild(new RandomizeItem(module));

	menu->addChild(new SeqMenuItem<quattroWidget>("Randomize Pitch", this, RANDOMIZE_PITCH));
	menu->addChild(new SeqMenuItem<quattroWidget>("Randomize Modes", this, RANDOMIZE_MODE));
	menu->addChild(new SeqMenuItem<quattroWidget>("Randomize Direction", this, RANDOMIZE_DIRECTION));
	return menu;
}

void quattroWidget::onMenu(int action)
{
	switch(action)
	{
		case RANDOMIZE_PITCH: std_randomize(quattro::VOLTAGE_1, quattro::VOLTAGE_1 + QUATTRO_NUM_STEPS); break;
		case RANDOMIZE_MODE: std_randomize(quattro::MODE, quattro::MODE + QUATTRO_NUM_STEPS); break;
		case RANDOMIZE_DIRECTION: std_randomize(quattro::BACKWARD, quattro::BACKWARD + NUM_STRIPS); break;
	}
}

quattroWidget::RandomizeSubItemItem::RandomizeSubItemItem(Module *m, const char *title, int action)
{
	md = (quattro *)m;
	text = title;
	randomizeDest = action;
	rightText = CHECKMARK((md->theRandomizer & randomizeDest) != 0);
}

void quattroWidget::RandomizeSubItemItem::onAction(const event::Action &e)
{
	md->theRandomizer ^= randomizeDest;
}


void quattroStrip::Init(quattro *pmodule, int n)
{
	stripID = n;
	pModule = pmodule;
	resetPulseGuard.reset();
	curStep = 0;
	beginPulse();
}

void quattroStrip::process(int forceStep, float deltaTime)
{
	if(resetTrig.process(pModule->inputs[quattro::RESET1 + stripID].value))
		reset(deltaTime);
	else
	{
		int pulseStatus = resetPulseGuard.process(deltaTime);
		if(pulseStatus == 0) //gioco regolare, nessun reset pending
		{
			if(forceStep >= 0)
				prenotazioneDiChiamata = forceStep;
			
			int clk = clockTrigger.process(pModule->inputs[quattro::CLOCK1 + stripID].value); // 1=rise, -1=fall
			if(clk == 1)
			{
				move_next();					
				beginPulse(false);
			} else if(clk == -1)
				endPulse();			

		} else if(pulseStatus == -1)
			endPulse();
	}
}

void quattroStrip::move_next()
{
	if(prenotazioneDiChiamata >= 0)
	{
		curStep = prenotazioneDiChiamata;
		prenotazioneDiChiamata = -1;
		return;
	}
	bool backwd = (pModule->inputs[quattro::DIRECTION1 + stripID].getNormalVoltage(0.0) + pModule->params[quattro::BACKWARD + stripID].value) > 0.5;
	if(getStepMode() == RESET)
	{
		curStep = backwd ? QUATTRO_NUM_STEPS -1 : 0;
		return;
	}
	for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
	{
		if(backwd)
		{
			if(--curStep < 0)
				curStep = QUATTRO_NUM_STEPS - 1;
		} else
		{
			if(++curStep >= QUATTRO_NUM_STEPS)
				curStep = 0;
		}

		if(getStepMode() != SKIP)
			break;
	}

}

quattroStrip::STEPMODE quattroStrip::getStepMode()
{
	return  (quattroStrip::STEPMODE)(int)(pModule->params[quattro::MODE + curStep].value);
}

void quattroStrip::beginPulse(bool silent)
{
	pModule->outputs[quattro::CV1 + stripID].value = pModule->orng.Value(pModule->params[quattro::VOLTAGE_1 + curStep].value);
	pModule->outputs[quattro::GATE1 + stripID].value = silent ? LVL_OFF : LVL_ON;
	if(stripID == 0)
		pModule->outputs[quattro::CURSTEP1 + curStep].value = LVL_ON;
	for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
		pModule->lights[quattro::ledStrips[stripID] + k].value = k == curStep ? LED_ON : LED_OFF;
}

void quattroStrip::endPulse()
{
	pModule->outputs[quattro::GATE1 + stripID].value = LVL_OFF;
	if(stripID == 0)
	{
		for(int k = 0; k < QUATTRO_NUM_STEPS; k++)
			pModule->outputs[quattro::CURSTEP1 + k].value = LVL_OFF;
	}
}

void quattroStrip::reset(float deltaTime)
{
	if(resetPulseGuard.process(deltaTime) == 0)
	{
		endPulse();
		curStep = 0;
		prenotazioneDiChiamata = -1;
		resetPulseGuard.trigger(pulseTime);
		beginPulse();
	}
}