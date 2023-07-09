export const id = "EVENT_CRAWLER_DOOR_FAR";
export const name = "Crawler Door Far";
export const groups = ["Crawler Events"];

export const fields = [
	{
		label: "Far Door",
	},
	{
		key: "actorId",
		label: "Door sprite",
		type: "actor",
		defaultValue: "$self$",
	},
	{
		key: "farVisibleVar",
		label: "Variable to save into",
		type: "variable",
	},
];

export const compile = (input, helpers) => {
	const { _getMemUInt8 } = helpers;
	_getMemUInt8(input.farVisibleVar, "door_far_visible");
};
