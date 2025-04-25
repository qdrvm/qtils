/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include <qtils/strict_sptr.hpp>
#include <vector>
#include <set>

using namespace qtils;

/**
 * @given A null shared_ptr
 * @when Constructing StrictSharedPtr
 * @then Throws std::invalid_argument
 */
TEST(StrictSharedPtrTest, ThrowsOnNull) {
  std::shared_ptr<int> null_ptr;
  EXPECT_THROW(StrictSharedPtr<int> ptr(null_ptr), std::invalid_argument);
}

/**
 * @given A valid shared_ptr<int>
 * @when Wrapped into StrictSharedPtr
 * @then Dereferencing and pointer access work correctly
 */
TEST(StrictSharedPtrTest, BasicUsage) {
  auto raw = std::make_shared<int>(42);
  StrictSharedPtr<int> ptr(raw);

  EXPECT_EQ(*ptr, 42);
  EXPECT_EQ(ptr.get(), raw.get());
  EXPECT_TRUE(ptr);
  EXPECT_EQ(ptr.use_count(), 2);
}

/**
 * @given A valid StrictSharedPtr<int>
 * @when Used in boolean context
 * @then Returns true
 */
TEST(StrictSharedPtrTest, BoolOperator) {
  StrictSharedPtr<int> ptr(std::make_shared<int>(1));
  EXPECT_TRUE(ptr);
}

/**
 * @given A shared_ptr<int> and StrictSharedPtr<int> pointing to the same object
 * @when Compared using operator== and !=
 * @then Return correct equality results
 */
TEST(StrictSharedPtrTest, EqualityWithSharedPtr) {
  auto raw = std::make_shared<int>(100);
  StrictSharedPtr<int> ptr1(raw);
  std::shared_ptr<int> sp2 = raw;

  EXPECT_TRUE(ptr1 == sp2);
  EXPECT_FALSE(ptr1 != sp2);
}

/**
 * @given Two StrictSharedPtr<int> pointing to the same object
 * @when Compared using operator== and !=
 * @then Return true and false respectively
 */
TEST(StrictSharedPtrTest, EqualityWithStrictSharedPtr) {
  auto raw = std::make_shared<int>(100);
  StrictSharedPtr<int> ptr1(raw);
  StrictSharedPtr<int> ptr2(raw);

  EXPECT_TRUE(ptr1 == ptr2);
  EXPECT_FALSE(ptr1 != ptr2);
}

/**
 * @given Two StrictSharedPtr<int> with different values
 * @when Swapped
 * @then Their values are exchanged
 */
TEST(StrictSharedPtrTest, Swap) {
  auto p1 = StrictSharedPtr<int>(std::make_shared<int>(1));
  auto p2 = StrictSharedPtr<int>(std::make_shared<int>(2));

  swap(p1, p2);

  EXPECT_EQ(*p1, 2);
  EXPECT_EQ(*p2, 1);
}

/**
 * @given Two distinct StrictSharedPtr<int> instances
 * @when owner_before is called
 * @then One is ordered before the other, but not both
 */
TEST(StrictSharedPtrTest, OwnerBefore) {
  auto p1 = StrictSharedPtr<int>(std::make_shared<int>(1));
  auto p2 = StrictSharedPtr<int>(std::make_shared<int>(2));

  bool a = p1.owner_before(p2);
  bool b = p2.owner_before(p1);

  EXPECT_NE(a, b);
}

/**
 * @given A std::set of StrictSharedPtr<int>
 * @when Inserting two different pointers
 * @then Both are stored without conflict
 */
TEST(StrictSharedPtrTest, OperatorLessForSet) {
  std::set<StrictSharedPtr<int>> s;
  s.insert(StrictSharedPtr<int>(std::make_shared<int>(5)));
  s.insert(StrictSharedPtr<int>(std::make_shared<int>(10)));

  EXPECT_EQ(s.size(), 2);
}

/**
 * @given A StrictSharedPtr<std::vector<int>> containing elements
 * @when Accessing an element via operator[]
 * @then Returns the correct value
 */
TEST(StrictSharedPtrTest, IndexOperatorVector) {
  auto vec = std::make_shared<std::vector<int>>();
  vec->push_back(123);
  StrictSharedPtr<std::vector<int>> ptr(vec);

  EXPECT_EQ(ptr[0], 123);
}