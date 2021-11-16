#define BOOST_TEST_MODULE test_version

#include "ip_filter/ip_filter.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

const IPList TEST_IP_LIST = {IPAddress{"1.2.3.4"}, IPAddress{"1.2.3.4"},
                             IPAddress{"1.2.5.3"}, IPAddress{"2.0.0.0"},
                             IPAddress{"2.2.2.2"}, IPAddress{"2.3.1.1"},
                             IPAddress{"3.4.4.1"}};

TEST(test_split, empty_string) {
  ASSERT_THAT(split("", '.'), testing::ElementsAre(""));
}

TEST(test_split, no_delimeter) {
  ASSERT_THAT(split("11", '.'), testing::ElementsAre("11"));
}

TEST(test_split, only_delimeters) {
  ASSERT_THAT(split("..", '.'), testing::ElementsAre("", "", ""));
}

TEST(test_split, delimeter_at_the_end) {
  ASSERT_THAT(split("11.", '.'), testing::ElementsAre("11", ""));
}

TEST(test_split, delimeter_at_the_begining) {
  ASSERT_THAT(split(".11", '.'), testing::ElementsAre("", "11"));
}

TEST(test_split, delimeter_in_the_middle) {
  ASSERT_THAT(split("11.12", '.'), testing::ElementsAre("11", "12"));
}

TEST(test_ip, test_valid_ip) {
  IPAddress ip{"1.2.3.4"};
  EXPECT_EQ(ip.ToString(), "1.2.3.4");
}

TEST(test_ip, test_wrong_count_of_parts) {
  EXPECT_THROW(IPAddress{"1.2.3.4.5"}, std::invalid_argument);
  EXPECT_THROW(IPAddress{"1.2.3"}, std::invalid_argument);
}

TEST(test_ip, test_part_out_of_rang) {
  EXPECT_THROW(IPAddress{"1.2.3.257"}, std::invalid_argument);
  EXPECT_THROW(IPAddress{"-1.2.3.4"}, std::invalid_argument);
}

TEST(test_ip, test_invalid_part) {
  EXPECT_THROW(IPAddress{"1.a.3.4."}, std::invalid_argument);
}

TEST(test_ip, test_comparison) {
  EXPECT_GT(IPAddress{"1.2.3.4"}, IPAddress{"0.2.3.4"});
  EXPECT_GT(IPAddress{"1.2.3.5"}, IPAddress{"1.2.3.4"});
}

TEST(test_ip_filter, test_filter_by_1_part) {
  const auto filtered = filter(TEST_IP_LIST, 1);
  ASSERT_THAT(filtered,
              testing::ElementsAre(IPAddress{"1.2.3.4"}, IPAddress{"1.2.3.4"},
                                   IPAddress{"1.2.5.3"}));
}

TEST(test_ip_filter, test_filter_by_2_parts) {
  const auto filtered = filter(TEST_IP_LIST, 1, 2);
  ASSERT_THAT(filtered,
              testing::ElementsAre(IPAddress{"1.2.3.4"}, IPAddress{"1.2.3.4"},
                                   IPAddress{"1.2.5.3"}));
}

TEST(test_ip_filter, test_filter_by_3_parts) {
  const auto filtered = filter(TEST_IP_LIST, 1, 2, 3);
  ASSERT_THAT(filtered,
              testing::ElementsAre(IPAddress{"1.2.3.4"}, IPAddress{"1.2.3.4"}));
}

TEST(test_ip_filter, test_filter_by_4_parts) {
  const auto filtered = filter(TEST_IP_LIST, 1, 2, 5, 3);
  ASSERT_THAT(filtered, testing::ElementsAre(IPAddress{"1.2.5.3"}));
}

TEST(test_ip_filter, test_filter_any) {
  const auto filtered = filterAny(TEST_IP_LIST, 3);
  ASSERT_THAT(filtered,
              testing::ElementsAre(IPAddress{"1.2.3.4"}, IPAddress{"1.2.3.4"},
                                   IPAddress{"1.2.5.3"}, IPAddress{"2.3.1.1"},
                                   IPAddress{"3.4.4.1"}));
}
