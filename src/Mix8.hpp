#pragma once

#include "bogaudio.hpp"
#include "mixer.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMix8;

namespace bogaudio {

struct Mix8 : Module {
	enum ParamsIds {
		LEVEL1_PARAM,
		MUTE1_PARAM,
		PAN1_PARAM,
		LEVEL2_PARAM,
		MUTE2_PARAM,
		PAN2_PARAM,
		LEVEL3_PARAM,
		MUTE3_PARAM,
		PAN3_PARAM,
		LEVEL4_PARAM,
		MUTE4_PARAM,
		PAN4_PARAM,
		LEVEL5_PARAM,
		MUTE5_PARAM,
		PAN5_PARAM,
		LEVEL6_PARAM,
		MUTE6_PARAM,
		PAN6_PARAM,
		LEVEL7_PARAM,
		MUTE7_PARAM,
		PAN7_PARAM,
		LEVEL8_PARAM,
		MUTE8_PARAM,
		PAN8_PARAM,
		MIX_PARAM,
		MIX_MUTE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		PAN1_INPUT,
		IN1_INPUT,
		CV2_INPUT,
		PAN2_INPUT,
		IN2_INPUT,
		CV3_INPUT,
		PAN3_INPUT,
		IN3_INPUT,
		CV4_INPUT,
		PAN4_INPUT,
		IN4_INPUT,
		CV5_INPUT,
		PAN5_INPUT,
		IN5_INPUT,
		CV6_INPUT,
		PAN6_INPUT,
		IN6_INPUT,
		CV7_INPUT,
		PAN7_INPUT,
		IN7_INPUT,
		CV8_INPUT,
		PAN8_INPUT,
		IN8_INPUT,
		MIX_CV_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		L_OUTPUT,
		R_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	MixerChannel* _channel1;
	MixerChannel* _channel2;
	MixerChannel* _channel3;
	MixerChannel* _channel4;
	MixerChannel* _channel5;
	MixerChannel* _channel6;
	MixerChannel* _channel7;
	MixerChannel* _channel8;
	Amplifier _amplifier;
	bogaudio::dsp::SlewLimiter _slewLimiter;
	Saturator _saturator;
	RootMeanSquare _rms;
	float _rmsLevel = 0.0f;

	Mix8() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		float levelDefault = fabsf(MixerChannel::minDecibels) / (MixerChannel::maxDecibels - MixerChannel::minDecibels);
		configParam(LEVEL1_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN1_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE1_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL2_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN2_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE2_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL3_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN3_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE3_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL4_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN4_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE4_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL5_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN5_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE5_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL6_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN6_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE6_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL7_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN7_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE7_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL8_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN8_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE8_PARAM, 0.0f, 3.0f, 0.0);
		configParam(MIX_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(MIX_MUTE_PARAM, 0.0f, 1.0f, 0.0f);

		_channel1 = new MixerChannel(params[LEVEL1_PARAM], params[PAN1_PARAM], params[MUTE1_PARAM], inputs[IN1_INPUT], inputs[CV1_INPUT], inputs[PAN1_INPUT]);
		_channel2 = new MixerChannel(params[LEVEL2_PARAM], params[PAN2_PARAM], params[MUTE2_PARAM], inputs[IN2_INPUT], inputs[CV2_INPUT], inputs[PAN2_INPUT]);
		_channel3 = new MixerChannel(params[LEVEL3_PARAM], params[PAN3_PARAM], params[MUTE3_PARAM], inputs[IN3_INPUT], inputs[CV3_INPUT], inputs[PAN3_INPUT]);
		_channel4 = new MixerChannel(params[LEVEL4_PARAM], params[PAN4_PARAM], params[MUTE4_PARAM], inputs[IN4_INPUT], inputs[CV4_INPUT], inputs[PAN4_INPUT]);
		_channel5 = new MixerChannel(params[LEVEL5_PARAM], params[PAN5_PARAM], params[MUTE5_PARAM], inputs[IN5_INPUT], inputs[CV5_INPUT], inputs[PAN5_INPUT]);
		_channel6 = new MixerChannel(params[LEVEL6_PARAM], params[PAN6_PARAM], params[MUTE6_PARAM], inputs[IN6_INPUT], inputs[CV6_INPUT], inputs[PAN6_INPUT]);
		_channel7 = new MixerChannel(params[LEVEL7_PARAM], params[PAN7_PARAM], params[MUTE7_PARAM], inputs[IN7_INPUT], inputs[CV7_INPUT], inputs[PAN7_INPUT]);
		_channel8 = new MixerChannel(params[LEVEL8_PARAM], params[PAN8_PARAM], params[MUTE8_PARAM], inputs[IN8_INPUT], inputs[CV8_INPUT], inputs[PAN8_INPUT]);
		onSampleRateChange();
		_rms.setSensitivity(0.05f);
	}
	virtual ~Mix8() {
		delete _channel1;
		delete _channel2;
		delete _channel3;
		delete _channel4;
		delete _channel5;
		delete _channel6;
		delete _channel7;
		delete _channel8;
	}

	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
