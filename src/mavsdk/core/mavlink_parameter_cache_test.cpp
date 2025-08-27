#include <gtest/gtest.h>
#include "param_value.h"
#include "mavlink_parameter_cache.h"

using namespace mavsdk;

TEST(ParamValue, CustomComparator)
{
    ParamValue param_value1;
    param_value1.set(static_cast<int>(10));
    ParamValue param_value2;
    param_value2.set(static_cast<int>(10));
    // comparing 2 values of same type and same value
    EXPECT_EQ(param_value1, param_value2);
    // samy type but different values
    param_value2.set(static_cast<int>(11));
    EXPECT_NE(param_value1, param_value2);
    // different type, same value but int and float.
    param_value2.set(static_cast<float>(10));
    EXPECT_EQ(false, param_value1.is_same_type(param_value2));
    EXPECT_NE(param_value1, param_value2);
    // string comparisons
    param_value1.set(std::string("Hello"));
    param_value2.set(std::string("Hello"));
    EXPECT_EQ(param_value1, param_value2);
    param_value1.set(static_cast<int>(10));
    param_value2.set(std::string("10"));
    EXPECT_NE(param_value1, param_value2);
}

struct TestParam {
    const std::string id;
    ParamValue value;
};

TEST(MavlinkParameterCache, AddingAndUpdating)
{
    TestParam int_value1{"INT_VALUE", {}};
    int_value1.value.set(static_cast<int32_t>(10));
    TestParam custom_value1{"CUST_VALUE", {}};
    custom_value1.value.set(std::string("10"));

    MavlinkParameterCache cache;
    EXPECT_EQ(
        MavlinkParameterCache::AddNewParamResult::Ok,
        cache.add_new_param(int_value1.id, int_value1.value));
    // cannot add same id twice
    EXPECT_EQ(
        MavlinkParameterCache::AddNewParamResult::AlreadyExists,
        cache.add_new_param(int_value1.id, int_value1.value));
    EXPECT_EQ(
        MavlinkParameterCache::UpdateExistingParamResult::Ok,
        cache.update_existing_param(int_value1.id, int_value1.value));
    // haven't added it yet
    EXPECT_EQ(
        MavlinkParameterCache::UpdateExistingParamResult::MissingParam,
        cache.update_existing_param(custom_value1.id, custom_value1.value));
    EXPECT_EQ(
        MavlinkParameterCache::AddNewParamResult::Ok,
        cache.add_new_param(custom_value1.id, custom_value1.value));
    // now it should be available
    EXPECT_EQ(
        MavlinkParameterCache::UpdateExistingParamResult::Ok,
        cache.update_existing_param(custom_value1.id, custom_value1.value));
    // update existing but wrong type
    EXPECT_EQ(
        MavlinkParameterCache::UpdateExistingParamResult::WrongType,
        cache.update_existing_param(custom_value1.id, int_value1.value));
}

TEST(MavlinkParameterCache, MissingIndicesSorted)
{
    MavlinkParameterCache cache;
    ParamValue value;
    // We use all the same value, we don't care about that part.
    value.set_int(42);

    cache.add_new_param("PARAM0", value);
    cache.add_new_param("PARAM1", value);

    // No result when the count matches the contents.
    EXPECT_EQ(cache.next_missing_indices(2, 10).size(), 0);

    // The next entry when the count is bigger.
    ASSERT_EQ(cache.next_missing_indices(3, 10).size(), 1);
    EXPECT_EQ(cache.next_missing_indices(3, 10)[0], 2);
    EXPECT_EQ(cache.next_missing_indices(10, 10)[0], 2);

    cache.add_new_param("PARAM2", value, 5);
    std::vector<uint16_t> result = {2, 3, 4};
    EXPECT_EQ(cache.next_missing_indices(6, 10), result);
}

TEST(MavlinkParameterCache, MissingInChunks)
{
    MavlinkParameterCache cache;
    ParamValue value;
    // We use all the same value, we don't care about that part.
    value.set_int(42);

    cache.add_new_param("PARAM0", value);
    cache.add_new_param("PARAM1", value);
    cache.add_new_param("PARAM2", value);
    cache.add_new_param("PARAM3", value);

    // All remaining one are returned.
    EXPECT_EQ(cache.next_missing_indices(10, 10).size(), 6);

    // Only up to chunk size are returned
    EXPECT_EQ(cache.next_missing_indices(10, 3).size(), 3);
}

TEST(MavlinkParameterCache, MissingIndicesNotSorted)
{
    MavlinkParameterCache cache;
    ParamValue value;
    // We use all the same value, we don't care about that part.
    value.set_int(42);

    cache.add_new_param("PARAM0", value, 1);
    cache.add_new_param("PARAM1", value, 3);

    // It should still work when not sorted.
    std::vector<uint16_t> result = {0, 2};
    EXPECT_EQ(cache.next_missing_indices(3, 10), result);
}
