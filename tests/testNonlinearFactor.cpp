/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 *  @file  testNonlinearFactor.cpp
 *  @brief Unit tests for Non-Linear Factor, 
 *  create a non linear factor graph and a values structure for it and
 *  calculate the error for the factor.
 *  @author Christian Potthast
 **/

/*STL/C++*/
#include <iostream>

#include <CppUnitLite/TestHarness.h>

// TODO: DANGEROUS, create shared pointers
#define GTSAM_MAGIC_GAUSSIAN 2

// Magically casts strings like "x3" to a Symbol('x',3) key, see Key.h
#define GTSAM_MAGIC_KEY

#include <gtsam/base/Testable.h>
#include <gtsam/base/Matrix.h>
#include <gtsam/base/LieVector.h>
#include <gtsam/slam/smallExample.h>
#include <gtsam/slam/simulated2D.h>
#include <gtsam/linear/GaussianFactor.h>
#include <gtsam/nonlinear/NonlinearFactorGraph-inl.h>
#include <gtsam/nonlinear/Values-inl.h>

using namespace std;
using namespace gtsam;
using namespace example;

typedef boost::shared_ptr<NonlinearFactor<VectorValues> > shared_nlf;

/* ************************************************************************* */
TEST( NonlinearFactor, equals )
{
	SharedNoiseModel sigma(noiseModel::Isotropic::Sigma(2,1.0));

	// create two nonlinear2 factors
	Point2 z3(0.,-1.);
	simulated2D::Measurement f0(z3, sigma, 1,1);

	// measurement between x2 and l1
	Point2 z4(-1.5, -1.);
	simulated2D::Measurement f1(z4, sigma, 2,1);

	CHECK(assert_equal(f0,f0));
	CHECK(f0.equals(f0));
	CHECK(!f0.equals(f1));
	CHECK(!f1.equals(f0));
}

/* ************************************************************************* */
TEST( NonlinearFactor, equals2 )
{
  // create a non linear factor graph
  Graph fg = createNonlinearFactorGraph();

  // get two factors
  Graph::sharedFactor f0 = fg[0], f1 = fg[1];

  CHECK(f0->equals(*f0));
// SL-FIX  CHECK(!f0->equals(*f1));
// SL-FIX  CHECK(!f1->equals(*f0));
}

