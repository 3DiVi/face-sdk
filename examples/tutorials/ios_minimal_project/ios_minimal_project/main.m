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


BOOL copyLicense(NSString *srcDir, NSString *dstDir)
{
	NSString *dstLicenseFile = [dstDir stringByAppendingPathComponent:@"3divi_face_sdk.lic"];
	NSFileManager* fileMgr = [NSFileManager defaultManager];
	if ([fileMgr fileExistsAtPath:dstLicenseFile])
	{
		NSLog(@"The license file already exists in the target directory");
		return YES;
	}

	NSError *error;
	// create destination directory
	if (![fileMgr createDirectoryAtPath:dstDir
		withIntermediateDirectories:YES
		attributes:nil
		error:&error])
	{
		NSLog(@"Create license directory error: %@", error);
		return NO;
	}

	// copy license and config files
	if (![fileMgr copyItemAtPath:[srcDir stringByAppendingPathComponent:@"3divi_face_sdk.lic"]
		toPath:dstLicenseFile
		error:&error])
	{
		NSLog(@"Copy file error: %@", error);
		return NO;
	}
	if (![fileMgr copyItemAtPath:[srcDir stringByAppendingPathComponent:@"net_params.xml"]
		toPath:[dstDir stringByAppendingPathComponent:@"net_params.xml"]
		error:&error])
	{
	    NSLog(@"Copy file error: %@", error);
	    return NO;
	}
	if (![fileMgr copyItemAtPath:[srcDir stringByAppendingPathComponent:@"log_params.xml"]
		toPath:[dstDir stringByAppendingPathComponent:@"log_params.xml"]
		error:&error])
	{
		NSLog(@"Copy file error: %@", error);
		return NO;
	}

	return YES;
};


int main(int argc, char * argv[]) {

	std::cout << "Start application" << std::endl;

	std::cout << "Copy license to writable directory" << std::endl;
	// FaceSDK requires write permission to the license directory
	// copy the license from the resource directory to the application support directory
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	NSString *applicationSupportDirectory = [paths firstObject];
	NSString *dstLicenseDir = [applicationSupportDirectory  stringByAppendingPathComponent:@"license"];
	NSString *srcLicenseDir = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"license"];
	if(!copyLicense(srcLicenseDir, dstLicenseDir))
	{
		std::cout << "Something went wrong while copying the license to a writable location" << std::endl;
		return -1;
	};

	const std::string frameworks_dir = [[[NSBundle mainBundle] privateFrameworksPath] UTF8String];
	const std::string resources_dir = [[[NSBundle mainBundle] resourcePath] UTF8String];

	std::cout << "Create FacerecService" << std::endl;
	const pbio::FacerecService::Ptr service = pbio::FacerecService::createService(
		frameworks_dir + "/facerec.framework/libfacerec.dylib",
		resources_dir + "/conf/facerec",
		[dstLicenseDir UTF8String]);

	std::cout << "Get license state" << std::endl;
	const pbio::FacerecService::LicenseState ls = service->getLicenseState();
	std::cout << "ls.online         : "  << ls.online         << std::endl;
	std::cout << "ls.android_app_id : "  << ls.android_app_id << std::endl;
	std::cout << "ls.ios_app_id     : "  << ls.ios_app_id     << std::endl;
	std::cout << "ls.hardware_reg   : "  << ls.hardware_reg   << std::endl;

	std::cout << "Create Capturer" << std::endl;
	const pbio::Capturer::Ptr capturer = service->createCapturer("fda_fake_detector_singleface.xml");

	std::cout << "Read image" << std::endl;
	std::vector<char> bytes;
	std::ifstream file((resources_dir + "/01100.jpg").c_str(), std::ios_base::binary);
	file.seekg(0, file.end);
	bytes.resize(file.tellg());
	file.seekg(0, file.beg);
	file.read(bytes.data(), bytes.size());
	file.close();


	std::cout << "Detect faces" << std::endl;
	const std::vector<pbio::RawSample::Ptr> samples = capturer->capture((const unsigned char*)bytes.data(), bytes.size());

	std::cout << samples.size() << " faces detected" << std::endl;

	for(const pbio::RawSample::Ptr &sample : samples)
	{
		const std::vector<pbio::Point> pts = sample->getLandmarks();
		for(const pbio::Point &p : pts)
			std::cout << "(" << p.x << ", " << p.y << "), ";
		std::cout << std::endl;
	}

	std::cout << "Done" << std::endl;

	@autoreleasepool {
		return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	}
}
