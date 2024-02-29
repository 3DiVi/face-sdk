#ifndef CONTEXT_H
#define CONTEXT_H

#ifndef WITHOUT_PROCESSING_BLOCK

#if defined(_WIN32)
#define NOMINMAX
#endif

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <pbio/DllHandle.h>
#include <pbio/RawImage.h>
#include <pbio/RawSample.h>


namespace pbio {

typedef LightSmartPtr<import::DllHandle>::tPtr DHPtr;

inline void tdvCheckException(const DHPtr& dll_handle, ContextEH*& out_exception) {
	if(out_exception)
	{
		auto err = Error(dll_handle->TDVException_getErrorCode(out_exception), dll_handle->TDVException_getMessage(out_exception));
		dll_handle->TDVException_deleteException(out_exception);
		out_exception = nullptr;
		throw err;
	}
}

class ContextRef;

class Context
{

	friend class FacerecService;
	friend class RawSample;

	template<bool isConst=false>
	class ContextArrayIterator
	{
		class charsList {
		public:
			size_t length_;
			char** ptr_;
			charsList(size_t length, char** ptr) : length_(length), ptr_(ptr) {}
			char* operator[](size_t index) const {
				return (index<length_) ? *(ptr_+index) : nullptr;
			}
		};

		const Context& base_;
		size_t length_;
		size_t index_;
		Context* curr_;
		const bool isObj_;
		std::shared_ptr<charsList> keys_;

		void charsList_deleter(charsList* ptr) {
			for(size_t indx=0; indx < ptr->length_; ++indx)
				base_.dll_handle->TDVContext_freePtr(ptr->ptr_[indx]);
			base_.dll_handle->TDVContext_freePtr(ptr->ptr_);
		};

	public:
		typedef std::ptrdiff_t                                                            difference_type;
		typedef std::forward_iterator_tag                                                 iterator_category;
		typedef Context                                                                   value_type;
		typedef typename std::conditional<isConst, const value_type*, value_type*>::type  pointer;
		typedef typename std::conditional<isConst, const value_type&, value_type&>::type  reference;
		typedef const value_type&                                                         const_reference;

		ContextArrayIterator(const Context& ctx, long long index);
		ContextArrayIterator(const Context& ctx, const std::string& key);

		ContextArrayIterator(const ContextArrayIterator& iter);
		ContextArrayIterator& operator=(const ContextArrayIterator& iter) = delete;
		ContextArrayIterator(ContextArrayIterator&& iter);
		ContextArrayIterator& operator=(ContextArrayIterator&& iter) = delete;

		~ContextArrayIterator() {
			if(curr_)
				delete curr_;
		}

		bool operator==(const ContextArrayIterator& other) const {
			return ((this->base_ == other.base_) && (this->curr_ == other.curr_));
		}

		bool operator!=(const ContextArrayIterator& other) const {
			return (!(this->base_ == other.base_) || !(this->curr_ == other.curr_));
		}

		ContextArrayIterator& operator++();
		ContextArrayIterator operator++(int);

		reference operator*() {
			return *curr_;
		}

		pointer operator->() {
			return curr_;
		}

		std::string key() const {
			if (keys_ && index_ < length_)
				return std::string(keys_->operator[](index_));
			return std::string();
		}
	};

protected:

	Context(const DHPtr &dll_handle) : dll_handle(dll_handle), weak_(false), eh_(nullptr) {
		handle_ = dll_handle->TDVContext_create(&eh_);
		tdvCheckException(dll_handle, eh_);
	}

	Context(const DHPtr &dll_handle, HContext* handle, bool weak = true) : dll_handle(dll_handle), weak_(weak), eh_(nullptr) {
		if(weak_)
			handle_ = handle;
		else
		{
			handle_ = dll_handle->TDVContext_clone(handle, &eh_);
			tdvCheckException(dll_handle, eh_);
		}
	}

	DHPtr dll_handle;
	HContext* handle_;
	bool weak_;
	mutable ContextEH* eh_;

public:

	typedef ContextRef Ref;

	virtual ~Context()
	{
		if(!weak_) {
			dll_handle->TDVContext_destroy(handle_, &eh_);
			if (eh_ && std::uncaught_exception())
				std::cerr << Error(dll_handle->TDVException_getErrorCode(eh_), dll_handle->TDVException_getMessage(eh_)).what();
			else
				tdvCheckException(dll_handle, eh_);
		}
	}

