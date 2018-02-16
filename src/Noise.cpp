
#include "bogaudio.hpp"
#include "dsp/noise.hpp"

using namespace bogaudio::dsp;

struct Noise : Module {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		ABS_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		WHITE_OUTPUT,
		PINK_OUTPUT,
		RED_OUTPUT,
		GAUSS_OUTPUT,
		ABS_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	WhiteNoiseGenerator _white;
	PinkNoiseGenerator _pink;
	RedNoiseGenerator _red;
	GaussianNoiseGenerator _gauss;

	Noise() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}

	virtual void step() override;
};

void Noise::step() {
	if (outputs[WHITE_OUTPUT].active) {
		outputs[WHITE_OUTPUT].value = _white.next() * 10.0;
	}
	if (outputs[PINK_OUTPUT].active) {
		outputs[PINK_OUTPUT].value = _pink.next() * 10.0;
	}
	if (outputs[RED_OUTPUT].active) {
		outputs[RED_OUTPUT].value = _red.next() * 10.0;
	}
	if (outputs[GAUSS_OUTPUT].active) {
		outputs[GAUSS_OUTPUT].value = _gauss.next();
	}

	float in = 0.0;
	if (inputs[ABS_INPUT].active) {
		in = inputs[ABS_INPUT].value;
		if (in < 0.0) {
			in = -in;
		}
	}
	outputs[ABS_OUTPUT].value = in;
}


struct NoiseWidget : ModuleWidget {
	NoiseWidget(Noise* module);
};

NoiseWidget::NoiseWidget(Noise* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Noise.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

	// generated by svg_widgets.rb
	auto absInputPosition = Vec(10.5, 245.0);

	auto whiteOutputPosition = Vec(10.5, 24.0);
	auto pinkOutputPosition = Vec(10.5, 65.0);
	auto redOutputPosition = Vec(10.5, 106.0);
	auto gaussOutputPosition = Vec(10.5, 147.0);
	auto absOutputPosition = Vec(10.5, 283.0);
	// end generated by svg_widgets.rb

	addInput(Port::create<Port24>(absInputPosition, Port::INPUT, module, Noise::ABS_INPUT));

	addOutput(Port::create<Port24>(whiteOutputPosition, Port::OUTPUT, module, Noise::WHITE_OUTPUT));
	addOutput(Port::create<Port24>(pinkOutputPosition, Port::OUTPUT, module, Noise::PINK_OUTPUT));
	addOutput(Port::create<Port24>(redOutputPosition, Port::OUTPUT, module, Noise::RED_OUTPUT));
	addOutput(Port::create<Port24>(gaussOutputPosition, Port::OUTPUT, module, Noise::GAUSS_OUTPUT));
	addOutput(Port::create<Port24>(absOutputPosition, Port::OUTPUT, module, Noise::ABS_OUTPUT));
}


Model* modelNoise = Model::create<Noise, NoiseWidget>("Bogaudio", "Bogaudio-Noise", "Noise", NOISE_TAG, UTILITY_TAG);
