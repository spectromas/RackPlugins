#include "common.hpp"

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;

#define NUM_ATTENUATORS  (3)
#define NUM_VLIMITERS  (3)

struct Attenuator;
struct AttenuatorWidget : ModuleWidget
{
	AttenuatorWidget(Attenuator * module);
private:
	float yncscape(float y, float height)
	{
		return RACK_GRID_HEIGHT - mm2px(y + height);
	}
};

struct Attenuator : Module
{
	enum ParamIds
	{
		ATT_1,
		OFFS_1 = ATT_1 + NUM_ATTENUATORS,
		LIM1_MIN = OFFS_1 + NUM_ATTENUATORS,
		LIM1_MAX = LIM1_MIN + NUM_VLIMITERS,
		NUM_PARAMS = LIM1_MAX + NUM_VLIMITERS
	};
	enum InputIds
	{
		IN_1,
		NUM_INPUTS = IN_1 + NUM_ATTENUATORS + NUM_VLIMITERS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_ATTENUATORS + NUM_VLIMITERS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};
	Attenuator() : Module()
	{	
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		for(int k = 0; k < NUM_ATTENUATORS; k++)
		{
			configParam(Attenuator::ATT_1+k, 0.0, 2.0, 1.0, "Amplitude", "%", 0, 100);
			configParam(Attenuator::OFFS_1+k, -10.0, 10.0, 0.0, "Offset", "V");
		}

		for(int k = 0; k < NUM_VLIMITERS; k++)
		{
			configParam(Attenuator::LIM1_MIN+k, -10.0, 10.0, -10.0, "Min Voltage", "V");
			configParam(Attenuator::LIM1_MAX+k, -10.0, 10.0, 10.0, "Max Voltage", "V");
		}
	}
	void process(const ProcessArgs &args) override;
};
