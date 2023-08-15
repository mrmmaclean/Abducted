export const id = "EVENT_CRAWLER_ENGINE_VARIABLES";
export const name = "Crawler Engine Variables";
export const groups = ["Crawler Events"];

export const fields = [
	{
		label: "Crawler Engine Variables",
	},
	{
		key: "nearVisibleVar",
		label: "crawler_sprite_near_visible",
		type: "variable",
	},
	{
		key: "middleVisibleVar",
		label: "crawler_sprite_middle_visible",
		type: "variable",
	},
	{
		key: "farVisibleVar",
		label: "crawler_sprite_far_visible",
		type: "variable",
	},
];

export const compile = (input, helpers) => {
	const { getVariableAlias, appendRaw } = helpers;

	const nearVariableAlias = getVariableAlias(input.nearVisibleVar);
	const midVariableAlias = getVariableAlias(input.middleVisibleVar);
	const farVariableAlias = getVariableAlias(input.farVisibleVar);

	appendRaw(
		`VM_GET_UINT8 ${nearVariableAlias}, _crawler_sprite_near_visible
		VM_GET_UINT8 ${midVariableAlias}, _crawler_sprite_middle_visible
		VM_GET_UINT8 ${farVariableAlias}, _crawler_sprite_far_visible`
	);
};

module.exports = {
	id,
	name,
	groups,
	fields,
	compile,
	allowedBeforeInitFade: false,
};
