#include "ssostr.hpp"

namespace ds {
namespace detail {

void Allocated_Str::init(cz::Str str) {
    _buffer = str.buffer;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    _len = (str.len << 1);
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    _len = (str.len & LEN_MASK);
#else
#error Must specify __BYTE_ORDER__
#endif
}

size_t Allocated_Str::len() const {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return _len >> 1;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (_len & LEN_MASK);
#else
#error Must specify __BYTE_ORDER__
#endif
}

void Short_Str::init(cz::Str str) {
    memcpy(_buffer, str.buffer, str.len);
    _buffer[MAX] = (char)((str.len << 1) | 1);
}

}

SSOStr SSOStr::from_constant(cz::Str str) {
    SSOStr self;
    if (str.len <= detail::Short_Str::MAX) {
        new (&self.short_) detail::Short_Str;
        self.short_.init(str);
    } else {
        new (&self.allocated) detail::Allocated_Str;
        self.allocated.init(str);
    }
    return self;
}

SSOStr SSOStr::from_char(char c) {
    SSOStr self;
    new (&self.short_) detail::Short_Str;
    self.short_.init({&c, 1});
    return self;
}

SSOStr SSOStr::as_duplicate(cz::Allocator allocator, cz::Str str) {
    SSOStr self;
    if (str.len <= detail::Short_Str::MAX) {
        new (&self.short_) detail::Short_Str;
        self.short_.init(str);
    } else {
        char* buffer = (char*)allocator.alloc({str.len, 1});
        memcpy(buffer, str.buffer, str.len);
        new (&self.allocated) detail::Allocated_Str;
        self.allocated.init({buffer, str.len});
    }
    return self;
}

}
