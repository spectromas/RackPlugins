#include "common.hpp"
#include "outRange.hpp"
#include "../include/tosLink.hpp"
#include "../include/z8k.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define z8kStrip_NUM_STEPS  (16)

struct z8kStrip;
struct z8kStripWidget : SequencerWidget
{
	
	z8kStripWidget(z8kStrip *module);
private:
	void create_strip(z8kStrip *module, int n);
};

struct z8kStripStrip
{
public:
	void Init(z8kStrip *pmodule);
	void process(Z8K::z8kTosData *pData, int forceStep, float deltaTime);
	void reset(float deltaTime);

private:
	enum STEPMODE
	{
		RESET,
		NORMAL,
		SKIP
	};
	int curStep;
	z8kStrip *pModule;
	dsp::SchmittTrigger resetTrig;
	SchmittTrigger2 clockTrigger;
	void beginPulse(Z8K::z8kTosData *pData = NULL);
	void endPulse();
	STEPMODE getStepMode();
	void move_next();
	PulseGenerator2 resetPulseGuard;
	bool resetting;
	const float pulseTime = 0.001;
	int prenotazioneDiChiamata;
};

struct z8kStrip : Module
{
	enum ParamIds
	{
		MODE ,
		BACKWARD = MODE + z8kStrip_NUM_STEPS,
		NUM_PARAMS
	};
	enum InputIds
	{
		SETSTEP1,
		RESET1 = SETSTEP1 + z8kStrip_NUM_STEPS,
		DIRECTION1,
		CLOCK1,
		IN_TOS,

		NUM_INPUTS 
	};
	enum OutputIds
	{
		CURSTEP1,
		CV1 = CURSTEP1 + z8kStrip_NUM_STEPS,
		GATE1 ,
		OUT_TOS,
		NUM_OUTPUTS 
	};
	enum LightIds
	{
		LEDSTRIP1,
		NUM_LIGHTS = LEDSTRIP1 + z8kStrip_NUM_STEPS
	};
	z8kStrip() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < z8kStrip_NUM_STEPS; k++)
		{
			configParam(MODE + k, 0.0, 2.0, 1.0);
		}
		strip.Init(this);

		on_loaded();
	}

	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void onRandomize() override { load(); }
	TOSLinkInput tosIn;
	TOSLinkOutput tosOut;

private:
	void on_loaded();
	void load();
	void reset(float deltaTime);
	z8kStripStrip strip;
	dsp::SchmittTrigger setStepTrig[z8kStrip_NUM_STEPS];
};
