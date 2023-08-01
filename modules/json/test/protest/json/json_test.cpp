#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "protest/json/json.h"

TEST(json, should_parse)
{
  protest::json::JsonParser json;

  auto ret =
      json.parse("../../../modules/json/test/protest/json/test_file.json");

  ASSERT_TRUE(ret);
}

TEST(json, should_identify_object)
{
  protest::json::JsonParser json;

  json.parse("../../../modules/json/test/protest/json/test_file.json");
  auto value = json.getValue();

  ASSERT_TRUE(value.isObject());
}

TEST(json, should_access_object)
{
  protest::json::JsonParser json;

  json.parse("../../../modules/json/test/protest/json/test_file.json");
  auto value = json.getValue();

  ASSERT_TRUE(value.isObject());
}

TEST(json, should_check_if_key_exists)
{
  protest::json::JsonParser json;

  json.parse("../../../modules/json/test/protest/json/test_file.json");
  auto value = json.getValue();
  auto object = value.getAsObject();

  ASSERT_TRUE(object.hasKey("test"));
}

TEST(json, should_check_if_key_doesnt_exists)
{
  protest::json::JsonParser json;

  json.parse("../../../modules/json/test/protest/json/test_file.json");
  auto value = json.getValue();
  auto object = value.getAsObject();

  ASSERT_FALSE(object.hasKey("iuae"));
}

TEST(json, should_access_value_of_key)
{
  protest::json::JsonParser json;

  json.parse("../../../modules/json/test/protest/json/test_file.json");
  auto value = json.getValue();
  auto object = value.getAsObject();
  auto child = object.get("test");

  ASSERT_TRUE(child.isInteger());
}

TEST(json, should_return_integer_value)
{
  protest::json::JsonParser json;

  json.parse("../../../modules/json/test/protest/json/test_file.json");
  auto value = json.getValue();
  auto object = value.getAsObject();
  auto child = object.get("test");

  ASSERT_EQ(child.getInteger(), 42);
}

TEST(json, should_return_array)
{
  protest::json::JsonParser json;

  json.parse("../../../modules/json/test/protest/json/test_file2.json");
  auto value = json.getValue();
  auto object = value.getAsObject();
  auto child = object.get("test");
  auto array = child.getAsArray();
  auto first = array.get(0);

  ASSERT_TRUE(child.isArray());
  ASSERT_EQ(array.numberOfElements(), 4);
  ASSERT_TRUE(first.isInteger());
}
