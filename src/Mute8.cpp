
#include "Mute8.hpp"
#include "mixer.hpp"

#define LATCHING_CVS "latching_cvs"

const float Mute8::maxDecibels = 0.0f;
const float Mute8::minDecibels = Amplifier::minDecibels;
const float Mute8::slewTimeMS = 5.0f;

json_t* Mute8::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, LATCHING_CVS, json_boolean(_latchingCVs));
	return root;
}

void Mute8::dataFromJson(json_t* root) {
	json_t* l = json_object_get(root, LATCHING_CVS);
	if (l) {
		_latchingCVs = json_is_true(l);
	}
}

void Mute8::reset() {
	for (int i = 0; i < 8; ++i) {
		for (int c = 0; c < maxChannels; ++c) {
			_triggers[i][c].reset();
		}
	}
}

void Mute8::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	for (int i = 0; i < 8; ++i) {
		for (int c = 0; c < maxChannels; ++c) {
			_slewLimiters[i][c].setParams(sampleRate, slewTimeMS, maxDecibels - minDecibels);
		}
	}
}

void Mute8::processAll(const ProcessArgs& args) {
	bool solo = false;
	for (int i = 0; i < 8; ++i) {
		solo = solo || params[MUTE1_PARAM + i].getValue() > 1.5f;
	}
	for (int i = 0; i < 8; ++i) {
		stepChannel(i, solo);
	}
}

void Mute8::stepChannel(int i, bool solo) {
	bool allMuted = solo ? params[MUTE1_PARAM + i].getValue() < 2.0f : params[MUTE1_PARAM + i].getValue() > 0.5f;

	if (inputs[INPUT1_INPUT + i].isConnected()) {
		int n = inputs[INPUT1_INPUT + i].getChannels();
		outputs[OUTPUT1_OUTPUT + i].setChannels(n);
		int mutedCount = 0;
		for (int c = 0; c < n; ++c) {
			if (_triggers[i][c].process(inputs[MUTE1_INPUT + i].getPolyVoltage(c))) {
				_latches[i][c] = !_latches[i][c];
			}
			bool muted = allMuted || (!_latchingCVs && _triggers[i][c].isHigh()) || (_latchingCVs && _latches[i][c]);
			if (muted) {
				++mutedCount;
				_amplifiers[i][c].setLevel(_slewLimiters[i][c].next(minDecibels));
			}
			else {
				_amplifiers[i][c].setLevel(_slewLimiters[i][c].next(maxDecibels));
			}
			outputs[OUTPUT1_OUTPUT + i].setChannels(n);
			outputs[OUTPUT1_OUTPUT + i].setVoltage(_amplifiers[i][c].next(inputs[INPUT1_INPUT + i].getVoltage(c)), c);
		}
		lights[MUTE1_LIGHT + i].value = mutedCount / (float)n;
	}
	else {
		if (_triggers[i][0].process(inputs[MUTE1_INPUT + i].getVoltage())) {
			_latches[i][0] = !_latches[i][0];
		}
		bool muted = allMuted || (!_latchingCVs && _triggers[i][0].isHigh()) || (_latchingCVs && _latches[i][0]);
		if (muted) {
			lights[MUTE1_LIGHT + i].value = 1.0f;
			_amplifiers[i][0].setLevel(_slewLimiters[i][0].next(minDecibels));
		}
		else {
			lights[MUTE1_LIGHT + i].value = 0.0f;
			_amplifiers[i][0].setLevel(_slewLimiters[i][0].next(maxDecibels));
		}
		outputs[OUTPUT1_OUTPUT + i].setChannels(1);
		outputs[OUTPUT1_OUTPUT + i].setVoltage(_amplifiers[i][0].next(5.0f));
	}
}

struct Mute8Widget : ModuleWidget {
	static constexpr int hp = 10;

