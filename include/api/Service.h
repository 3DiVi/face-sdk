#ifndef API_SERVICE_H
#define API_SERVICE_H

#include <pbio/FacerecService.h>


namespace api
{

	using namespace pbio;

class Service: public pbio::FacerecService
{

public:
	ProcessingBlock createProcessingBlock(const Context& ctx);
	static Service createService(std::string path_to_dir);
private:
	typedef LightSmartPtr<import::DllHandle>::tPtr DHPtr;
	Service(
		const DHPtr &dll_handle,
		const std::string &facerec_conf_dir,
		void* impl,
		std::string path_to_dir): FacerecService(dll_handle, facerec_conf_dir, impl), path_to_dir(path_to_dir) {}
	std::string path_to_dir;
};

inline ProcessingBlock Service::createProcessingBlock(const Context& ctx)
{
	Context new_ctx = ctx;
	new_ctx["@sdk_path"] = path_to_dir;
	return FacerecService::createProcessingBlock(new_ctx);
}

inline Service Service::createService(std::string path_to_dir)
{
	std::string facerec_conf_dir = path_to_dir + "/conf/facerec/";
	std::string license = path_to_dir + "/license/";

#ifdef WIN32
	std::string dll_path = path_to_dir + "\\bin\\facerec.dll";
#else
	std::string dll_path = path_to_dir + "/lib/libfacerec.so";
#endif

#ifdef __STATIC_LIBFACEREC_BUILD__
	(void) dll_path;

	const DHPtr dll_handle( DHPtr::make() );
#else
	const DHPtr dll_handle( DHPtr::make(dll_path.c_str()) );
#endif

	void* exception = NULL;

	void* the_impl;

	the_impl = dll_handle->FacerecService_constructor2(
			facerec_conf_dir.c_str(),
			license.c_str(),
			dll_path.c_str(),
			&exception);

	checkException(exception, *dll_handle);

	return Service(dll_handle, facerec_conf_dir, the_impl, path_to_dir);
}

}

#endif // API_SERVICE_H
