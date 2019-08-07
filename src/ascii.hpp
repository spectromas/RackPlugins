#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct ascii;
struct asciiWidget : ModuleWidget
{
	asciiWidget(ascii *module);

	json_t *toJson() override 
	{
		json_t *rootJ = ModuleWidget::toJson();
		json_object_set_new(rootJ, "text", json_string(textField->text.c_str()));
		return rootJ;
	}

	void fromJson(json_t *rootJ) override 
	{
		ModuleWidget::fromJson(rootJ);
		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ)
			textField->text = json_string_value(textJ);
	}
private:
	TextField *textField;
};

struct ascii : Module
{
	enum ParamIds
	{
		M_RESET,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET,
		CLK,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	ascii() : Module()
	{		
		textField = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void setField(TextField *p) { textField = p; }
	void process(const ProcessArgs &args) override;

private:
	float getValue(char c) { return clamp(rescale(c, 32.0, 127.0, LVL_OFF, LVL_MAX), LVL_OFF, LVL_MAX); }
	dsp::SchmittTrigger resetTrigger;
	TextField *textField;
	SchmittTrigger2 clockTrig;
	dsp::SchmittTrigger masterReset;
};
