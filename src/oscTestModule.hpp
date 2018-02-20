#include "common.hpp"
#ifdef OSCTEST_MODULE
#include "dsp/digital.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *plugin;

struct OscTestWidget : ModuleWidget
{
	OscTestWidget();
};

#endif
