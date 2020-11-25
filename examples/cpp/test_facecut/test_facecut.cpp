/**
	\file test_facecut.cpp
	\brief Example of face capturing, quality, emotions, age and gender estimation, and face cutting.
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>



#include <facerec/import.h>
#include <facerec/libfacerec.h>


void load_file (const std::string path, std::vector<uint8_t> *data);

int main(int argc, char const *argv[])
{
	try
	{

		if(argc < 4)
		{
			std::cout << "Usage: '" << argv[0] << " <dll_path> <conf_dir_path> <image1_path> [<image2_path> [...] ]'" << std::endl;
			std::cout << "Example: '" << argv[0] << " ../lib/libfacerec.so ../conf/facerec ~/Pictures/1.png'" << std::endl;
			return 0;
		}

		const std::string dll_path = argv[1];
		const std::string conf_dir_path = argv[2];

		std::vector<std::string> image_files;
		for(int i = 3; i < argc; ++i)
			image_files.push_back(argv[i]);



		// create facerec service (initialize facerec)
		const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(dll_path, conf_dir_path);

		// create capturer
		const pbio::Capturer::Ptr capturer = service->createCapturer("common_capturer_blf_fda_back.xml");


		// create quality estimator
		const pbio::QualityEstimator::Ptr quality_estimator = service->createQualityEstimator("quality_estimator_iso.xml");

		// create age and gender estimator
		const pbio::AgeGenderEstimator::Ptr age_gender_estimator = service->createAgeGenderEstimator("age_gender_estimator.xml");
		//const pbio::AgeGenderEstimator::Ptr age_gender_estimator = service->createAgeGenderEstimator("age_gender_estimator_v2.xml");

		// create emotions estimator
		const pbio::EmotionsEstimator::Ptr emotions_estimator = service->createEmotionsEstimator("emotions_estimator.xml");

		for(size_t file_i = 0; file_i < image_files.size(); ++file_i)
		{
			const std::string filepath = image_files[file_i];

			std::cout << "\n\n\nProcessing file '" << filepath << "' ..." << std::endl;

			// load image file
			std::vector<uint8_t> file_data;
			load_file(filepath, &file_data);

			// capture face samples
			std::vector<pbio::RawSample::Ptr> samples = capturer->capture(file_data.data(), file_data.size());

			std::cout << samples.size() << " face(s) captured" << std::endl;

			for(size_t i = 0; i < samples.size(); ++i)
			{
				// get sample info
				const pbio::RawSample::Rectangle rect = samples[i]->getRectangle();
				const pbio::RawSample::Point left_eye = samples[i]->getLeftEye();
				const pbio::RawSample::Point right_eye = samples[i]->getRightEye();
				const pbio::RawSample::Angles angles = samples[i]->getAngles();
				const std::vector<pbio::RawSample::Point> all_points = samples[i]->getLandmarks();

				// estimate quality
				const pbio::QualityEstimator::Quality quality = quality_estimator->estimateQuality(*samples[i]);

				// estimate age and gender
				const pbio::AgeGenderEstimator::AgeGender age_gender = age_gender_estimator->estimateAgeGender(*samples[i]);

				// estimate emotions
				const std::vector<pbio::EmotionsEstimator::EmotionConfidence> emotions =
					emotions_estimator->estimateEmotions(*samples[i]);

				// print sample info
				std::cout << "\nsample " << i << ":\n" <<
					" rect: " << rect.width << "x" << rect.height << " from point (" << rect.x << "; " << rect.y << ")\n" <<
					" left_eye: (" << left_eye.x << "; " << left_eye.y << ")\n" <<
					" right_eye: (" << right_eye.x << "; " << right_eye.y << ")\n" <<
					" angles:\n  yaw: " << angles.yaw << "\n  pitch: " << angles.pitch << "\n  roll: " << angles.roll << "\n" <<
					" all landmarks count: " << all_points.size() << "\n" << std::endl;


				// print quality
				std::cout << " total quality: " << quality.total << "\n" <<
					"  lighting quality: " << quality.lighting << "\n" <<
					"  noise quality: " << quality.noise << "\n" <<
					"  sharpness quality: " << quality.sharpness << "\n" <<
					"  flare quality: " << quality.flare << "\n" << std::endl;


				// print age
				std::cout << " age: ";
				switch(age_gender.age)
				{
					case pbio::AgeGenderEstimator::AGE_KID:    std::cout << "kid";    break;
					case pbio::AgeGenderEstimator::AGE_YOUNG:  std::cout << "young";  break;
					case pbio::AgeGenderEstimator::AGE_ADULT:  std::cout << "adult";  break;
					case pbio::AgeGenderEstimator::AGE_SENIOR: std::cout << "senior"; break;
				}

				std::cout << "\n" << std::endl;

				// print gender
				std::cout << " gender: ";
				switch(age_gender.gender)
				{
					case pbio::AgeGenderEstimator::GENDER_FEMALE: std::cout << "female"; break;
					case pbio::AgeGenderEstimator::GENDER_MALE:   std::cout << "male";   break;
				}

				std::cout << "\n" << std::endl;

				// print emotions
				std::cout << " emotions: \n";

				for(size_t j = 0; j < emotions.size(); ++j)
				{
					const pbio::EmotionsEstimator::Emotion emotion = emotions[j].emotion;
					const float confidence = emotions[j].confidence;

					switch(emotion)
					{
						case pbio::EmotionsEstimator::EMOTION_NEUTRAL:  std::cout << "  neutral";  break;
						case pbio::EmotionsEstimator::EMOTION_HAPPY:    std::cout << "  happy";    break;
						case pbio::EmotionsEstimator::EMOTION_ANGRY:    std::cout << "  angry";    break;
						case pbio::EmotionsEstimator::EMOTION_SURPRISE: std::cout << "  surprise"; break;
					}

					std::cout << " " << int(confidence * 100) << "%" << std::endl;
				}

				std::cout << std::endl;


				// cut face
				std::ostringstream file_to_save;
				file_to_save << filepath << ".face_" << i << ".jpg";

				std::ofstream cut_file(file_to_save.str().c_str(), std::ios_base::binary);
				if(!cut_file.is_open())
				{
					std::cout << "can't open '" << file_to_save.str() << "' for save face cut" << std::endl;
				}
				else
				{
					samples[i]->cutFaceImage(
						cut_file,
						pbio::RawSample::IMAGE_FORMAT_JPG,
						pbio::RawSample::FACE_CUT_FULL_FRONTAL);

					if(cut_file.good())
					{
						std::cout << "cutted face image saved in '" << file_to_save.str() << "'" << std::endl;
					}
					else
					{
						std::cout << "error saving cutted face image in '" << file_to_save.str() << "'" << std::endl;
					}
				}
			}

		}

	}
	catch(const pbio::Error &e)
	{
		std::cerr << "facerec exception catched: '" << e.what() << "' code: " << std::hex << e.code() << std::endl;
	}
	catch(const std::exception &e)
	{
		std::cerr << "exception catched: '" << e.what() << "'" << std::endl;
	}

	return 0;
}


void load_file(const std::string path, std::vector<uint8_t> *data)
{
	if(!data)
	{
		throw std::runtime_error("load_file: got NULL data pointer");
	}

	data->clear();

	std::ifstream file(path.c_str(), std::ios_base::binary);

	if(!file.is_open())
	{
		throw std::runtime_error("load_file: file '" + path + "' not opened");
	}

	file.seekg(0, file.end);
	const size_t len = (size_t) file.tellg();

	file.seekg(0, file.beg);

	data->resize(len);

	file.read((char*)data->data(), len);

	if(!file.good())
	{
		throw std::runtime_error("load_file: error reading file '" + path + "'");
	}
}
