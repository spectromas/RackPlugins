#include "common.hpp"
#include "quantizer.hpp"

void Quantizer::process(const ProcessArgs &args)
{
	if(outputs[IN_1].isConnected()) 
	{
		for(int k = 0; k < PORT_MAX_CHANNELS; k++)
		{
			float v = inputs[IN_1].getVoltage(k);
			if(v >= 0)
			{
				float octave = round(v);	// 1v/octave
				float semitone = quantize(v, octave);
				outputs[OUT_1].setVoltage(semitone+octave, k); 
				outputs[OUT_NOTRANSPOSE].setVoltage(semitone, k);
			}
		}
	}
}

float Quantizer::quantize(float v, float octave)
{
	float semitone = (round((v - octave)*12.0)) / 12.0;
	auto nearest = std::lower_bound(currentScale.begin(), currentScale.end(), semitone);
	return *nearest;
}

void Quantizer::calcScale()
{
	currentScale.clear();
	float root_offs = SEMITONE * root;	//root key
	for(int k = 0; k < int(availableScales.at(scale).notes.size()); k++)
		currentScale.push_back(SEMITONE * availableScales.at(scale).notes.at(k) + root_offs);
}

void Quantizer::on_loaded()
{
	load();
}

void Quantizer::load()
{
}

void Quantizer::initializeScale()
{
	availableScales.push_back(QScale("Chromatic"				,  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }));
	availableScales.push_back(QScale("Octaves"					,  { 0 }));
	availableScales.push_back(QScale("Fifths"					,  { 0, 7 }));
	availableScales.push_back(QScale("PentAmbig"				,  { 0, 2, 7 }));
	availableScales.push_back(QScale("Fourths"					,  { 0, 5, 10 }));
	availableScales.push_back(QScale("cbm_q0"					,  { 0, 4, 5, 9 }));
	availableScales.push_back(QScale("cbm_q2"					,  { 0, 3, 5, 8 }));
	availableScales.push_back(QScale("Tristan"					,  { 0, 3, 6, 10 }));
	availableScales.push_back(QScale("cbm_p0"					,  { 0, 2, 5, 7, 11 }));
	availableScales.push_back(QScale("PentaMaj"					,  { 0, 2, 4, 7, 9 }));
	availableScales.push_back(QScale("PentaMin"					,  { 0, 2, 3, 7, 8 }));
	availableScales.push_back(QScale("Slendro"					,  { 0, 2, 5, 7, 10 }));
	availableScales.push_back(QScale("Pelog"					,  { 0, 1, 3, 7, 8 }));
	availableScales.push_back(QScale("Miyako-bushi"				,  { 0, 1, 4, 7, 8 }));
	availableScales.push_back(QScale("Blues"					,  { 0, 3, 5, 6, 7, 10 }));
	availableScales.push_back(QScale("SixtoneSym"				,  { 0, 1, 4, 5, 8, 9 }));
	availableScales.push_back(QScale("Petrushka"				,  { 0, 1, 4, 6, 7, 10 }));
	availableScales.push_back(QScale("Prometheus"				,  { 0, 2, 4, 6, 9, 10 }));
	availableScales.push_back(QScale("Japanese"					,  { 0, 1, 5, 7, 8, 10 }));
	availableScales.push_back(QScale("Wholetone"				,  { 0, 2, 4, 6, 8, 10 }));
	availableScales.push_back(QScale("Augmented"				,  { 0, 3, 4, 6, 8, 11 }));
	availableScales.push_back(QScale("AugmentedMaj"				,  { 0, 3, 4, 5, 7, 8, 11 }));
	availableScales.push_back(QScale("Major"					,  { 0, 2, 4, 5, 7, 9, 11 }));
	availableScales.push_back(QScale("Minor"					,  { 0, 2, 3, 5, 7, 9, 11 }));
	availableScales.push_back(QScale("BebopMaj"					,  { 0, 2, 4, 5, 7, 8, 9 }));
	availableScales.push_back(QScale("BebopMin"					,  { 0, 3, 4, 5, 7, 9, 10 }));
	availableScales.push_back(QScale("BebopMelMin"				,  { 0, 2, 3, 5, 7, 8, 9 }));
	availableScales.push_back(QScale("HarmonicMaj"				,  { 0, 2, 4, 5, 7, 8, 11 }));
	availableScales.push_back(QScale("HarmonicMin"				,  { 0, 2, 3, 5, 7, 8, 11 }));
	availableScales.push_back(QScale("NeopolitanMaj"			,  { 0, 1, 3, 5, 7, 9, 11 }));
	availableScales.push_back(QScale("NeopolitanMin"			,  { 0, 1, 3, 5, 7, 8, 11 }));
	availableScales.push_back(QScale("Dorian"					,  { 0, 2, 3, 5, 7, 9, 10 }));
	availableScales.push_back(QScale("Phrygian"					,  { 0, 1, 3, 5, 7, 8, 10 }));
	availableScales.push_back(QScale("Lydian"					,  { 0, 2, 4, 6, 7, 9, 11 }));
	availableScales.push_back(QScale("Aeolian"					,  { 0, 2, 3, 5, 7, 8, 10 }));
	availableScales.push_back(QScale("Mixolydian"				,  { 0, 2, 4, 5, 7, 9, 10 }));
	availableScales.push_back(QScale("Locrian"					,  { 0, 1, 3, 5, 6, 8, 10 }));
	availableScales.push_back(QScale("Algerian"					,  { 0, 2, 3, 5, 6, 7, 8, 11 }));
	availableScales.push_back(QScale("Enigmatic"				,  { 0, 1, 4, 6, 8, 10, 11 }));
	availableScales.push_back(QScale("Arabic"					,  { 0, 1, 4, 5, 7, 8, 11 }));
	availableScales.push_back(QScale("Todi"						,  { 0, 1, 3, 6, 7, 8, 11 }));
	availableScales.push_back(QScale("Purvi"					,  { 0, 1, 4, 6, 7, 8, 11 }));
	availableScales.push_back(QScale("OvertoneDom"				,  { 0, 2, 4, 6, 7, 9, 10 }));
	availableScales.push_back(QScale("Hindu"					,  { 0, 2, 4, 5, 7, 8, 10 }));
	availableScales.push_back(QScale("Altered"					,  { 0, 1, 3, 4, 6, 8, 10 }));
	availableScales.push_back(QScale("PhrygianDom"				,  { 0, 1, 4, 5, 7, 8, 10 }));
	availableScales.push_back(QScale("Octatonic"				,  { 0, 1, 3, 4, 6, 7, 9, 10 }));
	availableScales.push_back(QScale("Lydian_Minor"				,  { 0, 2, 4, 6, 7, 8, 10 }));
	availableScales.push_back(QScale("Rag_Madhukant"			,  { 0, 2, 3, 6, 7, 9, 10 }));
	availableScales.push_back(QScale("Rag_Madhuvanti"			,  { 0, 2, 3, 6, 7, 9, 11 }));
	availableScales.push_back(QScale("Rag_Ahir_Bhairav"			,  { 0, 1, 4, 5, 7, 9, 10 }));
	availableScales.push_back(QScale("Rag_Arnand_Bhairav"		,  { 0, 1, 4, 5, 7, 9, 11 }));
	availableScales.push_back(QScale("BebopDom"					,  { 0, 2, 4, 5, 7, 10, 11 }));
	availableScales.push_back(QScale("Saranga"					,  { 0, 2, 3, 6, 7, 10, 11 }));
	availableScales.push_back(QScale("Sambah"					,  { 0, 2, 3, 4, 7, 8, 10 }));
	availableScales.push_back(QScale("HungarianMaj"				,  { 0, 3, 4, 6, 7, 9, 10 }));
	availableScales.push_back(QScale("InstantJazz"				,  { 0, 1, 3, 6, 7, 9, 11 }));
	availableScales.push_back(QScale("Jeth"						,  { 0, 2, 3, 5, 6, 9, 11 }));
	availableScales.push_back(QScale("MixoBlues"				,  { 0, 3, 4, 5, 6, 7, 10 }));
	availableScales.push_back(QScale("HungarianMin"				,  { 0, 2, 3, 6, 7, 8, 11 }));
	availableScales.push_back(QScale("Diminished"				,  { 0, 2, 3, 5, 6, 8, 9, 11 }));
	availableScales.push_back(QScale("Bhairav_That"				,  { 0, 1, 2, 5, 7, 8, 11 }));
	availableScales.push_back(QScale("SpanishEightTone"			,  { 0, 1, 3, 4, 5, 6, 8, 10 }));
	availableScales.push_back(QScale("Ninetone"					,  { 0, 2, 3, 4, 6, 7, 8, 9, 11 }));
}

QuantizerWidget::QuantizerWidget(Quantizer *module) : ModuleWidget()
{
	display = NULL;
	if(module != NULL)
		module->setWidget(this); 

	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/quantizer.svg")));		
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	
	display = createWidget<qtzrDisplay>(mm2px(Vec(3.41891, 15.8373)));
	display->box.size = mm2px(Vec(33.840, 19));
	if(module != NULL)
		display->CreateInterface(module);
	addChild(display);
	
	addInput(createInput<PJ301BPort>(Vec(mm2px(16.192), yncscape(32.580, 8.255)), module, Quantizer::IN_1));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(4.486), yncscape(14.195, 8.255)), module, Quantizer::OUT_1));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(27.899), yncscape(14.195, 8.255)), module, Quantizer::OUT_NOTRANSPOSE));
}

