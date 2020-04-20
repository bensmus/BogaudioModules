
#include "LVCO.hpp"

#define FM_MODE "fm_mode"
#define LINEAR_MODE "linear_mode"

json_t* LVCO::dataToJson() {
	json_t* root = VCOBase::dataToJson();
	json_object_set_new(root, FM_MODE, json_boolean(_fmLinearMode));
	json_object_set_new(root, LINEAR_MODE, json_boolean(_linearMode));
	return root;
}

void LVCO::dataFromJson(json_t* root) {
	VCOBase::dataFromJson(root);

	json_t* fm = json_object_get(root, FM_MODE);
	if (fm) {
		_fmLinearMode = json_is_true(fm);
	}

	json_t* l = json_object_get(root, LINEAR_MODE);
	if (l) {
		_linearMode = json_is_true(l);
	}
}

bool LVCO::active() {
	return outputs[OUT_OUTPUT].isConnected();
}

void LVCO::modulate() {
	_slowMode = params[SLOW_PARAM].getValue() > 0.5f;
	_wave = (Wave)params[WAVE_PARAM].getValue();
	_fmDepth = params[FM_DEPTH_PARAM].getValue();
}

void LVCO::modulateChannel(int c) {
	VCOBase::modulateChannel(c);
	Engine& e = *_engines[c];

	e.squareActive = false;
	switch (_wave) {
		case SQUARE_WAVE: {
			e.squareActive = true;
			e.square.setPulseWidth(e.squarePulseWidthSL.next(0.5f));
			break;
		}
		case PULSE_25_WAVE: {
			e.squareActive = true;
			e.square.setPulseWidth(e.squarePulseWidthSL.next(0.25f));
			break;
		}
		case PULSE_10_WAVE: {
			e.squareActive = true;
			e.square.setPulseWidth(e.squarePulseWidthSL.next(0.1f));
			break;
		}
		default: {
		}
	}
	e.sawActive = _wave == SAW_WAVE;
	e.triangleActive = _wave == TRIANGLE_WAVE;
	e.sineActive = _wave == SINE_WAVE;
}

void LVCO::processAlways(const ProcessArgs& args) {
	Wave wave = (Wave)params[WAVE_PARAM].getValue();
	lights[SINE_LIGHT].value = wave == SINE_WAVE;
	lights[TRIANGLE_LIGHT].value = wave == TRIANGLE_WAVE;
	lights[SAW_LIGHT].value = wave == SAW_WAVE;
	lights[SQUARE_LIGHT].value = wave == SQUARE_WAVE;
	lights[PULSE_25_LIGHT].value = wave == PULSE_25_WAVE;
	lights[PULSE_10_LIGHT].value = wave == PULSE_10_WAVE;
}

void LVCO::processChannel(const ProcessArgs& args, int c) {
	VCOBase::processChannel(args, c);
	Engine& e = *_engines[c];

	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(e.squareOut + e.sawOut + e.triangleOut + e.sineOut, c);
}

struct LVCOWidget : ModuleWidget {
	static constexpr int hp = 3;

	LVCOWidget(LVCO* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LVCO.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(9.5, 27.0);
		auto slowParamPosition = Vec(31.0, 62.0);
		auto waveParamPosition = Vec(18.0, 124.0);
		auto fmDepthParamPosition = Vec(14.5, 160.0);

		auto pitchInputPosition = Vec(10.5, 196.0);
		auto fmInputPosition = Vec(10.5, 231.0);
		auto syncInputPosition = Vec(10.5, 266.0);

		auto outOutputPosition = Vec(10.5, 304.0);

		auto sineLightPosition = Vec(2.0, 87.0);
		auto sawLightPosition = Vec(2.0, 100.0);
		auto pulse25LightPosition = Vec(2.0, 113.0);
		auto triangleLightPosition = Vec(24.0, 87.0);
		auto squareLightPosition = Vec(24.0, 100.0);
		auto pulse10LightPosition = Vec(24.0, 113.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(frequencyParamPosition, module, LVCO::FREQUENCY_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(slowParamPosition, module, LVCO::SLOW_PARAM));
		addParam(createParam<StatefulButton9>(waveParamPosition, module, LVCO::WAVE_PARAM));
		addParam(createParam<Knob16>(fmDepthParamPosition, module, LVCO::FM_DEPTH_PARAM));

		addInput(createInput<Port24>(pitchInputPosition, module, LVCO::PITCH_INPUT));
		addInput(createInput<Port24>(fmInputPosition, module, LVCO::FM_INPUT));
		addInput(createInput<Port24>(syncInputPosition, module, LVCO::SYNC_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, LVCO::OUT_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(sineLightPosition, module, LVCO::SINE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(sawLightPosition, module, LVCO::SAW_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(pulse25LightPosition, module, LVCO::PULSE_25_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(triangleLightPosition, module, LVCO::TRIANGLE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(squareLightPosition, module, LVCO::SQUARE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(pulse10LightPosition, module, LVCO::PULSE_10_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		LVCO* m = dynamic_cast<LVCO*>(module);
		assert(m);
		menu->addChild(new MenuLabel());

		OptionsMenuItem* fm = new OptionsMenuItem("FM mode");
		fm->addItem(OptionMenuItem("Exponential", [m]() { return !m->_fmLinearMode; }, [m]() { m->_fmLinearMode = false; }));
		fm->addItem(OptionMenuItem("Linear", [m]() { return m->_fmLinearMode; }, [m]() { m->_fmLinearMode = true; }));
		OptionsMenuItem::addToMenu(fm, menu);

		menu->addChild(new BoolOptionMenuItem("Lineary frequency mode", [m]() { return &m->_linearMode; }));

		OptionsMenuItem* p = new OptionsMenuItem("Polyphony channels from");
		p->addItem(OptionMenuItem("V/OCT input", [m]() { return m->_polyInputID == LVCO::PITCH_INPUT; }, [m]() { m->_polyInputID = LVCO::PITCH_INPUT; }));
		p->addItem(OptionMenuItem("FM input", [m]() { return m->_polyInputID == LVCO::FM_INPUT; }, [m]() { m->_polyInputID = LVCO::FM_INPUT; }));
		OptionsMenuItem::addToMenu(p, menu);
	}
};

Model* modelLVCO = createModel<LVCO, LVCOWidget>("Bogaudio-LVCO", "LVCO", "Oscillator", "Oscillator", "Polyphonic");
