
#include "LPG.hpp"

#define LPF_POLES "lpf_poles"

void LPG::Engine::reset() {
	trigger.reset();
}

void LPG::Engine::setSampleRate(float sr) {
	vcaSL.setParams(sr, 5.0f, 1.0f);
	finalHP.setParams(sr, MultimodeFilter::BUTTERWORTH_TYPE, 2, MultimodeFilter::HIGHPASS_MODE, 80.0f, MultimodeFilter::minQbw, MultimodeFilter::LINEAR_BANDWIDTH_MODE, MultimodeFilter::MINIMUM_DELAY_MODE);
}

void LPG::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void LPG::sampleRateChange() {
	_sampleRate = APP->engine->getSampleRate();
	_sampleTime = APP->engine->getSampleTime();
	for (int i = 0; i < _channels; ++i) {
		_engines[i]->setSampleRate(_sampleRate);
	}
}

json_t* LPG::toJson(json_t* root) {
	json_object_set_new(root, LPF_POLES, json_integer(_lpfPoles));
	return root;
}

void LPG::fromJson(json_t* root) {
	json_t* p = json_object_get(root, LPF_POLES);
	if (p) {
		_lpfPoles = json_integer_value(p);
	}
}

bool LPG::active() {
	return outputs[OUT_OUTPUT].isConnected();
}

int LPG::channels() {
	return inputs[GATE_INPUT].getChannels();
}

void LPG::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->setSampleRate(_sampleRate);
}

void LPG::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void LPG::modulateChannel(int c) {
	_engines[c]->slew.modulate(
		_sampleRate,
		params[RESPONSE_PARAM],
		&inputs[RESPONSE_INPUT],
		100.0f * _timeScale,
		params[RISE_SHAPE_PARAM],
		params[RESPONSE_PARAM],
		&inputs[RESPONSE_INPUT],
		2000.0f * _timeScale,
		params[FALL_SHAPE_PARAM],
		c
	);
}

