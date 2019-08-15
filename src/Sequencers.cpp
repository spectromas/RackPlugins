#include "../include/common.hpp"
#include "../include/Klee.hpp"
#include "../include/M581.hpp"
#include "../include/Z8K.hpp"
#include "../include/Renato.hpp"
#include "../include/SpiraloneModule.hpp"
#include "../include/pwmClock.hpp"
#include "../include/quantizer.hpp"
#include "../include/burst.hpp"
#include "../include/uncert.hpp"
#include "../include/attenuator.hpp"
#include "../include/boole.hpp"
#include "../include/mplex.hpp"
#include "../include/demplex.hpp"
#include "../include/switch.hpp"
#include "../include/counter.hpp"
#include "../include/nag.hpp"
#include "../include/empty.hpp"
#include "../include/ascii.hpp"
#include "../include/quattro.hpp"
#include "../include/z8kStrip.hpp"

#ifdef LPTEST_MODULE
#include "../include/lpTestModule.hpp"
#endif 

#ifdef OSCTEST_MODULE
#include "../include/oscTestModule.hpp"
#endif 

// The pluginInstance-wide instance of the Plugin class
Plugin *pluginInstance;

void init(rack::Plugin *p)
{
	pluginInstance = p;
	
	p->pluginUrl = "https://github.com/The-XOR/VCV-Sequencers";
	p->manualUrl = "https://github.com/The-XOR/VCV-Sequencers/blob/master/README.md";

	// For each module, specify the ModuleWidget subclass, manufacturer slug (for saving in patches), manufacturer human-readable name, module slug, and module name
	p->addModel(createModel<Klee, KleeWidget>("Klee"));
	p->addModel(createModel<M581, M581Widget>("M581"));
	p->addModel(createModel<Z8K, Z8KWidget>("Z8K"));
	p->addModel(createModel<Renato, RenatoWidget>("Renato"));
	p->addModel(createModel<Spiralone, SpiraloneWidget>("Spiralone"));
	p->addModel(createModel<Burst, BurstWidget>("Burst"));
	p->addModel(createModel<Uncertain, UncertainWidget>("Uncertain"));
	p->addModel(createModel<PwmClock, PwmClockWidget>("PWMClock"));
	p->addModel(createModel<Quantizer, QuantizerWidget>("Quantizer"));
	p->addModel(createModel<Attenuator, AttenuatorWidget>("AttAmp"));
	p->addModel(createModel<XSwitch, SwitchWidget>("XSwitch"));
	p->addModel(createModel<Boole, BooleWidget>("Boole"));
	p->addModel(createModel<Mplex, MplexWidget>("Mplex"));
	p->addModel(createModel<Dmplex, DmplexWidget>("Dmplex"));
	p->addModel(createModel<Counter, CounterWidget>("Counter"));
	p->addModel(createModel<midyQuant, midyQuantWidget>("midyQuant"));
	p->addModel(createModel<nag, nagWidget>("nag"));
	p->addModel(createModel<empty, emptyWidget>("empty"));
	p->addModel(createModel<ascii, asciiWidget>("ascii"));
	p->addModel(createModel<quattro, quattroWidget>("quattro"));
	p->addModel(createModel<z8kStrip, z8kStripWidget>("z8kStrip"));

#ifdef LPTEST_MODULE
	p->addModel(createModel<LaunchpadTest, LaunchpadTestWidget>("LaunchpadTest"));
#endif

#ifdef OSCTEST_MODULE
	p->addModel(createModel<OscTest, OscTestWidget>("OSCTest"));
#endif

	// Any other pluginInstance initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
