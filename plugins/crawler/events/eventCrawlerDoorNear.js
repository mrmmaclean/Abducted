export const id = "EVENT_CRAWLER_DOOR_NEAR";
export const name = "Crawler Door Near";
export const groups = ["Crawler Events"];

export const fields = [
	{
		label: "Near Door",
	},
	{
		key: "actorId",
		label: "Door sprite",
		type: "actor",
		defaultValue: "$self$",
	},
	{
		key: "nearVisibleVar",
		label: "Variable to save into",
		type: "variable",
	},
];

export const compile = (input, helpers) => {
	const { _getMemUInt8 } = helpers;
	_getMemUInt8(input.nearVisibleVar, "door_near_visible");
};
