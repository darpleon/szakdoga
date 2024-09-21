#include <gtest/gtest.h>
#include "Variable.hpp"

TEST(CreateVariablesTest, NotEqual) {
	Variable x;
	Variable y;
	EXPECT_NE(x, y);
}
