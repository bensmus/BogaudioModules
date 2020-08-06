
#include "VU.hpp"

void VU::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	_lRms.setSampleRate(sr);
	_rRms.setSampleRate(sr);
	_lPeakRms.setSampleRate(sr);
	_rPeakRms.setSampleRate(sr);
	_lPeakSlew.setParams(sr, 750.0f, 1.0f);
	_rPeakSlew.setParams(sr, 750.0f, 1.0f);
}

void VU::processChannel(const ProcessArgs& args, int _c) {
	float left = inputs[L_INPUT].getVoltageSum();
	outputs[L_OUTPUT].setChannels(inputs[L_INPUT].getChannels());
	outputs[L_OUTPUT].writeVoltages(inputs[L_INPUT].getVoltages());

	float right = 0.0f;
	if (inputs[R_INPUT].isConnected()) {
		right = inputs[R_INPUT].getVoltageSum();
		outputs[R_OUTPUT].setChannels(inputs[R_INPUT].getChannels());
		outputs[R_OUTPUT].writeVoltages(inputs[R_INPUT].getVoltages());
	}
	else {
		right = left;
		outputs[R_OUTPUT].setChannels(inputs[L_INPUT].getChannels());
		outputs[R_OUTPUT].writeVoltages(inputs[L_INPUT].getVoltages());
	}

	_lLevel = _lRms.next(left) / 5.0f;
	_rLevel = _rRms.next(right) / 5.0f;

	float lPeak = _lPeakRms.next(fabsf(left)) / 5.0f;
	if (lPeak < _lPeakLevel) {
		if (!_lPeakFalling) {
			_lPeakFalling = true;
			_lPeakSlew._last = _lPeakLevel;
		}
		lPeak = _lPeakSlew.next(lPeak);
	}
	else {
		_lPeakFalling = false;
	}
	_lPeakLevel = lPeak;

	float rPeak = _rPeakRms.next(fabsf(right)) / 5.0f;
	if (rPeak < _rPeakLevel) {
		if (!_rPeakFalling) {
			_rPeakFalling = true;
			_rPeakSlew._last = _rPeakLevel;
		}
		rPeak = _rPeakSlew.next(rPeak);
	}
	else {
		_rPeakFalling = false;
	}
	_rPeakLevel = rPeak;
}

struct VUDisplay : OpaqueWidget {
	struct Level {
		float db;
		NVGcolor color;
		Level(float db, const NVGcolor& color) : db(db), color(color) {}
	};

	const NVGcolor bgColor = nvgRGBA(0xaa, 0xaa, 0xaa, 0xff);
	VU* _module;
	std::vector<Level> _levels;

	VUDisplay(VU* module) : _module(module) {
		for (int i = 1; i <= 36; ++i) {
			float db = 12.0f - i*2.0f;
			_levels.push_back(Level(db, decibelsToColor(db)));
		}
	}

	void draw(const DrawArgs& args) override {
		float lDb = -100.0f;
		float rDb = -100.0f;
		float lPeakDb = -100.0f;
		float rPeakDb = -100.0f;
		if (_module) {
			lDb = amplitudeToDecibels(_module->_lLevel);
			rDb = amplitudeToDecibels(_module->_rLevel);
			lPeakDb = amplitudeToDecibels(_module->_lPeakLevel);
			rPeakDb = amplitudeToDecibels(_module->_rPeakLevel);
		}

		nvgSave(args.vg);
		for (int i = 0; i < 180; i += 5) {
			const Level& l = _levels.at(i / 5);

			nvgBeginPath(args.vg);
			nvgRect(args.vg, 3, i + 1, 5, 4);
			nvgFillColor(args.vg, bgColor);
			nvgFill(args.vg);
			if (lPeakDb > l.db && lPeakDb < l.db + 2.0f) {
				nvgFillColor(args.vg, nvgRGBA(0x00, 0xdd, 0xff, 0xff));
				nvgFill(args.vg);
			}
			if (lDb > l.db) {
				nvgFillColor(args.vg, l.color);
				nvgFill(args.vg);
			}

			nvgBeginPath(args.vg);
			nvgRect(args.vg, 10, i + 1, 5, 4);
			nvgFillColor(args.vg, bgColor);
			nvgFill(args.vg);
			if (rPeakDb > l.db && rPeakDb < l.db + 2.0f) {
				nvgFillColor(args.vg, nvgRGBA(0x00, 0xdd, 0xff, 0xff));
				nvgFill(args.vg);
			}
			if (rDb > l.db) {
				nvgFillColor(args.vg, l.color);
				nvgFill(args.vg);
			}
		}
		nvgRestore(args.vg);
	}
};

struct VUWidget : BGModuleWidget {
	static constexpr int hp = 3;

	VUWidget(VU* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "VU");

		{
			auto display = new VUDisplay(module);
			display->box.pos = Vec(13.5, 16.5);
			display->box.size = Vec(18, 180);
			addChild(display);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto lInputPosition = Vec(10.5, 203.0);
		auto rInputPosition = Vec(10.5, 238.0);

		auto lOutputPosition = Vec(10.5, 276.0);
		auto rOutputPosition = Vec(10.5, 311.0);
		// end generated by svg_widgets.rb

		addInput(createInput<Port24>(lInputPosition, module, VU::L_INPUT));
		addInput(createInput<Port24>(rInputPosition, module, VU::R_INPUT));

		addOutput(createOutput<Port24>(lOutputPosition, module, VU::L_OUTPUT));
		addOutput(createOutput<Port24>(rOutputPosition, module, VU::R_OUTPUT));
	}
};

Model* modelVU = bogaudio::createModel<VU, VUWidget>("Bogaudio-VU", "VU", "Stereo signal meter", "Visual");
