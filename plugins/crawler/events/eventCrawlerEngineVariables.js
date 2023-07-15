export const id = "EVENT_CRAWLER_ENGINE_VARIABLES";
export const name = "Crawler Engine Variables";
export const groups = ["Crawler Events"];

export const fields = [
	{
		label: "Crawler Engine Variables",
	},
	{
		key: "farVisibleVar",
		label: "crawler_sprite_far_visible",
		type: "variable",
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
	// {
	// 	key: "actorRef",
	// 	label: "crawler_actor",
	// 	type: "variable",
	// },
];

export const compile = (input, helpers) => {
	const { _getMemUInt8, _getMemInt16 } = helpers;
	_getMemUInt8(input.farVisibleVar, "crawler_sprite_far_visible");
	_getMemUInt8(input.middleVisibleVar, "crawler_sprite_middle_visible");
	_getMemUInt8(input.nearVisibleVar, "crawler_sprite_near_visible");
	// _getMemInt16(input.actorRef, "crawler_actor");
};
