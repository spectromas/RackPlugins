#include "empty.hpp"

struct emptyDisplay : LedDisplayChoice
{
	const empty *pModule;

	emptyDisplay() : LedDisplayChoice()
	{
		pModule = NULL;
		font = APP->window->loadFont(asset::system("res/fonts/DejaVuSans.ttf"));
	}

	void draw(const DrawArgs &args) override
	{
		enum NVGaling {
			// Horizontal align
			NVG_ALIGN_LEFT = 1 << 0,	// Default
			NVG_ALIGN_CENTER = 1 << 1,
			NVG_ALIGN_RIGHT = 1 << 2,
			// Vertical align
			NVG_ALIGN_TOP = 1 << 3,
			NVG_ALIGN_MIDDLE = 1 << 4,
			NVG_ALIGN_BOTTOM = 1 << 5,
			NVG_ALIGN_BASELINE = 1 << 6, // Default
		};

		if (pModule != NULL && font->handle >= 0) 
		{/*
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
			nvgFillColor(args.vg, nvgRGB(0xff, 0x00, 0x00));
			nvgFill(args.vg);
*/
			nvgScissor(args.vg, RECT_ARGS(args.clipBox));
			nvgFillColor(args.vg, pModule->color());
			nvgFontFaceId(args.vg, font->handle);
			nvgTextLetterSpacing(args.vg, 0.0);
			nvgTextAlign(args.vg, NVG_ALIGN_TOP | NVG_ALIGN_CENTER);
			nvgFontSize(args.vg, 15);
			nvgTextBox(args.vg, 0, 0, args.clipBox.size.x, pModule->txt(), NULL);
			nvgResetScissor(args.vg);
		}
	}
};

void empty::process(const ProcessArgs &args)
{
	if (rndTrigger.process(inputs[MOTTETTO].value) || btnTrig.process(params[MOTTBTN].value))
		getMottett();
}

emptyWidget::emptyWidget(empty *module)
{
	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/empty.svg")));
		addChild(panel);
	}
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	emptyDisplay *disp = createWidget<emptyDisplay>(mm2px(Vec(2.929, px2mm(101.743))));
	disp->box.size = mm2px(Vec(35.549, 41.743));
	disp->pModule = module;
	addChild(disp);

	addParam(createParam<XBTN>(Vec(mm2px(7.62), yncscape(97.836, 25.4)), module, empty::MOTTBTN));
	addInput(createInput<PJ301HPort>(Vec(mm2px(0.0), yncscape(0.0, 8.255)), module, empty::MOTTETTO));
}

