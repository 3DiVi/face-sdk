#ifndef PBIO_UTIL_H_693bcd72935a404d97eda9d21d9a0c77
#define PBIO_UTIL_H_693bcd72935a404d97eda9d21d9a0c77

#define def_param_field_693bcd72(CLASS_TYPE, TYPE, NAME) \
	CLASS_TYPE& NAME(const TYPE value) \
	{ \
		this->_##NAME = value; \
		return *this; \
	} \
	TYPE _##NAME;

#endif  // PBIO_UTIL_H_693bcd72935a404d97eda9d21d9a0c77