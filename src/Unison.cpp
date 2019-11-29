
#include "Unison.hpp"

void Unison::modulate() {
	_channels = clamp((int)params[CHANNELS_PARAM].getValue(), 1, 16);

	_cents = clamp(params[DETUNE_PARAM].getValue(), 0.0f, maxDetuneCents);
	if (inputs[DETUNE_INPUT].isConnected()) {
		_cents *= clamp(inputs[DETUNE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	_cents /= 100.0f;
}

void Unison::processChannel(const ProcessArgs& args, int c) {
	assert(c == 0);

	float pitch = inputs[PITCH_INPUT].getVoltage();
	float gate = inputs[GATE_INPUT].getVoltage();
	outputs[PITCH_OUTPUT].setChannels(_channels);
	outputs[GATE_OUTPUT].setChannels(_channels);

	if (_cents < 0.001f) {
		for (int c = 0; c < _channels; ++c) {
			outputs[PITCH_OUTPUT].setVoltage(pitch, c);
			outputs[GATE_OUTPUT].setVoltage(gate, c);
		}
	}
	else {
		int offset = 0;
		if (_channels % 2 == 1) {
			offset = 1;
			outputs[PITCH_OUTPUT].setVoltage(pitch, 0);
			outputs[GATE_OUTPUT].setVoltage(gate, 0);
		}
		float divisions = 1.0f / ((_channels - offset) / 2);
		for (int c = 0; c < _channels - offset; ++c) {
			float cents = _cents * divisions * (float)(1 + c / 2);
			float p = pitch + (c % 2 == 0 ? 1.0f : -1.0f) * cents * (1.0f / 12.0f);
			outputs[PITCH_OUTPUT].setVoltage(p, c + offset);
			outputs[GATE_OUTPUT].setVoltage(gate, c + offset);
		}
	}
}

struct UnisonWidget : ModuleWidget {
	static constexpr int hp = 3;

	UnisonWidget(Unison* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Unison.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto channelsParamPosition = Vec(9.5, 34.0);
		auto detuneParamPosition = Vec(9.5, 89.0);

		auto detuneInputPosition = Vec(10.5, 127.0);
		auto pitchInputPosition = Vec(10.5, 174.0);
		auto gateInputPosition = Vec(10.5, 209.0);

		auto pitchOutputPosition = Vec(10.5, 247.0);
		auto gateOutputPosition = Vec(10.5, 282.0);
		// end generated by svg_widgets.rb

		{
			auto w = createParam<Knob26>(channelsParamPosition, module, Unison::CHANNELS_PARAM);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}
		addParam(createParam<Knob26>(detuneParamPosition, module, Unison::DETUNE_PARAM));

		addInput(createInput<Port24>(detuneInputPosition, module, Unison::DETUNE_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, Unison::PITCH_INPUT));
		addInput(createInput<Port24>(gateInputPosition, module, Unison::GATE_INPUT));

		addOutput(createOutput<Port24>(pitchOutputPosition, module, Unison::PITCH_OUTPUT));
		addOutput(createOutput<Port24>(gateOutputPosition, module, Unison::GATE_OUTPUT));
	}
};

Model* modelUnison = createModel<Unison, UnisonWidget>("Bogaudio-Unison", "UNISON", "Poly unison voicing utility with detune", "Utility", "Polyphonic");
