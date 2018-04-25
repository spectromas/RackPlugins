#include "common.hpp"
#include "burst.hpp"

void Burst::on_loaded()
{
	load();
}

void Burst::load()
{
	trigger_pending = false;
	all_off();
}

void Burst::all_off()
{
	for(int k = 0; k < NUM_BURST_PORTS; k++)
		port(k, false);
	active = false;
}

void Burst::step()
{
	if(resetTrigger.process(inputs[RESET].value))
	{
		reset();
	} else
	{
		if(!active && !trigger_pending)
		{
			float tv = inputs[TRIGGER_THRESH_IN].active && inputs[TRIGGER_THRESH_IN].value > params[TRIG_THRESH].value ? 1.0 : 0.0;
			trigger_pending = trigger.process(params[TRIGGER].value + tv);
		}

		int clk = clock.process(inputs[CLOCK_IN].value); // 1=rise, -1=fall

		if(clk == 1)
		{
			if(!active && trigger_pending)
			{
				prepare_step();
			}

			if(active)
				next_step();
		} else if(active && clk == -1)
			end_step();
	}
}

void Burst::prepare_step()
{
	activating_params.first_cycle = true;
	activating_params.cycle_counter = activating_params.out_span = 0;
	activating_params.max_span = getInt(OUT_SPAN, OUT_SPAN_IN, 1, NUM_BURST_PORTS);
	activating_params.mode = (enum Burst::MODE)roundf(params[MODE].value);
	activating_params.invert_mode = roundf(params[MODE_INVERT].value) > 0.5;
	activating_params.retrogade = false;	
	activating_params.max_cycle = getInt(EVENT_COUNT, EVENT_COUNT_IN, 0, 23) + 1;
	trigger_pending = false;
	active = true;
}

void Burst::end_step()
{
	if(activating_params.cycle_counter >= activating_params.max_cycle)
	{
		all_off();	// ciclo terminato
	} else
	{
		switch(activating_params.mode)
		{
			case Burst::RAND:
			{
				port(activating_params.out_span, false);
			}
			break;

			case Burst::PEND:
			case Burst::FWD:
			{				
				if(!activating_params.invert_mode)
				{
					port(activating_params.out_span, false);
				}					
			}
			break;
		}
	}
}

void Burst::next_step()
{
	switch(activating_params.mode)
	{
		case Burst::RAND:
		{
			activating_params.out_span = int(rescale(randomUniform(), 0.0, 1.0, 0.0, activating_params.max_span));
			port(activating_params.out_span, true);
			activating_params.cycle_counter++;
		}
		break;

		case Burst::FWD:
		{
			if(activating_params.first_cycle)
				activating_params.first_cycle = false;
			else
			{
				if(++activating_params.out_span >= activating_params.max_span)
				{
					activating_params.out_span = 0;
					activating_params.cycle_counter++;
				}
			}
			if(activating_params.cycle_counter < activating_params.max_cycle)
			{
				if(activating_params.invert_mode)
					invert_port(activating_params.out_span);
				else
				{
					port(activating_params.out_span, true);
				}
			}
		}
		break;
			
		case Burst::PEND:
		{
			if(activating_params.first_cycle)
				activating_params.first_cycle = false;
			else
			{
				if(activating_params.retrogade)
				{
					if(--activating_params.out_span < 0)
					{
						if(activating_params.max_span > 0)
							activating_params.out_span = activating_params.invert_mode ? 0 : 1;
						else
							activating_params.out_span = 0;
						activating_params.retrogade = false;
						activating_params.cycle_counter++;
					}
				} else
				{
					if(++activating_params.out_span >= activating_params.max_span)
					{
						if(activating_params.max_span > 1)
							activating_params.out_span = activating_params.invert_mode ? activating_params.out_span-1 : activating_params.out_span - 2;
						else
							activating_params.out_span = 0;
						activating_params.retrogade = true;
						activating_params.cycle_counter++;
					}
				}
			}
			if(activating_params.cycle_counter < activating_params.max_cycle)
			{
				if(activating_params.invert_mode)
					invert_port(activating_params.out_span);
				else
					port(activating_params.out_span, true);
			}
		}
		break;
	}
}

int Burst::getInt(ParamIds p_id, InputIds i_id, float minValue, float maxValue)
{
	float offs = inputs[i_id].active ? rescale(inputs[i_id].value, 0.0, 5.0, minValue, maxValue) : 0.0;
	return (int)clamp(offs + params[p_id].value, minValue, maxValue);
}

BurstWidget::BurstWidget(Burst *module) : SequencerWidget(module)
{
	box.size = Vec(16 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Burst.svg")));		
		addChild(panel);
	}
	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));

	int pos_x = 20;
	int pos_y = 60;
	addInput(Port::create<PJ301RPort>(Vec(pos_x, pos_y-1), Port::INPUT, module, Burst::CLOCK_IN));
	addParam(ParamWidget::create<CKSSThree>(Vec(pos_x + 96, pos_y-4), module, Burst::MODE, 0.0, 2.0, 0.0));
	addParam(ParamWidget::create<CKSS>(Vec(pos_x +168, pos_y), module, Burst::MODE_INVERT, 0.0, 1.0, 0.0));
	
	pos_y += 65;
	addParam(ParamWidget::create<Rogan1PSWhiteSnapped>(Vec(pos_x + 40, pos_y), module, Burst::OUT_SPAN, 1.0, NUM_BURST_PORTS, 1.0));
	addInput(Port::create<PJ301GPort>(Vec(pos_x, pos_y+7), Port::INPUT, module, Burst::OUT_SPAN_IN));
	addParam(ParamWidget::create<Rogan1PSWhiteSnapped>(Vec(box.size.x -104, pos_y), module, Burst::EVENT_COUNT, 0.0, 23.0, 0.0));
	addInput(Port::create<PJ301GPort>(Vec(box.size.x - 48, pos_y + 7), Port::INPUT, module, Burst::EVENT_COUNT_IN));

	pos_y += 90;
	addInput(Port::create<PJ301GPort>(Vec(pos_x, pos_y + 7), Port::INPUT, module, Burst::TRIGGER_THRESH_IN));
	addParam(ParamWidget::create<Rogan1PSGreen>(Vec(pos_x + 40, pos_y), module, Burst::TRIG_THRESH, LVL_OFF, LVL_ON, LVL_OFF));
	addInput(Port::create<PJ301YPort>(Vec(pos_x+110, pos_y+7), Port::INPUT, module, Burst::RESET));
	addParam(ParamWidget::create<BefacoPush>(Vec(pos_x+168, pos_y+4), module, Burst::TRIGGER, 0.0, 1.0, 0.0));

	pos_y += 75;
	pos_x = 20;
	int dist_h = (box.size.x-20) / NUM_BURST_PORTS;
	for(int k = 0; k < NUM_BURST_PORTS; k++)
	{
		int x = pos_x + k * dist_h;
		if(k < NUM_BURST_PORTS / 2)
		{
			addOutput(Port::create<PJ301MPort>(Vec(x, pos_y), Port::OUTPUT, module, Burst::OUT_1+k));
			addChild(ModuleLightWidget::create<LargeLight<RedLight>>(Vec(4+x, pos_y+48), module, Burst::LEDOUT_1 + k));
		} else
		{
			addOutput(Port::create<PJ301MPort>(Vec(x, pos_y+44), Port::OUTPUT, module, Burst::OUT_1 + k));
			addChild(ModuleLightWidget::create<LargeLight<RedLight>>(Vec(4+x, pos_y+4), module, Burst::LEDOUT_1 + k));
		}
	}
}
