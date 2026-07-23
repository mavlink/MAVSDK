#include "param_value.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <sstream>

using namespace mavsdk;

TEST(ParamValue, SetFromXmlCommonTypes)
{
    ParamValue v;
    ASSERT_TRUE(v.set_from_xml("float", "1.25"));
    ASSERT_TRUE(v.get_float().has_value());
    EXPECT_FLOAT_EQ(v.get_float().value(), 1.25f);
    EXPECT_EQ(v.typestr(), "float");

    ASSERT_TRUE(v.set_from_xml("int32", "42"));
    ASSERT_TRUE(v.get_int().has_value());
    EXPECT_EQ(v.get_int().value(), 42);
    EXPECT_EQ(v.typestr(), "int32_t");

    ASSERT_TRUE(v.set_from_xml("int8", "-5"));
    ASSERT_TRUE(v.get_int().has_value());
    EXPECT_EQ(v.get_int().value(), -5);

    ASSERT_TRUE(v.set_from_xml("uint16", "65535"));
    ASSERT_TRUE(v.get_int().has_value());
    EXPECT_EQ(v.get_int().value(), 65535);

    ASSERT_TRUE(v.set_empty_type_from_xml("float"));
    EXPECT_EQ(v.typestr(), "float");
    ASSERT_TRUE(v.get_float().has_value());
}

TEST(ParamValue, SetAsSameTypePreservesKind)
{
    ParamValue f;
    f.set(0.0f);
    ASSERT_TRUE(f.set_as_same_type("3.5"));
    ASSERT_TRUE(f.get_float().has_value());
    EXPECT_FLOAT_EQ(f.get_float().value(), 3.5f);

    ParamValue i;
    i.set(static_cast<int32_t>(1));
    ASSERT_TRUE(i.set_as_same_type("99"));
    ASSERT_TRUE(i.get_int().has_value());
    EXPECT_EQ(i.get_int().value(), 99);
}

TEST(ParamValue, GetIntFloatCustomOptional)
{
    ParamValue i;
    i.set(static_cast<int16_t>(-7));
    ASSERT_TRUE(i.get_int().has_value());
    EXPECT_EQ(i.get_int().value(), -7);
    EXPECT_FALSE(i.get_float().has_value());
    EXPECT_FALSE(i.get_custom().has_value());

    ParamValue f;
    f.set(2.0f);
    ASSERT_TRUE(f.get_float().has_value());
    EXPECT_FALSE(f.get_int().has_value());

    ParamValue s;
    s.set(std::string("tag"));
    ASSERT_TRUE(s.get_custom().has_value());
    EXPECT_EQ(s.get_custom().value(), "tag");
    EXPECT_FALSE(s.get_int().has_value());
}

TEST(ParamValue, UpdateValueTypesafe)
{
    ParamValue a;
    a.set(static_cast<int32_t>(1));
    ParamValue b;
    b.set(static_cast<int32_t>(9));
    a.update_value_typesafe(b);
    EXPECT_EQ(a.get_int().value(), 9);
}

TEST(ParamValue, NeedsExtendedForStringAnd64Bit)
{
    ParamValue s;
    s.set(std::string("hello"));
    EXPECT_TRUE(s.needs_extended());

    ParamValue i64;
    i64.set(static_cast<int64_t>(1));
    EXPECT_TRUE(i64.needs_extended());

    ParamValue u64;
    u64.set(static_cast<uint64_t>(1));
    EXPECT_TRUE(u64.needs_extended());

    ParamValue d;
    d.set(1.0);
    EXPECT_TRUE(d.needs_extended());

    ParamValue f;
    f.set(1.0f);
    EXPECT_FALSE(f.needs_extended());

    ParamValue i32;
    i32.set(static_cast<int32_t>(3));
    EXPECT_FALSE(i32.needs_extended());
}

TEST(ParamValue, IsSameTypeAndEquality)
{
    ParamValue a;
    a.set(static_cast<int32_t>(3));
    ParamValue b;
    b.set(static_cast<int32_t>(3));
    ParamValue c;
    c.set(static_cast<int32_t>(4));
    ParamValue f;
    f.set(3.0f);

    EXPECT_TRUE(a.is_same_type(b));
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a != c);
    EXPECT_TRUE(a < c);
    EXPECT_TRUE(c > a);
    EXPECT_FALSE(a.is_same_type(f));
    // Different types: equality returns false and logs.
    EXPECT_FALSE(a == f);
}

TEST(ParamValue, StreamOperatorAndGetString)
{
    ParamValue i;
    i.set(static_cast<int32_t>(9));
    EXPECT_EQ(i.get_string(), "9");
    std::ostringstream oss;
    oss << i;
    EXPECT_FALSE(oss.str().empty());
    EXPECT_NE(oss.str().find('9'), std::string::npos);
}

TEST(ParamValue, InvalidXmlTypeRejected)
{
    ParamValue v;
    EXPECT_FALSE(v.set_from_xml("not_a_real_type", "1"));
    EXPECT_FALSE(v.set_empty_type_from_xml("not_a_real_type"));
}

TEST(ParamValue, SetIntAndFloatHelpers)
{
    ParamValue v;
    v.set(static_cast<int32_t>(0));
    ASSERT_TRUE(v.set_int(12));
    EXPECT_EQ(v.get_int().value(), 12);
    v.set_float(1.5f);
    ASSERT_TRUE(v.get_float().has_value());
    EXPECT_FLOAT_EQ(v.get_float().value(), 1.5f);
    v.set_custom("z");
    ASSERT_TRUE(v.get_custom().has_value());
    EXPECT_EQ(v.get_custom().value(), "z");
}
