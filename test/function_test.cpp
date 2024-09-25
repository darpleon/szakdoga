#include <gtest/gtest.h>
#include "Polynomial.hpp"
#include "PowerPermutation.hpp"
#include "Rational.hpp"
#include "Variable.hpp"

TEST(VariableTest, CreatedSeparately_NotEqual)
{
    Variable x{};
    Variable y{};
    EXPECT_NE(x, y);
}

TEST(VariableTest, CopyConstructed_Equal)
{
    Variable x{};
    Variable y{x};
    EXPECT_EQ(x, y);
}

class PowerPermutationTest : public testing::Test
{
protected:
    Variable x{}, y{}, z{};
};

TEST_F(PowerPermutationTest, DefaultConstructor)
{
    PowerPermutation p{};
    EXPECT_TRUE(p.monomials().empty());
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
    EXPECT_TRUE(pp2.monomials().empty());
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
    const auto& mn = product.monomials();
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
    Variable x{}, y{}, z{};
    Polynomial<double> p1{
	{{{x, 3}, {y, 2}}, 2.0},
	{{{z, 4}, {x, 1}}, 3.0}};
};

TEST_F(PolynomialTest, Identities)
{
    Polynomial<double> p0 = I<Polynomial<double>>::zero;
    EXPECT_TRUE(p0.terms().empty());
    Polynomial<double> p1 = I<Polynomial<double>>::one;
    const auto& terms = p1.terms();
    EXPECT_EQ(terms.size(), 1);
    EXPECT_DOUBLE_EQ(terms.at(PowerPermutation{}), 1.0);
}

TEST_F(PolynomialTest, OperatorConstruction)
{
    Polynomial<double> p1o = 2.0*(x^3)*(y^2) + 3.0*x*(z^4);
    EXPECT_EQ(p1o, p1);
    Polynomial<double> p2o = 1.0 + x;
    Polynomial<double> ref2{{{{x, 1}}, 1.0}, {{}, 1.0}};
    EXPECT_EQ(p2o, ref2);
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

TEST_F(PolynomialTest, Derivative)
{
    std::vector<Polynomial<double>> derivatives = p1.derivative({x, y, z});
    Polynomial<double> ref_x{
	{{{x, 2}, {y, 2}}, 6.0},
	{{{z, 4}}, 3.0}};
    Polynomial<double> ref_y{
	{{{x, 3}, {y, 1}}, 4.0}};
    Polynomial<double> ref_z{
	{{{z, 3}, {x, 1}}, 12.0}};
    EXPECT_EQ(derivatives[0], ref_x);
    EXPECT_EQ(derivatives[1], ref_y);
    EXPECT_EQ(derivatives[2], ref_z);
}

class RationalTest : public testing::Test
{
protected:
    Variable x{}, y{}, z{};
    Rational<double> r1{
	{
	    {{{x, 2}, {y, 1}}, 1.0},
	    {{{x, 1}}, 3.0}
	},
	{
	    {{{x, 1}, {y, 1}, {z, 1}}, 2.0},
	    {{{}}, 1.0}
	}
    };
    Rational<double> r2{
	{
	    {{{z, 2}}, 1.0}
	},
	{
	    {{{y, 1}}, 2.0},
	    {{{z, 1}}, 1.0}
	}
    };
};

TEST_F(RationalTest, OperatorConstruction)
{
    Rational<double> r = ((x^2)*y + 3.0*x) / (2.0*x*y*z+ 1.0);
    EXPECT_EQ(r, r1);
}

TEST_F(RationalTest, Multiply)
{
    Rational<double> product = r1 * r2;
    Rational<double> ref = ((x^2)*y*(z^2) + x*(z^2) * 3.0) /
			    (x*(y^2)*z * 4.0 + x*y*(z^2) * 2.0 + y * 2.0 + z);
    EXPECT_EQ(product, ref);
}

TEST_F(RationalTest, Divide)
{
    Rational<double> quotient = r1 / r2;
    Rational<double> ref = (2.0*(x^2)*(y^2) + (x^2)*y*z + 6.0*x*y + 3.0*x*z) /
			   (2.0*x*y*(z^3) + (z^2));
    EXPECT_EQ(quotient, ref);
}

TEST_F(RationalTest, Add)
{
    Rational<double> sum = r1 + r2;
    Rational<double> ref = (2.0*(x^2)*(y^2) + (x^2)*y*z + 2.0*x*y*(z^3) + 6.0*x*y + 3.0*x*z + (z^2)) /
			   (4.0*x*(y^2)*z + 2.0*x*y*(z^2) + 2.0*y + z);
    EXPECT_EQ(sum, ref);
}

TEST_F(RationalTest, EvaluateDouble)
{
    double result = r1.evaluate<double>({{x, -2.0}, {y, 1.0}, {z, 0.0}});
    EXPECT_DOUBLE_EQ(result, -2.0);
}

TEST_F(RationalTest, EvaluatePolynomial)
{
    Polynomial<double> py = z^2;
    Polynomial<double> pz = 1.0*x + y;
    Rational<double> result = r2.evaluate<Polynomial<double>>({{y, py}, {z, pz}});
    Rational<double> ref = ((x^2) + 2.0*x*y + (y^2)) / (1.0*x + y + 2.0*(z^2));
    EXPECT_EQ(result, ref);
}

TEST_F(RationalTest, EvaluateRational)
{
    Rational<double> ry{1.0, (z^2) + 1.0};
    Rational<double> rz{x, y};
    Rational<double> result = r2.evaluate<Rational<double>>({{y, ry}, {z, rz}});
    Rational<double> ref = (1.0*(x^2)*y*(z^2) + (x^2)*y) / (2.0*(y^3) + x*(y^2)*(z^2) + x*(y^2));
    EXPECT_EQ(result, ref);
}

TEST_F(RationalTest, Derivative)
{
    std::vector<Rational<double>> derivs = r1.derivative({x, y, z});
    Rational<double> ref_x = (2.0*(x^2)*(y^2)*z + 2.0*x*y + 3.0) /
			     (4.0*(x^2)*(y^2)*(z^2) + 4.0*x*y*z + 1.0);
    Rational<double> ref_y = ((x^2) + (-6.0)*(x^2)*z) /
			     (4.0*(x^2)*(y^2)*(z^2) + 4.0*x*y*z + 1.0);
    Rational<double> ref_z = ((-2.0)*(x^3)*(y^2) + (-6.0)*(x^2)*y) /
			     (4.0*(x^2)*(y^2)*(z^2) + 4.0*x*y*z + 1.0);
    EXPECT_EQ(derivs[0], ref_x);
    EXPECT_EQ(derivs[1], ref_y);
    EXPECT_EQ(derivs[2], ref_z);
}
