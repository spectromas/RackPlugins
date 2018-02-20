#include "OSCServer.hpp"
#ifdef OSC_ENABLE
#include "../digitalExt/osc/oscCommunicator.hpp"
#include "../digitalExt/osc/oscSkt.hpp"

struct OscServer : Module
{
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {

		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	OscServer() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	{
		on_loaded();
	}
	~OscServer()
	{
		ipc.Close();
		osc.Close();
	}
	void fromJson(json_t *root) override { Module::fromJson(root); on_loaded(); }
	json_t *toJson() override
	{
		json_t *rootJ = json_object();
		return rootJ;
	}

	void step() override;

private:
	void on_loaded();
	void load();
	void process_osc();
	void process_ipc();
	void sendToClient(OscReceivedMsg *rmsg);
	void sendToOsc(OSCMsg *msg);

	OSC osc;
	IPCServer ipc;
};

void OscServer::on_loaded()
{
	load();
}

void OscServer::load()
{
	ipc.Open();
	osc.Open("192.168.1.39");
}

void OscServer::step()
{
	process_osc();
	process_ipc();
}

void OscServer::process_ipc()
{
	for(int k = 0; k < NUM_SCENES; k++)
	{
		OSCMsg msg;
		if(ipc.Read(k, &msg))
		{
			sendToOsc(&msg);
		}
	}
}

void OscServer::process_osc()
{
	int len = osc.Read();
	if(len > 0)
	{
		if(osc.IsBundle())
		{
			OSCBundle bndl;
			osc.ParseBundle(&bndl, len);
			OscReceivedMsg osc_msg;
			while(osc_msg.GetNextMessage(&bndl))
			{
				sendToClient(&osc_msg);
			}
		} else
		{
			OscReceivedMsg osc_msg;
			osc.ParseMessage(&osc_msg, len);
			sendToClient(&osc_msg);
		}
	}
}

void OscServer::sendToOsc(OSCMsg *msg)
{
	char realAddress[80];
	sprintf(realAddress, "/scene%i/%s", msg->scene, msg->address);
	char buffer[2048]; 
	osc.Write(buffer, sizeof(buffer), realAddress, "f", msg->param.f);
}

void OscServer::sendToClient(OscReceivedMsg *rmsg)
{
	OSCMsg msg;
	const char *addr = rmsg->GetAddress();
	if(strncmp(addr, "/scene", 6) == 0 && strlen(addr) > 8)
	{
		msg.scene = addr[6];
		if(msg.scene > 0 && msg.scene <= NUM_SCENES)
		{
			addr += 8;
			strncpy(msg.address, addr, sizeof(msg.address));

			for(int k = 0; k < rmsg->NumParameters(); k++)
			{
				if(rmsg->GetNextParam(k, &msg.param))
				{
					ipc.Write(&msg);
				}
			}
		}
	}
}

OSCServerWidget::OSCServerWidget()
{
	OscServer *module = new OscServer();
	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		LightPanel *panel = new LightPanel();
		panel->box.size = box.size;
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
}
#endif
