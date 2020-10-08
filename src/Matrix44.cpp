
#include "Matrix44.hpp"

void Matrix44::elementsChanged() {
	// base-class caller holds lock on _elements.
	Input** cvs = NULL;
	Param** mutes = NULL;
	bool* soloByColumns = NULL;
	if (_elements.size() > 1) {
		auto e = _elements[1];
		assert(e);
		if (e->cvs) {
			cvs = e->cvs;
		}
		if (e->mutes) {
			mutes = e->mutes;
		}
		if (e->soloByColumns) {
			soloByColumns = e->soloByColumns;
		}
	}
	setCVInputs(cvs);
	setMuteParams(mutes);
	setSoloByColumns(soloByColumns);
}

void Matrix44::processAlways(const ProcessArgs& args) {
	if (expanderConnected()) {
		Matrix44ExpanderMessage* te = toExpander();
		te->baseID = _id;
		te->position = 1;
	}
}

struct Matrix44Widget : KnobMatrixModuleWidget {
	static constexpr int hp = 10;

	Matrix44Widget(Matrix44* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Matrix44");
		createScrews();

		// generated by svg_widgets.rb
		auto mix11ParamPosition = Vec(17.2, 58.7);
		auto mix21ParamPosition = Vec(17.2, 115.2);
		auto mix31ParamPosition = Vec(17.2, 171.2);
		auto mix41ParamPosition = Vec(17.2, 227.2);
		auto mix12ParamPosition = Vec(49.2, 58.7);
		auto mix22ParamPosition = Vec(49.2, 115.2);
		auto mix32ParamPosition = Vec(49.2, 171.2);
		auto mix42ParamPosition = Vec(49.2, 227.2);
		auto mix13ParamPosition = Vec(81.2, 58.7);
		auto mix23ParamPosition = Vec(81.2, 115.2);
		auto mix33ParamPosition = Vec(81.2, 171.2);
		auto mix43ParamPosition = Vec(81.2, 227.2);
		auto mix14ParamPosition = Vec(113.2, 58.7);
		auto mix24ParamPosition = Vec(113.2, 115.2);
		auto mix34ParamPosition = Vec(113.2, 171.2);
		auto mix44ParamPosition = Vec(113.2, 227.2);

		auto in1InputPosition = Vec(15.0, 274.0);
		auto in2InputPosition = Vec(47.0, 274.0);
		auto in3InputPosition = Vec(79.0, 274.0);
		auto in4InputPosition = Vec(111.0, 274.0);

		auto out1OutputPosition = Vec(15.0, 318.0);
		auto out2OutputPosition = Vec(47.0, 318.0);
		auto out3OutputPosition = Vec(79.0, 318.0);
		auto out4OutputPosition = Vec(111.0, 318.0);
		// end generated by svg_widgets.rb

		createKnob(mix11ParamPosition, module, Matrix44::MIX11_PARAM);
		createKnob(mix21ParamPosition, module, Matrix44::MIX21_PARAM);
		createKnob(mix31ParamPosition, module, Matrix44::MIX31_PARAM);
		createKnob(mix41ParamPosition, module, Matrix44::MIX41_PARAM);
		createKnob(mix12ParamPosition, module, Matrix44::MIX12_PARAM);
		createKnob(mix22ParamPosition, module, Matrix44::MIX22_PARAM);
		createKnob(mix32ParamPosition, module, Matrix44::MIX32_PARAM);
		createKnob(mix42ParamPosition, module, Matrix44::MIX42_PARAM);
		createKnob(mix13ParamPosition, module, Matrix44::MIX13_PARAM);
		createKnob(mix23ParamPosition, module, Matrix44::MIX23_PARAM);
		createKnob(mix33ParamPosition, module, Matrix44::MIX33_PARAM);
		createKnob(mix43ParamPosition, module, Matrix44::MIX43_PARAM);
		createKnob(mix14ParamPosition, module, Matrix44::MIX14_PARAM);
		createKnob(mix24ParamPosition, module, Matrix44::MIX24_PARAM);
		createKnob(mix34ParamPosition, module, Matrix44::MIX34_PARAM);
		createKnob(mix44ParamPosition, module, Matrix44::MIX44_PARAM);

		addInput(createInput<Port24>(in1InputPosition, module, Matrix44::IN1_INPUT));
		addInput(createInput<Port24>(in2InputPosition, module, Matrix44::IN2_INPUT));
		addInput(createInput<Port24>(in3InputPosition, module, Matrix44::IN3_INPUT));
		addInput(createInput<Port24>(in4InputPosition, module, Matrix44::IN4_INPUT));

		addOutput(createOutput<Port24>(out1OutputPosition, module, Matrix44::OUT1_OUTPUT));
		addOutput(createOutput<Port24>(out2OutputPosition, module, Matrix44::OUT2_OUTPUT));
		addOutput(createOutput<Port24>(out3OutputPosition, module, Matrix44::OUT3_OUTPUT));
		addOutput(createOutput<Port24>(out4OutputPosition, module, Matrix44::OUT4_OUTPUT));
	}
};

Model* modelMatrix44 = createModel<Matrix44, Matrix44Widget>("Bogaudio-Matrix44", "MATRIX44", "4x4 matrix mixer", "Mixer", "Polyphonic");
