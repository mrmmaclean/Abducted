export const id = "EVENT_CRAWLER_GET_TREASURE_CHEST";
export const name = "Crawler Get Treasure Chest";
export const groups = ["Crawler Events"];

export const fields = [
	{
		key: "zone",
		label: "Zone variable",
		type: "variable",
	},
];

export const compile = (input, helpers) => {
	const { _stackPush, _callNative, _stackPop, getVariableAlias } = helpers;
	const zone = getVariableAlias(input.zone);

	_stackPush(zone);
	_callNative("get_treasure_chest");
	_stackPop(1);
};

module.exports = {
	id,
	name,
	groups,
	fields,
	compile,
	allowedBeforeInitFade: false,
};
