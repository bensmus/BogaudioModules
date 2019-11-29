
#include "Mono.hpp"

void Mono::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	_detectorRMS.setSampleRate(sr);
	_attackSL.setParams(sr, 50.0f);
	_releaseSL.setParams(sr, _releaseMS);
	for (int c = 0; c < maxChannels; ++c) {
		_channelRMSs[c].setSampleRate(sr);
	}
}

void Mono::modulate() {
	float comp = clamp(params[COMPRESSION_PARAM].getValue(), 0.0f, 1.0f);
	_ratio = (comp * comp) * 25.0f + 1.0f;
	_releaseMS = std::max(200.0f, comp * 500.0f);
	_releaseSL.setParams(APP->engine->getSampleRate(), _releaseMS);

	float level = clamp(params[LEVEL_PARAM].getValue(), 0.0f, 1.0f);
	level = 1.0f - level;
	level *= _levelAmp.minDecibels;
	_levelAmp.setLevel(level);
}

void Mono::processChannel(const ProcessArgs& args, int c) {
	assert(c == 0);

	_activeChannels = inputs[POLY_INPUT].getChannels();
	float out = 0.0f;
	for (int c = 0; c < _activeChannels; ++c) {
		float v = inputs[POLY_INPUT].getVoltage(c);
		out += v;
		_channelLevels[c] = _channelRMSs[c].next(v) / 5.0f;
	}
	for (int c = _activeChannels; c < maxChannels; ++c) {
		_channelLevels[c] = _channelRMSs[c].next(0.0f) / 5.0f;
	}

	float env = _detectorRMS.next(out);
	if (env > _lastEnv) {
		env = _attackSL.next(env, _lastEnv);
	}
	else {
		env = _releaseSL.next(env, _lastEnv);
	}
	_lastEnv = env;

	float detectorDb = amplitudeToDecibels(env / 5.0f);
	_compressionDb = _compressor.compressionDb(detectorDb, 0.0f, _ratio, true);
	_compAmp.setLevel(-_compressionDb);

	out = _compAmp.next(out);
	out = _levelAmp.next(out);
	out = _saturator.next(out);
	outputs[MONO_OUTPUT].setVoltage(out);
}

struct MonoWidget : ModuleWidget {
	struct ChannelsDisplay : OpaqueWidget {
		const NVGcolor inactiveBgColor = nvgRGBA(0xaa, 0xaa, 0xaa, 0xff);
		const NVGcolor activeBgColor = nvgRGBA(0x66, 0x66, 0x66, 0xff);
		Mono* _module;

		ChannelsDisplay(Mono* module) : _module(module) {
		}

		void draw(const DrawArgs& args) override {
			nvgSave(args.vg);
			for (int i = 0; i < _module->maxChannels; ++i) {
				nvgBeginPath(args.vg);
				if (!_module || i >= _module->_activeChannels) {
					nvgFillColor(args.vg, inactiveBgColor);
				}
				else {
					nvgFillColor(args.vg, activeBgColor);
				}
				nvgCircle(args.vg, (i % 4) * 10 + 5.0f, (i / 4) * 10 + 5.0f, 3.2f);
				nvgFill(args.vg);

				if (_module && _module->_channelLevels[i] > 0.0f) {
					nvgFillColor(args.vg, decibelsToColor(amplitudeToDecibels(_module->_channelLevels[i])));
					nvgFill(args.vg);
				}
			}
			nvgRestore(args.vg);
		}
	};

	struct CompressionDisplay : OpaqueWidget {
		struct Level {
			float db;
			NVGcolor color;
			Level(float db, const NVGcolor& color) : db(db), color(color) {}
		};

		const NVGcolor bgColor = nvgRGBA(0xaa, 0xaa, 0xaa, 0xff);
		Mono* _module;
		std::vector<Level> _levels;

		CompressionDisplay(Mono* module) : _module(module) {
			auto color = nvgRGBA(0xff, 0xaa, 0x00, 0xff);
			_levels.push_back(Level(12.0f, color));
			for (int i = 1; i <= 6; ++i) {
				float db = 12.0f - i*2.0f;
				_levels.push_back(Level(db, color));
			}
		}

		void draw(const DrawArgs& args) override {
			float compressionDb = 0.0f;
			if (_module) {
				compressionDb = _module->_compressionDb;
			}

			nvgSave(args.vg);
			for (int i = 0; i < 35; i += 5) {
				const Level& l = _levels.at(i / 5);

				nvgBeginPath(args.vg);
				nvgRect(args.vg, 3, i + 1, 5, 4);
				nvgFillColor(args.vg, bgColor);
				nvgFill(args.vg);
				if (compressionDb > l.db) {
					nvgFillColor(args.vg, l.color);
					nvgFill(args.vg);
				}
			}
			nvgRestore(args.vg);
		}
	};

	static constexpr int hp = 3;

	MonoWidget(Mono* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Mono.svg")));
			addChild(panel);
		}

		{
			auto display = new ChannelsDisplay(module);
			display->box.pos = Vec(2.5f, 30.0f);
			display->box.size = Vec(40.0f, 40.0f);
			addChild(display);
		}

		{
			auto display = new CompressionDisplay(module);
			display->box.pos = Vec(17.5f, 142.5f);
			display->box.size = Vec(18.0f, 50.0f);
			addChild(display);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto compressionParamPosition = Vec(9.5, 98.5);
		auto levelParamPosition = Vec(9.5, 206.5);

		auto polyInputPosition = Vec(10.5, 253.0);

		auto monoOutputPosition = Vec(10.5, 291.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(compressionParamPosition, module, Mono::COMPRESSION_PARAM));
		addParam(createParam<Knob26>(levelParamPosition, module, Mono::LEVEL_PARAM));

		addInput(createInput<Port24>(polyInputPosition, module, Mono::POLY_INPUT));

		addOutput(createOutput<Port24>(monoOutputPosition, module, Mono::MONO_OUTPUT));
	}
};

Model* modelMono = createModel<Mono, MonoWidget>("Bogaudio-Mono", "MONO", "Poly-to-mono converter with onboard compressor", "Utility", "Polyphonic");
