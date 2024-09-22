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

TEST_F(PowerPermutationTest, DefaultConstructor)
{
    PowerPermutation p{};
    EXPECT_TRUE(p.get_monomials().empty());
}

TEST_F(PowerPermutationTest, OperatorConstruction)
{
    PowerPermutation pp1 = (x^3) * (y^2) * (z^1);
    PowerPermutation pp2 = y * (x*x) * (x*y) * z;
    PowerPermutation ref{
	{x, 3},
	{y, 2},
	{z, 1}};
    EXPECT_EQ(pp1, ref);
    EXPECT_EQ(pp2, ref);
}

TEST_F(PowerPermutationTest, NoZeroPower)
{
    PowerPermutation pp1{
	{x, 0},
	{y, 3}};
    PowerPermutation ref{
	{y, 3}};
    EXPECT_EQ(pp1, ref);
    PowerPermutation pp2 = x^0;
    EXPECT_TRUE(pp2.get_monomials().empty());
}

TEST_F(PowerPermutationTest, MultiplyWithPowerPermutation)
{
    PowerPermutation pp1({
	{x, 2},
	{y, 5}});
    PowerPermutation pp2({
	{x, 1},
	{z, 4}});
    PowerPermutation product = pp1 * pp2;
    PowerPermutation ref{
	{x, 3},
	{y, 5},
	{z, 4}};
    const auto& mn = product.get_monomials();
    EXPECT_EQ(product, ref);
}

TEST_F(PowerPermutationTest, Hash)
{
    PowerPermutation pp1 = {{y, 1}, {x, 2}, {z, 4}};
    PowerPermutation pp2 = {{z, 4}, {y, 1}, {x, 2}};
    size_t hash1 = std::hash<PowerPermutation>{}(pp1);
    size_t hash2 = std::hash<PowerPermutation>{}(pp2);
    EXPECT_EQ(hash1, hash2);
}

class PolynomialTest : public testing::Test
{
protected:
    Variable x, y, z;
    Polynomial<double> p1{
	{{{x, 3}, {y, 2}}, 2.0},
	{{{z, 4}, {x, 1}}, 3.0}};
};

TEST_F(PolynomialTest, Identities)
{
    Polynomial<double> p0 = I<Polynomial<double>>::zero;
    EXPECT_TRUE(p0.get_terms().empty());
    Polynomial<double> p1 = I<Polynomial<double>>::one;
    const auto& terms = p1.get_terms();
    EXPECT_EQ(terms.size(), 1);
    EXPECT_DOUBLE_EQ(terms.at(PowerPermutation{}), 1.0);
}

TEST_F(PolynomialTest, OperatorConstruction)
{
    Polynomial<double> po = (x^3)*(y^2)*2.0 + (z^4)*x*3.0;
    EXPECT_EQ(po, p1);
}

TEST_F(PolynomialTest, MultiplyDouble)
{
    Polynomial<double> product = p1 * 3.0;
    Polynomial<double> ref{
	{{{x, 3}, {y, 2}}, 6.0},
	{{{z, 4}, {x, 1}}, 9.0}};
    EXPECT_EQ(product, ref);
}

TEST_F(PolynomialTest, MultiplyPolynomial)
{
    Polynomial<double> multiplier{
	{{{x, 1}, {y, 1}}, 2.0},
	{{{z, 2}}, 1.0}};
    Polynomial<double> ref{
	{{{x, 4}, {y, 3}}, 4.0},
	{{{x, 3}, {y, 2}, {z, 2}}, 2.0},
	{{{x, 2}, {y, 1}, {z, 4}}, 6.0},
	{{{x, 1}, {z, 6}}, 3.0}};
    Polynomial<double> product = p1 * multiplier;
    EXPECT_EQ(product, ref);
}

TEST_F(PolynomialTest, EvaluateDouble)
{
    double result = p1({{x, 3.0}, {y, -1.0}, {z, 2.0}});
    EXPECT_DOUBLE_EQ(result, 198.0);
}

TEST_F(PolynomialTest, EvaluatePolynomial)
{
    Polynomial<double> inner{
	{{{x, 1}}, 1.0},
	{{}, 1.0}};
    Polynomial<double> outer{
	{{{x, 2}}, 1.0}};
    Polynomial<double> ref{
	{{{x, 2}}, 1.0},
	{{{x, 1}}, 2.0},
	{{}, 1.0}};
    Polynomial<double> result = outer.evaluate<Polynomial<double>>({{x, inner}});
    EXPECT_EQ(result, ref);
}
