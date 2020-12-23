#include "veg/dynamic_stack.hpp"
#include <gtest/gtest.h>

#include <atomic>

using namespace veg;

struct S {
private:
  static auto n_instances_mut() -> std::atomic<int>& {
    static std::atomic<int> var{0};
    return var;
  }

public:
  static auto n_instances() -> int { return n_instances_mut(); }

  S(S const&) = delete;
  S(S&&) = delete;
  auto operator=(S const&) -> S& = delete;
  auto operator=(S&&) -> S& = delete;
  S() { ++n_instances_mut(); }
  ~S() { --n_instances_mut(); }
};

TEST(dynamic_stack, raii) {
  unsigned char buf[4096];

  dynamic_stack_view stack{slice<unsigned char>(buf)};

  {
    auto s1 = stack.make_new(tag<S>, 3).unwrap();
    EXPECT_NE(s1.data(), nullptr);
    EXPECT_EQ(s1.size(), 3);
    EXPECT_EQ(stack.remaining_bytes(), 4093);
    EXPECT_EQ(S::n_instances(), 3);

    {
      auto s2 = stack.make_new(tag<S>, 4).unwrap();
      EXPECT_NE(s2.data(), nullptr);
      EXPECT_EQ(s2.size(), 4);
      EXPECT_EQ(stack.remaining_bytes(), 4089);
      EXPECT_EQ(S::n_instances(), 7);

      {
        auto i3 = stack.make_new(tag<int>, 30000);
        EXPECT_FALSE(i3);
        EXPECT_EQ(stack.remaining_bytes(), 4089);
        {
          auto i4 = stack.make_new(tag<int>, 300).unwrap();
          EXPECT_NE(i4.data(), nullptr);
          EXPECT_EQ(i4.size(), 300);
          EXPECT_LT(stack.remaining_bytes(), 4089 - 300 * sizeof(int));
        }
      }
    }
    EXPECT_EQ(stack.remaining_bytes(), 4093);
    EXPECT_EQ(S::n_instances(), 3);
  }
  EXPECT_EQ(stack.remaining_bytes(), 4096);
  EXPECT_EQ(S::n_instances(), 0);

  auto s1 = stack.make_new(tag<S const>, 3).unwrap();
  EXPECT_EQ(stack.remaining_bytes(), 4093);
  EXPECT_EQ(S::n_instances(), 3);
}

TEST(dynamic_stack, evil_reorder) {
  unsigned char buf[4096];
  dynamic_stack_view stack{make::slice(buf)};
  auto good = [&] {
    auto s1 = stack.make_new(tag<int>, 30);
    auto s2 = stack.make_new(tag<double>, 20);
    auto s3 = VEG_MOV(s2);
    return true;
  };
  auto bad = [&] {
    auto s1 = stack.make_new(tag<int>, 30);
    auto s2 = stack.make_new(tag<double>, 20);
    auto s3 = VEG_MOV(s1);
  };
  EXPECT_TRUE(good());
  EXPECT_DEATH(bad(), "");
}

TEST(dynamic_stack, return ) {
  unsigned char buf[4096];
  dynamic_stack_view stack(make::slice(buf));

  auto s = [&] {
    auto s1 = stack.make_new(tag<S>, 3).unwrap();
    auto s2 = stack.make_new(tag<S>, 4).unwrap();
    auto s3 = stack.make_new(tag<S>, 5).unwrap();
    EXPECT_EQ(stack.remaining_bytes(), 4084);
    EXPECT_EQ(S::n_instances(), 12);
    return s1;
  }();

  EXPECT_EQ(stack.remaining_bytes(), 4093);
  EXPECT_EQ(S::n_instances(), 3);

  EXPECT_NE(s.data(), nullptr);
  EXPECT_EQ(s.size(), 3);
}

TEST(dynamic_stack, manual_lifetimes) {
  unsigned char buf[4096];
  dynamic_stack_view stack(make::slice(buf));

  auto s = stack.make_alloc(tag<S>, 3).unwrap();
  EXPECT_NE(s.data(), nullptr);
  EXPECT_EQ(s.size(), 3);
  EXPECT_EQ(S::n_instances(), 0);

  {
    new (s.data() + 0) S{};
    EXPECT_EQ(S::n_instances(), 1);
    new (s.data() + 1) S{};
    EXPECT_EQ(S::n_instances(), 2);
    new (s.data() + 2) S{};
    EXPECT_EQ(S::n_instances(), 3);

    (s.data() + 2)->~S();
    EXPECT_EQ(S::n_instances(), 2);
    (s.data() + 1)->~S();
    EXPECT_EQ(S::n_instances(), 1);
    (s.data() + 0)->~S();
    EXPECT_EQ(S::n_instances(), 0);
  }
  EXPECT_EQ(S::n_instances(), 0);
}
