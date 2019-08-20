#pragma once

struct Spiralone;
struct spiraloneSequencer
{
public:
	void Reset(Spiralone *pSpir);
	void Step(Spiralone *pSpir);
	int GetNumSteps(Spiralone *pSpir);
	int seq;

private:
	SchmittTrigger2 clockTrig;
	dsp::SchmittTrigger resetTrigger;
	int curPos;

	int ledID() { return ledID(curPos); }
	int ledID(int n) { return seq * TOTAL_STEPS + n; }
	int getInput(Spiralone *pSpir, int input_id, int knob_id, float minValue, float maxValue);
	void gate(int clk, Spiralone *pSpir);
	void outputVoltage(Spiralone *pSpir);
};
