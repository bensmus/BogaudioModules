
#include "LLFO.hpp"

void LLFO::onReset() {
	_resetTrigger.reset();
	_modulationStep = modulationSteps;
}

void LLFO::onSampleRateChange() {
	_phasor.setSampleRate(engineGetSampleRate());
	_modulationStep = modulationSteps;
}

void LLFO::step() {
	lights[SLOW_LIGHT].value = _slowMode = params[SLOW_PARAM].value > 0.5f;

	Wave wave = (Wave)params[WAVE_PARAM].value;
	lights[SINE_LIGHT].value = wave == SINE_WAVE;
	lights[TRIANGLE_LIGHT].value = wave == TRIANGLE_WAVE;
	lights[RAMP_UP_LIGHT].value = wave == RAMP_UP_WAVE;
	lights[RAMP_DOWN_LIGHT].value = wave == RAMP_DOWN_WAVE;
	lights[SQUARE_LIGHT].value = wave == SQUARE_WAVE;
	lights[PULSE_LIGHT].value = wave == PULSE_WAVE;
	if (!outputs[OUT_OUTPUT].active) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		setFrequency(_slowMode, params[FREQUENCY_PARAM], inputs[PITCH_INPUT], _phasor);

		_invert = false;
		switch (wave) {
			case SINE_WAVE: {
				_oscillator = &_sine;
				break;
			}
			case TRIANGLE_WAVE: {
				_oscillator = &_triangle;
				break;
			}
			case RAMP_UP_WAVE: {
				_oscillator = &_ramp;
				break;
			}
			case RAMP_DOWN_WAVE: {
				_oscillator = &_ramp;
				_invert = true;
				break;
			}
			case SQUARE_WAVE: {
				_oscillator = &_square;
				_square.setPulseWidth(0.5f);
				break;
			}
			case PULSE_WAVE: {
				_oscillator = &_square;
				_square.setPulseWidth(0.1f);
				break;
			}
		}

		_offset = params[OFFSET_PARAM].value * 5.0f;
		_scale = params[SCALE_PARAM].value;
	}

	if (_resetTrigger.next(inputs[RESET_INPUT].value)) {
		_phasor.resetPhase();
	}
	_phasor.advancePhase();
	float sample = _oscillator->nextFromPhasor(_phasor) * amplitude * _scale;
	if (_invert) {
		sample = -sample;
	}
	sample += _offset;
	outputs[OUT_OUTPUT].value = sample;
}

struct LLFOWidget : LFOBaseWidget {
	static constexpr int hp = 3;

	LLFOWidget(LLFO* module)
	: LFOBaseWidget(
		module,
		new SVGPanel(),
		SVG::load(assetPlugin(pluginInstance, "res/LLFO-classic.svg")),
		SVG::load(assetPlugin(pluginInstance, "res/LLFO.svg"))
	) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		_panel->box.size = box.size;
		addChild(_panel);

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(9.5, 27.0);
		auto slowParamPosition = Vec(34.0, 71.0);
		auto waveParamPosition = Vec(18.0, 126.0);
		auto offsetParamPosition = Vec(14.5, 158.5);
		auto scaleParamPosition = Vec(14.5, 199.5);

		auto pitchInputPosition = Vec(10.5, 231.0);
		auto resetInputPosition = Vec(10.5, 266.0);

		auto outOutputPosition = Vec(10.5, 304.0);

		auto slowLightPosition = Vec(2.0, 72.0);
		auto sineLightPosition = Vec(2.0, 89.0);
		auto rampUpLightPosition = Vec(2.0, 102.0);
		auto squareLightPosition = Vec(2.0, 115.0);
		auto triangleLightPosition = Vec(24.0, 89.0);
		auto rampDownLightPosition = Vec(24.0, 102.0);
		auto pulseLightPosition = Vec(24.0, 115.0);
		// end generated by svg_widgets.rb

		_frequencyKnob = ParamWidget::create<Knob26>(frequencyParamPosition, module, LLFO::FREQUENCY_PARAM, -8.0, 5.0, 0.0);
		addParam(_frequencyKnob);
		addParam(ParamWidget::create<StatefulButton9>(slowParamPosition, module, LLFO::SLOW_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<StatefulButton9>(waveParamPosition, module, LLFO::WAVE_PARAM, 0.0, 5.0, 0.0));
		addParam(ParamWidget::create<Knob16>(offsetParamPosition, module, LLFO::OFFSET_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(scaleParamPosition, module, LLFO::SCALE_PARAM, 0.0, 1.0, 1.0));

		addInput(Port::create<Port24>(pitchInputPosition, Port::INPUT, module, LLFO::PITCH_INPUT));
		addInput(Port::create<Port24>(resetInputPosition, Port::INPUT, module, LLFO::RESET_INPUT));

		addOutput(Port::create<Port24>(outOutputPosition, Port::OUTPUT, module, LLFO::OUT_OUTPUT));

		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(slowLightPosition, module, LLFO::SLOW_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(sineLightPosition, module, LLFO::SINE_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(rampUpLightPosition, module, LLFO::RAMP_UP_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(squareLightPosition, module, LLFO::SQUARE_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(triangleLightPosition, module, LLFO::TRIANGLE_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(rampDownLightPosition, module, LLFO::RAMP_DOWN_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(pulseLightPosition, module, LLFO::PULSE_LIGHT));
	}
};

Model* modelLLFO = bogaudio::createModel<LLFO, LLFOWidget>("Bogaudio-LLFO", "LLFO", "compact LFO", LFO_TAG);
