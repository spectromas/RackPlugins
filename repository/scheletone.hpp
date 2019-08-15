#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct scheletone;
struct scheletoneWidget : ModuleWidget
{
	scheletoneWidget(scheletone *module);
};

struct scheletone : Module
{
	enum ParamIds
	{
		NUM_PARAMS
	};
	enum InputIds
	{
		NUM_INPUTS
	};
	enum OutputIds
	{
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	scheletone() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override;

private:

};
