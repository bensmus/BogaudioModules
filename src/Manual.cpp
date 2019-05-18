
#include "Manual.hpp"

void Manual::onReset() {
	_trigger.reset();
	_pulse.process(10.0f);
}

void Manual::step() {
	bool high = _trigger.process(params[TRIGGER_PARAM].value) || _trigger.isHigh() || (_firstStep && _triggerOnLoad && _shouldTriggerOnLoad);
	if (high) {
		_pulse.trigger(0.001f);
		_pulse.process(engineGetSampleTime());
	}
	else {
		high = _pulse.process(engineGetSampleTime());
	}

	float out = high ? 5.0f : 0.0f;
	outputs[OUT1_OUTPUT].value = out;
	outputs[OUT2_OUTPUT].value = out;
	outputs[OUT3_OUTPUT].value = out;
	outputs[OUT4_OUTPUT].value = out;
	outputs[OUT5_OUTPUT].value = out;
	outputs[OUT6_OUTPUT].value = out;
	outputs[OUT7_OUTPUT].value = out;
	outputs[OUT8_OUTPUT].value = out;

	_firstStep = false;
}

struct ManualWidget : ModuleWidget {
	static constexpr int hp = 3;

	ManualWidget(Manual* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(pluginInstance, "res/Manual.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto triggerParamPosition = Vec(13.5, 22.0);

		auto out1OutputPosition = Vec(10.5, 57.0);
		auto out2OutputPosition = Vec(10.5, 87.0);
		auto out3OutputPosition = Vec(10.5, 117.0);
		auto out4OutputPosition = Vec(10.5, 147.0);
		auto out5OutputPosition = Vec(10.5, 177.0);
		auto out6OutputPosition = Vec(10.5, 207.0);
		auto out7OutputPosition = Vec(10.5, 237.0);
		auto out8OutputPosition = Vec(10.5, 267.0);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Button18>(triggerParamPosition, module, Manual::TRIGGER_PARAM, 0.0, 1.0, 0.0));

		addOutput(Port::create<Port24>(out1OutputPosition, Port::OUTPUT, module, Manual::OUT1_OUTPUT));
		addOutput(Port::create<Port24>(out2OutputPosition, Port::OUTPUT, module, Manual::OUT2_OUTPUT));
		addOutput(Port::create<Port24>(out3OutputPosition, Port::OUTPUT, module, Manual::OUT3_OUTPUT));
		addOutput(Port::create<Port24>(out4OutputPosition, Port::OUTPUT, module, Manual::OUT4_OUTPUT));
		addOutput(Port::create<Port24>(out5OutputPosition, Port::OUTPUT, module, Manual::OUT5_OUTPUT));
		addOutput(Port::create<Port24>(out6OutputPosition, Port::OUTPUT, module, Manual::OUT6_OUTPUT));
		addOutput(Port::create<Port24>(out7OutputPosition, Port::OUTPUT, module, Manual::OUT7_OUTPUT));
		addOutput(Port::create<Port24>(out8OutputPosition, Port::OUTPUT, module, Manual::OUT8_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		Manual* manual = dynamic_cast<Manual*>(module);
		assert(manual);
		menu->addChild(new MenuLabel());
		menu->addChild(new TriggerOnLoadMenuItem(manual, "Trigger on load"));
	}
};

Model* modelManual = bogaudio::createModel<Manual, ManualWidget>("Bogaudio-Manual", "Manual",  "manual gates / triggers");
