export const id = "EVENT_CRAWLER_ENEMY_FLASH";
export const name = "Crawler Enemy Flash";
export const groups = ["Crawler Events"];

export const fields = [
	{
		key: "flashes",
		label: "Number of flashes",
		type: "number",
		defaultValue: 5,
	},
];

export const compile = (input, helpers) => {
	const { _stackPushConst, _callNative, _stackPop } = helpers;
	const numFlashes = input.flashes ?? 5;

	if (numFlashes > 0) {
		_stackPushConst(numFlashes * 2); // Double so that palette is always returned to normal
		_callNative("flash_sprite");
		_stackPop(1);
	}
};
