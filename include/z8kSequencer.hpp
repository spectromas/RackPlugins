#pragma once

struct Z8K;
struct z8kSequencer
{
public:
	void Init(Input *pRst, Input *pDir, Input *pClk, Output *pOut, Light *pLights, std::vector<Param> &params, std::vector<int> steps)
	{
		curStep = 0;
		pReset = pRst;
		pDirection = pDir;
		pClock = pClk;
		pOutput = pOut;
		numSteps = steps.size();
		for(int k = 0; k < numSteps; k++)
		{
			sequence.push_back(&params[steps[k]]);
			leds.push_back(&pLights[steps[k]]);
			chain.push_back(steps[k]);
		}
	}

	inline void Reset()
	{
		curStep = 0;
		leds[0]->value = LED_ON;
		for(int k = 1; k < numSteps; k++)
			leds[k]->value =LED_OFF;
	}

	int Step(Z8K *pModule);
	
	z8kSequencer()
	{
		pReset = NULL;
		pDirection = NULL;
		pClock = NULL;
		pOutput = NULL;
	}

private:
	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	Input *pReset;
	Input *pDirection;
	Input *pClock;
	Output *pOutput;
	std::vector<Param *> sequence;
	std::vector<Light *> leds;
	std::vector<int> chain;
	int curStep;
	int numSteps;
};
