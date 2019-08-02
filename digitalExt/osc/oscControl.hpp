#pragma once
#ifdef OSC_ENABLE

struct OSCDriver;
struct oscControl
{
public:
	virtual ~oscControl() {};
	void Draw(OSCDriver *drv, bool force = false);

	bool isDirty() { return m_dirty; }
	bool Intersect(std::string address) 
	{ 
		return m_address.compare(address) == 0; 
	}
	void ChangeFromGUI(OSCDriver *drv);  // gui updated: the new value is already in the binded parameter
	void onOscMsg(Module *pModule, OSCMsg msg) 
	{
		setValue(pModule, msg.value); 		
	}
	bool DetectGUIChanges() { return getValue() != m_lastDrawnValue; }

	int ID() { return is_light ? pBindedLight->firstLightId : pBindedParam->paramQuantity->paramId; }
	void bindWidget(ModuleLightWidget *p) { pBindedLight = p; is_light = true; }
	void bindWidget(ParamWidget *p) { pBindedParam = p; }

	oscControl(std::string address)
	{
		m_address = address;
		is_light = false;
		pBindedLight = NULL;
		pBindedParam = NULL;
		m_dirty = true;
		m_lastDrawnValue = -10202020;
	}

private:
	float getValue() { return is_light ? pBindedLight->module->lights[pBindedLight->firstLightId].getBrightness() : pBindedParam->paramQuantity->getValue(); }	
	void setValue(Module *pModule, float v)
	{		
		if(is_light)
			pBindedLight->module->lights[pBindedLight->firstLightId].value = v;
		else
		{
			v = rescale(v, 0.0, 1.0, pBindedParam->paramQuantity->getMinValue(), pBindedParam->paramQuantity->getMaxValue());

			SvgKnob *pk = (SvgKnob *)dynamic_cast<SvgKnob *>(pBindedParam);
			if(pk != NULL)
			{
				pModule->params[pBindedParam->paramQuantity->paramId].value = v;
				pBindedParam->paramQuantity->setValue(v);
				pk->dirtyValue = 1+v;
			} else
			{
				SvgSlider *pk1 = (SvgSlider *)dynamic_cast<SvgSlider *>(pBindedParam);
				if(pk1 != NULL)
				{
					pModule->params[pBindedParam->paramQuantity->paramId].setValue(v);
					pBindedParam->paramQuantity->setValue(v);
					pk1->dirtyValue = 1+v;
				} else
					pBindedParam->paramQuantity->setValue(v);
			}
		}
		m_lastDrawnValue = v;
		m_dirty = false;
	}
	std::string m_address;
	ModuleLightWidget *pBindedLight;
	ParamWidget *pBindedParam;
	bool m_dirty;
	float m_lastDrawnValue;
	bool is_light;
};

#endif // OSC
