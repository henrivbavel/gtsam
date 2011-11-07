/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    testGraph.cpp
 * @brief   Unit test for two cameras and four landmarks
 * single camera
 * @author  Chris Beall
 * @author  Frank Dellaert
 * @author  Viorela Ila
 */

#include <CppUnitLite/TestHarness.h>
#include <boost/shared_ptr.hpp>
using namespace boost;

// Magically casts strings like "x3" to a Symbol('x',3) key, see Key.h
#define GTSAM_MAGIC_KEY

#include <gtsam/nonlinear/NonlinearFactorGraph-inl.h>
#include <gtsam/nonlinear/NonlinearOptimizer-inl.h>
#include <gtsam/inference/graph-inl.h>
#include <gtsam/slam/visualSLAM.h>

using namespace std;
using namespace gtsam;
using namespace boost;

static SharedNoiseModel sigma(noiseModel::Unit::Create(1));

/* ************************************************************************* */
Point3 landmark1(-1.0,-1.0, 0.0);
Point3 landmark2(-1.0, 1.0, 0.0);
Point3 landmark3( 1.0, 1.0, 0.0);
Point3 landmark4( 1.0,-1.0, 0.0);

Pose3 camera1(Matrix_(3,3,
		       1., 0., 0.,
		       0.,-1., 0.,
		       0., 0.,-1.
		       ),
	      Point3(0,0,6.25));

Pose3 camera2(Matrix_(3,3,
		       1., 0., 0.,
		       0.,-1., 0.,
		       0., 0.,-1.
		       ),
	      Point3(0,0,5.00));

/* ************************************************************************* */
visualSLAM::Graph testGraph() {
  Point2 z11(-100, 100);
	Point2 z12(-100,-100);
	Point2 z13( 100,-100);
	Point2 z14( 100, 100);
	Point2 z21(-125, 125);
	Point2 z22(-125,-125);
	Point2 z23( 125,-125);
	Point2 z24( 125, 125);

  shared_ptrK sK(new Cal3_S2(625, 625, 0, 0, 0));
  visualSLAM::Graph g;
  g.addMeasurement(z11, sigma, 1, 1, sK);
  g.addMeasurement(z12, sigma, 1, 2, sK);
  g.addMeasurement(z13, sigma, 1, 3, sK);
  g.addMeasurement(z14, sigma, 1, 4, sK);
  g.addMeasurement(z21, sigma, 2, 1, sK);
  g.addMeasurement(z22, sigma, 2, 2, sK);
  g.addMeasurement(z23, sigma, 2, 3, sK);
  g.addMeasurement(z24, sigma, 2, 4, sK);
  return g;
}

/* ************************************************************************* */
TEST( Graph, optimizeLM)
{
  // build a graph
  shared_ptr<visualSLAM::Graph> graph(new visualSLAM::Graph(testGraph()));
	// add 3 landmark constraints
  graph->addPointConstraint(1, landmark1);
  graph->addPointConstraint(2, landmark2);
  graph->addPointConstraint(3, landmark3);

  // Create an initial values structure corresponding to the ground truth
  boost::shared_ptr<visualSLAM::Values> initialEstimate(new visualSLAM::Values);
  initialEstimate->insert(1, camera1);
  initialEstimate->insert(2, camera2);
  initialEstimate->insert(1, landmark1);
  initialEstimate->insert(2, landmark2);
  initialEstimate->insert(3, landmark3);
  initialEstimate->insert(4, landmark4);

  // Create an ordering of the variables
  shared_ptr<Ordering> ordering(new Ordering);
  *ordering += "l1","l2","l3","l4","x1","x2";

  // Create an optimizer and check its error
  // We expect the initial to be zero because config is the ground truth
  visualSLAM::Optimizer optimizer(graph, initialEstimate, ordering);
  DOUBLES_EQUAL(0.0, optimizer.error(), 1e-9);

  // Iterate once, and the config should not have changed because we started
  // with the ground truth
  visualSLAM::Optimizer afterOneIteration = optimizer.iterate();
  DOUBLES_EQUAL(0.0, optimizer.error(), 1e-9);

  // check if correct
  CHECK(assert_equal(*initialEstimate,*(afterOneIteration.values())));
}


