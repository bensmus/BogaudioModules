
#include "PEQ14.hpp"

void PEQ14::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->setSampleRate(sr);
	}
}

bool PEQ14::active() {
	return
		outputs[OUT_OUTPUT].isConnected() ||
		outputs[ODDS_OUTPUT].isConnected() ||
		outputs[EVENS_OUTPUT].isConnected() ||
		outputs[OUT1_OUTPUT].isConnected() ||
		outputs[OUT2_OUTPUT].isConnected() ||
		outputs[OUT3_OUTPUT].isConnected() ||
		outputs[OUT4_OUTPUT].isConnected() ||
		outputs[OUT5_OUTPUT].isConnected() ||
		outputs[OUT6_OUTPUT].isConnected() ||
		outputs[OUT7_OUTPUT].isConnected() ||
		outputs[OUT8_OUTPUT].isConnected() ||
		outputs[OUT9_OUTPUT].isConnected() ||
		outputs[OUT10_OUTPUT].isConnected() ||
		outputs[OUT11_OUTPUT].isConnected() ||
		outputs[OUT12_OUTPUT].isConnected() ||
		outputs[OUT13_OUTPUT].isConnected() ||
		outputs[OUT14_OUTPUT].isConnected();
}

int PEQ14::channels() {
	return inputs[IN_INPUT].getChannels();
}

void PEQ14::addChannel(int c) {
	const int n = 14;
	_engines[c] = new PEQEngine(n);
	for (int i = 0; i < n; ++i) {
		_engines[c]->configChannel(
			i,
			c,
			params[LEVEL1_PARAM + i*3],
			params[FREQUENCY1_PARAM + i*3],
			params[FREQUENCY_CV1_PARAM + i*3],
			&params[FREQUENCY_CV_PARAM],
			params[BANDWIDTH_PARAM],
			inputs[LEVEL1_INPUT + i*2],
			inputs[FREQUENCY_CV1_INPUT + i*2],
			inputs[FREQUENCY_CV_INPUT],
			&inputs[BANDWIDTH_INPUT]
		);
	}
	_engines[c]->setSampleRate(APP->engine->getSampleRate());
}

void PEQ14::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void PEQ14::modulate() {
	_fullFrequencyMode = params[FMOD_PARAM].getValue() > 0.5f;

	_lowMode = params[LP_PARAM].getValue() > 0.5f ? MultimodeFilter::LOWPASS_MODE : MultimodeFilter::BANDPASS_MODE;
	_highMode = params[HP_PARAM].getValue() > 0.5f ? MultimodeFilter::HIGHPASS_MODE : MultimodeFilter::BANDPASS_MODE;
	for (int c = 0; c < _channels; ++c) {
		PEQEngine& e = *_engines[c];
		e.setLowFilterMode(_lowMode);
		e.setHighFilterMode(_highMode);
		e.setFrequencyMode(_fullFrequencyMode);
		e.modulate();
	}
}

void PEQ14::processAlways(const ProcessArgs& args) {
	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[ODDS_OUTPUT].setChannels(_channels);
	outputs[EVENS_OUTPUT].setChannels(_channels);
	for (int i = 0; i < 14; ++i) {
		outputs[OUT1_OUTPUT + i].setChannels(_channels);
	}
	std::fill(_rmsSums, _rmsSums + 14, 0.0f);
}

void PEQ14::processChannel(const ProcessArgs& args, int c) {
	PEQEngine& e = *_engines[c];
	float out = e.next(inputs[IN_INPUT].getVoltage(c), _rmsSums);
	outputs[OUT_OUTPUT].setVoltage(out, c);

	float oddOut = 0.0f;
	float evenOut = 0.0f;
	for (int i = 0; i < 14; ++i) {
		oddOut += e.outs[i] * (float)(i % 2 == 0 || (i == 13 && _highMode == MultimodeFilter::HIGHPASS_MODE));
		evenOut += e.outs[i] * (float)(i % 2 == 1 || (i == 0 && _lowMode == MultimodeFilter::LOWPASS_MODE));
		outputs[OUT1_OUTPUT + i].setVoltage(e.outs[i], c);
	}
	outputs[ODDS_OUTPUT].setVoltage(oddOut, c);
	outputs[EVENS_OUTPUT].setVoltage(evenOut, c);

	if (expanderConnected()) {
		auto m = toExpander();
		m->valid = true;
		std::copy(e.outs, e.outs + 14, m->outs[c]);
		std::copy(e.frequencies, e.frequencies + 14, m->frequencies[c]);
		m->bandwidths[c] = e.bandwidth;
		m->lowLP = _lowMode == MultimodeFilter::LOWPASS_MODE;
		m->highHP = _highMode == MultimodeFilter::HIGHPASS_MODE;
	}
}

