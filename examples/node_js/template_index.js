const facerec = require('face_sdk_3divi');
const yargs = require('yargs');
const fs = require('fs');
const path = require('path');

function parse_args()
{
	return yargs
		.version(false)
		.usage('Usage: $0 --input_image <path> [options]')
		.option('input_image', {type: 'string', describe: 'Path to input image', demandOption: true})
		.option('images_path_dir', {type: 'string', describe: 'Path to dir with images', demandOption: true})
		.option('sdk_path', {type: 'string', default: '../../', describe: 'Path to the SDK'})
		.option('use_cuda', {type: 'boolean', default: false, describe: 'Use CUDA acceleration',})
		.option('modification', {type: 'string', default: '100', describe: 'Modification string',})
		.help()
		.argv
}


function createTemplateIndex(imageDirPath, modification, pipeline)
{
	const index_config = new facerec.Context();
	index_config.get("modification").value = modification
	index_config.get("version").value = 1
	let template_index = new facerec.DynamicTemplateIndex(index_config);

	const files = fs.readdirSync(imageDirPath);

	const imageExtensions = ['.jpg', '.jpeg', '.png', '.gif', '.bmp', '.webp'];
	const imageFiles = files.filter(file => {
		const ext = path.extname(file).toLowerCase();
		return imageExtensions.includes(ext);
	});

	imageFiles.forEach(image_name => {
		const data = fs.readFileSync(`${imageDirPath}/${image_name}`);
		let ioData = new facerec.Context(data);

		pipeline.forEach(pb => {
			pb.process(ioData);
		});

		template_index.add(
			ioData.get("objects").get(0).get("face_template").get("template").value,
			image_name
		);
	});

	return template_index;
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

		const input_image = argv.input_image;
		const modification = argv.modification;
		const use_cuda = argv.use_cuda;

		fontScale = argv.font_scale;

		let detector_config = new facerec.Context();
		detector_config.get("unit_type").value = "FACE_DETECTOR";
		detector_config.get("modification").value = "ssyv_light";
		detector_config.get("use_cuda").value = use_cuda;

		let fitter_config = new facerec.Context();
		fitter_config.get("unit_type").value = "FACE_FITTER";
		fitter_config.get("use_cuda").value = use_cuda;

		let extractor_config = new facerec.Context();
		extractor_config.get("unit_type").value = "FACE_TEMPLATE_EXTRACTOR";
		extractor_config.get("use_cuda").value = use_cuda;
		extractor_config.get("modification").value = modification;

		const pipeline = [
			new facerec.ProcessingBlock(detector_config),
			new facerec.ProcessingBlock(fitter_config),
			new facerec.ProcessingBlock(extractor_config),
		];

		const index_config = new facerec.Context();
		index_config.get("modification").value = modification
		index_config.get("version").value = 1
		const template_index = new facerec.DynamicTemplateIndex(index_config);

		const images_path_dir = argv.images_path_dir;

		template_index.concat(createTemplateIndex(images_path_dir, modification, pipeline));


		const data = fs.readFileSync(input_image);
		let ioData = new facerec.Context(data);
		pipeline.forEach(pb => { pb.process(ioData); });
		for (const pb of pipeline)
		{
			pb.process(ioData);
		}

		const matcher_config = new facerec.Context();
		matcher_config.get("unit_type").value = "MATCHER_MODULE";
		matcher_config.get("modification").value = modification;
		const matcher = new facerec.ProcessingBlock(matcher_config);

		let matcherData = new facerec.Context();
		matcherData.get("template_index").value = template_index;
		matcherData.get("queries").value = ioData.get("objects").get(0).get("face_template").get("template");

		matcher.process(matcherData);

		const result = matcherData.get("results").get(0);
		console.log(`UUID: ${result.get("uuid").value}`)
		console.log(`Score: ${result.get("score").value}`)
	} catch (err) {
		console.log(err)
	}
}

main();