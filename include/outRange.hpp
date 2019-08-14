#pragma once
struct portSmall : _ioPort
{
	portSmall() : _ioPort("res/PJ301Bsmall.svg") {}
};

struct daviesVerySmall : _davies1900base
{
	daviesVerySmall() : _davies1900base("res/Davies1900hBlackVerySmall.svg") {}
};

struct outputRange
{
public:
	static const int NUMSLOTS = 2;
	void Create(ModuleWidget *pWidget, float x, float y, int port, int param)
	{
		portID = port;
		paramID = param;
		module = pWidget->module;
		pWidget->addInput(createInput<portSmall>(Vec(mm2px(x + 3.761f),  yncscape(y , 5.885f+0.595f)), module, portID));
		pWidget->addInput(createInput<portSmall>(Vec(mm2px(x + 11.495f), yncscape(y , 5.885f+0.595f)), module, portID + 1));

		ParamWidget *pwdg = createParam<daviesVerySmall>(Vec(mm2px(x + 0.407f), yncscape(y , 6.480f+6.f)), module, paramID);
		pWidget->addParam(pwdg);
		#ifdef OSCTEST_MODULE
		if(module != NULL)
			module->oscDrv->Add(new oscControl("RangeMin"), pwdg);
		#endif	
		pwdg = createParam<daviesVerySmall>(Vec(mm2px(x + 14.734f), yncscape(y, 6.480f + 6.f)), module, paramID + 1);
		pWidget->addParam(pwdg);
		#ifdef OSCTEST_MODULE
		if(module != NULL)
			module->oscDrv->Add(new oscControl("RangeMax"), pwdg);
		#endif
		configure(module);
	}

	float Value(float v) //v normalizzato 0-1
	{
		if(module != NULL)
		{
			float vmin = clamp(module->params[paramID].value + module->inputs[portID].getNormalVoltage(0.0), LVL_MIN, LVL_MAX);
			float vmax = clamp(module->params[paramID + 1].value + module->inputs[portID + 1].getNormalVoltage(0.0), LVL_MIN, LVL_MAX);
			return clamp(rescale(v, 0.0, 1.0, std::min(vmin, vmax), std::max(vmin, vmax)), LVL_MIN, LVL_MAX);
		}
		return 0;
	}

private:
	int portID;
	int paramID;
	Module *module = NULL;

	void configure(Module *module)
	{
		if(module != NULL)
		{
			module->configParam(paramID, LVL_MIN, LVL_MAX, 0.0, "Output Range: Min Voltage", "V");
			module->configParam(paramID + 1, LVL_MIN, LVL_MAX, LVL_MAX / 2.f, "Output Range: Max Voltage", "V");
		}
	}
};