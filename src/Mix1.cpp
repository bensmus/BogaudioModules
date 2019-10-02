
#include "Mix1.hpp"

void Mix1::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->setSampleRate(sr);
	}
}

bool Mix1::active() {
	return outputs[OUT_OUTPUT].isConnected();
}

int Mix1::channels() {
	return inputs[IN_INPUT].getChannels();
}

void Mix1::addEngine(int c) {
	_engines[c] = new MixerChannel(
		params[LEVEL_PARAM],
		params[LEVEL_PARAM], // not used
		params[MUTE_PARAM],
		inputs[LEVEL_INPUT],
		inputs[LEVEL_INPUT], // not used
		1000.0f,
		&inputs[MUTE_INPUT]
	);
	_engines[c]->setSampleRate(APP->engine->getSampleRate());
}

void Mix1::removeEngine(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void Mix1::always(const ProcessArgs& args) {
	_rmsSum = 0.0f;
}

void Mix1::processChannel(const ProcessArgs& args, int c) {
	MixerChannel& e = *_engines[c];
	e.next(inputs[IN_INPUT].getVoltage(c), false, false, c);
	_rmsSum += e.rms;
	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(e.out, c);
}

void Mix1::postProcess(const ProcessArgs& args) {
	_rms = _rmsSum / (float)_channels;
}

struct Mix1Widget : ModuleWidget {
	static constexpr int hp = 3;

	Mix1Widget(Mix1* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Mix1.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto levelParamPosition = Vec(13.5, 18.0);
		auto muteParamPosition = Vec(13.2, 175.7);

		auto muteInputPosition = Vec(10.5, 198.0);
		auto levelInputPosition = Vec(10.5, 233.0);
		auto inInputPosition = Vec(10.5, 268.0);

		auto outOutputPosition = Vec(10.5, 306.0);
		// end generated by svg_widgets.rb

		{
			auto slider = createParam<VUSlider151>(levelParamPosition, module, Mix1::LEVEL_PARAM);
			if (module) {
				dynamic_cast<VUSlider*>(slider)->setVULevel(&module->_rms);
			}
			addParam(slider);
		}
		addParam(createParam<MuteButton>(muteParamPosition, module, Mix1::MUTE_PARAM));

		addInput(createInput<Port24>(muteInputPosition, module, Mix1::MUTE_INPUT));
		addInput(createInput<Port24>(levelInputPosition, module, Mix1::LEVEL_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, Mix1::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, Mix1::OUT_OUTPUT));
	}
};

Model* modelMix1 = bogaudio::createModel<Mix1, Mix1Widget>("Bogaudio-Mix1", "MIX1", "fader/amplifier with mute", "VCA");
