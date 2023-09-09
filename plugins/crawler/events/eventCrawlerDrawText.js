export const id = "EVENT_CRAWLER_DRAW_TEXT";
export const name = "Crawler Draw Text";
export const groups = ["Crawler Events"];

export const fields = [
	{
		key: "value",
		label: "Variable to draw to background",
		type: "variable",
		defaultValue: 0,
	},
];

export const compile = (input, helpers) => {
	const {
		getVariableAlias,
		appendRaw,
		_loadStructuredText,
		_displayText,
		_overlayWait,
		_addNL,
	} = helpers;

	const val = getVariableAlias(input.value);

	appendRaw(`VM_SWITCH_TEXT_LAYER .TEXT_LAYER_BKG`);

	appendRaw(`; Text
VM_LOAD_TEXT 1
.dw WAL_${val}
.asciz 003\\000\\022%d
`);

	_displayText();

	_overlayWait(false, [".UI_WAIT_TEXT"]);

	_addNL();

	appendRaw(`VM_SWITCH_TEXT_LAYER .TEXT_LAYER_WIN`);
};

module.exports = {
	id,
	name,
	groups,
	fields,
	compile,
	allowedBeforeInitFade: false,
};
