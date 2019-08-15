#include "../include/common.hpp"
#include "../include/z8kStrip.hpp"

void z8kStrip::process(const ProcessArgs &args)
{
	float deltaTime = 1.0 / args.sampleRate;
	int forceStep = -1;
	tosIn.process();
	tosOut.process();
	Z8K::z8kTosData *pData = (Z8K::z8kTosData *)tosIn.GetPublishedData();
	if(pData != NULL)
	{
		for(int k = 0; k < z8kStrip_NUM_STEPS && forceStep == -1; k++)
		{
			if(setStepTrig[k].process(inputs[z8kStrip::SETSTEP1 + k].value))
			{
				forceStep = k;
				break;
			}
		}

		strip.process(pData, forceStep, deltaTime);
	}
}

void z8kStrip::reset(float deltaTime)
{
	strip.reset(deltaTime);
}

void z8kStrip::on_loaded()
{
	load();
}

void z8kStrip::load()
{
	reset(0.f);
}

z8kStripWidget::z8kStripWidget(z8kStrip *module) : SequencerWidget()
{
	CREATE_PANEL(module, this, 50, "res/modules/z8kStrip.svg");

	for(int k = 0; k < z8kStrip_NUM_STEPS; k++)
	{
		create_strip(module, k);
	}

	addInput(createInput<PJ301RPort>(Vec(mm2px(4.036), yncscape(72.184f , 8.255)), module, z8kStrip::CLOCK1 ));
	addInput(createInput<PJ301BPort>(Vec(mm2px(15.07), yncscape(72.184f , 8.255)), module, z8kStrip::DIRECTION1 ));
	addInput(createInput<PJ301YPort>(Vec(mm2px(30.867), yncscape(72.184f, 8.255)), module, z8kStrip::RESET1 ));
	addParam(createParam<TL1105Sw>(Vec(mm2px(24.336), yncscape(73.009, 6.607)), module, z8kStrip::BACKWARD ));

	addOutput(createOutput<PJ301GPort>(Vec(mm2px(223.548), yncscape(72.184f, 8.255)), module, z8kStrip::CV1 ));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(239.028), yncscape(72.184f, 8.255)), module, z8kStrip::GATE1 ));

	if(module != NULL)
	{
		module->tosIn.Create(this, TOSLink::DIGITAL_TYPE::Z8KMatrix, 223.548, 46.866, z8kStrip::IN_TOS);
		module->tosOut.Create(this, &module->tosIn, 223.548, 36.866, z8kStrip::OUT_TOS); //create tru port
	}
}

void z8kStripWidget::create_strip(z8kStrip *module, int n)
{
	int xleft = 38.588f + 11.651f * n;

	ParamWidget *pwdg = createParam<NKK2>(Vec(mm2px(xleft + 4.022f), yncscape(92.204f, 7.336)), module, z8kStrip::MODE + n);
	addParam(pwdg);

	int ybase = 73.723f;
	ModuleLightWidget *plight = createLight<LargeLight<RedLight>>(Vec(mm2px(xleft + 5.666f), yncscape(ybase, 5.179)), module, z8kStrip::LEDSTRIP1 + n);
	addChild(plight);
	
	addInput(createInput<PJ301BPort>(Vec(mm2px(xleft), yncscape(13.878, 8.255)), module, z8kStrip::SETSTEP1 + n));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(xleft + 7.895f), yncscape(4.882, 8.255)), module, z8kStrip::CURSTEP1 + n));

}

void z8kStripStrip::Init(z8kStrip *pmodule)
{
	pModule = pmodule;
	resetPulseGuard.reset();
	curStep = 0;
	beginPulse();
}

void z8kStripStrip::process(Z8K::z8kTosData *pData, int forceStep, float deltaTime)
{
	if(resetTrig.process(pModule->inputs[z8kStrip::RESET1].value))
		reset(deltaTime);
	else
	{
		int pulseStatus = resetPulseGuard.process(deltaTime);
		if(pulseStatus == 0) //gioco regolare, nessun reset pending
		{
			if(forceStep >= 0)
				prenotazioneDiChiamata = forceStep;

			int clk = clockTrigger.process(pModule->inputs[z8kStrip::CLOCK1].value); // 1=rise, -1=fall
			if(clk == 1)
			{
				move_next();
				beginPulse(pData);
			} else if(clk == -1)
				endPulse();

		} else if(pulseStatus == -1)
			endPulse();
	}
}

void z8kStripStrip::move_next()
{
	if(prenotazioneDiChiamata >= 0)
	{
		curStep = prenotazioneDiChiamata;
		prenotazioneDiChiamata = -1;
		return;
	}
	bool backwd = (pModule->inputs[z8kStrip::DIRECTION1].getNormalVoltage(0.0) + pModule->params[z8kStrip::BACKWARD].value) > 0.5;
	if(getStepMode() == RESET)
	{
		curStep = backwd ? z8kStrip_NUM_STEPS -1 : 0;
		return;
	}
	for(int k = 0; k < z8kStrip_NUM_STEPS; k++)
	{
		if(backwd)
		{
			if(--curStep < 0)
				curStep = z8kStrip_NUM_STEPS - 1;
		} else
		{
			if(++curStep >= z8kStrip_NUM_STEPS)
				curStep = 0;
		}

		if(getStepMode() != SKIP)
			break;
	}

}

z8kStripStrip::STEPMODE z8kStripStrip::getStepMode()
{
	return  (z8kStripStrip::STEPMODE)(int)(pModule->params[z8kStrip::MODE + curStep].value);
}

void z8kStripStrip::beginPulse(Z8K::z8kTosData *pData)
{
	if(pData)
	{
		pModule->outputs[z8kStrip::CV1].value = pData->pRng->Value(*pData->matriciana[curStep]);
		pModule->outputs[z8kStrip::GATE1].value = LVL_ON;
	} else
		pModule->outputs[z8kStrip::GATE1].value = LVL_OFF ;
	pModule->outputs[z8kStrip::CURSTEP1 + curStep].value = LVL_ON;
	for(int k = 0; k < z8kStrip_NUM_STEPS; k++)
		pModule->lights[z8kStrip::LEDSTRIP1 + k].value = k == curStep ? LED_ON : LED_OFF;
}

void z8kStripStrip::endPulse()
{
	pModule->outputs[z8kStrip::GATE1].value = LVL_OFF;
}

void z8kStripStrip::reset(float deltaTime)
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