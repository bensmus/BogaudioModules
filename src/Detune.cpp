
#include "Detune.hpp"

void Detune::step() {
	if (!(outputs[OUT_PLUS_OUTPUT].active || outputs[OUT_MINUS_OUTPUT].active || outputs[THRU_OUTPUT].active)) {
		return;
	}

	float cents = params[CENTS_PARAM].value;
	if (inputs[CV_INPUT].active) {
		cents *= clamp(inputs[CV_INPUT].value / 10.0f, 0.0f, 1.0f);
		cents = roundf(cents);
	}
	cents /= 100.0f;

	float inCV = 0.0f;
	if (inputs[IN_INPUT].active) {
		inCV = inputs[IN_INPUT].value;
	}

	if (_cents != cents || _inCV != inCV) {
		_cents = cents;
		_inCV = inCV;
		if (_cents < 0.001f) {
			_plusCV = _inCV;
			_minusCV = _inCV;
		}
		else {
			float semitone = cvToSemitone(_inCV);
			_plusCV = semitoneToCV(semitone + cents);
			_minusCV = semitoneToCV(semitone - cents);
		}
	}

	outputs[THRU_OUTPUT].value = _inCV;
	outputs[OUT_PLUS_OUTPUT].value = _plusCV;
	outputs[OUT_MINUS_OUTPUT].value = _minusCV;
}

struct DetuneWidget : ModuleWidget {
	static constexpr int hp = 3;

	DetuneWidget(Detune* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(pluginInstance, "res/Detune.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto centsParamPosition = Vec(9.5, 32.5);

		auto cvInputPosition = Vec(10.5, 77.0);
		auto inInputPosition = Vec(10.5, 126.0);

		auto thruOutputPosition = Vec(10.5, 164.0);
		auto outPlusOutputPosition = Vec(10.5, 200.0);
		auto outMinusOutputPosition = Vec(10.5, 236.0);
		// end generated by svg_widgets.rb

		{
			auto w = ParamWidget::create<Knob26>(centsParamPosition, module, Detune::CENTS_PARAM, 0.0, 50.0, 0.0);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}

		addInput(Port::create<Port24>(cvInputPosition, Port::INPUT, module, Detune::CV_INPUT));
		addInput(Port::create<Port24>(inInputPosition, Port::INPUT, module, Detune::IN_INPUT));

		addOutput(Port::create<Port24>(thruOutputPosition, Port::OUTPUT, module, Detune::THRU_OUTPUT));
		addOutput(Port::create<Port24>(outPlusOutputPosition, Port::OUTPUT, module, Detune::OUT_PLUS_OUTPUT));
		addOutput(Port::create<Port24>(outMinusOutputPosition, Port::OUTPUT, module, Detune::OUT_MINUS_OUTPUT));
	}
};

Model* modelDetune = bogaudio::createModel<Detune, DetuneWidget>("Bogaudio-Detune", "Detune",  "pitch CV processor", TUNER_TAG);
