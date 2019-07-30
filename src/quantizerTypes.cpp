#include "common.hpp"
#include "quantizer.hpp"

struct scaleRootChoiceItem : ui::MenuItem 
{
	Quantizer *pquantizer;
	int root_n;
	scaleRootChoiceItem(Quantizer *p, int n)
	{
		pquantizer = p;
		root_n = n;
		text = QuantizerRootNotes.at(root_n);
		rightText = CHECKMARK(root_n == pquantizer->getRoot());
	}
	void onAction(const event::Action &e) override 
	{
		pquantizer->SetRoot(root_n);
	}
};

struct scaleRootChoice : LedDisplayChoice 
{
	Quantizer *pquantizer;
	void onAction(const event::Action &e) override 
	{
		if (pquantizer != NULL)
		{
			ui::Menu *menu = createMenu();
			menu->addChild(createMenuLabel("Note Root"));
			for (int k=0; k<int(QuantizerRootNotes.size()); k++)
			{
				menu->addChild(new scaleRootChoiceItem(pquantizer, k));
			}
		}
	}

	void step() override 
	{
		int n = pquantizer->getRoot();
		text = n >= 0 ? QuantizerRootNotes.at(n) : "(No root)";
		color.a = n >= 0 ? 1.0 : 0.5f;
	}
};

struct scaleChoiceItem : ui::MenuItem 
{
	Quantizer *pquantizer;
	int scale_n;
	scaleChoiceItem(Quantizer *p, int n,const std::vector<QScale> &scaleList)
	{
		pquantizer = p;
		scale_n = n;
		text = scaleList.at(scale_n).name;
		rightText = CHECKMARK(scale_n == pquantizer->getScale());
	}
	void onAction(const event::Action &e) override 
	{
		pquantizer->SetScale(scale_n);
	}
};

struct ScaleChoice : LedDisplayChoice 
{
	Quantizer *pquantizer;
	void onAction(const event::Action &e) override 
	{
		if (pquantizer != NULL)
		{
			ui::Menu *menu = createMenu();
			menu->addChild(createMenuLabel("Scale"));
			const std::vector<QScale> &scaleList = pquantizer->getAvailableScales();
			for (int k=0; k<int(scaleList.size()); k++)
			{
				menu->addChild(new scaleChoiceItem(pquantizer, k, scaleList));
			}
		}
	}
	
	void step() override 
	{
		int n = pquantizer->getScale();
		text = n >= 0 ? pquantizer->getAvailableScales().at(n).name : "(No scale)";
		color.a = n >= 0 ? 1.0 : 0.5f;
	}
};

qtzrDisplay::qtzrDisplay()
{
	scaleChoice = NULL;
	rootChoice = NULL;
	separator = NULL;
}

void qtzrDisplay::CreateInterface(Quantizer *module)
{
	clearChildren();

	math::Vec pos;
	scaleChoice = createWidget<ScaleChoice>(pos);
	scaleChoice->box.size.x = box.size.x;
	scaleChoice->pquantizer = module;
	addChild(scaleChoice);
	pos = scaleChoice->box.getBottomLeft();

	separator = createWidget<LedDisplaySeparator>(pos);
	separator->box.size.x = box.size.x;
	addChild(separator);

	rootChoice = createWidget<scaleRootChoice>(pos);
	rootChoice->box.size.x = box.size.x;
	rootChoice->pquantizer = module;
	addChild(rootChoice);
	scaleChoice->color = rootChoice->color = nvgRGB(0xff, 0xff, 0xff);
}
