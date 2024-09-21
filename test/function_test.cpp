#include <gtest/gtest.h>
#include "PowerPermutation.hpp"
#include "Variable.hpp"

TEST(VariableTest, CreatedSeparately_NotEqual)
{
    Variable x;
    Variable y;
    EXPECT_NE(x, y);
}

TEST(VariableTest, CopyConstructed_Equal)
{
    Variable x;
    Variable y{x};
    EXPECT_EQ(x, y);
}

class PowerPermutationTest : public testing::Test
{
protected:
    PowerPermutationTest() :
	x{}, y{}, z{}
    {}

    Variable x, y, z;
};

TEST_F(PowerPermutationTest, OperatorConstructionWorks)
{
    PowerPermutation ppo = (x^3) * (y^2) * z;
    PowerPermutation ppc{
	{x, 3},
	{y, 2},
	{z, 1}};
    EXPECT_EQ(ppo, ppc);
}

TEST_F(PowerPermutationTest, NoZeroPower)
{
    PowerPermutation pp1{
	{x, 0},
	{y, 3}};

    PowerPermutation pp2{
	{y, 3},
	{z, 0}};
    EXPECT_EQ(pp1, pp2);
}

TEST_F(PowerPermutationTest, Multiply_SameVariablesMerge)
{
    PowerPermutation pp1({
	{x, 2},
	{y, 5}});

    PowerPermutation pp2({
	{x, 1},
	{z, 4}});

    PowerPermutation product = pp1 * pp2;
    auto& mn = product.get_monomials();
    EXPECT_EQ(mn.size(), 3);
    EXPECT_EQ(mn.at(x), 3);
}
