#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;
#define NUM_o88o_RECT   8

struct o88o;
struct o88oWidget : SequencerWidget
{
	o88oWidget(o88o *module);
	void SetPattern(int ptn);
};

struct o88o : Module
{
public:
	enum ParamIds
	{
		M_RESET,
		GATE,
		SWITCH_BACKW,
		SWITCH_VERT,
		SWITCH_LOOP,
		SWITCH_INVERT,
		FIRSTROW,
		LASTCOL,
		FIRSTCOL,
		PATTERN,
		LASTROW, 
		PTN_INC, 
		PTN_DEC,
		RANDOMIZE,
		LED_GATE,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET_IN,
		CLOCK_IN,
		GATE_IN,
		FIRSTROW_IN,
		LASTROW_IN,
		FIRSTCOL_IN,
		LASTCOL_IN, 
		PATTERN_IN,
		SWBACK_IN,
		SWVERT_IN,
		SWLOOP_IN,
		SWINVERT_IN,
		RANDOMIZE_IN,
		NUM_INPUTS
	};
	enum OutputIds
	{
		GATE_OUT,
		CURROW_OUT,
		CURCOL_OUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	o88o() : Module()
	{
		pWidget = NULL;
		curPtn = 0;
		lastCol = lastRow = NUM_o88o_RECT - 1;
		firstCol = firstRow = 0;
		invert_active = false;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PATTERN, 1, NUM_PATTERNS, 1, "Pattern", "#");
		configParam(FIRSTROW, 1, NUM_o88o_RECT, 1, "First Row", "#");
		configParam(LASTROW, 1, NUM_o88o_RECT, NUM_o88o_RECT, "Last Row", "#");
		configParam(FIRSTCOL, 1, NUM_o88o_RECT, 1, "First Column", "#");
		configParam(LASTCOL, 1, NUM_o88o_RECT, NUM_o88o_RECT, "Last Column", "#");
		reset();
	}
	void process(const ProcessArgs &args) override;
	NVGcolor getCellColor(int r, int c);
	int patternNumber() const { return curPtn; }
	void setWidget(o88oWidget *pwdg) { pWidget = pwdg; }

private:
	inline bool isCellEnabled(int r, int c) 	{return r >= firstRow && r <= lastRow && c >= firstCol && c <= lastCol; }
	void close_gate();
	void next_step();
	void reset();
	void getPatternLimits();
	void out_position();
	inline bool getSwitch(int paramId, int inputId) { return inputs[inputId].isConnected() ? inputs[inputId].getNormalVoltage(0.0) >= 1.0 : params[paramId].value > 0.5; }
	void open_gate();
	void next_column(bool vert, bool back, bool loop);
	void next_row(bool vert, bool back, bool loop);
	void process_keys();
	void randPattrn();
	inline int getCell(int r, int c) { return invert_active ? TheMatrix[curPtn][r][c] == 0 : TheMatrix[curPtn][r][c]; }

	enum CELLSTATUS
	{
		OFF = 0,
		DISABLED,
		ENABLED,
		CURRENT,
		CURRENT_DISABLED,
		CURRENT_OFF,
		NUM_COLORS
	};
	static const int NUM_PATTERNS = 51;
	static NVGcolor cellColors[NUM_COLORS];
	static int TheMatrix[NUM_PATTERNS][NUM_o88o_RECT][NUM_o88o_RECT];
	int curPtn;
	int curCol, curRow;
	int firstCol, lastCol;
	int firstRow, lastRow;
	bool invert_active;
	SchmittTrigger2 clockTrigger;
	dsp::SchmittTrigger masterReset;
	dsp::SchmittTrigger btninc;
	dsp::SchmittTrigger btndec;
	o88oWidget *pWidget;
	dsp::SchmittTrigger rndTrigger;
	dsp::SchmittTrigger rndBtnTrig;

};
