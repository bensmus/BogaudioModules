
#include "Stack.hpp"

bool Stack::active() {
	return outputs[OUT_OUTPUT].isConnected() || outputs[THRU_OUTPUT].isConnected();
}

int Stack::channels() {
	return inputs[IN_INPUT].getChannels();
}

void Stack::modulateChannel(int c) {
	_semitones[c] = roundf(params[OCTAVE_PARAM].getValue()) * 12.0f;
	_semitones[c] += roundf(params[SEMIS_PARAM].getValue());
	if (inputs[CV_INPUT].isConnected()) {
		_semitones[c] += clamp(inputs[CV_INPUT].getPolyVoltage(c), -5.0f, 5.0f) * 10.0f;
	}
	if (params[QUANTIZE_PARAM].getValue() > 0.5f) {
		_semitones[c] = roundf(_semitones[c]);
	}
}

void Stack::processChannel(const ProcessArgs& args, int c) {
	float inCV = clamp(inputs[IN_INPUT].getVoltage(c), _minCVOut, _maxCVOut);
	float fine = params[FINE_PARAM].getValue();

	if (_semitones[c] != _lastSemitones[c] || inCV != _lastInCV[c] || fine != _lastFine[c]) {
		_lastSemitones[c] = _semitones[c];
		_lastInCV[c] = inCV;
		_lastFine[c] = fine;
		_outCV[c] = clamp(semitoneToCV((inCV != 0.0f ? cvToSemitone(inCV) : referenceSemitone) + _semitones[c] + fine), _minCVOut, _maxCVOut);
	}

	if (inputs[IN_INPUT].isConnected()) {
		outputs[THRU_OUTPUT].setChannels(_channels);
		outputs[THRU_OUTPUT].setVoltage(inCV, c);
	}
	else {
		outputs[THRU_OUTPUT].setVoltage(_semitones[c] / 10.0);
	}
	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(_outCV[c], c);
}

struct StackWidget : ModuleWidget {
	static constexpr int hp = 3;

	StackWidget(Stack* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Stack.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto semisParamPosition = Vec(9.5, 32.5);
		auto octaveParamPosition = Vec(14.5, 86.5);
		auto fineParamPosition = Vec(14.5, 126.5);
		auto quantizeParamPosition = Vec(28.4, 191.9);

		auto cvInputPosition = Vec(10.5, 157.0);
		auto inInputPosition = Vec(10.5, 215.0);

		auto thruOutputPosition = Vec(10.5, 253.0);
		auto outOutputPosition = Vec(10.5, 289.0);
		// end generated by svg_widgets.rb

		{
			auto w = createParam<Knob26>(semisParamPosition, module, Stack::SEMIS_PARAM);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}
		{
			auto w = createParam<Knob16>(octaveParamPosition, module, Stack::OCTAVE_PARAM);
			auto k = dynamic_cast<SvgKnob*>(w);
			k->snap = true;
			k->minAngle = -0.5 * M_PI;
			k->maxAngle = 0.5 * M_PI;
			addParam(w);
		}
		addParam(createParam<Knob16>(fineParamPosition, module, Stack::FINE_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(quantizeParamPosition, module, Stack::QUANTIZE_PARAM));

		addInput(createInput<Port24>(cvInputPosition, module, Stack::CV_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, Stack::IN_INPUT));

		addOutput(createOutput<Port24>(thruOutputPosition, module, Stack::THRU_OUTPUT));
		addOutput(createOutput<Port24>(outOutputPosition, module, Stack::OUT_OUTPUT));
	}
};

Model* modelStack = bogaudio::createModel<Stack, StackWidget>("Bogaudio-Stack", "STACK", "Pitch CV transposer", "Tuner", "Polyphonic");
