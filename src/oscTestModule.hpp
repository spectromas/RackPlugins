#pragma once
#include "common.hpp"
#ifdef OSCTEST_MODULE
#include <sstream>
#include <iomanip>
#include <algorithm>

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;

struct OscTest;
struct OscTestWidget : ModuleWidget
{
	OscTestWidget(OscTest *module);
};

struct OscTest : Module
{
	enum ParamIds
	{
		BTN1,
		POT1,
		NUM_PARAMS
	};
	enum InputIds
	{

		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		LED1,
		NUM_LIGHTS
	};
	OscTest() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(OscTest::POT1, 0.0, 1.0, 0.0);
		configParam(OscTest::BTN1, 0.0, 1.0, 0.0);

		connected = 0;
		drv = new OSCDriver(this, 8);
		lasttime = clock();
	}
	~OscTest()
	{
		delete drv;
	}
	void process(const ProcessArgs &args) override;

	OSCDriver *drv;
	float connected;
	clock_t lasttime;
};

#endif
