#include "common.hpp"

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;

#define NUM_ATTENUATORS  (6)

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
		NUM_PARAMS = ATT_1 + NUM_ATTENUATORS
	};
	enum InputIds
	{
		IN_1,
		NUM_INPUTS = IN_1 + NUM_ATTENUATORS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_ATTENUATORS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};
	Attenuator() : Module()
	{	
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		//	void configParam(int paramId, float minValue, float maxValue, float defaultValue, std::string label = "", std::string unit = "", float displayBase = 0.f, float displayMultiplier = 1.f, float displayOffset = 0.f) {

		for(int k = 0; k < NUM_ATTENUATORS; k++)
		{
			configParam(Attenuator::ATT_1+k, 0.0, 2.0, 1.0);
		}

	}
	void process(const ProcessArgs &args) override;
};
