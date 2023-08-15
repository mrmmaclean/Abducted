export const id = "EVENT_CRAWLER_ENEMY_STATS";
export const name = "Crawler Enemy Stats";
export const groups = ["Crawler Events"];

export const fields = [
	{
		key: "enemy",
		label: "Enemy variable",
		type: "variable",
	},
];

export const compile = (input, helpers) => {
	const { _stackPush, _callNative, _stackPop, getVariableAlias } = helpers;

	const enemy = getVariableAlias(input.enemy);

	_stackPush(enemy);
	_callNative("get_enemy_stats");
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
