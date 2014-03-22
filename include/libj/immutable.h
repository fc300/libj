// Copyright (c) 2012-2014 Plenluno All rights reserved.

#ifndef LIBJ_IMMUTABLE_H_
#define LIBJ_IMMUTABLE_H_

#include <libj/object.h>
#include <libj/detail/gc_base.h>

namespace libj {

class Immutable
    : public Object
    , public detail::GCBase
    , public detail::ImmutableBase {
 public:
    typedef LIBJ_CPTR(Immutable) CPtr;

    static CPtr null();

    virtual CPtr self() const;

    virtual Boolean instanceof(TypeId id) const;
};

}  // namespace libj

#include <libj/impl/immutable.h>

#define LIBJ_IMMUTABLE(T) public libj::Immutable { \
    LIBJ_IMMUTABLE_DEFS(T, LIBJ_IMMUTABLE)

#endif  // LIBJ_IMMUTABLE_H_