void PEQ14::postProcessAlways(const ProcessArgs& args) {
	for (int i = 0; i < 14; ++i) {
		_rms[i] = _rmsSums[i] * _inverseChannels;
	}

	lights[FMOD_RELATIVE_LIGHT].value = !_fullFrequencyMode;
	lights[FMOD_FULL_LIGHT].value = _fullFrequencyMode;
}

struct PEQ14Widget : BGModuleWidget {
	static constexpr int hp = 46;

	PEQ14Widget(PEQ14* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "PEQ14");

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// generated by svg_widgets.rb
		auto frequencyCvParamPosition = Vec(47.0, 59.0);
		auto bandwidthParamPosition = Vec(47.0, 121.0);
		auto lpParamPosition = Vec(48.5, 180.0);
		auto hpParamPosition = Vec(48.5, 192.0);
		auto fmodParamPosition = Vec(32.5, 252.0);
		auto level1ParamPosition = Vec(87.5, 28.0);
		auto frequency1ParamPosition = Vec(88.5, 184.0);
		auto frequencyCv1ParamPosition = Vec(88.5, 224.0);
		auto level2ParamPosition = Vec(131.5, 28.0);
		auto frequency2ParamPosition = Vec(132.5, 184.0);
		auto frequencyCv2ParamPosition = Vec(132.5, 224.0);
		auto level3ParamPosition = Vec(175.5, 28.0);
		auto frequency3ParamPosition = Vec(176.5, 184.0);
		auto frequencyCv3ParamPosition = Vec(176.5, 224.0);
		auto level4ParamPosition = Vec(219.5, 28.0);
		auto frequency4ParamPosition = Vec(220.5, 184.0);
		auto frequencyCv4ParamPosition = Vec(220.5, 224.0);
		auto level5ParamPosition = Vec(263.5, 28.0);
		auto frequency5ParamPosition = Vec(264.5, 184.0);
		auto frequencyCv5ParamPosition = Vec(264.5, 224.0);
		auto level6ParamPosition = Vec(307.5, 28.0);
		auto frequency6ParamPosition = Vec(308.5, 184.0);
		auto frequencyCv6ParamPosition = Vec(308.5, 224.0);
		auto level7ParamPosition = Vec(351.5, 28.0);
		auto frequency7ParamPosition = Vec(352.5, 184.0);
		auto frequencyCv7ParamPosition = Vec(352.5, 224.0);
		auto level8ParamPosition = Vec(395.5, 28.0);
		auto frequency8ParamPosition = Vec(396.5, 184.0);
		auto frequencyCv8ParamPosition = Vec(396.5, 224.0);
		auto level9ParamPosition = Vec(439.5, 28.0);
		auto frequency9ParamPosition = Vec(440.5, 184.0);
		auto frequencyCv9ParamPosition = Vec(440.5, 224.0);
		auto level10ParamPosition = Vec(483.5, 28.0);
		auto frequency10ParamPosition = Vec(484.5, 184.0);
		auto frequencyCv10ParamPosition = Vec(484.5, 224.0);
		auto level11ParamPosition = Vec(527.5, 28.0);
		auto frequency11ParamPosition = Vec(528.5, 184.0);
		auto frequencyCv11ParamPosition = Vec(528.5, 224.0);
		auto level12ParamPosition = Vec(571.5, 28.0);
		auto frequency12ParamPosition = Vec(572.5, 184.0);
		auto frequencyCv12ParamPosition = Vec(572.5, 224.0);
		auto level13ParamPosition = Vec(615.5, 28.0);
		auto frequency13ParamPosition = Vec(616.5, 184.0);
		auto frequencyCv13ParamPosition = Vec(616.5, 224.0);
		auto level14ParamPosition = Vec(659.5, 28.0);
		auto frequency14ParamPosition = Vec(660.5, 184.0);
		auto frequencyCv14ParamPosition = Vec(660.5, 224.0);

