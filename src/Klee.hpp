#pragma once
#include "common.hpp"
#include <algorithm>

struct Klee;
struct KleeWidget : SequencerWidget
{
private:
	enum MENUACTIONS
	{
		RANDOMIZE_BUS,
		RANDOMIZE_PITCH,
		RANDOMIZE_LOAD,
		SET_RANGE_1V
	};
	Menu *addContextMenu(Menu *menu) override;

public:
	KleeWidget(Klee *module);
	void onMenu(int action);
};

struct Klee : Module
{
	enum ParamIds
	{
		PITCH_KNOB,
		GROUPBUS = PITCH_KNOB + 16,
		LOAD_BUS = GROUPBUS + 16,
		LOAD_PARAM = LOAD_BUS + 16,
		STEP_PARAM,
		X28_X16,
		RND_PAT,
		B_INV,
		RND_THRESHOLD,
		BUS1_LOAD,
		BUS_MERGE,
		RANGE = BUS_MERGE + 3,
		BUS2_MODE,
		NUM_PARAMS
	};

	enum InputIds
	{
		LOAD_INPUT,
		EXT_CLOCK_INPUT,
		RND_THRES_IN,
		RANGE_IN,
		NUM_INPUTS
	};

	enum OutputIds
	{
		CV_A,
		CV_B,
		CV_AB,
		CV_A__B,
		GATE_OUT,
		TRIG_OUT = GATE_OUT + 3,
		temp = TRIG_OUT + 3,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		LED_PITCH,

		LED_BUS = LED_PITCH + 16,
		temp1 = LED_BUS + 3,
		NUM_LIGHTS
	};

	Klee() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	
		for(int k = 0; k < 8; k++)
		{
			configParam(Klee::LOAD_BUS + k, 0.0, 1.0, 0.0);
			configParam(Klee::LOAD_BUS + k + 8, 0.0, 1.0, 0.0);
			configParam(Klee::GROUPBUS + k, 0.0, 2.0, 2.0);
			configParam(Klee::GROUPBUS + k + 8, 0.0, 2.0, 2.0);	
		}

		for(int k = 0; k < 3; k++)
		{
			configParam(Klee::BUS_MERGE + k, 0.0, 1.0, 0.0);	
		}
	
		configParam(Klee::BUS2_MODE, 0.0, 1.0, 0.0);
		configParam(Klee::LOAD_PARAM, 0.0, 1.0, 0.0);
		configParam(Klee::BUS1_LOAD, 0.0, 1.0, 0.0);
		configParam(Klee::STEP_PARAM, 0.0, 1.0, 0.0);
		configParam(Klee::X28_X16, 0.0, 1.0, 0.0);
		configParam(Klee::RND_PAT, 0.0, 1.0, 0.0);
		configParam(Klee::B_INV, 0.0, 1.0, 0.0);
		configParam(Klee::RANGE, 0.0001, 5.0, 1.0);
		configParam(Klee::RND_THRESHOLD, 0.0, 1.0, 0.0);

		for(int k = 0; k < 8; k++)
		{
			configParam(Klee::PITCH_KNOB + k, 0.0, 1.0, 0.125);
			configParam(Klee::PITCH_KNOB + 8 + k, 0.0, 1.0, 0.125);
		}
		#ifdef LAUNCHPAD
		drv = new LaunchpadBindingDriver(this, Scene1, 1);
		#endif
		#ifdef OSCTEST_MODULE
		oscDrv = new OSCDriver(this, 1);
		#endif

		on_loaded();
	}

	#ifdef DIGITAL_EXT
	~Klee()
	{
		#if defined(LAUNCHPAD)
		delete drv;
		#endif
		#if defined(OSCTEST_MODULE)
		delete oscDrv;
		#endif
	}
	#endif

	void dataFromJson(json_t *root) override { Module::dataFromJson(root); on_loaded(); }
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();

		return rootJ;
	}
	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void onRandomize() override { load(); }

	#ifdef DIGITAL_EXT
	float connected;
	#endif
	#ifdef LAUNCHPAD
	LaunchpadBindingDriver *drv;
	#endif
	#if defined(OSCTEST_MODULE)
	OSCDriver *oscDrv;
	#endif

private:
	const float pulseTime = 0.002;      //2msec trigger
	void showValues();
	void sr_rotate();
	bool chance();
	void populate_gate(int clk);
	void update_bus();
	void load();
	void on_loaded();
	void populate_outputs();
	void check_triggers(float deltaTime);
	bool isSwitchOn(int ptr);
	int getValue3(int k);
	dsp::SchmittTrigger loadTrigger;
	SchmittTrigger2 clockTrigger;
	dsp::PulseGenerator triggers[3];

	union
	{
		struct
		{
			bool A[8];
			bool B[8];
		};
		bool P[16];
	} shiftRegister;

	bool bus_active[3];
};