	Context(const Context& other) : dll_handle(other.dll_handle), weak_(false), eh_(nullptr) {
		handle_ = dll_handle->TDVContext_clone(other.handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	Context& operator=(const Context& other) {
		if (&other != this)
		{
			if(weak_)
				dll_handle->TDVContext_copy(other.handle_, handle_, &eh_);
			else {
				HContext* copy = dll_handle->TDVContext_clone(other.handle_, &eh_); // copy-swap
				tdvCheckException(dll_handle, eh_);
				std::swap(handle_, copy);
				dll_handle->TDVContext_destroy(copy, &eh_);
			}
			tdvCheckException(dll_handle, eh_);
		}
		return *this;
	}

	Context(Context&& other) : dll_handle(other.dll_handle), weak_(false), eh_(nullptr) {
		if(other.weak_)
		{
			handle_ = dll_handle->TDVContext_clone(other.handle_, &eh_);
			tdvCheckException(dll_handle, eh_);
		}
		else
		{
			handle_ = other.handle_;
			other.handle_ = nullptr;
			other.weak_ = true;
		}
	}

	Context& operator=(Context&& other){
		if (&other != this)
		{
			if(weak_) {
				dll_handle->TDVContext_copy(other.handle_, handle_, &eh_);
				tdvCheckException(dll_handle, eh_);
			}
			else
			{
				if(other.weak_)
				{
					HContext* copy = dll_handle->TDVContext_clone(other.handle_, &eh_);
					tdvCheckException(dll_handle, eh_);
					std::swap(handle_, copy);
					dll_handle->TDVContext_destroy(copy, &eh_);
					tdvCheckException(dll_handle, eh_);
				}
				else
				{
					handle_ = other.handle_;
					other.handle_ = nullptr;
					other.weak_ = true;
				}
			}
		}
		return *this;
	}

	operator ContextRef();

	template <typename T, typename = typename std::enable_if<!std::is_base_of<Context, typename std::decay<T>::type>::value>::type>
	Context& operator=(T&& value) {
		setValue(std::forward<T>(value));
		return *this;
	}

	bool operator==(const Context& other) const
	{return this->handle_ == other.handle_;}

	size_t size() const	{
		size_t lenght = dll_handle->TDVContext_getLength(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return lenght;
	}

	ContextArrayIterator<> begin(){
		return ContextArrayIterator<>(*this, 0);
	}

	ContextArrayIterator<> end(){
		return ContextArrayIterator<>(*this, -1);
	}

	ContextArrayIterator<true> begin() const {
		return ContextArrayIterator<true>(*this, 0);
	}

	ContextArrayIterator<true> end() const {
		return ContextArrayIterator<true>(*this, -1);
	}

	ContextArrayIterator<true> cbegin() const {
		return ContextArrayIterator<true>(*this, 0);
	}

	ContextArrayIterator<true> cend() const {
		return ContextArrayIterator<true>(*this, -1);
	}

	Ref operator[](const std::string& key);
	Ref operator[](const char* key);
	Ref operator[](const int index);

	const Ref operator[](const std::string& key) const;
	const Ref operator[](const char* key) const;
	const Ref operator[](const int index) const;

	Ref at(const char* key);
	Ref at(const std::string& key);
	Ref at(const int index);

	const Ref at(const char* key) const;
	const Ref at(const std::string& key) const;
	const Ref at(const int index) const;

	bool contains(const std::string& key) const {
		if (isObject())
		{
			/* discard return value as it does not produce new allocation */
			static_cast<void>(dll_handle->TDVContext_getByKey(handle_, key.c_str(), &eh_));
			if(eh_)
			{
				dll_handle->TDVException_deleteException(eh_);
				eh_ = nullptr;
			}
			else
				return true;
		}
		return false;
	}

	ContextArrayIterator<> find(const std::string& key) {
		return ContextArrayIterator<>(*this, key);
	}

	ContextArrayIterator<true> find(const std::string& key) const {
		return ContextArrayIterator<true>(*this, key);
	}

	template<typename T>
	typename std::enable_if<!std::is_base_of<Context, typename std::decay<T>::type>::value>::type push_back(T&& data) {
		Context elem(dll_handle);
		elem.setValue(std::forward<T>(data));
		// use r-value version of push_back without a copy
		push_back(std::move(elem));
	}

	void push_back(const Context& data) {
		dll_handle->TDVContext_pushBack(handle_, data.handle_, true, &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	void push_back(Context&& data) {
		dll_handle->TDVContext_pushBack(handle_, data.handle_, weak_, &eh_);  // can not move weak object
		tdvCheckException(dll_handle, eh_);
	}

	double getDouble() const {
		double ret = dll_handle->TDVContext_getDouble(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ret;
	}

	long getLong() const {
		long ret = dll_handle->TDVContext_getLong(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ret;
	}

	bool getBool() const {
		bool ret = dll_handle->TDVContext_getBool(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ret;
	}

	std::string getString() const {
		unsigned long str_size = dll_handle->TDVContext_getStrSize(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		std::string ret;
		ret.resize(str_size);
		dll_handle->TDVContext_getStr(handle_, &ret[0], &eh_);
		tdvCheckException(dll_handle, eh_);
		return ret; // copy elision (NRVO)
	}

	unsigned char* getDataPtr() const {
		unsigned char* ret = dll_handle->TDVContext_getDataPtr(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ret;
	}

	void setString(const char* str) {
		dll_handle->TDVContext_putStr(handle_, str, &eh_);
		tdvCheckException(dll_handle, eh_);
	}
	void setString(const std::string& str) {
		dll_handle->TDVContext_putStr(handle_, str.c_str(), &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	void setLong(long val) {
		dll_handle->TDVContext_putLong(handle_, val, &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	void setDouble(double val) {
		dll_handle->TDVContext_putDouble(handle_, val, &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	void setBool(bool val) {
		dll_handle->TDVContext_putBool(handle_, val, &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	unsigned char* setDataPtr(void* ptr, int copy_sz = 0) {
		unsigned char* ret{nullptr};
		if(copy_sz && !ptr)
			ret = dll_handle->TDVContext_allocDataPtr(handle_, copy_sz, &eh_);
		else
			ret = dll_handle->TDVContext_putDataPtr(handle_, static_cast<unsigned char*>(ptr), copy_sz, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ret;
	}

	unsigned char* setDataPtr(const void* ptr, int copy_sz) {
		unsigned char* ret = dll_handle->TDVContext_putConstDataPtr(handle_, static_cast<const unsigned char*>(ptr), copy_sz, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ret;
	}

	bool isNone() const {
		bool value = dll_handle->TDVContext_isNone(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return value;
	}

	bool isArray() const {
		bool value = dll_handle->TDVContext_isArray(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return value;
	}

	bool isObject() const {
		bool value = dll_handle->TDVContext_isObject(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return value;
	}

	bool isBool() const {
		bool val = dll_handle->TDVContext_isBool(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return val;
	}

	bool isLong() const {
		bool val = dll_handle->TDVContext_isLong(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return val;
	}

	bool isDouble() const {
		bool val = dll_handle->TDVContext_isDouble(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return val;
	}

	bool isString() const {
		bool val = dll_handle->TDVContext_isString(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return val;
	}

	bool isDataPtr() const {
		bool val = dll_handle->TDVContext_isDataPtr(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return val;
	}

	HContext* getHandle() {return handle_;}
	const HContext* getHandle() const {return handle_;}

	void clear() {
		dll_handle->TDVContext_clear(handle_, &eh_);
		tdvCheckException(dll_handle,eh_);
	}

protected:

	void setValue(const char* str) {
		dll_handle->TDVContext_putStr(handle_, str, &eh_);
		tdvCheckException(dll_handle, eh_);
	}
	void setValue(const std::string& str) {
		dll_handle->TDVContext_putStr(handle_, str.c_str(), &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	template<typename T, typename = typename std::enable_if<std::is_enum<T>::value || std::is_integral<T>::value>::type>
	void setValue(T val) {
		dll_handle->TDVContext_putLong(handle_, val, &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	void setValue(double val) {
		dll_handle->TDVContext_putDouble(handle_, val, &eh_);
		tdvCheckException(dll_handle, eh_);
	}
	void setValue(float val) {
		dll_handle->TDVContext_putDouble(handle_, val, &eh_);
		tdvCheckException(dll_handle, eh_);
	}
	void setValue(bool val) {
		dll_handle->TDVContext_putBool(handle_, val, &eh_);
		tdvCheckException(dll_handle, eh_);
	}
	void setValue(void* ptr, int copy_sz = 0) {
		dll_handle->TDVContext_putDataPtr(handle_, static_cast<unsigned char*>(ptr), copy_sz, &eh_);
		tdvCheckException(dll_handle, eh_);
	}
};

class ContextRef : public Context {
public:

	ContextRef(const DHPtr &dll_handle, HContext* handle) : Context(dll_handle ,handle, true) {}

	template <typename T, typename = typename std::enable_if<!std::is_base_of<Context, typename std::decay<T>::type>::value>::type>
	void operator=(T&& value) {
		setValue(std::forward<T>(value));
	}
};

template<bool isConst>
Context::ContextArrayIterator<isConst>::ContextArrayIterator(const Context& ctx, long long index) : base_(ctx), curr_(nullptr), isObj_(ctx.isObject())
{
	length_ = base_.dll_handle->TDVContext_getLength(base_.handle_, &(base_.eh_));
	tdvCheckException(base_.dll_handle, base_.eh_);
	index_ = (index > -1) ? std::min<size_t>(static_cast<size_t>(index), length_) : length_;
	if(index_ < length_) {
		if(isObj_)
		{
			auto keys = base_.dll_handle->TDVContext_getKeys(base_.handle_, length_, &(base_.eh_));
			tdvCheckException(base_.dll_handle, base_.eh_);
			keys_ = std::shared_ptr<charsList>(new charsList(length_, keys), std::bind(&ContextArrayIterator::charsList_deleter, this, std::placeholders::_1));
			curr_ = new Context(base_[keys_->operator[](index)]);
		}
		else
			curr_ = new Context(base_[index]);
	}
}

template<bool isConst>
Context::ContextArrayIterator<isConst>::ContextArrayIterator(const Context& ctx, const std::string& key) : base_(ctx), curr_(nullptr), isObj_(ctx.isObject())
{
	length_ = base_.dll_handle->TDVContext_getLength(base_.handle_, &(base_.eh_));
	tdvCheckException(base_.dll_handle, base_.eh_);
	index_ = length_;
	if(isObj_ && length_) {
		auto keys = base_.dll_handle->TDVContext_getKeys(base_.handle_, length_, &(base_.eh_));
		tdvCheckException(base_.dll_handle, base_.eh_);
		keys_ = std::shared_ptr<charsList>(new charsList(length_, keys), std::bind(&ContextArrayIterator::charsList_deleter, this, std::placeholders::_1));
		size_t i=0;
		do {
			if(!key.compare(keys_->operator[](i))) {
				index_ = i;
				curr_ = new Context(base_[key]);
				break;
			}
		} while((++i)<length_);
	}
}

template<bool isConst>
Context::ContextArrayIterator<isConst>::ContextArrayIterator(const ContextArrayIterator& iter) :
	base_(iter.base_), length_(iter.length_), index_(iter.index_), curr_(iter.curr_ ? new Context(*(iter.curr_)) : nullptr), isObj_(iter.isObj_), keys_(iter.keys_)
{};

template<bool isConst>
Context::ContextArrayIterator<isConst>::ContextArrayIterator(ContextArrayIterator&& iter) :
	base_(iter.base_), length_(iter.length_), index_(iter.index_), curr_(iter.curr_), isObj_(iter.isObj_), keys_(std::move(iter.keys_)) {
	iter.curr_ = nullptr;
}

template<bool isConst>
Context::ContextArrayIterator<isConst>& Context::ContextArrayIterator<isConst>::operator++() {
	if(curr_)
		delete curr_;
	index_ = std::min(index_+1, length_);
	if(isObj_)
		curr_ = (index_ < length_) ? new Context(base_[keys_->operator[](index_)]) : nullptr;
	else
		curr_ = (index_ < length_) ? new Context(base_[index_]) : nullptr;
	return *this;
}

template<bool isConst>
Context::ContextArrayIterator<isConst> Context::ContextArrayIterator<isConst>::operator++(int) {
	ContextArrayIterator tmp = *this;
	index_ = std::min(index_+1, length_);
	if(isObj_)
		curr_ = (index_ < length_) ? new Context(base_[keys_->operator[](index_)]) : nullptr;
	else
		curr_ = (index_ < length_) ? new Context(base_[index_]) : nullptr;
	return tmp;
}

inline Context::operator ContextRef()
{
	return ContextRef(dll_handle, handle_);
}

inline Context::Ref Context::operator[](const std::string& key) {
	HContext* handle = dll_handle->TDVContext_getOrInsertByKey(handle_, key.c_str(), &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline Context::Ref Context::operator[](const char* key) {
	HContext* handle = dll_handle->TDVContext_getOrInsertByKey(handle_, key, &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline Context::Ref Context::operator[](const int index) {
	HContext* handle = dll_handle->TDVContext_getByIndex(handle_, index, &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline const Context::Ref Context::operator[](const std::string& key) const {
	HContext* handle = dll_handle->TDVContext_getByKey(handle_, key.c_str(), &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline const Context::Ref Context::operator[](const char* key) const {
	HContext* handle = dll_handle->TDVContext_getByKey(handle_, key, &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline const Context::Ref Context::operator[](const int index) const {
	HContext* handle = dll_handle->TDVContext_getByIndex(handle_, index, &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline Context::Ref Context::at(const char* key) {
	HContext* handle = dll_handle->TDVContext_getByKey(handle_, key, &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline Context::Ref Context::at(const std::string& key) {
	HContext* handle = dll_handle->TDVContext_getByKey(handle_, key.c_str(), &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline Context::Ref Context::at(const int index) {
	HContext* handle = dll_handle->TDVContext_getByIndex(handle_, index, &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline const Context::Ref Context::at(const char* key) const {
	HContext* handle = dll_handle->TDVContext_getByKey(handle_, key, &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline const Context::Ref Context::at(const std::string& key) const {
	HContext* handle = dll_handle->TDVContext_getByKey(handle_, key.c_str(), &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}

inline const Context::Ref Context::at(const int index) const {
	HContext* handle = dll_handle->TDVContext_getByIndex(handle_, index, &eh_);
	tdvCheckException(dll_handle, eh_);
	return Ref(dll_handle, handle);
}


namespace context_utils {

static std::unordered_map<int, std::string> color_mode{{IRawImage::FORMAT_GRAY, "GRAY"}, {IRawImage::FORMAT_BGR, "BGR"}, {IRawImage::FORMAT_RGB, "RGB"}};

inline void putImage(Context& ctx, const RawImage& raw_image) {
	ctx.clear();
	ctx["format"] = "NDARRAY";
	ctx["color_space"] = color_mode.at(raw_image.format);
	size_t channels = (raw_image.format == IRawImage::FORMAT_GRAY) ? 1 : 3;
	size_t copy_sz = raw_image.height * raw_image.width * channels * sizeof(uint8_t);
	if (raw_image.with_crop)
	{
		unsigned char* buff{nullptr};
		buff = ctx["blob"].setDataPtr(buff, copy_sz);
		size_t shift = (raw_image.crop_info_offset_y*raw_image.crop_info_data_image_width + raw_image.crop_info_offset_x)*channels*sizeof(uint8_t);
		size_t stride = raw_image.width*channels*sizeof(uint8_t);
		unsigned char* ptr = buff;
		for(int row=0; row<raw_image.height; ++row)
		{
			std::memcpy(ptr, raw_image.data+shift, stride);
			shift += raw_image.crop_info_data_image_width*channels*sizeof(uint8_t);
			ptr += stride;
		}
	}
	else
		ctx["blob"].setDataPtr(raw_image.data, copy_sz);
	ctx["dtype"] = "uint8_t";
	ctx["shape"].push_back(static_cast<int64_t>(raw_image.height));
	ctx["shape"].push_back(static_cast<int64_t>(raw_image.width));
	ctx["shape"].push_back(static_cast<int64_t>(channels));
}

inline void putImage(Context& ctx, unsigned char* data, size_t height, size_t width, pbio::IRawImage::Format format, bool copy) {
	ctx.clear();
	ctx["format"] = "NDARRAY";
	ctx["color_space"] = color_mode.at(format);
	size_t channels = (format == IRawImage::FORMAT_GRAY) ? 1 : 3;
	size_t copy_sz = copy ? height * width * channels * sizeof(uint8_t) : 0;
	ctx["blob"].setDataPtr(data, copy_sz);
	ctx["dtype"] = "uint8_t";
	ctx["shape"].push_back(static_cast<int64_t>(height));
	ctx["shape"].push_back(static_cast<int64_t>(width));
	ctx["shape"].push_back(static_cast<int64_t>(channels));
}

}
}

#endif // WITHOUT_PROCESSING_BLOCK
#endif // CONTEXT_H
