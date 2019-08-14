#include "common.hpp"
#include "outRange.hpp"

using namespace rack;
extern Plugin* pluginInstance;

#define QUATTRO_NUM_STEPS  (8)
#define NUM_STRIPS  (4)

struct quattro;
struct quattroWidget : ModuleWidget
{
	quattroWidget(quattro* module);
	private:
	void create_strip(quattro* module, int n);

};

struct quattroStrip
{
	public:
	void Init(quattro *pmodule, int n);
	void process(int forceStep, float deltaTime);
	void reset(float deltaTime);
	
	private:
		enum STEPMODE
		{
			RESET,
			NORMAL,
			SKIP
		};
		int curStep;
		int stripID;
		quattro *pModule;
		dsp::SchmittTrigger resetTrig;
		SchmittTrigger2 clockTrigger;
		void beginPulse();
		void endPulse();
		STEPMODE getStepMode();
		void move_next();
		PulseGenerator2 resetPulseGuard;
		bool resetting;
		const float pulseTime = 0.01;
};

struct quattro : Module
{
	enum ParamIds
	{
		VOLTAGE_1,
		MODE = VOLTAGE_1 + QUATTRO_NUM_STEPS,
		BACKWARD = MODE + QUATTRO_NUM_STEPS,
		M_RESET = BACKWARD + NUM_STRIPS,
		RANGE,
		NUM_PARAMS = RANGE + outputRange::NUMSLOTS
	};
	enum InputIds
	{
		SETSTEP1,
		RESET1 = SETSTEP1 + QUATTRO_NUM_STEPS,
		DIRECTION1 = RESET1 + NUM_STRIPS,
		CLOCK1 = DIRECTION1 + NUM_STRIPS,
		MRESET_IN = CLOCK1 + NUM_STRIPS,
		RANGE_IN,
		NUM_INPUTS = RANGE_IN + outputRange::NUMSLOTS
	};
	enum OutputIds
	{
		CURSTEP1,
		CV1 = CURSTEP1 + QUATTRO_NUM_STEPS,
		GATE1 = CV1 + NUM_STRIPS,
		NUM_OUTPUTS = GATE1 + NUM_STRIPS
	};
	enum LightIds
	{
		LEDSTRIP1,
		LEDSTRIP2 = LEDSTRIP1 + QUATTRO_NUM_STEPS,
		LEDSTRIP3 = LEDSTRIP2 + QUATTRO_NUM_STEPS,
		LEDSTRIP4 = LEDSTRIP3 + QUATTRO_NUM_STEPS,
		NUM_LIGHTS = LEDSTRIP4 + QUATTRO_NUM_STEPS
	};
	quattro() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int k = 0; k < QUATTRO_NUM_STEPS; k++)
		{
			configParam(MODE + k, 0.0, 2.0, 1.0);
			configParam(VOLTAGE_1 + k, 0.0, 1.0, 0.5);
		}
		for (int k = 0; k < NUM_STRIPS; k++)
			strip[k].Init(this, k);
	}

	void process(const ProcessArgs& args) override;
	static int ledStrips[4];
	outputRange orng;

	private:
	quattroStrip strip[NUM_STRIPS];
	dsp::SchmittTrigger masterReset;
	dsp::SchmittTrigger setStepTrig[QUATTRO_NUM_STEPS];
};
