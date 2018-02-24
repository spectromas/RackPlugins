#pragma once
#include "common.hpp"

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
		prevTime = curTime = engineGetSampleTime();
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
		curTime += engineGetSampleTime();
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
		NUM_PARAMS
	};
	enum InputIds
	{
		NUM_INPUTS
	};

	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + OUT_SOCKETS
	};

	enum LightIds
	{
		NUM_LIGHTS
	};

	PwmClock() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	{

		on_loaded();
	}
	void step() override;

	json_t *toJson() override
	{
		json_t *rootJ = json_object();
		json_t *bpmJson = json_integer((int)bpm_integer);
		json_object_set_new(rootJ, "bpm_integer", bpmJson);
		return rootJ;
	}

	void fromJson(json_t *rootJ) override
	{
		json_t *bpmJson = json_object_get(rootJ, "bpm_integer");
		if(bpmJson)
			bpm_integer = json_integer_value(bpmJson);
		on_loaded();
	}

	void reset() override
	{
		bpm_integer = 120;

		load();
	}
	void randomize() override {}
	void setWidget(PwmClockWidget *pwdg) { pWidget = pwdg; }
	float bpm;

private:
	SchmittTrigger btnup;
	SchmittTrigger btndwn;
	PwmClockWidget *pWidget;
	uint32_t tick = UINT32_MAX;
	int bpm_integer = 120;

	void process_keys();
	void updateBpm()
	{
		float new_bpm = (roundf(params[BPMDEC].value) + 10 * bpm_integer) / 10.0;
		if(bpm != new_bpm)
		{
			bpm = new_bpm;
			duration[0] = 240.0 / bpm;	// 1/1
			duration[1] = duration[0] + duration[0] / 2.0;
			duration[2] = 2.0* duration[0] / 3.0;

			for(int k = 1; k < 7; k++)
			{
				duration[3 * k] = duration[3 * (k - 1)] / 2.0;
				duration[3 * k + 1] = duration[3 * (k - 1) + 1] / 2.0;
				duration[3 * k + 2] = duration[3 * (k - 1) + 2] / 2.0;
			}
		}
	}
	float duration[OUT_SOCKETS];
	void on_loaded();
	void load();
	SA_TIMER sa_timer[OUT_SOCKETS];
};
