#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>
#include <memory>
#include <string>

#include <pbio/DllHandle.h>


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


class Context
{
	DHPtr dll_handle;

	template<bool isConst=false>
	class ContextArrayIterator
	{
		const Context& base_;
		size_t length_;
		size_t index_;
		Context* curr_;
		char** keys_;
		const bool isObj_;

	public:
		typedef std::ptrdiff_t																difference_type;
		typedef std::forward_iterator_tag													iterator_category;
		typedef Context																		value_type;
		typedef typename std::conditional<isConst, const value_type*, value_type*>::type	pointer;
		typedef typename std::conditional<isConst, const value_type&, value_type&>::type	reference;
		typedef const value_type&															const_reference;

		ContextArrayIterator(const Context& ctx, size_t length, size_t index) :
			base_(ctx), length_(length), index_(index), isObj_(base_.isObject())
		{
			keys_ = (isObj_ && (index_ < length_)) ? base_.dll_handle->TDVContext_getKeys(base_.handle_, length_, &(base_.eh_)) : nullptr;
			tdvCheckException(base_.dll_handle, base_.eh_);
			if(isObj_)
				curr_ = (index < length_) ? new Context(base_[keys_[index]]) : nullptr;
			else
				curr_ = (index < length_) ? new Context(base_[index]) : nullptr;
		}

		~ContextArrayIterator()
		{
			if(keys_)
			{
				for(size_t indx=0; indx < length_; ++indx)
					free(keys_[indx]);
				free(keys_);
			}
			if(curr_)
				delete curr_;
		}

		bool operator==(const ContextArrayIterator& other) const {
			return ((this->base_ == other.base_) && (this->curr_ == other.curr_));
		}

		bool operator!=(const ContextArrayIterator& other) const {
			return (!(this->base_ == other.base_) || !(this->curr_ == other.curr_));
		}

		ContextArrayIterator& operator++() {
			if(curr_)
				delete curr_;
			++index_;
			if(isObj_)
				curr_ = (index_ < length_) ? new Context(base_[keys_[index_]]) : nullptr;
			else
				curr_ = (index_ < length_) ? new Context(base_[index_]) : nullptr;
			return *this;
		}

		ContextArrayIterator operator++(int) {
			ContextArrayIterator tmp = *this;
			++index_;
			if(isObj_)
				curr_ = (index_ < length_) ? new Context(base_[keys_[index_]]) : nullptr;
			else
				curr_ = (index_ < length_) ? new Context(base_[index_]) : nullptr;
			return tmp;
		}

		reference operator*() {
			return *curr_;
		}

		pointer operator->() {
			return curr_;
		}

		std::string key() const {
			if (!keys_ || index_ >= length_)
				return std::string();
			return std::string(keys_[index_]);
		}
	};

	Context(const DHPtr &dll_handle, HContext* handle, bool weak = true) : dll_handle(dll_handle), handle_(handle), weak_(weak) {}

	HContext* handle_;
	mutable ContextEH* eh_ = nullptr;
	bool weak_;


public:

	Context(const DHPtr &dll_handle) : dll_handle(dll_handle), weak_(false) {
		handle_ = dll_handle->TDVContext_create(&eh_);
		tdvCheckException(dll_handle, eh_);
	}
	~Context()
	{
		if(!weak_) {
			dll_handle->TDVContext_destroy(handle_, &eh_);
			if (eh_ && std::uncaught_exception())
				std::cerr << Error(dll_handle->TDVException_getErrorCode(eh_), dll_handle->TDVException_getMessage(eh_)).what();
			else
				tdvCheckException(dll_handle, eh_);
		}
	}

