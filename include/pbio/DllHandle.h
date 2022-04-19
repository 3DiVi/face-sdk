#ifndef __PBIO_DLL_HANDLE_H_989601403fee4b0d83e85ee9b5292036
#define __PBIO_DLL_HANDLE_H_989601403fee4b0d83e85ee9b5292036

//! @cond IGNORED

#include <string>
#include <sstream>
#include <iomanip>

#include "c_api_functions_list_macro.h"

#include "Error.h"
#include "SmartPtr.h"


#ifndef __STATIC_LIBFACEREC_BUILD__
	#ifdef _WIN32
		#include <windows.h>
	#else
		#include <dlfcn.h>
	#endif
#endif



#ifdef __STATIC_LIBFACEREC_BUILD__
///////
// with __STATIC_LIBFACEREC_BUILD__

#ifndef LEGACY_METASDK

	#define __META_STATIC_DECL(rtype, name, typed_args, args, return) \
		extern "C" rtype name typed_args;

	__TDV_METASDK_FLIST(__META_STATIC_DECL)
#endif

	#define __583e_STATIC_DECL(rtype, name, typed_args, args, return) \
		rtype _583e_ADD_NAMESPACE(name) typed_args;

	__583e_FLIST(__583e_STATIC_DECL)
	__TDV_FLIST(__583e_STATIC_DECL)

// with __STATIC_LIBFACEREC_BUILD__
///////
#endif

namespace pbio {
namespace import {

class DllHandle
{

private:
	int32_t refcounter4light_shared_ptr;

	friend class object_with_ref_counter<DllHandle>;


#ifdef __STATIC_LIBFACEREC_BUILD__
///////
// with __STATIC_LIBFACEREC_BUILD__

private:
	const int _placeholder;

public:
	#define __583e_STATIC_F_INIT(rtype, name, typed_args, args, return) \
		, name( _583e_ADD_NAMESPACE(name) )
	#define __META_STATIC_F_INIT(rtype, name, typed_args, args, return) \
		, name( ::name )

	DllHandle() :
		_placeholder(0)
		__583e_FLIST(__583e_STATIC_F_INIT)
		__TDV_FLIST(__583e_STATIC_F_INIT)
#ifndef LEGACY_METASDK
		__TDV_METASDK_FLIST(__META_STATIC_F_INIT)
#endif
	{
		// nothing else
	}

// with __STATIC_LIBFACEREC_BUILD__
///////
#else
///////
// without __STATIC_LIBFACEREC_BUILD__

public:

	#define __583e_SHARED_F_INIT(rtype, name, typed_args, args, return) \
		, name( (FuncType_##name) getSymbol( _583e_STR_ADD_NAMESPACE(name) ) )

	#define __TDV_SHARED_F_INIT(rtype, name, typed_args, args, return) \
		, name( (FuncType_##name) getSymbol( _583e_STRINGISE2(name) ) )

	DllHandle(const char* const dll_path) :
		_dll_path(dll_path),
#ifdef _WIN32
		_dll_sense4( loadDll(make_sense4_dll_path(dll_path).c_str()) ),
#endif
		_dll( loadDll(dll_path) )
		__583e_FLIST(__583e_SHARED_F_INIT)
		__TDV_FLIST(__TDV_SHARED_F_INIT)
#ifndef LEGACY_METASDK
		__TDV_METASDK_FLIST(__TDV_SHARED_F_INIT)
#endif
	{
		// nothig else
	}

	~DllHandle()
	{
		#if defined(_WIN32)
			FreeLibrary(_dll);
			FreeLibrary(_dll_sense4);
		#else
			dlclose(_dll);
		#endif
	}

private:

	#ifndef _WIN32
		typedef void* HINSTANCE;
	#endif

	#ifdef _WIN32
	static
	std::string make_sense4_dll_path(char const* const dll_path)
	{
		int pos = -1;
		for(int i = 0; dll_path[i]; ++i)
			if(dll_path[i] == '/' || dll_path[i] == '\\')
				pos = i;

		return std::string(dll_path, dll_path + (pos + 1)) + "sense4.dll";
	}
	#endif

	static
	HINSTANCE loadDll(char const* const dll_path)
	{
		#if defined(_WIN32)
			HINSTANCE const result = LoadLibraryA(dll_path);
		#else
			HINSTANCE const result = dlopen( dll_path, RTLD_NOW|RTLD_LOCAL);
		#endif

		if(!result)
		{
			std::string error;
			#if defined(_WIN32)
			{
				char error_buffer[1024];

				const DWORD err_code = GetLastError();

				std::ostringstream oss;

				oss << " last error: " << std::hex << err_code;

				if(FormatMessageA(
					/* dwFlags       = */ FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					/* lpSource      = */ NULL,
					/* dwMessageId   = */ err_code,
					/* dwLanguageId  = */ 0,
					/* lpBuffer      = */ error_buffer,
					/* nSize         = */ sizeof(error_buffer) - 1,
					/* Arguments     = */ NULL ) > 0 )
				{
					oss << " : '" << error_buffer << "'";
				}

				error = oss.str();
			}
			#else
			{
				char const* const error_c_str = dlerror();
				if(error_c_str)
					error = " dlerror: '" + std::string(error_c_str) + "'";
			}
			#endif

			throw pbio::Error(0xbd2483c0, "Error in pbio::FacerecService::createService"
				" can't open dll file'" + std::string(dll_path) + "', error code: 0xbd2483c0" + error);
		}

		return result;
	}

	void* getSymbol(char const* const name) const
	{
		#ifdef _WIN32
			void* const result = reinterpret_cast<void*>( GetProcAddress(_dll, name) );
		#else
			void* const result = dlsym(_dll, name);
		#endif

		if(!result)
		{
			throw pbio::Error(0x5146c155, "Error in pbio::FacerecService::createService"
				" can't fynd symbol '" + std::string(name) + "' in dll '" + _dll_path + "', error code: 0x5146c155");
		}

		return result;
	}

	const std::string _dll_path;

#ifdef _WIN32
	HINSTANCE const _dll_sense4;
#endif

	HINSTANCE const _dll;

// without __STATIC_LIBFACEREC_BUILD__
///////
#endif


public:

	#define __583e_F_FIELD_DECL(rtype, name, typed_args, args, return) \
		typedef rtype (*FuncType_##name) typed_args; \
		const FuncType_##name name;

	#define __TDV_F_FIELD_DECL(rtype, name, typed_args, args, return) \
		typedef rtype (*FuncType_##name) typed_args; \
		const FuncType_##name name;

	__583e_FLIST(__583e_F_FIELD_DECL)
	__TDV_FLIST(__TDV_F_FIELD_DECL)
#ifndef LEGACY_METASDK
	__TDV_METASDK_FLIST(__583e_F_FIELD_DECL)
#endif

};

}  // pbio namespace
}  // import namespace


//! @endcond

#endif  // __PBIO_DLL_HANDLE_H_989601403fee4b0d83e85ee9b5292036
