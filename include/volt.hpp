#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct volt;
struct voltWidget : ModuleWidget
{
	voltWidget(volt *module);
};

struct volt : Module
{
	enum ParamIds
	{
		INC,
		DEC,
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_1,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	volt() : Module()
	{
		divider = DIVIDER_MIN;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override;
	int divider;

private:
	void process_keys();
	dsp::SchmittTrigger btninc;
	dsp::SchmittTrigger btndec;
	const int DIVIDER_MIN = 1;
	const int DIVIDER_MAX = 16;
};
