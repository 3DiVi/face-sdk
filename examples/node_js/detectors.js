const facerec = require('face_sdk_3divi');
const yargs = require('yargs');
const fs = require('fs');

const unit_types =
{
	"face": "FACE_DETECTOR",
	"body": "HUMAN_BODY_DETECTOR",
	"objects": "OBJECT_DETECTOR"
};

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

		console.log(`Confidence: ${obj.get("confidence").value.toFixed(2)}`);
		console.log(`Class: ${obj.get("class").value}\n`);
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
		.option('unit_type', {type: 'string', default: 'face', describe: 'Unit type',})
		.option('modification', {type: 'string', default: '', describe: 'Modification string',})
		.option('version', {type: 'number', default: -1, describe: 'Model version to use',})
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
		const unit_type = argv.unit_type;
		const modification = argv.modification;
		const version = argv.version;
		const use_cuda = argv.use_cuda;

		let detector_config = new facerec.Context();
		detector_config.get("unit_type").value = unit_types[unit_type];
		detector_config.get("use_cuda").value = use_cuda;

		if (modification !== '')
			detector_config.get("modification").value = modification;

		if (version > 0)
			detector_config.get("version").value = version;

		const detector = new facerec.ProcessingBlock(detector_config);

		const buffer = fs.readFileSync(image_path);
		let ioData = new facerec.Context(buffer);

		console.time("detection time");
		detector.process(ioData);
		console.timeEnd("detection time");

		print_output(ioData);

	} catch (err) {
		console.log(err)
	}
}

main();