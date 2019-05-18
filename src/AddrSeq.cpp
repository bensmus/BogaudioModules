
#include "AddrSeq.hpp"

#define SELECT_ON_CLOCK "select_on_clock"

void AddrSeq::onReset() {
	_step = 0;
	_clock.reset();
	_reset.reset();
}

void AddrSeq::onSampleRateChange() {
	_timer.setParams(engineGetSampleRate(), 0.001f);
}

json_t* AddrSeq::toJson() {
	json_t* root = json_object();
	json_object_set_new(root, SELECT_ON_CLOCK, json_boolean(_selectOnClock));
	return root;
}

void AddrSeq::fromJson(json_t* root) {
	json_t* s = json_object_get(root, SELECT_ON_CLOCK);
	if (s) {
		_selectOnClock = json_is_true(s);
	}
}

void AddrSeq::step() {
	bool reset = _reset.process(inputs[RESET_INPUT].value);
	if (reset) {
		_timer.reset();
	}
	bool timer = _timer.next();
	bool clock = _clock.process(inputs[CLOCK_INPUT].value) && !timer;

	int steps = clamp(params[STEPS_PARAM].value, 1.0f, 8.0f);
	int reverse = 1 - 2 * (params[DIRECTION_PARAM].value == 0.0f);
	_step = (_step + reverse * clock) % steps;
	_step += (_step < 0) * steps;
	_step -= _step * reset;
	int select = params[SELECT_PARAM].value;
	select += clamp(inputs[SELECT_INPUT].value, 0.0f, 10.0f) * 0.1f * 8.0f;
	if (!_selectOnClock || clock) {
		_select = select;
	}
	int step = _step + _select;
	step = step % 8;

	float out = 0.0f;
	for (int i = 0; i < 8; ++i) {
		out += params[OUT1_PARAM + i].value * (step == i);
		lights[OUT1_LIGHT + i].value = step == i;
	}
	outputs[OUT_OUTPUT].value = out * 10.0f;
}

struct SelectOnClockMenuItem : MenuItem {
	AddrSeq* _module;

	SelectOnClockMenuItem(AddrSeq* module, const char* label)
	: _module(module)
	{
		this->text = label;
	}

	void onAction(EventAction &e) override {
		_module->_selectOnClock = !_module->_selectOnClock;
	}

	void step() override {
		rightText = _module->_selectOnClock ? "✔" : "";
	}
};

struct AddrSeqWidget : ModuleWidget {
	static constexpr int hp = 6;

	AddrSeqWidget(AddrSeq* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(pluginInstance, "res/AddrSeq.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto stepsParamPosition = Vec(15.5, 131.5);
		auto directionParamPosition = Vec(16.0, 167.5);
		auto selectParamPosition = Vec(9.0, 230.0);
		auto out1ParamPosition = Vec(58.5, 36.0);
		auto out2ParamPosition = Vec(58.5, 77.3);
		auto out3ParamPosition = Vec(58.5, 118.6);
		auto out4ParamPosition = Vec(58.5, 159.9);
		auto out5ParamPosition = Vec(58.5, 201.1);
		auto out6ParamPosition = Vec(58.5, 242.4);
		auto out7ParamPosition = Vec(58.5, 283.7);
		auto out8ParamPosition = Vec(58.5, 325.0);

		auto clockInputPosition = Vec(11.5, 35.0);
		auto resetInputPosition = Vec(11.5, 72.0);
		auto selectInputPosition = Vec(11.5, 270.0);

		auto outOutputPosition = Vec(11.5, 324.0);

		auto out1LightPosition = Vec(66.5, 58.5);
		auto out2LightPosition = Vec(66.5, 99.8);
		auto out3LightPosition = Vec(66.5, 141.1);
		auto out4LightPosition = Vec(66.5, 182.4);
		auto out5LightPosition = Vec(66.5, 223.6);
		auto out6LightPosition = Vec(66.5, 264.9);
		auto out7LightPosition = Vec(66.5, 306.2);
		auto out8LightPosition = Vec(66.5, 347.5);
		// end generated by svg_widgets.rb

		{
			auto w = ParamWidget::create<Knob16>(stepsParamPosition, module, AddrSeq::STEPS_PARAM, 1.0, 8.0, 8.0);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}
		addParam(ParamWidget::create<SliderSwitch2State14>(directionParamPosition, module, AddrSeq::DIRECTION_PARAM, 0.0, 1.0, 1.0));
		{
			auto w = ParamWidget::create<Knob29>(selectParamPosition, module, AddrSeq::SELECT_PARAM, 0.0, 7.0, 0.0);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}
		addParam(ParamWidget::create<Knob16>(out1ParamPosition, module, AddrSeq::OUT1_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(out2ParamPosition, module, AddrSeq::OUT2_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(out3ParamPosition, module, AddrSeq::OUT3_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(out4ParamPosition, module, AddrSeq::OUT4_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(out5ParamPosition, module, AddrSeq::OUT5_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(out6ParamPosition, module, AddrSeq::OUT6_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(out7ParamPosition, module, AddrSeq::OUT7_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(out8ParamPosition, module, AddrSeq::OUT8_PARAM, -1.0, 1.0, 0.0));

		addInput(Port::create<Port24>(clockInputPosition, Port::INPUT, module, AddrSeq::CLOCK_INPUT));
		addInput(Port::create<Port24>(resetInputPosition, Port::INPUT, module, AddrSeq::RESET_INPUT));
		addInput(Port::create<Port24>(selectInputPosition, Port::INPUT, module, AddrSeq::SELECT_INPUT));

		addOutput(Port::create<Port24>(outOutputPosition, Port::OUTPUT, module, AddrSeq::OUT_OUTPUT));

		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(out1LightPosition, module, AddrSeq::OUT1_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(out2LightPosition, module, AddrSeq::OUT2_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(out3LightPosition, module, AddrSeq::OUT3_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(out4LightPosition, module, AddrSeq::OUT4_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(out5LightPosition, module, AddrSeq::OUT5_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(out6LightPosition, module, AddrSeq::OUT6_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(out7LightPosition, module, AddrSeq::OUT7_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(out8LightPosition, module, AddrSeq::OUT8_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		AddrSeq* m = dynamic_cast<AddrSeq*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		menu->addChild(new SelectOnClockMenuItem(m, "Select on clock"));
	}
};

Model* modelAddrSeq = bogaudio::createModel<AddrSeq, AddrSeqWidget>("Bogaudio-AddrSeq", "ADDR-SEQ", "voltage-addressable sequencer", SEQUENCER_TAG);
