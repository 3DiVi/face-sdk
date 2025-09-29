const facerec = require('face_sdk_3divi');
const yargs = require('yargs');
const fs = require('fs');

const unit_types =
{
	"emotions": "EMOTION_ESTIMATOR",
	"age": "AGE_ESTIMATOR",
	"gender": "GENDER_ESTIMATOR",
	"mask": "MASK_ESTIMATOR",
	"glasses": "GLASSES_ESTIMATOR",
	"eye_openness": "EYE_OPENNESS_ESTIMATOR",
	"liveness": "LIVENESS_ESTIMATOR",
	"deepfake": "DEEPFAKE_ESTIMATOR",
	"quality": "QUALITY_ASSESSMENT_ESTIMATOR"
};

function print_output(data, print_function)
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
		print_function(obj);
		console.log("\n")
	}
}

function print_emotions(obj)
{
	for (const emotion_object of obj.get("emotions"))
	{
		const emotion = emotion_object.get("emotion").value;
		const confidence = emotion_object.get("confidence").value.toFixed(2);
		console.log(`${emotion}: ${confidence}`);
	}
}

function print_age(obj)
{
	console.log(`Age: ${obj.get("age").value}`);
}


function print_gender(obj)
{
	console.log(`Gender: ${obj.get("gender").value}`);
}

function print_mask(obj)
{
	const has_medical_mask = obj.get('has_medical_mask');
	console.log(`Has mask: ${has_medical_mask.get("value").value}`);
	console.log(`Confidence: ${has_medical_mask.get("confidence").value.toFixed(2)}`);
}

function print_glasses(obj)
{
	const glasses = obj.get('glasses');
	console.log(`Has glasses: ${glasses.get("value").value}`);
	console.log(`Confidence: ${glasses.get("confidence").value.toFixed(2)}`);
}

function print_deepfake(obj)
{
	const deepfake = obj.get('deepfake');
	console.log(`Has deepfake: ${deepfake.get("value").value}`);
	console.log(`Confidence: ${deepfake.get("confidence").value.toFixed(2)}`);
}

function print_eye_openness(obj)
{
	const is_right_eye_open = obj.get('is_right_eye_open');
	const is_left_eye_open = obj.get('is_left_eye_open');

	console.log(`Right eye open: ${is_right_eye_open.get("value").value}`);
	console.log(`Right eye confidence: ${is_right_eye_open.get("confidence").value.toFixed(2)}`);
	console.log(`Left eye open: ${is_left_eye_open.get("value").value}`);
	console.log(`Left eye confidence: ${is_left_eye_open.get("confidence").value.toFixed(2)}`);
}

function print_liveness(obj)
{
	const liveness = obj.get('liveness');
	console.log(`Liveness: ${liveness.get("value").value}`);
	console.log(`Confidence: ${liveness.get("confidence").value.toFixed(2)}`);
}

function print_quality(obj)
{
	console.log(`Quality score: ${obj.get('quality').get("total_score").value}`);
}

const print_functions =
{
	"emotions": print_emotions,
	"age": print_age,
	"gender": print_gender,
	"mask": print_mask,
	"glasses": print_glasses,
	"eye_openness": print_eye_openness,
	"liveness": print_liveness,
	"deepfake": print_deepfake,
	"quality": print_quality,
}

function parse_args()
{
	return yargs
		.version(false)
		.usage('Usage: $0 --input_image <path> [options]')
		.option('input_image', {type: 'string', describe: 'Path to input image', demandOption: true})
		.option('sdk_path', {type: 'string', default: '../../', describe: 'Path to the SDK'})
		.option('use_cuda', {type: 'boolean', default: false, describe: 'Use CUDA acceleration',})
		.option('unit_type', {type: 'string', default: 'age', describe: 'Unit type',})
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

		if (!(unit_type in unit_types)) {
			throw new Error(`unknown: ${unit_type}`);
		}

		let detector_config = new facerec.Context();
		detector_config.get("unit_type").value = "FACE_DETECTOR";
		detector_config.get("modification").value = "ssyv_light";
		detector_config.get("use_cuda").value = use_cuda;

		let fitter_config = new facerec.Context();
		fitter_config.get("unit_type").value = "FACE_FITTER";
		fitter_config.get("use_cuda").value = use_cuda;

		let estimator_config = new facerec.Context();
		estimator_config.get("unit_type").value = unit_types[unit_type];
		estimator_config.get("use_cuda").value = use_cuda;

		if (modification !== '')
			estimator_config.get("modification").value = modification;

		if (version > 0)
			estimator_config.get("version").value = version;


		const detector = new facerec.ProcessingBlock(detector_config);
		const fitter = new facerec.ProcessingBlock(fitter_config);
		const estimator = new facerec.ProcessingBlock(estimator_config);

		const buffer = fs.readFileSync(image_path);
		let ioData = new facerec.Context(buffer);

		console.time("detection time");
		detector.process(ioData);
		fitter.process(ioData);
		console.timeEnd("detection time");

		console.time("estimation time");
		estimator.process(ioData);
		console.timeEnd("estimation time");

		print_output(ioData, print_functions[unit_type]);

	} catch (err) {
		console.log(err.message)
	}
}

main();