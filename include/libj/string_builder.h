// Copyright (c) 2013-2014 Plenluno All rights reserved.

#ifndef LIBJ_STRING_BUILDER_H_
#define LIBJ_STRING_BUILDER_H_

#include <libj/mutable.h>
#include <libj/string.h>

namespace libj {

class StringBuilder : LIBJ_MUTABLE(StringBuilder)
 public:
    static Ptr create();

    static Ptr create(Size capacity);

    static Ptr create(String::CPtr str);

    virtual Size length() const = 0;

    virtual Size capacity() const = 0;

    virtual void ensureCapacity(Size capacity) = 0;

    virtual Ptr append(const Value& val) = 0;

    virtual Ptr appendChar(Char c) = 0;

    virtual Ptr appendStr(const char* str) = 0;

    virtual Ptr appendStr(const Char* str) = 0;

    virtual Ptr appendStr(String::CPtr str) = 0;

    virtual Char charAt(Size index) const = 0;

    virtual Boolean setCharAt(Size index, Char c) = 0;

    virtual void clear() = 0;

    virtual const Char* data() const = 0;

    virtual std::string toStdString(
        String::Encoding enc = String::UTF8) const = 0;
};

}  // namespace libj

#endif  // LIBJ_STRING_BUILDER_H_
