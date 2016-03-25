#include "String.hpp"

namespace Yupei
{
    template class basic_string<string_type::wide>;
    template class basic_string<string_type::utf8>;
    template class basic_string<string_type::utf16>;
    template class basic_string<string_type::utf32>;
}