//
//  main.m
//  ios_minimal_project
//
//  Created by 3DiVi on 20.02.2020.
//  Copyright © 2020 3divi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"


#include "pbio/FacerecService.h"

#include <vector>
#include <fstream>
#include <iostream>


int main(int argc, char * argv[]) {


	std::cout << "log 1" << std::endl;

	const std::string frameworks_dir = [[[NSBundle mainBundle] privateFrameworksPath] UTF8String];
	const std::string resources_dir = [[[NSBundle mainBundle] resourcePath] UTF8String];

	const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(
		frameworks_dir + "/facerec.framework/libfacerec.dylib",
		resources_dir + "/conf/facerec");

	std::cout << "log 2" << std::endl;

	const pbio::FacerecService::LicenseState ls = service->getLicenseState();

	std::cout << "ls.online         : "  << ls.online         << std::endl;
	std::cout << "ls.android_app_id : "  << ls.android_app_id << std::endl;
	std::cout << "ls.ios_app_id     : "  << ls.ios_app_id     << std::endl;
	std::cout << "ls.hardware_reg   : "  << ls.hardware_reg   << std::endl;

	std::cout << "log 3" << std::endl;

	const pbio::Capturer::Ptr capturer = service->createCapturer("fda_fake_detector_singleface.xml");

	std::cout << "log 4" << std::endl;

	std::vector<char> bytes;
	std::ifstream file((resources_dir + "/01100.jpg").c_str(), std::ios_base::binary);
	file.seekg(0, file.end);
	bytes.resize(file.tellg());
	file.seekg(0, file.beg);
	file.read(bytes.data(), bytes.size());
	file.close();

	std::cout << "log 5" << std::endl;

	const std::vector<pbio::RawSample::Ptr> samples = capturer->capture((const unsigned char*)bytes.data(), bytes.size());

	std::cout << "samples.size: " << samples.size() << std::endl;

	for(const pbio::RawSample::Ptr &sample : samples)
	{
		const std::vector<pbio::Point> pts = sample->getLandmarks();
		for(const pbio::Point &p : pts)
			std::cout << " " << p.x << " " << p.y << std::endl;
	}

	std::cout << "log 6" << std::endl;

	@autoreleasepool {
		return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	}
}
