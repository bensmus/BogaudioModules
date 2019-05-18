
#include "ShaperPlus.hpp"

struct ShaperPlusWidget : ModuleWidget {
	static constexpr int hp = 15;

	ShaperPlusWidget(ShaperPlus* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(pluginInstance, "res/ShaperPlus.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// generated by svg_widgets.rb
		auto attackParamPosition = Vec(29.08, 33.08);
		auto triggerParamPosition = Vec(89.04, 43.04);
		auto onParamPosition = Vec(29.08, 89.08);
		auto speedParamPosition = Vec(119.0, 96.0);
		auto decayParamPosition = Vec(29.08, 145.08);
		auto loopParamPosition = Vec(119.0, 152.0);
		auto offParamPosition = Vec(29.08, 201.08);
		auto envParamPosition = Vec(82.38, 257.08);
		auto signalParamPosition = Vec(82.38, 313.08);

		auto triggerInputPosition = Vec(114.0, 40.0);
		auto attackInputPosition = Vec(152.0, 40.0);
		auto onInputPosition = Vec(152.0, 96.0);
		auto decayInputPosition = Vec(152.0, 152.0);
		auto offInputPosition = Vec(152.0, 208.0);
		auto envInputPosition = Vec(152.0, 264.0);
		auto signalInputPosition = Vec(11.5, 320.0);
		auto signalcvInputPosition = Vec(152.0, 320.0);

		auto attackOutputPosition = Vec(189.0, 40.0);
		auto onOutputPosition = Vec(189.0, 96.0);
		auto decayOutputPosition = Vec(189.0, 152.0);
		auto offOutputPosition = Vec(189.0, 208.0);
		auto envOutputPosition = Vec(11.5, 264.0);
		auto invOutputPosition = Vec(40.5, 264.0);
		auto triggerOutputPosition = Vec(189.0, 264.0);
		auto signalOutputPosition = Vec(40.5, 320.0);

		auto attackLightPosition = Vec(12.0, 80.0);
		auto onLightPosition = Vec(12.0, 121.0);
		auto decayLightPosition = Vec(12.0, 189.0);
		auto offLightPosition = Vec(12.0, 237.0);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Knob38>(attackParamPosition, module, ShaperPlus::ATTACK_PARAM, 0.0, 1.0, 0.12));
		addParam(ParamWidget::create<Knob38>(onParamPosition, module, ShaperPlus::ON_PARAM, 0.0, 1.0, 0.32));
		addParam(ParamWidget::create<Knob38>(decayParamPosition, module, ShaperPlus::DECAY_PARAM, 0.0, 1.0, 0.32));
		addParam(ParamWidget::create<Knob38>(offParamPosition, module, ShaperPlus::OFF_PARAM, 0.0, 1.0, 0.07));
		addParam(ParamWidget::create<Knob38>(envParamPosition, module, ShaperPlus::ENV_PARAM, 0.0, 1.0, 1.0));
		addParam(ParamWidget::create<Knob38>(signalParamPosition, module, ShaperPlus::SIGNAL_PARAM, 0.0, 1.0, 0.1));

		addParam(ParamWidget::create<Button18>(triggerParamPosition, module, ShaperPlus::TRIGGER_PARAM, 0.0, 1.0, 0.0));
		addInput(Port::create<Port24>(triggerInputPosition, Port::INPUT, module, ShaperPlus::TRIGGER_INPUT));

		addParam(ParamWidget::create<SliderSwitch2State14>(speedParamPosition, module, ShaperPlus::SPEED_PARAM, 0.0, 1.0, 1.0));
		addParam(ParamWidget::create<SliderSwitch2State14>(loopParamPosition, module, ShaperPlus::LOOP_PARAM, 0.0, 1.0, 1.0));
		addOutput(Port::create<Port24>(triggerOutputPosition, Port::OUTPUT, module, ShaperPlus::TRIGGER_OUTPUT));

		addOutput(Port::create<Port24>(envOutputPosition, Port::OUTPUT, module, ShaperPlus::ENV_OUTPUT));
		addOutput(Port::create<Port24>(invOutputPosition, Port::OUTPUT, module, ShaperPlus::INV_OUTPUT));

		addInput(Port::create<Port24>(signalInputPosition, Port::INPUT, module, ShaperPlus::SIGNAL_INPUT));
		addOutput(Port::create<Port24>(signalOutputPosition, Port::OUTPUT, module, ShaperPlus::SIGNAL_OUTPUT));

		addInput(Port::create<Port24>(attackInputPosition, Port::INPUT, module, ShaperPlus::ATTACK_INPUT));
		addInput(Port::create<Port24>(onInputPosition, Port::INPUT, module, ShaperPlus::ON_INPUT));
		addInput(Port::create<Port24>(decayInputPosition, Port::INPUT, module, ShaperPlus::DECAY_INPUT));
		addInput(Port::create<Port24>(offInputPosition, Port::INPUT, module, ShaperPlus::OFF_INPUT));
		addInput(Port::create<Port24>(envInputPosition, Port::INPUT, module, ShaperPlus::ENV_INPUT));
		addInput(Port::create<Port24>(signalcvInputPosition, Port::INPUT, module, ShaperPlus::SIGNALCV_INPUT));

		addOutput(Port::create<Port24>(attackOutputPosition, Port::OUTPUT, module, ShaperPlus::ATTACK_OUTPUT));
		addOutput(Port::create<Port24>(onOutputPosition, Port::OUTPUT, module, ShaperPlus::ON_OUTPUT));
		addOutput(Port::create<Port24>(decayOutputPosition, Port::OUTPUT, module, ShaperPlus::DECAY_OUTPUT));
		addOutput(Port::create<Port24>(offOutputPosition, Port::OUTPUT, module, ShaperPlus::OFF_OUTPUT));

		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(attackLightPosition, module, ShaperPlus::ATTACK_LIGHT));
		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(onLightPosition, module, ShaperPlus::ON_LIGHT));
		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(decayLightPosition, module, ShaperPlus::DECAY_LIGHT));
		addChild(ModuleLightWidget::create<TinyLight<GreenLight>>(offLightPosition, module, ShaperPlus::OFF_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		ShaperPlus* shaperPlus = dynamic_cast<ShaperPlus*>(module);
		assert(shaperPlus);
		menu->addChild(new MenuLabel());
		menu->addChild(new TriggerOnLoadMenuItem(shaperPlus, "Resume loop on load"));
	}
};

Model* modelShaperPlus = bogaudio::createModel<ShaperPlus, ShaperPlusWidget>("Bogaudio-ShaperPlus", "Shaper+",  "envelope and amplifier", ENVELOPE_GENERATOR_TAG, AMPLIFIER_TAG);
