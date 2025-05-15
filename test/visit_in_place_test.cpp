/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include <qtils/visit_in_place.hpp>
#include <utility>

struct Animal {
  virtual ~Animal() = default;
  virtual std::string_view say() const = 0;
};

struct Cat : Animal {
  std::string_view say() const override {
    return "meow";
  };
};

struct Dog : Animal {
  std::string_view say() const override {
    return "woof";
  };
};

struct Said {
  Said(auto said) : said{said} {}
  std::string said;
};

using Pet = std::variant<Cat, Dog>;

TEST(VisitInPlace, AllVisitors) {
  Pet pet = Cat{};

  auto said = qtils::visit_in_place(
      pet,
      [](Cat &animal) { return animal.say(); },
      [](Dog &animal) { return animal.say(); });

  EXPECT_EQ(said, "meow");
}

TEST(VisitInPlace, UniversalVisitor) {
  Pet pet = Cat{};

  auto said = qtils::visit_in_place(  //
      pet,
      [](auto &animal) { return animal.say(); });

  EXPECT_EQ(said, "meow");
}

TEST(VisitInPlace, CommonVisitor) {
  Pet pet = Cat{};

  auto said = qtils::visit_in_place(  //
      pet,
      [](Animal &animal) { return animal.say(); });

  EXPECT_EQ(said, "meow");
}

TEST(VisitInPlace, VisitorWithDefinedResultType) {
  Pet pet = Cat{};

  auto said = qtils::visit_in_place<Said>(  //
      pet,
      [](Animal &animal) { return animal.say(); });

  EXPECT_EQ(said.said, "meow");
}

TEST(VisitInPlace, VisitorWithInferedResultType) {
  Pet pet = Cat{};

  decltype(auto) said = qtils::visit_in_place(
      pet,
      [](Cat &animal) -> Said { return animal.say(); },
      [](Dog &animal) { return std::string{animal.say()}; });

  EXPECT_EQ(said.said, "meow");
}

TEST(VisitInPlace, VisitByValue) {
  Pet pet = Cat{};

  auto said = qtils::visit_in_place(  //
      pet,
      [](auto animal) { return animal.say(); });

  EXPECT_EQ(said, "meow");
}

TEST(VisitInPlace, VisitByRef) {
  Pet pet = Cat{};

  auto said = qtils::visit_in_place(  //
      pet,
      [](auto &animal) { return animal.say(); });

  EXPECT_EQ(said, "meow");
}

TEST(VisitInPlace, VisitByConstRef) {
  Pet pet = Cat{};

  auto said = qtils::visit_in_place(  //
      pet,
      [](auto &animal) { return animal.say(); });

  EXPECT_EQ(said, "meow");
}

TEST(VisitInPlace, VisitOfMoved) {
  Pet pet = Cat{};

  auto said = qtils::visit_in_place(  //
      std::move(pet),
      [](auto &&animal) { return animal.say(); });

  EXPECT_EQ(said, "meow");
}
