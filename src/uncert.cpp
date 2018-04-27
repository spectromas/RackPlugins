#include "common.hpp"
#include "uncert.hpp"

void Uncertain::on_loaded()
{
	load();
}

void Uncertain::load()
{
	fluctParams.reset();
}

void Uncertain::step()
{
	if(inputs[CLOCK_FLUCT].active)
		out_fluct(clock_fluct.process(inputs[CLOCK_FLUCT].value));
	else
		fluctParams.reset();

	if(inputs[CLOCK_QUANTIZED].active)
		out_quantized(clock_quantized.process(inputs[CLOCK_QUANTIZED].value));

	if(inputs[CLOCK_STORED].active)
		out_stored(clock_stored.process(inputs[CLOCK_STORED].value));
}

void Uncertain::out_quantized(int clk)
{
	if(clk == 1)
	{
		int position = getInt(QUANTIZED_AMT, IN_QUANTIZED, 0.0, 5.0)+1;
		outputs[OUT_QUANTIZED_N1].value = roundf(rescale(randomUniform(), 0.0, 1.0, 0.0, position));		// 1V
		float n2= roundf(rescale(randomUniform(), 0.0, 1.0, 1.0, 2 << position));
		outputs[OUT_QUANTIZED_2N].value = clamp(Uncertain::SEMITONE*n2, 0.0, 10.0);
	}
}

void Uncertain::out_stored(int clk)
{
	if(clk == 1)
	{
		outputs[OUT_STORED_RND].value = clamp(roundf(rescale(randomUniform(), 0.0, 1.0, 0.0, 1000.0)) * MIN_VOLTAGE, 0.0,10.0);
		outputs[OUT_STORED_BELL].value = rndGaussianVoltage();
	}
}

float Uncertain::rndGaussianVoltage()
{
	float mu = getFloat(STORED_AMT, IN_STORED, MIN_VOLTAGE, MAX_VOLTAGE);
	float sigma = getFloat(CURVEAMP_AMT, IN_CURVEAMP, 0.01, 2.0);
	float u1 = 1.0 - randomUniform();
	float u2 = 1.0 - randomUniform();
	float randStdNormal = sqrtf(-2.0 * logf(u1)) * sinf(2.0 * M_PI * u2); //random normal(0,1)
	return clamp(mu + sigma * randStdNormal, MIN_VOLTAGE, MAX_VOLTAGE);
}

float Uncertain::rndFluctVoltage() 
{ 
	float vmax = getFloat(FLUCT_AMT, IN_FLUCT, MIN_VOLTAGE, MAX_VOLTAGE);
	return clamp(rescale(randomUniform(), 0.0, 1.0, MIN_VOLTAGE, vmax), MIN_VOLTAGE, MAX_VOLTAGE);
}

void Uncertain::out_fluct(int clk)
{
	switch(clk)
	{
		case 1: //rise
		{
			if(fluctParams.duration == 0)	// 0 if first cycle
			{
				outputs[OUT_FLUCT].value = fluctParams.vA = fluctParams.vB = rndFluctVoltage();
			}

			fluctParams.tStart = clock();
		}
		break;

		case -1: //fall
		{
			fluctParams.vB = rndFluctVoltage();
			fluctParams.duration = clock() - fluctParams.tStart;
			fluctParams.tStart = 0;
			if(fluctParams.duration > 0)
			{
				fluctParams.vA = outputs[OUT_FLUCT].value;
				fluctParams.deltaV = (fluctParams.vB - fluctParams.vA) / fluctParams.duration;
			}
		}
		break;

		default: // in progress
		{
			if(fluctParams.duration != 0 && fluctParams.tStart != 0)	// 0 if first cycle
			{
				clock_t elapsed = clock() - fluctParams.tStart;
				float v = fluctParams.vA + fluctParams.deltaV * elapsed;
				outputs[OUT_FLUCT].value = clamp(v, MIN_VOLTAGE, MAX_VOLTAGE);
			}
		}
		break;
	}
}

float Uncertain::getFloat(ParamIds p_id, InputIds i_id, float minValue, float maxValue)
{
	float offs = inputs[i_id].active ? rescale(inputs[i_id].value, 0.0, 5.0, minValue, maxValue) : 0.0;
	return clamp(offs + params[p_id].value, minValue, maxValue);
}

UncertainWidget::UncertainWidget(Uncertain *module) : SequencerWidget(module)
{
	box.size = Vec(12 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/uncert.svg")));		
		addChild(panel);
	}
	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));

	int pos_x = 18;
	int pos_y = 50;
	int x_out = pos_x+122;
	addInput(Port::create<PJ301RPort>(Vec(pos_x, pos_y -5), Port::INPUT, module, Uncertain::CLOCK_FLUCT));
	addInput(Port::create<PJ301WPort>(Vec(pos_x, pos_y + 40), Port::INPUT, module, Uncertain::IN_FLUCT));
	addParam(ParamWidget::create<Davies1900hRedKnob>(Vec(pos_x + 55, pos_y+10), module, Uncertain::FLUCT_AMT, Uncertain::MIN_VOLTAGE, Uncertain::MAX_VOLTAGE, Uncertain::MIN_VOLTAGE));
	addOutput(Port::create<PJ301GPort>(Vec(x_out, pos_y + 18), Port::OUTPUT, module, Uncertain::OUT_FLUCT));

	pos_y += 110;
	addInput(Port::create<PJ301RPort>(Vec(pos_x, pos_y -5), Port::INPUT, module, Uncertain::CLOCK_QUANTIZED));
	addInput(Port::create<PJ301WPort>(Vec(pos_x, pos_y + 40), Port::INPUT, module, Uncertain::IN_QUANTIZED));
	ParamWidget *pw = ParamWidget::create<Davies1900hWhiteKnob>(Vec(pos_x + 55, pos_y + 10), module, Uncertain::QUANTIZED_AMT, 0.0, 5.0, 0.0);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	addOutput(Port::create<PJ301GPort>(Vec(x_out, pos_y -5), Port::OUTPUT, module, Uncertain::OUT_QUANTIZED_N1));
	addOutput(Port::create<PJ301GPort>(Vec(x_out, pos_y + 40), Port::OUTPUT, module, Uncertain::OUT_QUANTIZED_2N));

	pos_y += 110;
	addInput(Port::create<PJ301RPort>(Vec(pos_x, pos_y -5), Port::INPUT, module, Uncertain::CLOCK_STORED));
	addInput(Port::create<PJ301WPort>(Vec(pos_x, pos_y + 50), Port::INPUT, module, Uncertain::IN_STORED));
	addInput(Port::create<PJ301WPort>(Vec(pos_x+60, pos_y + 50), Port::INPUT, module, Uncertain::IN_CURVEAMP));
	addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(pos_x + 25, pos_y +15), module, Uncertain::STORED_AMT, Uncertain::MIN_VOLTAGE + 2.5, Uncertain::MAX_VOLTAGE - 2.5, 5.0));
	addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(pos_x + 80, pos_y + 15), module, Uncertain::CURVEAMP_AMT, 0.0,2.0,1.0));
	addOutput(Port::create<PJ301GPort>(Vec(x_out, pos_y -5), Port::OUTPUT, module, Uncertain::OUT_STORED_RND));
	addOutput(Port::create<PJ301GPort>(Vec(x_out, pos_y + 50), Port::OUTPUT, module, Uncertain::OUT_STORED_BELL));
}
