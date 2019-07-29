#include "common.hpp"

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;

#define NUM_BOOL_OP		(5)		//not, and, or, (the) xor, implication
struct Boole;
struct BooleWidget : ModuleWidget
{
	BooleWidget(Boole * module);
private:
	float yncscape(float y, float height)
	{
		return RACK_GRID_HEIGHT - mm2px(y + height);
	}
};

struct Boole : Module
{
	enum ParamIds
	{
		INVERT_1,
		THRESH_1 = INVERT_1 + NUM_BOOL_OP,
		NUM_PARAMS = THRESH_1 + 2* NUM_BOOL_OP-1
	};
	enum InputIds
	{
		IN_1,
		NUM_INPUTS = IN_1 + 2 * NUM_BOOL_OP-1
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_BOOL_OP
	};
	enum LightIds
	{
		LED_1,
		NUM_LIGHTS = LED_1 + 3* NUM_BOOL_OP-1
	};

	Boole() : Module()
	{		
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < NUM_BOOL_OP; k++)
		{
			int index = 2 * k;
			if(k > 0)
				index--;

			configParam(Boole::INVERT_1 + k, 0.0, 1.0, 0.0);
			configParam(Boole::THRESH_1 + index, 0.0, 10.0, 0.0);
			if(k > 0)
			{
				index++;
				configParam(Boole::THRESH_1 + index, 0.0, 10.0, 0.0);
			}
		}
	}
	void process(const ProcessArgs &args) override;

private:
	bool process(int num_op, int index);
};
