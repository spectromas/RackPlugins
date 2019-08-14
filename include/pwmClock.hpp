#pragma once
#include "common.hpp"
#include <chrono>
#define BPM_MINVALUE (10)
#define BPM_MAXVALUE (300)
#define PWM_MINVALUE (0.05)
#define PWM_MAXVALUE (0.95)
#define SWING_MINVALUE (0.0)
#define SWING_MAXVALUE (0.5)

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

struct MIDICLOCK_TIMER
{
	void reset()
	{
		midiClockCounter = 0;
		lastclockpulse = std::chrono::high_resolution_clock::now();
		bpm = BPM_MINVALUE;
		resetStat();	
	}

	float getBpm(float trigger)
	{
		if (midiClock.process(trigger))
		{
			if ((midiClockCounter++ % 24) == 0)
			{
				midiClockCounter = 0;
				std::chrono::time_point<std::chrono::system_clock> now = std::chrono::high_resolution_clock::now();
				long elapsed_msec = (long)std::chrono::duration_cast<std::chrono::milliseconds>(now - lastclockpulse).count();
				if (elapsed_msec > 0)
				{
					if (addSample(now, roundf(600000.0f / elapsed_msec) / 10.0))
						bpm = clamp(readStat(), (float)BPM_MINVALUE, (float)BPM_MAXVALUE);
					lastclockpulse = now;
				}
			}
		}

		return bpm;
	}

	private:
		dsp::SchmittTrigger midiClock;
		unsigned char midiClockCounter;
		std::chrono::time_point<std::chrono::system_clock> lastclockpulse;
		float bpm = BPM_MINVALUE;
		int meanCalcSamples;
		float meanCalcTempValue;
		std::chrono::time_point<std::chrono::system_clock> meanCalcStart;
		void resetStat()
		{
			meanCalcStart = std::chrono::high_resolution_clock::now();
			meanCalcSamples = 0;
			meanCalcTempValue = 0.f;
		}

		bool addSample(std::chrono::time_point<std::chrono::system_clock> now, float v)
		{
			meanCalcSamples++;
			meanCalcTempValue += v;
			return std::chrono::duration_cast<std::chrono::milliseconds>(now - meanCalcStart).count() >= 500; // aggiorna bpm ogni 500msec
		}

		float readStat()
		{
			float rv = meanCalcTempValue / meanCalcSamples;
			resetStat();
			return rv;
		}
};

struct PwmClock : Module
{
	enum ParamIds
	{
		BPM_INC, BPM_DEC,
		PWM, BPM, BPMDEC,
		SWING,
		OFFON,
		PULSE,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET,
		EXT_BPM,
		PWM_IN,
		SWING_IN,
		REMOTE_IN,
		MIDI_STOP,
		MIDI_START,
		MIDI_CLOCK,
		MIDI_CONTINUE,
		PULSE_IN,
		NUM_INPUTS
	};

	enum OutputIds
	{
		OUT_1,
		ONSTOP = OUT_1 + OUT_SOCKETS,
		NUM_OUTPUTS
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
		configParam(PwmClock::BPM, BPM_MINVALUE, BPM_MAXVALUE, 120.0, "BPM","Beats per minute");
		configParam(PwmClock::OFFON, 0.0, 1.0, 0.0);
		configParam(PwmClock::SWING, SWING_MINVALUE, SWING_MAXVALUE, SWING_MINVALUE, "Swing", "%", 0, 100);
		configParam(PwmClock::PWM, PWM_MINVALUE, PWM_MAXVALUE, 0.5, "Duty cycle", "%", 0, 100);

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
	SchmittTrigger2 pulseTrigger;
	SchmittTrigger2 onTrigger;
	SchmittTrigger2 offTrigger;
	dsp::SchmittTrigger manualTrigger;
	dsp::PulseGenerator onStopPulse;
	dsp::PulseGenerator onManualStep;

	const float pulseTime = 0.1;      //2msec trigger
	void process_keys();
	void updateBpm(bool externalMidiClock);
	void process_active(const ProcessArgs &args);
	void process_inactive(const ProcessArgs &args);

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
	bool isGeneratorActive();
	SA_TIMER sa_timer[OUT_SOCKETS];
	MIDICLOCK_TIMER midiClock;
};
