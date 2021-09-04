#pragma once

#include <limits.h>
#include <string.h>
#include <cz/allocator.hpp>
#include <cz/str.hpp>
#include <new>

namespace ds {

namespace detail {

struct Allocated_Str {
    constexpr static const size_t LEN_MASK = (((size_t)1 << ((sizeof(size_t) - 1) * CHAR_BIT)) - 1);

    const char* _buffer;
    size_t _len;

    void init(cz::Str str);
    void drop(cz::Allocator allocator) { allocator.dealloc({(char*)_buffer, _len}); }
    const char* buffer() const { return _buffer; }
    size_t len() const;
};

struct Short_Str {
    constexpr static const size_t MAX = sizeof(Allocated_Str) - 1;

    char _buffer[sizeof(Allocated_Str)];

    void init(cz::Str str);
    const char* buffer() const { return _buffer; }
    size_t len() const { return _buffer[MAX] >> 1; }
    bool is_short() const { return _buffer[MAX] & 1; }
};

}

struct SSOStr {
    constexpr static const size_t MAX_SHORT_LEN = detail::Short_Str::MAX;

    union {
        detail::Allocated_Str allocated;
        detail::Short_Str short_;
    };

    static SSOStr from_constant(cz::Str str);

    static SSOStr from_char(char c);

    static SSOStr as_duplicate(cz::Allocator allocator, cz::Str str);

    void drop(cz::Allocator allocator) {
        if (!is_short()) {
            allocated.drop(allocator);
        }
    }

    bool is_short() const { return short_.is_short(); }

    const char* buffer() const {
        if (is_short()) {
            return short_.buffer();
        } else {
            return allocated.buffer();
        }
    }

    size_t len() const {
        if (is_short()) {
            return short_.len();
        } else {
            return allocated.len();
        }
    }

    cz::Str as_str() const {
        if (is_short()) {
            return {short_.buffer(), short_.len()};
        } else {
            return {allocated.buffer(), allocated.len()};
        }
    }

    SSOStr clone(cz::Allocator allocator) const {
        return SSOStr::as_duplicate(allocator, as_str());
    }
};

}