/* ************************************************************************* */
TEST( Graph, optimizeLM2)
{
  // build a graph
  shared_ptr<visualSLAM::Graph> graph(new visualSLAM::Graph(testGraph()));
	// add 2 camera constraints
  graph->addPoseConstraint(1, camera1);
  graph->addPoseConstraint(2, camera2);

  // Create an initial values structure corresponding to the ground truth
  boost::shared_ptr<visualSLAM::Values> initialEstimate(new visualSLAM::Values);
  initialEstimate->insert(1, camera1);
  initialEstimate->insert(2, camera2);
  initialEstimate->insert(1, landmark1);
  initialEstimate->insert(2, landmark2);
  initialEstimate->insert(3, landmark3);
  initialEstimate->insert(4, landmark4);

  // Create an ordering of the variables
  shared_ptr<Ordering> ordering(new Ordering);
  *ordering += "l1","l2","l3","l4","x1","x2";

  // Create an optimizer and check its error
  // We expect the initial to be zero because config is the ground truth
  visualSLAM::Optimizer optimizer(graph, initialEstimate, ordering);
  DOUBLES_EQUAL(0.0, optimizer.error(), 1e-9);

  // Iterate once, and the config should not have changed because we started
  // with the ground truth
  visualSLAM::Optimizer afterOneIteration = optimizer.iterate();
  DOUBLES_EQUAL(0.0, optimizer.error(), 1e-9);

  // check if correct
  CHECK(assert_equal(*initialEstimate,*(afterOneIteration.values())));
}


/* ************************************************************************* */
TEST( Graph, CHECK_ORDERING)
{
  // build a graph
  shared_ptr<visualSLAM::Graph> graph(new visualSLAM::Graph(testGraph()));
  // add 2 camera constraints
  graph->addPoseConstraint(1, camera1);
  graph->addPoseConstraint(2, camera2);

  // Create an initial values structure corresponding to the ground truth
  boost::shared_ptr<visualSLAM::Values> initialEstimate(new visualSLAM::Values);
  initialEstimate->insert(1, camera1);
  initialEstimate->insert(2, camera2);
  initialEstimate->insert(1, landmark1);
  initialEstimate->insert(2, landmark2);
  initialEstimate->insert(3, landmark3);
  initialEstimate->insert(4, landmark4);

  Ordering::shared_ptr ordering = graph->orderingCOLAMD(*initialEstimate);

  // Create an optimizer and check its error
  // We expect the initial to be zero because config is the ground truth
  visualSLAM::Optimizer optimizer(graph, initialEstimate, ordering);
  DOUBLES_EQUAL(0.0, optimizer.error(), 1e-9);

  // Iterate once, and the config should not have changed because we started
  // with the ground truth
  visualSLAM::Optimizer afterOneIteration = optimizer.iterate();
  DOUBLES_EQUAL(0.0, optimizer.error(), 1e-9);

  // check if correct
  CHECK(assert_equal(*initialEstimate,*(afterOneIteration.values())));
}

/* ************************************************************************* */
TEST( Values, update_with_large_delta) {
	// this test ensures that if the update for delta is larger than
	// the size of the config, it only updates existing variables
	visualSLAM::Values init;
	init.insert(1, Pose3());
	init.insert(1, Point3(1.0, 2.0, 3.0));

	visualSLAM::Values expected;
	expected.insert(1, Pose3(Rot3(), Point3(0.1, 0.1, 0.1)));
	expected.insert(1, Point3(1.1, 2.1, 3.1));

	Ordering largeOrdering;
	visualSLAM::Values largeValues = init;
	largeValues.insert(2, Pose3());
	largeOrdering += "x1","l1","x2";
	VectorValues delta(largeValues.dims(largeOrdering));
	delta[largeOrdering["x1"]] = Vector_(6, 0.0, 0.0, 0.0, 0.1, 0.1, 0.1);
	delta[largeOrdering["l1"]] = Vector_(3, 0.1, 0.1, 0.1);
	delta[largeOrdering["x2"]] = Vector_(6, 0.0, 0.0, 0.0, 100.1, 4.1, 9.1);
	visualSLAM::Values actual = init.retract(delta, largeOrdering);

	CHECK(assert_equal(expected,actual));
}

/* ************************************************************************* */
int main() { TestResult tr; return TestRegistry::runAllTests(tr); }
/* ************************************************************************* */
