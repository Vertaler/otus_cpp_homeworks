#define BOOST_TEST_MODULE test_version

#include "helloworld/lib.h"

#include <gtest/gtest.h>

TEST(test_version, test_valid_version) {
  EXPECT_GT(version(), 0) << "Version must be greater than zero";
}