	Mute8Widget(Mute8* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Mute8.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(0, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto mute1ParamPosition = Vec(78.2, 40.7);
		auto mute2ParamPosition = Vec(78.2, 80.7);
		auto mute3ParamPosition = Vec(78.2, 120.7);
		auto mute4ParamPosition = Vec(78.2, 160.7);
		auto mute5ParamPosition = Vec(78.2, 200.7);
		auto mute6ParamPosition = Vec(78.2, 240.7);
		auto mute7ParamPosition = Vec(78.2, 280.7);
		auto mute8ParamPosition = Vec(78.2, 318.7);

		auto input1InputPosition = Vec(11.0, 36.0);
		auto input2InputPosition = Vec(11.0, 76.0);
		auto input3InputPosition = Vec(11.0, 116.0);
		auto input4InputPosition = Vec(11.0, 156.0);
		auto input5InputPosition = Vec(11.0, 196.0);
		auto input6InputPosition = Vec(11.0, 236.0);
		auto input7InputPosition = Vec(11.0, 276.0);
		auto input8InputPosition = Vec(11.0, 316.0);
		auto mute1InputPosition = Vec(46.0, 36.0);
		auto mute2InputPosition = Vec(46.0, 76.0);
		auto mute3InputPosition = Vec(46.0, 116.0);
		auto mute4InputPosition = Vec(46.0, 156.0);
		auto mute5InputPosition = Vec(46.0, 196.0);
		auto mute6InputPosition = Vec(46.0, 236.0);
		auto mute7InputPosition = Vec(46.0, 276.0);
		auto mute8InputPosition = Vec(46.0, 316.0);

		auto output1OutputPosition = Vec(115.0, 36.0);
		auto output2OutputPosition = Vec(115.0, 76.0);
		auto output3OutputPosition = Vec(115.0, 116.0);
		auto output4OutputPosition = Vec(115.0, 156.0);
		auto output5OutputPosition = Vec(115.0, 196.0);
		auto output6OutputPosition = Vec(115.0, 236.0);
		auto output7OutputPosition = Vec(115.0, 276.0);
		auto output8OutputPosition = Vec(115.0, 316.0);

		auto mute1LightPosition = Vec(100.5, 46.8);
		auto mute2LightPosition = Vec(100.5, 86.8);
		auto mute3LightPosition = Vec(100.5, 126.8);
		auto mute4LightPosition = Vec(100.5, 166.8);
		auto mute5LightPosition = Vec(100.5, 206.8);
		auto mute6LightPosition = Vec(100.5, 246.8);
		auto mute7LightPosition = Vec(100.5, 286.8);
		auto mute8LightPosition = Vec(100.5, 324.8);
		// end generated by svg_widgets.rb

		addParam(createParam<SoloMuteButton>(mute1ParamPosition, module, Mute8::MUTE1_PARAM));
		addParam(createParam<SoloMuteButton>(mute2ParamPosition, module, Mute8::MUTE2_PARAM));
		addParam(createParam<SoloMuteButton>(mute3ParamPosition, module, Mute8::MUTE3_PARAM));
		addParam(createParam<SoloMuteButton>(mute4ParamPosition, module, Mute8::MUTE4_PARAM));
		addParam(createParam<SoloMuteButton>(mute5ParamPosition, module, Mute8::MUTE5_PARAM));
		addParam(createParam<SoloMuteButton>(mute6ParamPosition, module, Mute8::MUTE6_PARAM));
		addParam(createParam<SoloMuteButton>(mute7ParamPosition, module, Mute8::MUTE7_PARAM));
		addParam(createParam<SoloMuteButton>(mute8ParamPosition, module, Mute8::MUTE8_PARAM));

		addInput(createInput<Port24>(input1InputPosition, module, Mute8::INPUT1_INPUT));
		addInput(createInput<Port24>(input2InputPosition, module, Mute8::INPUT2_INPUT));
		addInput(createInput<Port24>(input3InputPosition, module, Mute8::INPUT3_INPUT));
		addInput(createInput<Port24>(input4InputPosition, module, Mute8::INPUT4_INPUT));
		addInput(createInput<Port24>(input5InputPosition, module, Mute8::INPUT5_INPUT));
		addInput(createInput<Port24>(input6InputPosition, module, Mute8::INPUT6_INPUT));
		addInput(createInput<Port24>(input7InputPosition, module, Mute8::INPUT7_INPUT));
		addInput(createInput<Port24>(input8InputPosition, module, Mute8::INPUT8_INPUT));
		addInput(createInput<Port24>(mute1InputPosition, module, Mute8::MUTE1_INPUT));
		addInput(createInput<Port24>(mute2InputPosition, module, Mute8::MUTE2_INPUT));
		addInput(createInput<Port24>(mute3InputPosition, module, Mute8::MUTE3_INPUT));
		addInput(createInput<Port24>(mute4InputPosition, module, Mute8::MUTE4_INPUT));
		addInput(createInput<Port24>(mute5InputPosition, module, Mute8::MUTE5_INPUT));
		addInput(createInput<Port24>(mute6InputPosition, module, Mute8::MUTE6_INPUT));
		addInput(createInput<Port24>(mute7InputPosition, module, Mute8::MUTE7_INPUT));
		addInput(createInput<Port24>(mute8InputPosition, module, Mute8::MUTE8_INPUT));

		addOutput(createOutput<Port24>(output1OutputPosition, module, Mute8::OUTPUT1_OUTPUT));
		addOutput(createOutput<Port24>(output2OutputPosition, module, Mute8::OUTPUT2_OUTPUT));
		addOutput(createOutput<Port24>(output3OutputPosition, module, Mute8::OUTPUT3_OUTPUT));
		addOutput(createOutput<Port24>(output4OutputPosition, module, Mute8::OUTPUT4_OUTPUT));
		addOutput(createOutput<Port24>(output5OutputPosition, module, Mute8::OUTPUT5_OUTPUT));
		addOutput(createOutput<Port24>(output6OutputPosition, module, Mute8::OUTPUT6_OUTPUT));
		addOutput(createOutput<Port24>(output7OutputPosition, module, Mute8::OUTPUT7_OUTPUT));
		addOutput(createOutput<Port24>(output8OutputPosition, module, Mute8::OUTPUT8_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(mute1LightPosition, module, Mute8::MUTE1_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mute2LightPosition, module, Mute8::MUTE2_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mute3LightPosition, module, Mute8::MUTE3_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mute4LightPosition, module, Mute8::MUTE4_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mute5LightPosition, module, Mute8::MUTE5_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mute6LightPosition, module, Mute8::MUTE6_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mute7LightPosition, module, Mute8::MUTE7_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mute8LightPosition, module, Mute8::MUTE8_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		Mute8* m = dynamic_cast<Mute8*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		menu->addChild(new BoolOptionMenuItem("Latching CV triggers", [m]() { return &m->_latchingCVs; }));
	}
};

Model* modelMute8 = bogaudio::createModel<Mute8, Mute8Widget>("Bogaudio-Mute8", "MUTE8", "8 independent mutes with CV control", "Polyphonic");
