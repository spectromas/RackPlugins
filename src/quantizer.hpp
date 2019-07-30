#include "common.hpp"

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;
struct Quantizer;
struct ScaleChoice;
struct scaleRootChoice;

static std::vector<const char *> QuantizerRootNotes = {"C", "C#", "D", "D#", "E", "F", "F#","G","G#","A","A#","B"};

struct qtzrDisplay : LedDisplay 
{
	ScaleChoice *scaleChoice;
	LedDisplaySeparator *separator;
	scaleRootChoice *rootChoice;
	qtzrDisplay();

	void CreateInterface(Quantizer *module);
};

struct QScale
{
	const char *name;
	std::vector<int> notes;
	QScale(const char *n, std::vector<int> ns)
	{
		name = n;
		notes = ns;
	}
};

struct QuantizerWidget : ModuleWidget
{
	QuantizerWidget(Quantizer *module);
	
private:
	qtzrDisplay *display;
	float yncscape(float y, float height)
	{
		return RACK_GRID_HEIGHT - mm2px(y + height);
	}
};

struct Quantizer : Module
{
	enum ParamIds
	{
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_1,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		OUT_NOTRANSPOSE,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	Quantizer() : Module()
	{		
		pWidget = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);		
		inputs[IN_1].setChannels(PORT_MAX_CHANNELS);
		outputs[OUT_1].setChannels(PORT_MAX_CHANNELS);
		outputs[OUT_NOTRANSPOSE].setChannels(PORT_MAX_CHANNELS);		
		initializeScale();
		root = 0;
		SetScale(0);
	}

	void process(const ProcessArgs &args) override;

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "scale", json_integer(scale));
		json_object_set_new(rootJ, "root", json_integer(root));
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *bpmJson = json_object_get(rootJ, "scale");
		if(bpmJson)
			scale = json_integer_value(bpmJson);
		bpmJson = json_object_get(rootJ, "root");
		if(bpmJson)
			root = json_integer_value(bpmJson);
		on_loaded();
	}

	void setWidget(QuantizerWidget *p) {pWidget = p;}
	inline const std::vector<QScale> &getAvailableScales() {return availableScales;}
	void SetRoot(int n) {root = n; calcScale();}
	void SetScale(int n) {scale = n; calcScale();}
	inline int getScale() {return scale;}
	inline int getRoot() {return root;}

private:
	static constexpr float SEMITONE = 1.0 / 12.0;// 1/12 V
	int scale;
	int root;

	float quantize(float v, float octave);
	void on_loaded();
	void load();
	void calcScale();
	void initializeScale();
	std::vector<QScale> availableScales;
	std::vector<float> currentScale;
	QuantizerWidget *pWidget;
};
