// Copyright (c) 2012-2013 Plenluno All rights reserved.

#include <gtest/gtest.h>
#include <float.h>
#include <libj/array_list.h>
#include <libj/error.h>
#include <libj/json.h>
#include <libj/js_array.h>
#include <libj/js_date.h>
#include <libj/js_object.h>
#include <libj/map.h>
#include <libj/string_builder.h>

namespace libj {

TEST(GTestJson, TestStringify) {
    ASSERT_FALSE(json::stringify(UNDEFINED));

    ASSERT_TRUE(json::stringify(static_cast<Byte>(3))
        ->equals(String::create("3")));

    ASSERT_TRUE(json::stringify(static_cast<Short>(4))
        ->equals(String::create("4")));

    ASSERT_TRUE(json::stringify(static_cast<Int>(5))
        ->equals(String::create("5")));

    ASSERT_TRUE(json::stringify(static_cast<Long>(6))
        ->equals(String::create("6")));

    ASSERT_TRUE(json::stringify(static_cast<Float>(2.5))
        ->equals(String::create("2.5")));

    ASSERT_TRUE(json::stringify(static_cast<Double>(3.5))
        ->equals(String::create("3.5")));

    ASSERT_TRUE(json::stringify(DBL_MAX)
        ->equals(String::create("1.7976931348623157e+308")));

    ASSERT_TRUE(json::stringify(-DBL_MIN)
        ->equals(String::create("-2.2250738585072014e-308")));

    ASSERT_TRUE(json::stringify(true)
        ->equals(String::create("true")));

    ASSERT_TRUE(json::stringify(false)
        ->equals(String::create("false")));

    ASSERT_TRUE(json::stringify(Object::null())
        ->equals(String::create("null")));

    ASSERT_TRUE(json::stringify(String::create("456"))
        ->equals(String::create("\"456\"")));

    ASSERT_TRUE(json::stringify(Map::create())
        ->equals(String::create("{}")));

    ASSERT_TRUE(json::stringify(ArrayList::create())
        ->equals(String::create("[]")));

    ASSERT_TRUE(json::stringify(JsDate::create(1364685226134))
        ->equals(String::create("\"2013-03-30T23:13:46Z\"")));

    Map::Ptr m = Map::create();
    m->put(3, false);
    m->put(String::create("w"), UNDEFINED);
    m->put(String::create("x"), 123);
    m->put(String::create("y"), String::create("456"));
    m->put(String::create("z"), String::null());

    String::CPtr s1 = json::stringify(m);
    String::CPtr s2 = String::create("{\"x\":123,\"y\":\"456\",\"z\":null}");
    String::CPtr s3 = String::create("\"x\":123");
    String::CPtr s4 = String::create("\"y\":\"456\"");
    String::CPtr s5 = String::create("\"z\":null");
    ASSERT_EQ(s2->length(), s1->length());
    ASSERT_NE(NO_POS, s1->indexOf(s3));
    ASSERT_NE(NO_POS, s1->indexOf(s4));
    ASSERT_NE(NO_POS, s1->indexOf(s5));

    ArrayList::Ptr a = ArrayList::create();
    a->add(3);
    a->add(UNDEFINED);
    a->add(false);
    ASSERT_TRUE(json::stringify(a)
        ->equals(String::create("[3,null,false]")));
}

TEST(GTestJson, TestRecursiveStringify) {
    Map::Ptr m = Map::create();
    m->put(String::create("x"), 123);
    m->put(String::create("y"), ArrayList::create());

    JsObject::Ptr jo = JsObject::create();
    jo->put(3, false);
    jo->put(Object::null(), m);

    String::CPtr s1 = json::stringify(jo);
    String::CPtr s2 = str("{\"3\":false,\"null\":{\"x\":123,\"y\":[]}}");
    String::CPtr s3 = str("\"3\":false");
    String::CPtr s4 = str("\"null\":{");
    String::CPtr s5 = str("\"x\":123");
    String::CPtr s6 = str("\"y\":[]");
    ASSERT_EQ(s2->length(), s1->length());
    ASSERT_NE(NO_POS, s1->indexOf(s3));
    ASSERT_NE(NO_POS, s1->indexOf(s4));
    ASSERT_NE(NO_POS, s1->indexOf(s5));
    ASSERT_NE(NO_POS, s1->indexOf(s6));

    ArrayList::Ptr a = ArrayList::create();
    a->add(3);
    a->add(m);

    JsArray::Ptr ja = JsArray::create();
    ja->add(UNDEFINED);
    ja->add(Object::null());
    ja->add(a);

    String::CPtr s7 = json::stringify(ja);
    String::CPtr s8 = str("[null,null,[3,{\"x\":123,\"y\":[]}]]");
    String::CPtr s9 = str("[null,null,[3,{\"y\":[],\"x\":123}]]");
    ASSERT_TRUE(s7->equals(s8) || s7->equals(s9));
}

TEST(GTestJson, TestParse) {
    String::CPtr json =
        String::create("{\"x\":123,\"y\":[3.5,false],\"z\":null}");
    Value v = json::parse(json);
    ASSERT_TRUE(v.instanceof(Type<Map>::id()));
    ASSERT_TRUE(v.instanceof(Type<JsObject>::id()));

    JsObject::CPtr m = toCPtr<JsObject>(v);
    ASSERT_EQ(3, m->size());

    Value xv = m->get(String::create("x"));
    ASSERT_EQ(Type<Long>::id(), xv.type());

    Long l;
    to<Long>(xv, &l);
    ASSERT_EQ(123, l);

    Value yv = m->get(String::create("y"));
    ASSERT_TRUE(yv.instanceof(Type<ArrayList>::id()));
    ASSERT_TRUE(yv.instanceof(Type<JsArray>::id()));

    JsArray::CPtr a = toCPtr<JsArray>(yv);
    Value a0 = a->get(0);
    ASSERT_EQ(Type<Double>::id(), a0.type());

    Double d;
    to<Double>(a0, &d);
    ASSERT_EQ(3.5, d);

    Value a1 = a->get(1);
    ASSERT_EQ(Type<Boolean>::id(), a1.type());

    Boolean b;
    to<Boolean>(a1, &b);
    ASSERT_FALSE(b);

    Value zv = m->get(String::create("z"));
    ASSERT_TRUE(zv.equals(Object::null()));
}

TEST(GTestJson, TestParseString) {
    String::CPtr s = String::create("\"\\u0026\"");
    ASSERT_TRUE(json::parse(s).equals(String::create("&")));

    String::CPtr s1 = String::create("\"\\ud84c\\udfd0\"");
#ifdef LIBJ_USE_UTF32
    String::CPtr s2 = String::create(0x233d0);
#else
    std::u16string s16;
    s16 += static_cast<char16_t>(0xd84c);
    s16 += static_cast<char16_t>(0xdfd0);
    String::CPtr s2 = String::create(s16);
#endif
    ASSERT_TRUE(json::parse(s1).equals(s2));

    std::u32string s32;
    s32 += 0x3042;
    s32 += 0x2000b;
    s = String::create(s32);
    StringBuilder::Ptr sb = StringBuilder::create();
    sb->appendStr("\"");
    sb->append(s);
    sb->appendStr("\"");
    ASSERT_TRUE(json::parse(sb->toString()).equals(s));
}

TEST(GTestJson, TestParseNumber) {
    String::CPtr s1 = String::create("1.0");
    Value v1 = json::parse(s1);
    ASSERT_EQ(Type<Long>::id(), v1.type());
    ASSERT_TRUE(v1.equals(static_cast<Long>(1)));

    String::CPtr s2 = String::create("9007199254740990");
    Value v2 = json::parse(s2);
    ASSERT_TRUE(v2.equals(static_cast<Long>(9007199254740990)));

    String::CPtr s3 = String::create("9007199254740991");
    Value v3 = json::parse(s3);
    ASSERT_TRUE(v3.equals(static_cast<Long>(9007199254740991)));

    String::CPtr s4 = String::create("9007199254740992");
    Value v4 = json::parse(s4);
    ASSERT_TRUE(v4.equals(9007199254740992.0));

    String::CPtr s5 = String::create("9007199254740993");
    Value v5 = json::parse(s5);
    ASSERT_TRUE(v5.equals(9007199254740992.0));

    String::CPtr s6 = String::create("-9007199254740990");
    Value v6 = json::parse(s6);
    ASSERT_TRUE(v6.equals(static_cast<Long>(-9007199254740990)));

    String::CPtr s7 = String::create("-9007199254740991");
    Value v7 = json::parse(s7);
    ASSERT_TRUE(v7.equals(static_cast<Long>(-9007199254740991)));

    String::CPtr s8 = String::create("-9007199254740992");
    Value v8 = json::parse(s8);
    ASSERT_TRUE(v8.equals(-9007199254740992.0));

    String::CPtr s9 = String::create("-9007199254740993");
    Value v9 = json::parse(s9);
    ASSERT_TRUE(v9.equals(-9007199254740992.0));

    String::CPtr s10 = String::create("1.7976931348623157e+308");
    Value v10 = json::parse(s10);
    ASSERT_TRUE(v10.equals(1.7976931348623157e+308));
}

TEST(GTestJson, TestParseError) {
    Value v1 = json::parse(String::null());
    ASSERT_TRUE(v1.instanceof(Type<Error>::id()));

    Value v2 = json::parse(String::create("\""));
    ASSERT_TRUE(v2.instanceof(Type<Error>::id()));
}

TEST(GTestJson, TestEscape) {
    String::CPtr str = String::create("\b\f\n\r\t'\"\\");
    String::CPtr json = String::create("\"\\b\\f\\n\\r\\t'\\\"\\\\\"");
    String::CPtr s = toCPtr<String>(json::parse(json));
    ASSERT_TRUE(s->equals(str));
    s = json::stringify(str);
    ASSERT_TRUE(s->equals(json));
}

}  // namespace libj
