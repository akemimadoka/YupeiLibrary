#pragma once

#define DISABLECOPY(TYPE) \
	TYPE(const TYPE&) = delete;\
	TYPE& operator=(const TYPE&) = delete;

#define DEFAULTCOPY(TYPE) \
	TYPE(const TYPE&) = default;\
	TYPE& operator=(const TYPE&) = default;
	
