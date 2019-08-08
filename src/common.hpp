#pragma once
//#define DEBUG
#include "rack.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

#define LVL_MIN   (-10.0f)
#define LVL_MAX   (10.0f)
#define LVL_OFF   (0.0f)
#define LVL_ON    (10.0f)
#define LED_OFF    (0.0f)
#define LED_ON    (10.0f)

using namespace rack;
extern Plugin *pluginInstance;

#if defined(ARCH_WIN) && defined(USE_LAUNCHPAD)
#define LAUNCHPAD
#endif

#ifdef LAUNCHPAD
#include "../digitalExt/launchpad.hpp"
#include "../digitalExt/launchpadControls.hpp"
#ifdef DEBUG
#define LPTEST_MODULE
#endif
#endif

#if defined(ARCH_WIN) && defined(USE_OSC)
#define OSC_ENABLE
#ifdef DEBUG
#define OSCTEST_MODULE
#endif
#include "../digitalExt/osc/oscDriver.hpp"
#endif

#if defined(LAUNCHPAD) || defined(OSCTEST_MODULE)
#define DIGITAL_EXT
#endif


struct PatternBtn : SvgSwitch {
	PatternBtn() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Patternbtn_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Patternbtn_1.svg")));
	}
};

struct HiddenButton : SvgSwitch {
	HiddenButton() {
		momentary = true;

		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/hidden_0.svg")));
	}
};

struct UPSWITCH : SvgSwitch
{
	UPSWITCH()
	{
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/upswitch_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/upswitch_1.svg")));
	}
};

struct DNSWITCH : SvgSwitch
{
	DNSWITCH()
	{
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/dnswitch_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/dnswitch_1.svg")));
	}
};

struct _davies1900base : Davies1900hKnob 
{
	_davies1900base(const char *res) 
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, res)));
	}

	void randomize() override
	{
		if(snap)
			paramQuantity->setValue(roundf(rescale(random::uniform(), 0.0, 1.0, paramQuantity->getMinValue(), paramQuantity->getMaxValue())));
		else
			Davies1900hKnob::randomize();
	}
};

struct Davies1900hFixWhiteKnob : _davies1900base 
{
	Davies1900hFixWhiteKnob() : _davies1900base("res/Davies1900hWhite.svg") {}
};

struct Davies1900hFixBlackKnob : _davies1900base 
{
	Davies1900hFixBlackKnob() : _davies1900base("res/Davies1900hBlack.svg") {}
};

struct Davies1900hFixRedKnob : _davies1900base 
{
	Davies1900hFixRedKnob() : _davies1900base("res/Davies1900hRed.svg") {}
};

struct Davies1900hFixWhiteKnobSmall : _davies1900base
{
	Davies1900hFixWhiteKnobSmall() : _davies1900base("res/Davies1900hWhiteSmall.svg") {}
};

struct Davies1900hFixBlackKnobSmall : _davies1900base
{
	Davies1900hFixBlackKnobSmall() : _davies1900base("res/Davies1900hBlackSmall.svg") {}
};

struct Davies1900hFixRedKnobSmall : _davies1900base
{
	Davies1900hFixRedKnobSmall() : _davies1900base("res/Davies1900hRedSmall.svg") {}
};


struct _ioPort : SvgPort
{
	_ioPort(const char *res)
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, res)));
		sw->wrap();
		box.size = sw->box.size;
	}
};

struct PJ301HPort : _ioPort
{
	PJ301HPort() : _ioPort("res/PJ301H.svg") {}
};

struct PJ301YPort : _ioPort 
{
	PJ301YPort() : _ioPort("res/PJ301Y.svg") {}
};

struct PJ301BPort : _ioPort
{
	PJ301BPort() : _ioPort("res/PJ301B.svg") {}
};

struct PJ301GPort : _ioPort
{
	PJ301GPort() : _ioPort("res/PJ301G.svg") {}
};
struct PJ301GRPort : _ioPort
{
	PJ301GRPort() : _ioPort("res/PJ301GR.svg") {}
};

struct PJ301RPort : _ioPort
{
	PJ301RPort() : _ioPort("res/PJ301R.svg") {}
};

struct PJ301WPort : _ioPort
{
	PJ301WPort() : _ioPort("res/PJ301W.svg") {}
};

struct PJ301OPort : _ioPort
{
	PJ301OPort() : _ioPort("res/PJ301O.svg") {}
};

struct PJ301BLUPort : _ioPort
{
	PJ301BLUPort() : _ioPort("res/PJ301BLU.svg") {}
};

