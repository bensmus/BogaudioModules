
#include "PEQ14XF.hpp"

void PEQ14XF::addChannel(int c) {
	_engines[c] = new Engine();
}

void PEQ14XF::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void PEQ14XF::modulateChannel(int c) {
	Engine& e = *_engines[c];

	float sr = APP->engine->getSampleRate();
	float response = sensitivity(params[DAMP_PARAM], &inputs[DAMP_INPUT], c);
	if (e.response != response) {
		e.response = response;
		for (int i = 0; i < 14; ++i) {
			e.efs[i].setParams(sr, e.response);
		}
	}

	e.gain.setLevel(gain(params[GAIN_PARAM], &inputs[GAIN_INPUT], c));
}

void PEQ14XF::processAll(const ProcessArgs& args) {
	for (int i = 0; i < 14; ++i) {
		outputs[EF1_OUTPUT + i].setChannels(_channels);
	}

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

void PEQ14XF::processChannel(const ProcessArgs& args, int c) {
	if (_baseMessage && _baseMessage->valid) {
		Engine& e = *_engines[c];
		for (int i = 0; i < 14; ++i) {
			float out = e.efs[i].next(_baseMessage->outs[c][i]);
			out = scaleEF(out, _baseMessage->frequencies[c][i], _baseMessage->bandwidths[c]);
			out = e.gain.next(out);
			out = _saturator.next(out);
			outputs[EF1_OUTPUT + i].setVoltage(out, c);
		}
	}
	else {
		for (int i = 0; i < 14; ++i) {
			outputs[EF1_OUTPUT + i].setVoltage(0.0f, c);
		}
	}
}

struct PEQ14XFWidget : BGModuleWidget {
	static constexpr int hp = 5;

	PEQ14XFWidget(PEQ14XF* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "PEQ14XF");

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto dampParamPosition = Vec(12.0, 30.0);
		auto gainParamPosition = Vec(47.0, 29.5);

		auto dampInputPosition = Vec(8.0, 63.0);
		auto gainInputPosition = Vec(43.0, 62.5);

		auto ef1OutputPosition = Vec(10.5, 111.0);
		auto ef2OutputPosition = Vec(10.5, 146.0);
		auto ef3OutputPosition = Vec(10.5, 181.0);
		auto ef4OutputPosition = Vec(10.5, 216.0);
		auto ef5OutputPosition = Vec(10.5, 251.0);
		auto ef6OutputPosition = Vec(10.5, 286.0);
		auto ef7OutputPosition = Vec(10.5, 321.0);
		auto ef8OutputPosition = Vec(41.5, 111.0);
		auto ef9OutputPosition = Vec(41.5, 146.0);
		auto ef10OutputPosition = Vec(41.5, 181.0);
		auto ef11OutputPosition = Vec(41.5, 216.0);
		auto ef12OutputPosition = Vec(41.5, 251.0);
		auto ef13OutputPosition = Vec(41.5, 286.0);
		auto ef14OutputPosition = Vec(41.5, 321.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob16>(dampParamPosition, module, PEQ14XF::DAMP_PARAM));
		addParam(createParam<Knob16>(gainParamPosition, module, PEQ14XF::GAIN_PARAM));

		addInput(createInput<Port24>(dampInputPosition, module, PEQ14XF::DAMP_INPUT));
		addInput(createInput<Port24>(gainInputPosition, module, PEQ14XF::GAIN_INPUT));

		addOutput(createOutput<Port24>(ef1OutputPosition, module, PEQ14XF::EF1_OUTPUT));
		addOutput(createOutput<Port24>(ef2OutputPosition, module, PEQ14XF::EF2_OUTPUT));
		addOutput(createOutput<Port24>(ef3OutputPosition, module, PEQ14XF::EF3_OUTPUT));
		addOutput(createOutput<Port24>(ef4OutputPosition, module, PEQ14XF::EF4_OUTPUT));
		addOutput(createOutput<Port24>(ef5OutputPosition, module, PEQ14XF::EF5_OUTPUT));
		addOutput(createOutput<Port24>(ef6OutputPosition, module, PEQ14XF::EF6_OUTPUT));
		addOutput(createOutput<Port24>(ef7OutputPosition, module, PEQ14XF::EF7_OUTPUT));
		addOutput(createOutput<Port24>(ef8OutputPosition, module, PEQ14XF::EF8_OUTPUT));
		addOutput(createOutput<Port24>(ef9OutputPosition, module, PEQ14XF::EF9_OUTPUT));
		addOutput(createOutput<Port24>(ef10OutputPosition, module, PEQ14XF::EF10_OUTPUT));
		addOutput(createOutput<Port24>(ef11OutputPosition, module, PEQ14XF::EF11_OUTPUT));
		addOutput(createOutput<Port24>(ef12OutputPosition, module, PEQ14XF::EF12_OUTPUT));
		addOutput(createOutput<Port24>(ef13OutputPosition, module, PEQ14XF::EF13_OUTPUT));
		addOutput(createOutput<Port24>(ef14OutputPosition, module, PEQ14XF::EF14_OUTPUT));
	}
};

Model* modelPEQ14XF = createModel<PEQ14XF, PEQ14XFWidget>("Bogaudio-PEQ14XF", "PEQ14XF", "PEQ14 envelope followers expander", "Filter", "Expander", "Polyphonic");
