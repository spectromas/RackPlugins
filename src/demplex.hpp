#include "common.hpp"

#define NUM_DEMULTIPLEX_OUTPUTS		(8)

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;


struct Dmplex;
struct DmplexWidget : ModuleWidget
{
	DmplexWidget(Dmplex * module);
private:
	float yncscape(float y, float height)
	{
		return RACK_GRID_HEIGHT - mm2px(y + height);
	}
};

struct Dmplex : Module
{
	enum ParamIds
	{
		BTUP, BTDN,
		NUM_PARAMS
	};
	enum InputIds
	{
		INUP,
		INDN,
		RESET,
		RANDOM,
		IN_1,		
		NUM_INPUTS 
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_DEMULTIPLEX_OUTPUTS
	};
	enum LightIds
	{
		LED_1,
		NUM_LIGHTS = LED_1 + NUM_DEMULTIPLEX_OUTPUTS
	};

	Dmplex() : Module()
	{		
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Dmplex::BTUP, 0.0, 1.0, 0.0);
		configParam(Dmplex::BTDN, 0.0, 1.0, 0.0);

		load();
	}

	void dataFromJson(json_t *root) override { Module::dataFromJson(root); on_loaded(); }
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();

		return rootJ;
	}
	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void onRandomize() override 
	{
		set_output(getRand(NUM_DEMULTIPLEX_OUTPUTS));
	}

private:
	void load();
	void on_loaded();
	void set_output(int n);
	int getRand(int rndMax) { return int(random::uniform() * rndMax); }

	int cur_sel;
	dsp::SchmittTrigger upTrigger;
	dsp::SchmittTrigger dnTrigger;
	dsp::SchmittTrigger reset;
	dsp::SchmittTrigger random;
};
