export const id = "EVENT_CRAWLER_HERO_STATS";
export const name = "Crawler Hero Stats";
export const groups = ["Crawler Events"];

export const fields = [
	{
		key: "level",
		label: "Level variable",
		type: "variable",
	},
	{
		key: "shield",
		label: "Shield level variable",
		type: "variable",
	},
	{
		key: "armour",
		label: "Armour level variable",
		type: "variable",
	},
	{
		key: "weapon",
		label: "Weapon level variable",
		type: "variable",
	},
];

export const compile = (input, helpers) => {
	const { _stackPush, _callNative, _stackPop, getVariableAlias } = helpers;

	const level = getVariableAlias(input.level);
	const shield = getVariableAlias(input.shield);
	const armour = getVariableAlias(input.armour);
	const weapon = getVariableAlias(input.weapon);

	_stackPush(level);
	_stackPush(shield);
	_stackPush(armour);
	_stackPush(weapon);
	_callNative("get_stats");
	_stackPop(4);
};

module.exports = {
	id,
	name,
	groups,
	fields,
	compile,
	allowedBeforeInitFade: false,
};
