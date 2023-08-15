export const id = "EVENT_CRAWLER_HERO_STATS";
export const name = "Crawler Hero Stats";
export const groups = ["Crawler Events"];

export const fields = [
	{
		key: "level",
		label: "Level variable",
		type: "variable",
	},
];

export const compile = (input, helpers) => {
	const { _stackPush, _callNative, _stackPop, getVariableAlias } = helpers;

	const level = getVariableAlias(input.level);

	_stackPush(level);
	_callNative("get_stats");
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
