
#include "PEQ14XR.hpp"

void PEQ14XR::Engine::setSampleRate(float sr) {
	for (int i = 0; i < 14; ++i) {
		oscillators[i].setSampleRate(sr);
	}
}

void PEQ14XR::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->setSampleRate(sr);
	}
}

void PEQ14XR::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->setSampleRate(APP->engine->getSampleRate());
}

void PEQ14XR::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void PEQ14XR::modulateChannel(int c) {
	Engine& e = *_engines[c];

	float sr = APP->engine->getSampleRate();
	float response = sensitivity(params[DAMP_PARAM], &inputs[DAMP_INPUT], c);
	if (e.response != response) {
		e.response = response;
		for (int i = 0; i < 14; ++i) {
			e.efs[i].setParams(sr, e.response);
		}
	}

	e.efGain.setLevel(gain(params[GAIN_PARAM], &inputs[GAIN_INPUT], c));
}

void PEQ14XR::processAlways(const ProcessArgs& args) {
	outputs[OUT_OUTPUT].setChannels(_channels);

	_baseMessage = NULL;
	if (baseConnected()) {
		_baseMessage = fromBase();
	}

	if (expanderConnected()) {
		if (_baseMessage) {
			// *toExpander() = *_baseMessage;
			_baseMessage->copyTo(toExpander());
		}
		else {
			toExpander()->reset();
		}
	}
}

void PEQ14XR::processChannel(const ProcessArgs& args, int c) {
	if (_baseMessage && _baseMessage->valid) {
		Engine& e = *_engines[c];
		float out = 0.0f;
		for (int i = 0; i < 14; ++i) {
			e.oscillators[i].setFrequency(_baseMessage->frequencies[c][i]);

			float level = e.efs[i].next(_baseMessage->outs[c][i]);
			level = scaleEF(level, _baseMessage->frequencies[c][i], _baseMessage->bandwidths[c]);
			level = e.efGain.next(level);
			level *= 0.1f;
			level = std::max(0.0f, std::min(1.0f, level));
			level = 1.0f - level;
			level *= Amplifier::minDecibels;
			e.amplifiers[i].setLevel(level);

			out += e.amplifiers[i].next(e.oscillators[i].next());
		}
		outputs[OUT_OUTPUT].setVoltage(_saturator.next(out), c);
	}
	else {
		outputs[OUT_OUTPUT].setVoltage(0.0f, c);
	}
}

struct PEQ14XRWidget : BGModuleWidget {
	static constexpr int hp = 5;

	PEQ14XRWidget(PEQ14XR* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "PEQ14XR");

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto dampParamPosition = Vec(12.0, 30.0);
		auto gainParamPosition = Vec(47.0, 29.5);

		auto dampInputPosition = Vec(8.0, 63.0);
		auto gainInputPosition = Vec(43.0, 62.5);

		auto outOutputPosition = Vec(25.5, 322.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob16>(dampParamPosition, module, PEQ14XR::DAMP_PARAM));
		addParam(createParam<Knob16>(gainParamPosition, module, PEQ14XR::GAIN_PARAM));

		addInput(createInput<Port24>(dampInputPosition, module, PEQ14XR::DAMP_INPUT));
		addInput(createInput<Port24>(gainInputPosition, module, PEQ14XR::GAIN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, PEQ14XR::OUT_OUTPUT));
	}
};

Model* modelPEQ14XR = createModel<PEQ14XR, PEQ14XRWidget>("Bogaudio-PEQ14XR", "PEQ14XR", "PEQ14 resynthesizer expander", "Filter", "Vocoder", "Expander", "Polyphonic");
