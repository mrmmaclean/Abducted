export const id = "EVENT_CRAWLER_CAMERA_WOBBLE";
export const name = "Crawler Camera Wobble";
export const groups = ["Crawler Events"];

export const fields = [
	{
		key: "wobbles",
		label: "Number of wobbles",
		type: "number",
		defaultValue: 3,
	},
];

export const compile = (input, helpers) => {
	const { _setConst, _setMemInt8, _invoke, _declareLocal } = helpers;

	const numWobbles = input.wobbles ?? 3;
	_setMemInt8("wobble_index", 0);
	_setMemInt8("counter", 0);

	const cameraWobbleArgsRef = _declareLocal("camera_wobble_args", 1, true);
	_setConst(cameraWobbleArgsRef, numWobbles);
	_invoke("camera_wobble_frames", 0, cameraWobbleArgsRef);
};