struct CL1362YPort : _ioPort
{
	CL1362YPort() : _ioPort("res/CL1362Y.svg") {}
};

struct CL1362GPort : _ioPort
{
	CL1362GPort() : _ioPort("res/CL1362G.svg") {}
};

struct CL1362RPort : _ioPort
{
	CL1362RPort() : _ioPort("res/CL1362R.svg") {}
};

struct CL1362WPort : _ioPort
{
	CL1362WPort() : _ioPort("res/CL1362W.svg") {}
};

struct BefacoPushBig : app::SvgSwitch {
	BefacoPushBig() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BefacoPush_0big.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BefacoPush_1big.svg")));
	}
};

struct CKSSFix : app::SvgSwitch  {
	CKSSFix() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_1.svg")));
	}
	void randomize() override
	{
		paramQuantity->setValue(roundf(rescale(random::uniform(), 0.0, 1.0, paramQuantity->getMinValue(), paramQuantity->getMaxValue())));
	}
};

struct CKSSFixH : app::SvgSwitch  {
	CKSSFixH() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_0H.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_1H.svg")));
	}
	void randomize() override
	{
		paramQuantity->setValue(roundf(rescale(random::uniform(), 0.0, 1.0, paramQuantity->getMinValue(), paramQuantity->getMaxValue())));
	}
};

struct CKSSThreeFix : app::SvgSwitch  {
	CKSSThreeFix() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSSThree_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSSThree_1.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSSThree_2.svg")));
	}
	void randomize() override
	{
		paramQuantity->setValue(roundf(rescale(random::uniform(), 0.0, 1.0, paramQuantity->getMinValue(), paramQuantity->getMaxValue())));
	}
};

struct TL1105Sw : app::SvgSwitch  {
	TL1105Sw() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TL1105_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TL1105_1.svg")));
	}
};

struct SchmittTrigger2
{
	// UNKNOWN is used to represent a stable state when the previous state is not yet set
	enum { UNKNOWN, LOW, HIGH } state = UNKNOWN;
	float low = 0.0;
	float high = 1.0;
	void setThresholds(float low, float high)
	{
		this->low = low;
		this->high = high;
	}

	int process(float in)
	{
		switch(state)
		{
			case LOW:
			if(in >= high)
			{
				state = HIGH;
				return 1;
			}
			break;
			case HIGH:
			if(in <= low)
			{
				state = LOW;
				return -1;
			}
			break;
			default:
			if(in >= high)
			{
				state = HIGH;
			} else if(in <= low)
			{
				state = LOW;
			}
			break;
		}
		return 0;
	}

	void reset()
	{
		state = UNKNOWN;
	}
};

struct NKK1 : app::SvgSwitch 
{
	NKK1() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NKK_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NKK_2.svg")));
	}

	void randomize() override
	{
		paramQuantity->setValue(roundf(rescale(random::uniform(), 0.0, 1.0, paramQuantity->getMinValue(), paramQuantity->getMaxValue())));
	}
};

struct NKK2 : app::SvgSwitch 
{
	NKK2() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NKK_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NKK_1.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NKK_2.svg")));
	}

	void randomize() override
	{
		paramQuantity->setValue(roundf(rescale(random::uniform(), 0.0, 1.0, paramQuantity->getMinValue(), paramQuantity->getMaxValue())));
	}
};

struct VerticalSwitch : SvgSlider 
{
	VerticalSwitch()
	{
		snap = true;
		maxHandlePos = Vec(-mm2px(2.3-2.3/2.0), 0);
		minHandlePos = Vec(-mm2px(2.3-2.3/2.0),mm2px(13-2.8));
		background->svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/counterSwitchShort.svg"));
		background->wrap();
		background->box.pos = Vec(0, 0);
		box.size = background->box.size;
		handle->svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/counterSwitchPotHandle.svg"));
		handle->wrap();
	}

	void randomize() override { paramQuantity->setValue(roundf(random::uniform() * paramQuantity->getMaxValue())); }

};

template<class T> struct SeqMenuItem : ui::MenuItem
{
public:
	SeqMenuItem(const char *title, T *pW, int act)
	{
		text = title;
		widget = pW;
		action = act;
	};

	void onAction(const event::Action &e) override { widget->onMenu(action); };

private:
	T *widget;
	int action;
};

class SequencerWidget : public ModuleWidget
{
public:
	void std_randomize(int first_index, int last_index)
	{
		for(int k = first_index; k < last_index; k++)
		{
			int index = getParamIndex(k);
			if(index >= 0)
			{
				params[index]->randomize();
			}
		}
	}

protected:
	SequencerWidget() : ModuleWidget() 	
	{
	}
	
