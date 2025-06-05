const facerec = require('face_sdk_3divi');
const yargs = require('yargs');
const fs = require('fs');

function print_output(data)
{
	const image_shape = data.get("image").get("shape");
	const height = Number(image_shape.get(0).value);
	const width = Number(image_shape.get(1).value);
	for (const obj of data.get("objects"))
	{
		const bbox = obj.get("bbox");
		console.log("Bounding box: " +
			`(${Math.round(bbox.get(0).value * width)}, ${Math.round(bbox.get(1).value * height)}), ` +
			`(${Math.round(bbox.get(2).value * width)}, ${Math.round(bbox.get(3).value * height)})`);

		const keypoints = obj.get("keypoints");
		const keys = keypoints.getKeys().filter(item => (item !== 'points' && item !== "fitter_type"));
		console.log("Named points:")
		for (const key of keys)
		{
			const point = keypoints.get(key).get("proj");
			console.log(`\t${key}: (${Math.round(point.get(0).value * width)}, ${Math.round(point.get(1).value * height)})`);
		}

		const pose = obj.get("pose");
		console.log("Angles:")
		console.log(`\tYaw: ${pose.get("yaw").value}`)
		console.log(`\tPitch: ${pose.get("pitch").value}`)
		console.log(`\tRoll: ${pose.get("roll").value}`)

		console.log("\n")
	}
}

function parse_args()
{
	return yargs
		.version(false)
		.usage('Usage: $0 --input_image <path> [options]')
		.option('input_image', {type: 'string', describe: 'Path to input image', demandOption: true})
		.option('sdk_path', {type: 'string', default: '../../', describe: 'Path to the SDK'})
		.option('use_cuda', {type: 'boolean', default: false, describe: 'Use CUDA acceleration',})
		.option('modification', {type: 'string', default: '', describe: 'Modification string',})
		.help()
		.argv
}

function main()
{
	try {
		const argv = parse_args();
		const sdk_path = argv.sdk_path;
		const dll_path = sdk_path + "/lib/libfacerec.so";
		const conf_path = sdk_path + "/conf/facerec";
		const license_path = sdk_path + "/license";

		facerec.Init(dll_path, conf_path, license_path);

		const image_path = argv.input_image;
		const modification = argv.modification;
		const use_cuda = argv.use_cuda;

		let detector_config = new facerec.Context();
		detector_config.get("unit_type").value = "FACE_DETECTOR";
		detector_config.get("modification").value = "ssyv_light";
		detector_config.get("use_cuda").value = use_cuda;

		let fitter_config = new facerec.Context();
		fitter_config.get("unit_type").value = "FACE_FITTER";
		fitter_config.get("use_cuda").value = use_cuda;

		if (modification !== '')
			fitter_config.get("modification").value = modification;

		const detector = new facerec.ProcessingBlock(detector_config);
		const fitter = new facerec.ProcessingBlock(fitter_config);

		const buffer = fs.readFileSync(image_path);
		let ioData = new facerec.Context(buffer);

		console.time("detection time");
		detector.process(ioData);
		console.timeEnd("detection time");

		console.time("fitter time");
		fitter.process(ioData);
		console.timeEnd("fitter time");


		print_output(ioData);

	} catch (err) {
		console.log(err)
	}
}

main();