		auto frequencyCvInputPosition = Vec(9.5, 56.0);
		auto bandwidthInputPosition = Vec(9.5, 121.0);
		auto inInputPosition = Vec(10.5, 290.0);
		auto level1InputPosition = Vec(84.5, 255.0);
		auto frequencyCv1InputPosition = Vec(84.5, 290.0);
		auto level2InputPosition = Vec(128.5, 255.0);
		auto frequencyCv2InputPosition = Vec(128.5, 290.0);
		auto level3InputPosition = Vec(172.5, 255.0);
		auto frequencyCv3InputPosition = Vec(172.5, 290.0);
		auto level4InputPosition = Vec(216.5, 255.0);
		auto frequencyCv4InputPosition = Vec(216.5, 290.0);
		auto level5InputPosition = Vec(260.5, 255.0);
		auto frequencyCv5InputPosition = Vec(260.5, 290.0);
		auto level6InputPosition = Vec(304.5, 255.0);
		auto frequencyCv6InputPosition = Vec(304.5, 290.0);
		auto level7InputPosition = Vec(348.5, 255.0);
		auto frequencyCv7InputPosition = Vec(348.5, 290.0);
		auto level8InputPosition = Vec(392.5, 255.0);
		auto frequencyCv8InputPosition = Vec(392.5, 290.0);
		auto level9InputPosition = Vec(436.5, 255.0);
		auto frequencyCv9InputPosition = Vec(436.5, 290.0);
		auto level10InputPosition = Vec(480.5, 255.0);
		auto frequencyCv10InputPosition = Vec(480.5, 290.0);
		auto level11InputPosition = Vec(524.5, 255.0);
		auto frequencyCv11InputPosition = Vec(524.5, 290.0);
		auto level12InputPosition = Vec(568.5, 255.0);
		auto frequencyCv12InputPosition = Vec(568.5, 290.0);
		auto level13InputPosition = Vec(612.5, 255.0);
		auto frequencyCv13InputPosition = Vec(612.5, 290.0);
		auto level14InputPosition = Vec(656.5, 255.0);
		auto frequencyCv14InputPosition = Vec(656.5, 290.0);

		auto oddsOutputPosition = Vec(40.5, 290.0);
		auto outOutputPosition = Vec(10.5, 325.0);
		auto evensOutputPosition = Vec(40.5, 325.0);
		auto out1OutputPosition = Vec(84.5, 325.0);
		auto out2OutputPosition = Vec(128.5, 325.0);
		auto out3OutputPosition = Vec(172.5, 325.0);
		auto out4OutputPosition = Vec(216.5, 325.0);
		auto out5OutputPosition = Vec(260.5, 325.0);
		auto out6OutputPosition = Vec(304.5, 325.0);
		auto out7OutputPosition = Vec(348.5, 325.0);
		auto out8OutputPosition = Vec(392.5, 325.0);
		auto out9OutputPosition = Vec(436.5, 325.0);
		auto out10OutputPosition = Vec(480.5, 325.0);
		auto out11OutputPosition = Vec(524.5, 325.0);
		auto out12OutputPosition = Vec(568.5, 325.0);
		auto out13OutputPosition = Vec(612.5, 325.0);
		auto out14OutputPosition = Vec(656.5, 325.0);

