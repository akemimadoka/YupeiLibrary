#include "String.hpp"

namespace Yupei
{
	/*
	template class basic_string<string_type::wide>;
	template class basic_string<string_type::utf8>;
	template class basic_string<string_type::utf16>;
	template class basic_string<string_type::utf32>;
	*/
	utf8_string to_utf8(const utf16_string::view_type & utf16)
	{
		utf8_string res;
		res.reserve(utf16.size() * 2);
		convert(res, make_utf8_encoder(make_utf16_decoder(make_reader(utf16))));
		return res;
	}

	utf8_string to_utf8(const wide_string::view_type & wide)
	{
		return to_utf8({ reinterpret_cast<const char16_t*>(wide.data()), wide.size() });
	}

	utf8_string to_utf8(const utf32_string::view_type & utf32)
	{
		utf8_string res;
		res.reserve(utf32.size() * 4);
		convert(res, make_utf8_encoder(make_reader(utf32)));
		return res;
	}

	utf16_string to_utf16(const utf8_string::view_type & utf8)
	{
		utf16_string res;
		res.reserve(utf8.size());
		convert(res, make_utf16_encoder(make_utf8_decoder(make_reader(utf8))));
		return res;
	}

	utf32_string to_utf32(const utf16_string::view_type & utf16)
	{
		utf32_string res;
		res.reserve(utf16.size());
		convert(res, make_utf16_decoder(make_reader(utf16)));
		return res;
	}

	utf32_string to_utf32(const utf8_string::view_type & utf8)
	{
		utf32_string res;
		res.reserve(utf8.size());
		convert(res, make_utf8_decoder(make_reader(utf8)));
		return res;
	}

	utf32_string to_utf32(const wide_string::view_type & wide)
	{
		return to_utf32({ reinterpret_cast<const char16_t*>(wide.data()), wide.size() });
	}
}