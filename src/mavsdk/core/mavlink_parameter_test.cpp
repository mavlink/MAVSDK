#include <gtest/gtest.h>
#include "param_value.h"
#include "mavlink_parameter_set.h"

using namespace mavsdk;

TEST(ParamValue, CustomComparator)
{
    ParamValue param_value1;
    param_value1.set(static_cast<int>(10));
    ParamValue param_value2;
    param_value2.set(static_cast<int>(10));
    // comparing 2 values of same type and same value
    EXPECT_EQ(param_value1,param_value2);
    // samy type but different values
    param_value2.set(static_cast<int>(11));
    EXPECT_NE(param_value1,param_value2);
    // different type ,same value but int and float.
    param_value2.set(static_cast<float>(10));
    EXPECT_NE(param_value1,param_value2);
    //
    param_value1.set(std::string("Hello"));
    param_value2.set(std::string("Hello"));
    EXPECT_EQ(param_value1,param_value2);
    param_value1.set(static_cast<int>(10));
    param_value2.set(std::string("10"));
    EXPECT_NE(param_value1,param_value2);
}

struct TestParam{
    const std::string id;
    ParamValue value;
};

TEST(MavlinkParameterSet, SimpleTest)
{
    TestParam int_value1{"INT_VALUE",{}};
    int_value1.value.set(static_cast<int32_t>(10));
    TestParam custom_value1{"CUST_VALUE",{}};
    custom_value1.value.set(std::string("10"));

    MavlinkParameterSet mavlink_parameter_set;
    EXPECT_EQ(true,mavlink_parameter_set.add_new_parameter(int_value1.id,int_value1.value));
    // cannot add same id twice
    EXPECT_EQ(false,mavlink_parameter_set.add_new_parameter(int_value1.id,int_value1.value));
    EXPECT_EQ(MavlinkParameterSet::UpdateExistingParamResult::SUCCESS,mavlink_parameter_set.update_existing_parameter(int_value1.id,int_value1.value));
    // haven't added it yet
    EXPECT_EQ(MavlinkParameterSet::UpdateExistingParamResult::MISSING_PARAM,mavlink_parameter_set.update_existing_parameter(custom_value1.id,custom_value1.value));
    EXPECT_EQ(true,mavlink_parameter_set.add_new_parameter(custom_value1.id,custom_value1.value));
    // now it should be available
    EXPECT_EQ(MavlinkParameterSet::UpdateExistingParamResult::SUCCESS,mavlink_parameter_set.update_existing_parameter(custom_value1.id,custom_value1.value));
    // update existing but wrong type
    EXPECT_EQ(MavlinkParameterSet::UpdateExistingParamResult::WRONG_PARAM_TYPE,mavlink_parameter_set.update_existing_parameter(custom_value1.id,int_value1.value));
}