/* ************************************************************************* */
TEST( NonlinearFactor, NonlinearFactor )
{
  // create a non linear factor graph
  Graph fg = createNonlinearFactorGraph();

  // create a values structure for the non linear factor graph
  example::Values cfg = createNoisyValues();

  // get the factor "f1" from the factor graph
  Graph::sharedFactor factor = fg[0];

  // calculate the error_vector from the factor "f1"
  // error_vector = [0.1 0.1]
  Vector actual_e = boost::dynamic_pointer_cast<NoiseModelFactor<example::Values> >(factor)->unwhitenedError(cfg);
  CHECK(assert_equal(0.1*ones(2),actual_e));

  // error = 0.5 * [1 1] * [1;1] = 1
  double expected = 1.0; 

  // calculate the error from the factor "f1"
  double actual = factor->error(cfg);
  DOUBLES_EQUAL(expected,actual,0.00000001);
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f1 )
{
	example::Values c = createNoisyValues();

  // Grab a non-linear factor
  Graph nfg = createNonlinearFactorGraph();
  Graph::sharedFactor nlf = nfg[0];

  // We linearize at noisy config from SmallExample
  GaussianFactor::shared_ptr actual = nlf->linearize(c, *c.orderingArbitrary());

  GaussianFactorGraph lfg = createGaussianFactorGraph(*c.orderingArbitrary());
  GaussianFactor::shared_ptr expected = lfg[0];

  CHECK(assert_equal(*expected,*actual));

  // The error |A*dx-b| approximates (h(x0+dx)-z) = -error_vector
  // Hence i.e., b = approximates z-h(x0) = error_vector(x0)
	//CHECK(assert_equal(nlf->error_vector(c),actual->get_b()));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f2 )
{
	example::Values c = createNoisyValues();

  // Grab a non-linear factor
  Graph nfg = createNonlinearFactorGraph();
  Graph::sharedFactor nlf = nfg[1];

  // We linearize at noisy config from SmallExample
  GaussianFactor::shared_ptr actual = nlf->linearize(c, *c.orderingArbitrary());

  GaussianFactorGraph lfg = createGaussianFactorGraph(*c.orderingArbitrary());
  GaussianFactor::shared_ptr expected = lfg[1];

  CHECK(assert_equal(*expected,*actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f3 )
{
  // Grab a non-linear factor
  Graph nfg = createNonlinearFactorGraph();
  Graph::sharedFactor nlf = nfg[2];

  // We linearize at noisy config from SmallExample
  example::Values c = createNoisyValues();
  GaussianFactor::shared_ptr actual = nlf->linearize(c, *c.orderingArbitrary());

  GaussianFactorGraph lfg = createGaussianFactorGraph(*c.orderingArbitrary());
  GaussianFactor::shared_ptr expected = lfg[2];

  CHECK(assert_equal(*expected,*actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f4 )
{
  // Grab a non-linear factor
  Graph nfg = createNonlinearFactorGraph();
  Graph::sharedFactor nlf = nfg[3];

  // We linearize at noisy config from SmallExample
  example::Values c = createNoisyValues();
  GaussianFactor::shared_ptr actual = nlf->linearize(c, *c.orderingArbitrary());

  GaussianFactorGraph lfg = createGaussianFactorGraph(*c.orderingArbitrary());
  GaussianFactor::shared_ptr expected = lfg[3];

  CHECK(assert_equal(*expected,*actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, size )
{
	// create a non linear factor graph
	Graph fg = createNonlinearFactorGraph();
	
	// create a values structure for the non linear factor graph
	example::Values cfg = createNoisyValues();
	
	// get some factors from the graph
	Graph::sharedFactor factor1 = fg[0], factor2 = fg[1],
			factor3 = fg[2];
	
	CHECK(factor1->size() == 1);
	CHECK(factor2->size() == 2);
	CHECK(factor3->size() == 2);
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_constraint1 )
{
	Vector sigmas = Vector_(2, 0.2, 0.0);
	SharedDiagonal constraint = noiseModel::Constrained::MixedSigmas(sigmas);

	Point2 mu(1., -1.);
	Graph::sharedFactor f0(new simulated2D::Prior(mu, constraint, 1));

	example::Values config;
	config.insert(simulated2D::PoseKey(1), Point2(1.0, 2.0));
	GaussianFactor::shared_ptr actual = f0->linearize(config, *config.orderingArbitrary());

	// create expected
	Ordering ord(*config.orderingArbitrary());
	Vector b = Vector_(2, 0., -3.);
	JacobianFactor expected(ord["x1"], eye(2), b, constraint);
	CHECK(assert_equal((const GaussianFactor&)expected, *actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_constraint2 )
{
	Vector sigmas = Vector_(2, 0.2, 0.0);
	SharedDiagonal constraint = noiseModel::Constrained::MixedSigmas(sigmas);

	Point2 z3(1.,-1.);
	simulated2D::Measurement f0(z3, constraint, 1,1);

	example::Values config;
	config.insert(simulated2D::PoseKey(1), Point2(1.0, 2.0));
	config.insert(simulated2D::PointKey(1), Point2(5.0, 4.0));
	GaussianFactor::shared_ptr actual = f0.linearize(config, *config.orderingArbitrary());

	// create expected
	Ordering ord(*config.orderingArbitrary());
	Vector b = Vector_(2, -3., -3.);
	JacobianFactor expected(ord["x1"], -1*eye(2), ord["l1"], eye(2), b, constraint);
	CHECK(assert_equal((const GaussianFactor&)expected, *actual));
}

/* ************************************************************************* */
typedef TypedSymbol<LieVector, 'x'> TestKey;
typedef gtsam::Values<TestKey> TestValues;

/* ************************************************************************* */
class TestFactor4 : public NonlinearFactor4<TestValues, TestKey, TestKey, TestKey, TestKey> {
public:
  typedef NonlinearFactor4<TestValues, TestKey, TestKey, TestKey, TestKey> Base;
  TestFactor4() : Base(sharedSigmas(Vector_(1, 2.0)), 1, 2, 3, 4) {}

  virtual Vector
    evaluateError(const LieVector& x1, const LieVector& x2, const LieVector& x3, const LieVector& x4,
        boost::optional<Matrix&> H1 = boost::none,
        boost::optional<Matrix&> H2 = boost::none,
        boost::optional<Matrix&> H3 = boost::none,
        boost::optional<Matrix&> H4 = boost::none) const {
    if(H1) {
      *H1 = Matrix_(1,1, 1.0);
      *H2 = Matrix_(1,1, 2.0);
      *H3 = Matrix_(1,1, 3.0);
      *H4 = Matrix_(1,1, 4.0);
    }
    return (Vector(1) << x1 + x2 + x3 + x4).finished();
  }
};

/* ************************************ */
TEST(NonlinearFactor, NonlinearFactor4) {
  TestFactor4 tf;
  TestValues tv;
  tv.insert(1, LieVector(1, 1.0));
  tv.insert(2, LieVector(1, 2.0));
  tv.insert(3, LieVector(1, 3.0));
  tv.insert(4, LieVector(1, 4.0));
  EXPECT(assert_equal(Vector_(1, 10.0), tf.unwhitenedError(tv)));
  DOUBLES_EQUAL(25.0/2.0, tf.error(tv), 1e-9);
  Ordering ordering; ordering += TestKey(1), TestKey(2), TestKey(3), TestKey(4);
  JacobianFactor jf(*boost::dynamic_pointer_cast<JacobianFactor>(tf.linearize(tv, ordering)));
  LONGS_EQUAL(jf.keys()[0], 0);
  LONGS_EQUAL(jf.keys()[1], 1);
  LONGS_EQUAL(jf.keys()[2], 2);
  LONGS_EQUAL(jf.keys()[3], 3);
  EXPECT(assert_equal(Matrix_(1,1, 0.5), jf.getA(jf.begin())));
  EXPECT(assert_equal(Matrix_(1,1, 1.0), jf.getA(jf.begin()+1)));
  EXPECT(assert_equal(Matrix_(1,1, 1.5), jf.getA(jf.begin()+2)));
  EXPECT(assert_equal(Matrix_(1,1, 2.0), jf.getA(jf.begin()+3)));
  EXPECT(assert_equal(Vector_(1, -5.0), jf.getb()));
}

/* ************************************************************************* */
class TestFactor5 : public NonlinearFactor5<TestValues, TestKey, TestKey, TestKey, TestKey, TestKey> {
public:
  typedef NonlinearFactor5<TestValues, TestKey, TestKey, TestKey, TestKey, TestKey> Base;
  TestFactor5() : Base(sharedSigmas(Vector_(1, 2.0)), 1, 2, 3, 4, 5) {}

  virtual Vector
    evaluateError(const X1& x1, const X2& x2, const X3& x3, const X4& x4, const X5& x5,
        boost::optional<Matrix&> H1 = boost::none,
        boost::optional<Matrix&> H2 = boost::none,
        boost::optional<Matrix&> H3 = boost::none,
        boost::optional<Matrix&> H4 = boost::none,
        boost::optional<Matrix&> H5 = boost::none) const {
    if(H1) {
      *H1 = Matrix_(1,1, 1.0);
      *H2 = Matrix_(1,1, 2.0);
      *H3 = Matrix_(1,1, 3.0);
      *H4 = Matrix_(1,1, 4.0);
      *H5 = Matrix_(1,1, 5.0);
    }
    return (Vector(1) << x1 + x2 + x3 + x4 + x5).finished();
  }
};

/* ************************************ */
TEST(NonlinearFactor, NonlinearFactor5) {
  TestFactor5 tf;
  TestValues tv;
  tv.insert(1, LieVector(1, 1.0));
  tv.insert(2, LieVector(1, 2.0));
  tv.insert(3, LieVector(1, 3.0));
  tv.insert(4, LieVector(1, 4.0));
  tv.insert(5, LieVector(1, 5.0));
  EXPECT(assert_equal(Vector_(1, 15.0), tf.unwhitenedError(tv)));
  DOUBLES_EQUAL(56.25/2.0, tf.error(tv), 1e-9);
  Ordering ordering; ordering += TestKey(1), TestKey(2), TestKey(3), TestKey(4), TestKey(5);
  JacobianFactor jf(*boost::dynamic_pointer_cast<JacobianFactor>(tf.linearize(tv, ordering)));
  LONGS_EQUAL(jf.keys()[0], 0);
  LONGS_EQUAL(jf.keys()[1], 1);
  LONGS_EQUAL(jf.keys()[2], 2);
  LONGS_EQUAL(jf.keys()[3], 3);
  LONGS_EQUAL(jf.keys()[4], 4);
  EXPECT(assert_equal(Matrix_(1,1, 0.5), jf.getA(jf.begin())));
  EXPECT(assert_equal(Matrix_(1,1, 1.0), jf.getA(jf.begin()+1)));
  EXPECT(assert_equal(Matrix_(1,1, 1.5), jf.getA(jf.begin()+2)));
  EXPECT(assert_equal(Matrix_(1,1, 2.0), jf.getA(jf.begin()+3)));
  EXPECT(assert_equal(Matrix_(1,1, 2.5), jf.getA(jf.begin()+4)));
  EXPECT(assert_equal(Vector_(1, -7.5), jf.getb()));
}

/* ************************************************************************* */
class TestFactor6 : public NonlinearFactor6<TestValues, TestKey, TestKey, TestKey, TestKey, TestKey, TestKey> {
public:
  typedef NonlinearFactor6<TestValues, TestKey, TestKey, TestKey, TestKey, TestKey, TestKey> Base;
  TestFactor6() : Base(sharedSigmas(Vector_(1, 2.0)), 1, 2, 3, 4, 5, 6) {}

  virtual Vector
    evaluateError(const X1& x1, const X2& x2, const X3& x3, const X4& x4, const X5& x5, const X6& x6,
        boost::optional<Matrix&> H1 = boost::none,
        boost::optional<Matrix&> H2 = boost::none,
        boost::optional<Matrix&> H3 = boost::none,
        boost::optional<Matrix&> H4 = boost::none,
        boost::optional<Matrix&> H5 = boost::none,
        boost::optional<Matrix&> H6 = boost::none) const {
    if(H1) {
      *H1 = Matrix_(1,1, 1.0);
      *H2 = Matrix_(1,1, 2.0);
      *H3 = Matrix_(1,1, 3.0);
      *H4 = Matrix_(1,1, 4.0);
      *H5 = Matrix_(1,1, 5.0);
      *H6 = Matrix_(1,1, 6.0);
    }
    return (Vector(1) << x1 + x2 + x3 + x4 + x5 + x6).finished();
  }
};

/* ************************************ */
TEST(NonlinearFactor, NonlinearFactor6) {
  TestFactor6 tf;
  TestValues tv;
  tv.insert(1, LieVector(1, 1.0));
  tv.insert(2, LieVector(1, 2.0));
  tv.insert(3, LieVector(1, 3.0));
  tv.insert(4, LieVector(1, 4.0));
  tv.insert(5, LieVector(1, 5.0));
  tv.insert(6, LieVector(1, 6.0));
  EXPECT(assert_equal(Vector_(1, 21.0), tf.unwhitenedError(tv)));
  DOUBLES_EQUAL(110.25/2.0, tf.error(tv), 1e-9);
  Ordering ordering; ordering += TestKey(1), TestKey(2), TestKey(3), TestKey(4), TestKey(5), TestKey(6);
  JacobianFactor jf(*boost::dynamic_pointer_cast<JacobianFactor>(tf.linearize(tv, ordering)));
  LONGS_EQUAL(jf.keys()[0], 0);
  LONGS_EQUAL(jf.keys()[1], 1);
  LONGS_EQUAL(jf.keys()[2], 2);
  LONGS_EQUAL(jf.keys()[3], 3);
  LONGS_EQUAL(jf.keys()[4], 4);
  LONGS_EQUAL(jf.keys()[5], 5);
  EXPECT(assert_equal(Matrix_(1,1, 0.5), jf.getA(jf.begin())));
  EXPECT(assert_equal(Matrix_(1,1, 1.0), jf.getA(jf.begin()+1)));
  EXPECT(assert_equal(Matrix_(1,1, 1.5), jf.getA(jf.begin()+2)));
  EXPECT(assert_equal(Matrix_(1,1, 2.0), jf.getA(jf.begin()+3)));
  EXPECT(assert_equal(Matrix_(1,1, 2.5), jf.getA(jf.begin()+4)));
  EXPECT(assert_equal(Matrix_(1,1, 3.0), jf.getA(jf.begin()+5)));
  EXPECT(assert_equal(Vector_(1, -10.5), jf.getb()));

}

/* ************************************************************************* */
int main() { TestResult tr; return TestRegistry::runAllTests(tr);}
/* ************************************************************************* */