	int getParamIndex(int index)
	{
		auto it = std::find_if(params.begin(), params.end(), [&index](const ParamWidget *m) -> bool { return m->paramQuantity->paramId == index; });
		if(it != params.end())
			return std::distance(params.begin(), it);
		return -1;
	}

	void appendContextMenu(ui::Menu *menu) override
	{
		menu->addChild(createMenuLabel("*** A la carte ***"));
		addContextMenu(menu);
	}

	virtual Menu *addContextMenu(Menu *menu) { return menu; }
};

#if defined(LAUNCHPAD) || defined(OSC_ENABLE)
struct DigitalLed : SvgWidget
{
	float *value;
	std::vector<std::shared_ptr<Svg>> frames;

	DigitalLed(int x, int y, float *pVal)
	{
		frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/digitalLed_off.svg")));
		frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/digitalLed_on.svg")));
		setSvg(frames[0]);
		wrap();
		box.pos = Vec(x, y);
		value = pVal;
	}

	void draw(const DrawArgs &args) override
	{
		int index = (*value > 0) ? 1 : 0;
		setSvg(frames[index]);
		SvgWidget::draw(args);
	}
};
#endif

struct SigDisplayWidget : TransparentWidget
{
private:
	int digits;
	int precision;
	std::shared_ptr<Font> font;

public:
	float *value;
	SigDisplayWidget(int digit, int precis = 0)
	{
		value = NULL;
		digits = digit;
		precision = precis;
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
	};

	void draw(const DrawArgs &args) override
	{
		if(value != NULL)
		{
			// Background
			NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
			NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
			nvgFillColor(args.vg, backgroundColor);
			nvgFill(args.vg);
			nvgStrokeWidth(args.vg, 1.0);
			nvgStrokeColor(args.vg, borderColor);
			nvgStroke(args.vg);
			// text
			nvgFontSize(args.vg, 18);
			nvgFontFaceId(args.vg, font->handle);
			nvgTextLetterSpacing(args.vg, 2.5);

			std::stringstream to_display;
			if(precision == 0)
				to_display << std::setw(digits) << std::round(*value);
			else
				to_display << std::fixed << std::setw(digits) << std::setprecision(precision) << *value;

			Vec textPos = Vec(3, 17);
			
			NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
			nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
			//nvgText(args.vg, textPos.x, textPos.y, "~~", NULL);

			textColor = nvgRGB(0xda, 0xe9, 0x29);
			nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
			//nvgText(args.vg, textPos.x, textPos.y, "\\\\", NULL);
			
			textColor = nvgRGB(0xf0, 0x00, 0x00);
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
		}
	}
};

struct TIMER
{
	float Reset()
	{
		prevTime = clock();
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
		clock_t curTime = clock();
		clock_t deltaTime = curTime - prevTime;
		prevTime = curTime;
		float t = float(deltaTime) / CLOCKS_PER_SEC;
		totalPulseTime += t;
		stopwatch += t;
		return t;
	}

private:
	clock_t prevTime;
	float totalPulseTime;
	float stopwatch;
};

struct XorPanel : SvgPanel
{
	struct bgGradient : TransparentWidget
	{
		bgGradient(const Vec &size)
		{
			box.pos = Vec(0, 0);
			box.size = size;
		}

		void draw(const Widget::DrawArgs &args) override
		{
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
			nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, 0, box.size.y /*/ 2*/, nvgRGBAf(.51f, 0.51f, 0.51f, 0.35f), nvgRGBAf(0.2f, 0.2f, 0.2f, 0.1f)));
			nvgFill(args.vg);
		}
	};

	XorPanel(ModuleWidget *pWidget, int units, const char *svg) : SvgPanel()
	{
		pWidget->box.size = box.size = Vec(units * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
		if (svg != NULL)
			setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, svg)));
		addChild(new bgGradient(box.size));
	}

	void AddScrews(ModuleWidget *pWidget)
	{
		pWidget->addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		pWidget->addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		pWidget->addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		pWidget->addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}
};

#define CREATE_PANEL(modul, widg,unit,svg)  { \
	setModule(modul); \
	XorPanel *panel = new XorPanel(widg, unit, svg); \
	addChild(panel); \
	panel->AddScrews(widg); \
}

inline float px2mm(float px) {return px * (MM_PER_IN / SVG_DPI ); }
inline float yncscape(float y, float height) {	return RACK_GRID_HEIGHT - mm2px(y + height);}