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
		FTM,
		RANGE,
		NUM_PARAMS = RANGE + outputRange::NUMSLOTS
	};
	enum InputIds
	{
		RESET,
		CLK,
		RANGE_IN,
		NUM_INPUTS = RANGE_IN + outputRange::NUMSLOTS
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
		orng.configure(this, RANGE);
	}
	void setField(TextField *p) { textField = p; }
	void process(const ProcessArgs &args) override;
	outputRange orng;

private:
	float getValue(char c);
	void manifesto();
	dsp::SchmittTrigger resetTrigger;
	TextField *textField;
	SchmittTrigger2 clockTrig;
	dsp::SchmittTrigger masterReset;
	dsp::SchmittTrigger manifestoTrigger;
};
