/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include <qtils/shared_ref.hpp>

#include <set>
#include <vector>

using namespace qtils;

/**
 * @given A null shared_ptr
 * @when Constructing SharedRef
 * @then Throws std::invalid_argument
 */
TEST(SharedRefTest, ThrowsOnNull) {
  std::shared_ptr<int> null_ptr;
  EXPECT_THROW(SharedRef<int> ptr(null_ptr), std::invalid_argument);
}

/**
 * @given A valid shared_ptr<int>
 * @when Wrapped into SharedRef
 * @then Dereferencing and pointer access work correctly
 */
TEST(SharedRefTest, BasicUsage) {
  auto raw = std::make_shared<int>(42);
  SharedRef<int> ptr(raw);

  EXPECT_EQ(*ptr, 42);
  EXPECT_EQ(ptr.get(), raw.get());
  EXPECT_TRUE(ptr);
  EXPECT_EQ(ptr.use_count(), 2);
}

/**
 * @given A valid SharedRef<int>
 * @when Used in boolean context
 * @then Returns true
 */
TEST(SharedRefTest, BoolOperator) {
  SharedRef<int> ptr(std::make_shared<int>(1));
  EXPECT_TRUE(ptr);
}

/**
 * @given A shared_ptr<int> and SharedRef<int> pointing to the same object
 * @when Compared using operator== and !=
 * @then Return correct equality results
 */
TEST(SharedRefTest, EqualityWithSharedPtr) {
  auto raw = std::make_shared<int>(100);
  SharedRef<int> ptr1(raw);
  std::shared_ptr<int> sp2 = raw;

  EXPECT_TRUE(ptr1 == sp2);
  EXPECT_FALSE(ptr1 != sp2);
}

/**
 * @given Two SharedRef<int> pointing to the same object
 * @when Compared using operator== and !=
 * @then Return true and false respectively
 */
TEST(SharedRefTest, EqualityWithSharedRef) {
  auto raw = std::make_shared<int>(100);
  SharedRef<int> ptr1(raw);
  SharedRef<int> ptr2(raw);

  EXPECT_TRUE(ptr1 == ptr2);
  EXPECT_FALSE(ptr1 != ptr2);
}

/**
 * @given Two SharedRef<int> with different values
 * @when Swapped
 * @then Their values are exchanged
 */
TEST(SharedRefTest, Swap) {
  auto p1 = SharedRef<int>(std::make_shared<int>(1));
  auto p2 = SharedRef<int>(std::make_shared<int>(2));

  swap(p1, p2);

  EXPECT_EQ(*p1, 2);
  EXPECT_EQ(*p2, 1);
}

/**
 * @given Two distinct SharedRef<int> instances
 * @when owner_before is called
 * @then One is ordered before the other, but not both
 */
TEST(SharedRefTest, OwnerBefore) {
  auto p1 = SharedRef<int>(std::make_shared<int>(1));
  auto p2 = SharedRef<int>(std::make_shared<int>(2));

  bool a = p1.owner_before(p2);
  bool b = p2.owner_before(p1);

  EXPECT_NE(a, b);
}

/**
 * @given A std::set of SharedRef<int>
 * @when Inserting two different pointers
 * @then Both are stored without conflict
 */
TEST(SharedRefTest, OperatorLessForSet) {
  std::set<SharedRef<int>> s;
  s.insert(SharedRef<int>(std::make_shared<int>(5)));
  s.insert(SharedRef<int>(std::make_shared<int>(10)));

  EXPECT_EQ(s.size(), 2);
}

/**
 * @given A SharedRef<std::vector<int>> containing elements
 * @when Accessing an element via operator[]
 * @then Returns the correct value
 */
TEST(SharedRefTest, IndexOperatorVector) {
  auto vec = std::make_shared<std::vector<int>>();
  vec->push_back(123);
  SharedRef<std::vector<int>> ptr(vec);

  EXPECT_EQ(ptr[0], 123);
}
