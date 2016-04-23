#include "String.hpp"

namespace Yupei
{
	/*
	template class basic_string<string_type::wide>;
	template class basic_string<string_type::utf8>;
	template class basic_string<string_type::utf16>;
	template class basic_string<string_type::utf32>;
	*/
	u8string to_utf8(const u16string::view_type & utf16)
	{
		u8string res;
		res.reserve(utf16.size() * 2);
		convert(res, make_utf8_encoder(make_utf16_decoder(make_reader(utf16))));
		return res;
	}

	u8string to_utf8(const wstring::view_type & wide)
	{
		return to_utf8({ reinterpret_cast<const char16_t*>(wide.data()), wide.size() });
	}

	u8string to_utf8(const u32string::view_type & utf32)
	{
		u8string res;
		res.reserve(utf32.size() * 4);
		convert(res, make_utf8_encoder(make_reader(utf32)));
		return res;
	}

	u16string to_utf16(const u8string::view_type & utf8)
	{
		u16string res;
		res.reserve(utf8.size());
		convert(res, make_utf16_encoder(make_utf8_decoder(make_reader(utf8))));
		return res;
	}

	u32string to_utf32(const u16string::view_type & utf16)
	{
		u32string res;
		res.reserve(utf16.size());
		convert(res, make_utf16_decoder(make_reader(utf16)));
		return res;
	}

	u32string to_utf32(const u8string::view_type & utf8)
	{
		u32string res;
		res.reserve(utf8.size());
		convert(res, make_utf8_decoder(make_reader(utf8)));
		return res;
	}

	u32string to_utf32(const wstring::view_type & wide)
	{
		return to_utf32({ reinterpret_cast<const char16_t*>(wide.data()), wide.size() });
	}
}