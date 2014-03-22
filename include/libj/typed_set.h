// Copyright (c) 2013-2014 Plenluno All rights reserved.

#ifndef LIBJ_TYPED_SET_H_
#define LIBJ_TYPED_SET_H_

#include <libj/set.h>
#include <libj/typed_collection.h>

namespace libj {

template<typename T>
class TypedSet
    : public TypedCollection<T>
    , LIBJ_SET_TEMPLATE(TypedSet)
 public:
    static Ptr create();
};

}  // namespace libj

#include <libj/impl/typed_set.h>

#endif  // LIBJ_TYPED_SET_H_