void LPG::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	if (e.trigger.process(inputs[GATE_INPUT].getPolyVoltage(c))) {
		float time = clamp(params[RESPONSE_PARAM].getValue(), 0.0f, 1.0f);
		if (inputs[RESPONSE_INPUT].isConnected()) {
			time *= clamp(inputs[RESPONSE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		}
		time *= time;
		time *= _timeScale * 0.1f;
		time += 0.01f;
		e.gateSeconds = time;

		e.gateElapsedSeconds = 0.0f;
		e.gateSeconds = time;
	}
	else {
		e.gateElapsedSeconds += _sampleTime;
	}

	float gate = 0.0f;
	if (e.gateElapsedSeconds < e.gateSeconds) {
		gate = 10.0f;
	}
	float env = e.slew.next(gate);
	env /= 10.0f;

	float lpfEnv = clamp(params[LPF_ENV_PARAM].getValue(), -1.0f, 1.0f);
	float lpfBias = clamp(params[LPF_BIAS_PARAM].getValue(), -1.0f, 1.0f);
	if (inputs[LPF_INPUT].isConnected()) {
		float cv = clamp(inputs[LPF_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		lpfBias = clamp(lpfBias + cv, -1.0f, 1.0f);
	}
	lpfBias *= lpfBias;
	float f = clamp(lpfBias + env * lpfEnv, 0.0f, 1.0f);
	f *= maxFilterCutoff;
	f = std::max(f, MultimodeFilter4::minFrequency);
	e.lpf.setParams(
		_sampleRate,
		MultimodeFilter::BUTTERWORTH_TYPE,
		_lpfPoles,
		MultimodeFilter::LOWPASS_MODE,
		f,
		0.0f
	);

	bool linear = params[LINEAR_VCA_PARAM].getValue() > 0.5f;
	float vcaEnv = clamp(params[VCA_ENV_PARAM].getValue(), -1.0f, 1.0f);
	float vcaBias = clamp(params[VCA_BIAS_PARAM].getValue(), 0.0f, 1.0f);
	if (inputs[VCA_INPUT].isConnected()) {
		float cv = clamp(inputs[VCA_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		vcaBias = clamp(vcaBias + cv, 0.0f, 1.0f);
	}
	float level = clamp(vcaBias + env * vcaEnv, 0.0f, 1.0f);
	level = e.vcaSL.next(level);

	float out = inputs[IN_INPUT].getPolyVoltage(c);
	out = e.finalHP.next(e.lpf.next(out));
	if (linear) {
		out *= level;
	}
	else {
		e.vca.setLevel(Amplifier::minDecibels * (1.0f - level));
		out = e.vca.next(out);
	}
	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(out, c);
}

struct LPGWidget : BGModuleWidget {
	static constexpr int hp = 8;

	LPGWidget(LPG* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "LPG");
		createScrews();

		// generated by svg_widgets.rb
		auto responseParamPosition = Vec(19.5, 50.0);
		auto times10xParamPosition = Vec(26.0, 106.0);
		auto riseShapeParamPosition = Vec(88.0, 50.0);
		auto fallShapeParamPosition = Vec(88.0, 95.0);
		auto lpfEnvParamPosition = Vec(27.0, 152.0);
		auto lpfBiasParamPosition = Vec(75.5, 152.0);
		auto vcaEnvParamPosition = Vec(27.5, 219.0);
		auto vcaBiasParamPosition = Vec(75.0, 219.0);
		auto linearVcaParamPosition = Vec(45.0, 258.0);

		auto responseInputPosition = Vec(18.5, 287.0);
		auto lpfInputPosition = Vec(48.5, 287.0);
		auto vcaInputPosition = Vec(78.5, 287.0);
		auto gateInputPosition = Vec(18.5, 324.0);
		auto inInputPosition = Vec(48.5, 324.0);

		auto outOutputPosition = Vec(78.5, 324.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob45>(responseParamPosition, module, LPG::RESPONSE_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(times10xParamPosition, module, LPG::LONG_PARAM));
		addParam(createParam<Knob16>(riseShapeParamPosition, module, LPG::RISE_SHAPE_PARAM));
		addParam(createParam<Knob16>(fallShapeParamPosition, module, LPG::FALL_SHAPE_PARAM));
		addParam(createParam<Knob26>(lpfEnvParamPosition, module, LPG::LPF_ENV_PARAM));
		addParam(createParam<Knob26>(lpfBiasParamPosition, module, LPG::LPF_BIAS_PARAM));
		addParam(createParam<Knob26>(vcaEnvParamPosition, module, LPG::VCA_ENV_PARAM));
		addParam(createParam<Knob26>(vcaBiasParamPosition, module, LPG::VCA_BIAS_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(linearVcaParamPosition, module, LPG::LINEAR_VCA_PARAM));

		addInput(createInput<Port24>(responseInputPosition, module, LPG::RESPONSE_INPUT));
		addInput(createInput<Port24>(lpfInputPosition, module, LPG::LPF_INPUT));
		addInput(createInput<Port24>(vcaInputPosition, module, LPG::VCA_INPUT));
		addInput(createInput<Port24>(gateInputPosition, module, LPG::GATE_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, LPG::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, LPG::OUT_OUTPUT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<LPG*>(module);
		assert(m);
		OptionsMenuItem* bwm = new OptionsMenuItem("LPF poles");
		bwm->addItem(OptionMenuItem("1", [m]() { return m->_lpfPoles == 1; }, [m]() { m->_lpfPoles = 1; }));
		bwm->addItem(OptionMenuItem("2", [m]() { return m->_lpfPoles == 2; }, [m]() { m->_lpfPoles = 2; }));
		bwm->addItem(OptionMenuItem("3", [m]() { return m->_lpfPoles == 3; }, [m]() { m->_lpfPoles = 3; }));
		bwm->addItem(OptionMenuItem("4", [m]() { return m->_lpfPoles == 4; }, [m]() { m->_lpfPoles = 4; }));
		OptionsMenuItem::addToMenu(bwm, menu);
	}
};

Model* modelLPG = createModel<LPG, LPGWidget>("Bogaudio-LPG", "LPG", "Low-pass gate", "Low-pass gate", "Polyphonic");
