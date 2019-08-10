#pragma once
#include "common.hpp"

#include <sstream>
#include <iomanip>

#include <algorithm>

#define NUM_SEQUENCERS (5)
#define TOTAL_STEPS (32)
#include "sprlnSequencer.hpp"


////////////////////
// module widgets
////////////////////

struct SpiraloneWidget : SequencerWidget
{
public:
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH = 0x01,
		RANDOMIZE_LEN = 0x02,
		RANDOMIZE_STRIDE = 0x04,
		RANDOMIZE_XPOSE = 0x08,
		RANDOMIZE_MODE = 0x10,
		RANDOMIZE_LAQUALUNQUE = 0x20
	};

	SpiraloneWidget(Spiralone *module);
	Menu *addContextMenu(Menu *menu) override;
	void onMenu(int action);

	struct RandomizeSubItemItem : MenuItem {
		RandomizeSubItemItem(Module *spir, const char *title, int action);
	
		int randomizeDest;
		Spiralone *spiro;
		void onAction(const event::Action &e) override;
	};

	struct RandomizeItem : ui::MenuItem
	{
	public:
		RandomizeItem(Module *sp)
		{
			spiro = sp;
			text = "Force the hand of chance";
			rightText = RIGHT_ARROW;
		};
		Menu *createChildMenu() override
		{
			Menu *sub_menu = new Menu;
			sub_menu->addChild(new RandomizeSubItemItem(spiro, "Ov Pitch", RANDOMIZE_PITCH));
			sub_menu->addChild(new RandomizeSubItemItem(spiro, "Ov Length", RANDOMIZE_LEN));
			sub_menu->addChild(new RandomizeSubItemItem(spiro, "Ov Stride", RANDOMIZE_STRIDE));
			sub_menu->addChild(new RandomizeSubItemItem(spiro, "Ov Transpose", RANDOMIZE_XPOSE));
			sub_menu->addChild(new RandomizeSubItemItem(spiro, "Ov Mode", RANDOMIZE_MODE));
			sub_menu->addChild(new RandomizeSubItemItem(spiro, "Ov Power", RANDOMIZE_LAQUALUNQUE));
			return sub_menu;
		}

	private:
		Module *spiro;
	};
private:
	void createSequencer(int seq);
	ModuleLightWidget *createLed(int seq, Vec pos, Module *module, int firstLightId, bool big = false);
	NVGcolor color[NUM_SEQUENCERS];
};

