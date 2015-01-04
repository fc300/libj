// Copyright (c) 2012-2015 Plenluno All rights reserved.

#include <libj/error.h>
#include <libj/js_array.h>
#include <libj/js_object.h>
#include <libj/glue/json.h>

#include <math.h>
#include <rapidjson/reader.h>

#include <string>
#include <sstream>

namespace libj {
namespace glue {
namespace json {

static Value doubleToValue(Double d) {
    static const Double JSON_INTEGER_MAX = +9007199254740991.0;
    static const Double JSON_INTEGER_MIN = -9007199254740991.0;

    Double intpart;
    if (modf(d, &intpart) == 0.0 &&
        intpart <= JSON_INTEGER_MAX &&
        intpart >= JSON_INTEGER_MIN) {
        Long l = static_cast<Long>(intpart);
        return l;
    } else {
        return d;
    }
}

static Value longToValue(Long l) {
    static const Long JSON_INTEGER_MAX = +9007199254740991;
    static const Long JSON_INTEGER_MIN = -9007199254740991;

    if (l <= JSON_INTEGER_MAX && l >= JSON_INTEGER_MIN) {
        return l;
    } else {
        return static_cast<Double>(l);
    }
}

static Value ulongToValue(ULong ul) {
    static const ULong JSON_INTEGER_MAX = +9007199254740991;

    if (ul <= JSON_INTEGER_MAX) {
        return static_cast<Long>(ul);
    } else {
        return static_cast<Double>(ul);
    }
}

static String::CPtr createString(const Char* s, Size len) {
#ifdef LIBJ_USE_UTF32
    return String::create(s, String::UTF32, len);
#else
    return String::create(s, String::UTF16, len);
#endif
}

#ifdef LIBJ_USE_UTF32
typedef rapidjson::UTF32<Char> JsonEnc;
#else
typedef rapidjson::UTF16<Char> JsonEnc;
#endif

typedef rapidjson::GenericStringStream<JsonEnc> JsonStream;
typedef rapidjson::GenericReader<JsonEnc, JsonEnc> JsonReader;

class JsonHandler {
 public:
    JsonHandler()
        : stack_(JsArray::create())
        , array_(JsArray::null())
        , object_(JsObject::null())
        , name_(String::null()) {}

    Value root() {
        return root_;
    }

    void add(const Value& v) {
        if (array_) {
            array_->add(v);
        } else if (object_) {
            assert(name_);
            object_->put(name_, v);
            name_ = String::null();
        } else {
            assert(root_.isUndefined());
            root_ = v;
        }
    }

    void push() {
        if (array_) {
            stack_->push(array_);
            array_ = JsArray::null();
        } else if (object_) {
            stack_->push(object_);
            object_ = JsObject::null();
        }
    }

    void pop() {
        if (stack_->isEmpty()) return;

        Value v = stack_->pop();
        JsArray::Ptr ary = toPtr<JsArray>(v);
        if (ary) {
            array_ = ary;
        } else {
            JsObject::Ptr obj = toPtr<JsObject>(v);
            assert(obj);
            object_ = obj;
        }
    }

    bool Null()  {
        add(Object::null());
        return true;
    }

    bool Bool(bool b)  {
        add(b);
        return true;
    }

    bool Int(int i) {
        add(static_cast<Long>(i));
        return true;
    }

    bool Uint(unsigned ui)  {
        add(static_cast<Long>(ui));
        return true;
    }

    bool Int64(int64_t l) {
        add(longToValue(l));
        return true;
    }

    bool Uint64(uint64_t ul) {
        add(ulongToValue(ul));
        return true;
    }

    bool Double(double d) {
        add(doubleToValue(d));
        return true;
    }

    bool String(const Char* str, rapidjson::SizeType len, bool copy) {
        assert(!object_ || name_);
        add(createString(str, len));
        return true;
    }

    bool StartObject() {
        JsObject::Ptr obj = JsObject::create();
        add(obj);
        push();
        object_ = obj;
        return true;
    }

    bool Key(const Char* str, rapidjson::SizeType len, bool copy) {
        assert(object_ && !name_);
        name_ = createString(str, len);
        return true;
    }

    bool EndObject(rapidjson::SizeType size) {
        assert(object_->size() == size);
        object_ = JsObject::null();
        pop();
        return true;
    }

    bool StartArray() {
        JsArray::Ptr ary = JsArray::create();
        add(ary);
        push();
        array_ = ary;
        return true;
    }

    bool EndArray(rapidjson::SizeType len) {
        assert(array_->length() == len);
        array_ = JsArray::null();
        pop();
        return true;
    }

 private:
    Value root_;
    JsArray::Ptr stack_;
    JsArray::Ptr array_;
    JsObject::Ptr object_;
    String::CPtr name_;
};

Value parse(String::CPtr str) {
    if (!str) return Error::create(Error::ILLEGAL_ARGUMENT);

    JsonReader reader;
    JsonStream stream(str->data());
    JsonHandler handler;
    const unsigned int parseFlags = rapidjson::kParseDefaultFlags;
    reader.Parse<parseFlags, JsonStream, JsonHandler>(stream, handler);
    if (reader.HasParseError()) {
        return Error::create(Error::ILLEGAL_ARGUMENT);
    } else {
        return handler.root();
    }
}

}  // namespace json
}  // namespace glue
}  // namespace libj
