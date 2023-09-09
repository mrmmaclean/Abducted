export const id = "EVENT_CRAWLER_FACE_OPEN_CELL";
export const name = "Crawler Face Open Cell";
export const groups = ["Crawler Events"];

export const fields = [
	{
		label: "Face open cell",
	},
	{
		key: "ret",
		type: "variable",
		label: "True if player turns",
	},
];

export const compile = (input, helpers) => {
	const { _callNative, appendRaw, getVariableAlias } = helpers;
	const retVal = getVariableAlias(input.ret);

	_callNative("get_is_facing_cell");
	appendRaw(`VM_GET_UINT8 ${retVal} _is_facing_cell`);
};

module.exports = {
	id,
	name,
	groups,
	fields,
	compile,
	allowedBeforeInitFade: false,
};
