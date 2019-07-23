#include "common.hpp"

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;

#define NUM_SWITCHES  (5)

struct XSwitch;
struct SwitchWidget : ModuleWidget
{
	SwitchWidget(XSwitch *module);
private:
	float yncscape(float y, float height)
	{
		return RACK_GRID_HEIGHT - mm2px(y + height);
	}
};

struct XSwitch : Module
{
	enum ParamIds
	{
		SW_1,
		NUM_PARAMS = SW_1 + NUM_SWITCHES
	};
	enum InputIds
	{
		IN_1,
		MOD_1= IN_1 + NUM_SWITCHES,
		NUM_INPUTS = MOD_1 + NUM_SWITCHES
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_SWITCHES
	};
	enum LightIds
	{
		LED_1,
		NUM_LIGHTS = LED_1 + NUM_SWITCHES
	};
	XSwitch() : Module()
	{		
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < NUM_SWITCHES; k++)
			configParam(XSwitch::SW_1+k, 0.0, 1.0, 0.0);
	}
	void process(const ProcessArgs &args) override;

private:

	bool getSwitch(int n)
	{
		return (inputs[MOD_1 + n].getNormalVoltage(0.0) + params[SW_1 + n].value) > 0.5;
	}
};