		auto fmodRelativeLightPosition = Vec(28.5, 228.0);
		auto fmodFullLightPosition = Vec(28.5, 241.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob16>(frequencyCvParamPosition, module, PEQ14::FREQUENCY_CV_PARAM));
		addParam(createParam<Knob16>(bandwidthParamPosition, module, PEQ14::BANDWIDTH_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(lpParamPosition, module, PEQ14::LP_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(hpParamPosition, module, PEQ14::HP_PARAM));
		addParam(createParam<StatefulButton9>(fmodParamPosition, module, PEQ14::FMOD_PARAM));
		addSlider(level1ParamPosition, module, PEQ14::LEVEL1_PARAM, module ? &module->_rms[0] : NULL);
		addParam(createParam<Knob16>(frequency1ParamPosition, module, PEQ14::FREQUENCY1_PARAM));
		addParam(createParam<Knob16>(frequencyCv1ParamPosition, module, PEQ14::FREQUENCY_CV1_PARAM));
		addSlider(level2ParamPosition, module, PEQ14::LEVEL2_PARAM, module ? &module->_rms[1] : NULL);
		addParam(createParam<Knob16>(frequency2ParamPosition, module, PEQ14::FREQUENCY2_PARAM));
		addParam(createParam<Knob16>(frequencyCv2ParamPosition, module, PEQ14::FREQUENCY_CV2_PARAM));
		addSlider(level3ParamPosition, module, PEQ14::LEVEL3_PARAM, module ? &module->_rms[2] : NULL);
		addParam(createParam<Knob16>(frequency3ParamPosition, module, PEQ14::FREQUENCY3_PARAM));
		addParam(createParam<Knob16>(frequencyCv3ParamPosition, module, PEQ14::FREQUENCY_CV3_PARAM));
		addSlider(level4ParamPosition, module, PEQ14::LEVEL4_PARAM, module ? &module->_rms[3] : NULL);
		addParam(createParam<Knob16>(frequency4ParamPosition, module, PEQ14::FREQUENCY4_PARAM));
		addParam(createParam<Knob16>(frequencyCv4ParamPosition, module, PEQ14::FREQUENCY_CV4_PARAM));
		addSlider(level5ParamPosition, module, PEQ14::LEVEL5_PARAM, module ? &module->_rms[4] : NULL);
		addParam(createParam<Knob16>(frequency5ParamPosition, module, PEQ14::FREQUENCY5_PARAM));
		addParam(createParam<Knob16>(frequencyCv5ParamPosition, module, PEQ14::FREQUENCY_CV5_PARAM));
		addSlider(level6ParamPosition, module, PEQ14::LEVEL6_PARAM, module ? &module->_rms[5] : NULL);
		addParam(createParam<Knob16>(frequency6ParamPosition, module, PEQ14::FREQUENCY6_PARAM));
		addParam(createParam<Knob16>(frequencyCv6ParamPosition, module, PEQ14::FREQUENCY_CV6_PARAM));
		addSlider(level7ParamPosition, module, PEQ14::LEVEL7_PARAM, module ? &module->_rms[6] : NULL);
		addParam(createParam<Knob16>(frequency7ParamPosition, module, PEQ14::FREQUENCY7_PARAM));
		addParam(createParam<Knob16>(frequencyCv7ParamPosition, module, PEQ14::FREQUENCY_CV7_PARAM));
		addSlider(level8ParamPosition, module, PEQ14::LEVEL8_PARAM, module ? &module->_rms[7] : NULL);
		addParam(createParam<Knob16>(frequency8ParamPosition, module, PEQ14::FREQUENCY8_PARAM));
		addParam(createParam<Knob16>(frequencyCv8ParamPosition, module, PEQ14::FREQUENCY_CV8_PARAM));
		addSlider(level9ParamPosition, module, PEQ14::LEVEL9_PARAM, module ? &module->_rms[8] : NULL);
		addParam(createParam<Knob16>(frequency9ParamPosition, module, PEQ14::FREQUENCY9_PARAM));
		addParam(createParam<Knob16>(frequencyCv9ParamPosition, module, PEQ14::FREQUENCY_CV9_PARAM));
		addSlider(level10ParamPosition, module, PEQ14::LEVEL10_PARAM, module ? &module->_rms[9] : NULL);
		addParam(createParam<Knob16>(frequency10ParamPosition, module, PEQ14::FREQUENCY10_PARAM));
		addParam(createParam<Knob16>(frequencyCv10ParamPosition, module, PEQ14::FREQUENCY_CV10_PARAM));
		addSlider(level11ParamPosition, module, PEQ14::LEVEL11_PARAM, module ? &module->_rms[10] : NULL);
		addParam(createParam<Knob16>(frequency11ParamPosition, module, PEQ14::FREQUENCY11_PARAM));
		addParam(createParam<Knob16>(frequencyCv11ParamPosition, module, PEQ14::FREQUENCY_CV11_PARAM));
		addSlider(level12ParamPosition, module, PEQ14::LEVEL12_PARAM, module ? &module->_rms[11] : NULL);
		addParam(createParam<Knob16>(frequency12ParamPosition, module, PEQ14::FREQUENCY12_PARAM));
		addParam(createParam<Knob16>(frequencyCv12ParamPosition, module, PEQ14::FREQUENCY_CV12_PARAM));
		addSlider(level13ParamPosition, module, PEQ14::LEVEL13_PARAM, module ? &module->_rms[12] : NULL);
		addParam(createParam<Knob16>(frequency13ParamPosition, module, PEQ14::FREQUENCY13_PARAM));
		addParam(createParam<Knob16>(frequencyCv13ParamPosition, module, PEQ14::FREQUENCY_CV13_PARAM));
		addSlider(level14ParamPosition, module, PEQ14::LEVEL14_PARAM, module ? &module->_rms[13] : NULL);
		addParam(createParam<Knob16>(frequency14ParamPosition, module, PEQ14::FREQUENCY14_PARAM));
		addParam(createParam<Knob16>(frequencyCv14ParamPosition, module, PEQ14::FREQUENCY_CV14_PARAM));

		addInput(createInput<Port24>(frequencyCvInputPosition, module, PEQ14::FREQUENCY_CV_INPUT));
		addInput(createInput<Port24>(bandwidthInputPosition, module, PEQ14::BANDWIDTH_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, PEQ14::IN_INPUT));
		addInput(createInput<Port24>(level1InputPosition, module, PEQ14::LEVEL1_INPUT));
		addInput(createInput<Port24>(frequencyCv1InputPosition, module, PEQ14::FREQUENCY_CV1_INPUT));
		addInput(createInput<Port24>(level2InputPosition, module, PEQ14::LEVEL2_INPUT));
		addInput(createInput<Port24>(frequencyCv2InputPosition, module, PEQ14::FREQUENCY_CV2_INPUT));
		addInput(createInput<Port24>(level3InputPosition, module, PEQ14::LEVEL3_INPUT));
		addInput(createInput<Port24>(frequencyCv3InputPosition, module, PEQ14::FREQUENCY_CV3_INPUT));
		addInput(createInput<Port24>(level4InputPosition, module, PEQ14::LEVEL4_INPUT));
		addInput(createInput<Port24>(frequencyCv4InputPosition, module, PEQ14::FREQUENCY_CV4_INPUT));
		addInput(createInput<Port24>(level5InputPosition, module, PEQ14::LEVEL5_INPUT));
		addInput(createInput<Port24>(frequencyCv5InputPosition, module, PEQ14::FREQUENCY_CV5_INPUT));
		addInput(createInput<Port24>(level6InputPosition, module, PEQ14::LEVEL6_INPUT));
		addInput(createInput<Port24>(frequencyCv6InputPosition, module, PEQ14::FREQUENCY_CV6_INPUT));
		addInput(createInput<Port24>(level7InputPosition, module, PEQ14::LEVEL7_INPUT));
		addInput(createInput<Port24>(frequencyCv7InputPosition, module, PEQ14::FREQUENCY_CV7_INPUT));
		addInput(createInput<Port24>(level8InputPosition, module, PEQ14::LEVEL8_INPUT));
		addInput(createInput<Port24>(frequencyCv8InputPosition, module, PEQ14::FREQUENCY_CV8_INPUT));
		addInput(createInput<Port24>(level9InputPosition, module, PEQ14::LEVEL9_INPUT));
		addInput(createInput<Port24>(frequencyCv9InputPosition, module, PEQ14::FREQUENCY_CV9_INPUT));
		addInput(createInput<Port24>(level10InputPosition, module, PEQ14::LEVEL10_INPUT));
		addInput(createInput<Port24>(frequencyCv10InputPosition, module, PEQ14::FREQUENCY_CV10_INPUT));
		addInput(createInput<Port24>(level11InputPosition, module, PEQ14::LEVEL11_INPUT));
		addInput(createInput<Port24>(frequencyCv11InputPosition, module, PEQ14::FREQUENCY_CV11_INPUT));
		addInput(createInput<Port24>(level12InputPosition, module, PEQ14::LEVEL12_INPUT));
		addInput(createInput<Port24>(frequencyCv12InputPosition, module, PEQ14::FREQUENCY_CV12_INPUT));
		addInput(createInput<Port24>(level13InputPosition, module, PEQ14::LEVEL13_INPUT));
		addInput(createInput<Port24>(frequencyCv13InputPosition, module, PEQ14::FREQUENCY_CV13_INPUT));
		addInput(createInput<Port24>(level14InputPosition, module, PEQ14::LEVEL14_INPUT));
		addInput(createInput<Port24>(frequencyCv14InputPosition, module, PEQ14::FREQUENCY_CV14_INPUT));

		addOutput(createOutput<Port24>(oddsOutputPosition, module, PEQ14::ODDS_OUTPUT));
		addOutput(createOutput<Port24>(outOutputPosition, module, PEQ14::OUT_OUTPUT));
		addOutput(createOutput<Port24>(evensOutputPosition, module, PEQ14::EVENS_OUTPUT));
		addOutput(createOutput<Port24>(out1OutputPosition, module, PEQ14::OUT1_OUTPUT));
		addOutput(createOutput<Port24>(out2OutputPosition, module, PEQ14::OUT2_OUTPUT));
		addOutput(createOutput<Port24>(out3OutputPosition, module, PEQ14::OUT3_OUTPUT));
		addOutput(createOutput<Port24>(out4OutputPosition, module, PEQ14::OUT4_OUTPUT));
		addOutput(createOutput<Port24>(out5OutputPosition, module, PEQ14::OUT5_OUTPUT));
		addOutput(createOutput<Port24>(out6OutputPosition, module, PEQ14::OUT6_OUTPUT));
		addOutput(createOutput<Port24>(out7OutputPosition, module, PEQ14::OUT7_OUTPUT));
		addOutput(createOutput<Port24>(out8OutputPosition, module, PEQ14::OUT8_OUTPUT));
		addOutput(createOutput<Port24>(out9OutputPosition, module, PEQ14::OUT9_OUTPUT));
		addOutput(createOutput<Port24>(out10OutputPosition, module, PEQ14::OUT10_OUTPUT));
		addOutput(createOutput<Port24>(out11OutputPosition, module, PEQ14::OUT11_OUTPUT));
		addOutput(createOutput<Port24>(out12OutputPosition, module, PEQ14::OUT12_OUTPUT));
		addOutput(createOutput<Port24>(out13OutputPosition, module, PEQ14::OUT13_OUTPUT));
		addOutput(createOutput<Port24>(out14OutputPosition, module, PEQ14::OUT14_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(fmodRelativeLightPosition, module, PEQ14::FMOD_RELATIVE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(fmodFullLightPosition, module, PEQ14::FMOD_FULL_LIGHT));
	}

	void addSlider(Vec position, PEQ14* module, int id, float* rms) {
		auto slider = createParam<VUSlider151>(position, module, id);
		if (rms) {
			dynamic_cast<VUSlider*>(slider)->setVULevel(rms);
		}
		addParam(slider);
	}
};

Model* modelPEQ14 = createModel<PEQ14, PEQ14Widget>("Bogaudio-PEQ14", "PEQ14", "14-channel parametric equalizer / filter bank", "Filter", "Vocoder", "Polyphonic");
