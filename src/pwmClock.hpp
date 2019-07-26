#pragma once
#include "common.hpp"
#define BPM_MINVALUE (10)
#define BPM_MAXVALUE (300)
#define PWM_MINVALUE (0.05)
#define PWM_MAXVALUE (0.95)
#define SWING_MINVALUE (0.0)
#define SWING_MAXVALUE (0.5)

struct Rogan1PSRedSmall : Rogan
{
	Rogan1PSRedSmall()
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Rogan2PSRedSmall.svg")));
	}
};

#define OUT_SOCKETS (21)
struct PwmClock;
struct PwmClockWidget : SequencerWidget
{
	PwmClockWidget(PwmClock *module);
	void SetBpm(float bpmint);
};


struct SA_TIMER	//sample accurate version
{
	float Reset()
	{
		prevTime = curTime = APP->engine->getSampleTime();
		return Begin();
	}

	void RestartStopWatch() { stopwatch = 0; }
	float Begin()
	{
		RestartStopWatch();
		return totalPulseTime = 0;
	}
	float Elapsed() { return totalPulseTime; }
	float StopWatch() { return stopwatch; }

	float Step()
	{
		curTime += APP->engine->getSampleTime();
		float deltaTime = curTime - prevTime;
		prevTime = curTime;
		totalPulseTime += deltaTime;
		stopwatch += deltaTime;
		return deltaTime;
	}

private:
	float curTime;
	float prevTime;
	float totalPulseTime;
	float stopwatch;
};

struct PwmClock : Module
{
	enum ParamIds
	{
		BPM_INC, BPM_DEC,
		PWM, BPM, BPMDEC,
		SWING,
		OFFON,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET,
		EXT_BPM,
		PWM_IN,
		SWING_IN,
		OFFON_IN,
		OFF_IN,
		ON_IN,
		NUM_INPUTS
	};

	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + OUT_SOCKETS
	};

	enum LightIds
	{
		ACTIVE,
		NUM_LIGHTS
	};

	PwmClock() : Module()
	{
		pWidget = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PwmClock::BPM_INC, 0.0, 1.0, 0.0);
		configParam(PwmClock::BPM_DEC, 0.0, 1.0, 0.0);
		configParam(PwmClock::BPMDEC, 0.0, 9.0, 0.0);
		configParam(PwmClock::BPM, BPM_MINVALUE, BPM_MAXVALUE, 120.0);
		configParam(PwmClock::OFFON, 0.0, 1.0, 0.0);
		configParam(PwmClock::SWING, SWING_MINVALUE, SWING_MAXVALUE, SWING_MINVALUE);
		configParam(PwmClock::PWM, PWM_MINVALUE, PWM_MAXVALUE, 0.5);

		on_loaded();
	}
	void process(const ProcessArgs &args) override;

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_t *bpmJson = json_integer((int)bpm_integer);
		json_object_set_new(rootJ, "bpm_integer", bpmJson);
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *bpmJson = json_object_get(rootJ, "bpm_integer");
		if(bpmJson)
			bpm_integer = json_integer_value(bpmJson);
		on_loaded();
	}

	void onReset() override
	{
		bpm_integer = 120;

		load();
	}
	void onRandomize() override {}
	void setWidget(PwmClockWidget *pwdg) { pWidget = pwdg; }
	float bpm;
	float swing;

private:
	dsp::SchmittTrigger btnup;
	dsp::SchmittTrigger btndwn;
	PwmClockWidget *pWidget;
	uint32_t tick = UINT32_MAX;
	int bpm_integer = 120;
	SchmittTrigger2 resetTrigger;
	SchmittTrigger2 onTrigger;
	SchmittTrigger2 offTrigger;

	void process_keys();
	void updateBpm();
	
	float getDuration(int n)
	{
		return odd_beat[n] ? swingAmt[n] : duration[n];
	}
	float duration[OUT_SOCKETS];
	float swingAmt[OUT_SOCKETS];
	bool odd_beat[OUT_SOCKETS];
	void on_loaded();
	void load();
	void _reset();
	float getPwm();
	float getSwing();
	SA_TIMER sa_timer[OUT_SOCKETS];
};