	Context(const Context& other) : dll_handle(other.dll_handle), weak_(false) {
		handle_ = dll_handle->TDVContext_clone(other.handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	Context& operator=(const Context& other) {
		if (&other != this)
		{
			if(weak_)
				dll_handle->TDVContext_copy(other.handle_, handle_, &eh_);
			else {
				dll_handle->TDVContext_destroy(handle_, &eh_);
				tdvCheckException(dll_handle, eh_);
				handle_ = dll_handle->TDVContext_clone(other.handle_, &eh_);
			}
			tdvCheckException(dll_handle, eh_);
		}
		return *this;
	}

	Context(Context&& other) : dll_handle(other.dll_handle), weak_(false) {
		handle_ = other.weak_ ? dll_handle->TDVContext_clone(other.handle_, &eh_) : other.handle_;
		tdvCheckException(dll_handle, eh_);
	}

	Context& operator=(Context&& other){
		if (&other != this)
		{
			if(weak_)
				dll_handle->TDVContext_copy(other.handle_, handle_, &eh_);
			else
			{
				dll_handle->TDVContext_destroy(handle_, &eh_);
				tdvCheckException(dll_handle, eh_);
				handle_ = other.weak_ ? dll_handle->TDVContext_clone(other.handle_, &eh_) : other.handle_;
			}
			tdvCheckException(dll_handle, eh_);
		}
		return *this;
	}

	template <typename T, typename = typename std::enable_if<!std::is_same<typename std::decay<T>::type, Context>::value>::type>
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
		unsigned long cxt_length_ = dll_handle->TDVContext_getLength(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ContextArrayIterator<>(*this, cxt_length_, 0);
	}

	ContextArrayIterator<> end(){
		unsigned long cxt_length_ = dll_handle->TDVContext_getLength(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ContextArrayIterator<>(*this, cxt_length_, cxt_length_);
	}

	ContextArrayIterator<true> begin() const {
		unsigned long cxt_length_ = dll_handle->TDVContext_getLength(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ContextArrayIterator<true>(*this, cxt_length_, 0);
	}

	ContextArrayIterator<true> end() const {
		unsigned long cxt_length_ = dll_handle->TDVContext_getLength(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ContextArrayIterator<true>(*this, cxt_length_, cxt_length_);
	}

	ContextArrayIterator<true> cbegin() const {
		unsigned long cxt_length_ = dll_handle->TDVContext_getLength(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ContextArrayIterator<true>(*this, cxt_length_, 0);
	}

	ContextArrayIterator<true> cend() const {
		unsigned long cxt_length_ = dll_handle->TDVContext_getLength(handle_, &eh_);
		tdvCheckException(dll_handle, eh_);
		return ContextArrayIterator<true>(*this, cxt_length_, cxt_length_);
	}

	Context operator[](const std::string& key) {
		HContext* handle = dll_handle->TDVContext_getOrInsertByKey(handle_, key.c_str(), &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	Context operator[](const char* key) {
		HContext* handle = dll_handle->TDVContext_getOrInsertByKey(handle_, key, &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	Context operator[](const int index) {
		HContext* handle = dll_handle->TDVContext_getByIndex(handle_, index, &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	const Context operator[](const std::string& key) const {
		HContext* handle = dll_handle->TDVContext_getByKey(handle_, key.c_str(), &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	const Context operator[](const char* key) const {
		HContext* handle = dll_handle->TDVContext_getByKey(handle_, key, &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	const Context operator[](const int index) const {
		HContext* handle = dll_handle->TDVContext_getByIndex(handle_, index, &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	Context at(const char* key) {
		HContext* handle = dll_handle->TDVContext_getByKey(handle_, key, &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	Context at(const std::string& key) {
		HContext* handle = dll_handle->TDVContext_getByKey(handle_, key.c_str(), &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	Context at(const int index) {
		HContext* handle = dll_handle->TDVContext_getByIndex(handle_, index, &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	const Context at(const char* key) const {
		HContext* handle = dll_handle->TDVContext_getByKey(handle_, key, &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	const Context at(const std::string& key) const {
		HContext* handle = dll_handle->TDVContext_getByKey(handle_, key.c_str(), &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	const Context at(const int index) const {
		HContext* handle = dll_handle->TDVContext_getByIndex(handle_, index, &eh_);
		tdvCheckException(dll_handle, eh_);
		return Context(dll_handle, handle);
	}

	template<typename T>
	typename std::enable_if<!std::is_same<typename std::decay<T>::type, Context>::value>::type push_back(T&& data) {
		Context elem(dll_handle);
		elem.setValue(std::forward<T>(data));
		// use r-value version of push_back without a copy
		push_back(std::move(elem));
	}

	void push_back(const Context& data) {
		dll_handle->TDVContext_pushBack(handle_, data.handle_, false, &eh_);
		tdvCheckException(dll_handle, eh_);
	}

	void push_back(Context&& data) {
		dll_handle->TDVContext_pushBack(handle_, data.handle_, true, &eh_);
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

	void setDataPtr(void* ptr, int copy_sz = 0) {
		dll_handle->TDVContext_putDataPtr(handle_, static_cast<unsigned char*>(ptr), copy_sz, &eh_);
		tdvCheckException(dll_handle, eh_);
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

private:

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

}

#endif // CONTEXT_H
