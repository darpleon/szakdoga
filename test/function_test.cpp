#include <gtest/gtest.h>
#include "Polynomial.hpp"
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
    Variable x, y, z;
};

TEST_F(PowerPermutationTest, OperatorConstructionWorks)
{
    PowerPermutation pp1 = (x^3) * (y^2) * (z^1);
    PowerPermutation pp2 = y * (x*x) * (x*y) * z;
    PowerPermutation ppc{
	{x, 3},
	{y, 2},
	{z, 1}};
    EXPECT_EQ(pp1, ppc);
    EXPECT_EQ(pp2, ppc);
}

TEST_F(PowerPermutationTest, NoZeroPower)
{
    PowerPermutation pp1{
	{x, 0},
	{y, 3}};
    PowerPermutation ppc{
	{y, 3}};
    EXPECT_EQ(pp1, ppc);
    PowerPermutation pp2 = x^0;
    EXPECT_TRUE(pp2.get_monomials().empty());
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

class PolynomialTest : public testing::Test
{
protected:
    Variable x, y, z;
};

TEST_F(PolynomialTest, OperatorConstructionWorks)
{
    Polynomial<double> po = (x^3)*(y^2)*2.0 + (z^4)*x*4.0;
    Polynomial<double> pi{
	{{{x, 3}, {y, 2}}, 2.0},
	{{{z, 4}, {x, 1}}, 4.0}};
    EXPECT_EQ(po, pi);
}
