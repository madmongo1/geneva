/**
 * @file GDataExchange_test.cpp
 *
 * This test checks all public member functions of the GDataExchange class plus
 * dependent classes. You should run this test both in DEBUG and in RELEASE mode,
 * as some functions may work differently in this case.
 */

/* Copyright (C)2009 Dr. Ruediger Berlich
 *
 * This file is part of Geneva, Gemfony scientific's optimization library.
 *
 * Geneva is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Geneva library.  If not, see <http://www.gnu.org/licenses/>.
 */

// Standard header files go here
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

// Boost header files go here

#define BOOST_TEST_MODULE GDataExchange_test
#include <boost/test/unit_test.hpp>

#include <boost/shared_ptr.hpp>

// Geneva header files go here
#include "GRandom.hpp"
#include "GDataExchange.hpp"

using namespace Gem;
using namespace Gem::GenEvA;
using namespace Gem::Util;

/***********************************************************************************/
/**
 * Tests the various GNumericParameterT-class derivatives as a means
 * of storing individual parameters including boundaries.
 */
BOOST_AUTO_TEST_CASE(gnumericparametert_basicfunctionality_no_failure_expected)
{
	{
		// Test basic construction
		boost::shared_ptr<GDoubleParameter> d0(new GDoubleParameter());
		boost::shared_ptr<GLongParameter> l0(new GLongParameter());
		boost::shared_ptr<GBoolParameter> b0(new GBoolParameter());
		boost::shared_ptr<GCharParameter> c0(new GCharParameter());

		// objects will be destroyed at this point, allowing for a test of the destructor
	}

	// Test construction with value assignment
	boost::shared_ptr<GDoubleParameter> d1(new GDoubleParameter(1.));
	boost::shared_ptr<GLongParameter> l1(new GLongParameter(2));
	boost::shared_ptr<GBoolParameter> b1(new GBoolParameter(false));
	boost::shared_ptr<GCharParameter> c1(new GCharParameter('x'));

	// Test construction with value assignment and boundaries
	boost::shared_ptr<GDoubleParameter> d2(new GDoubleParameter(2.,0.,2.));
	boost::shared_ptr<GLongParameter> l2(new GLongParameter(3,1,3));
	boost::shared_ptr<GBoolParameter> b2(new GBoolParameter(true,false,true));
	boost::shared_ptr<GCharParameter> c2(new GCharParameter('y',0,127));

	// Test copy construction
	boost::shared_ptr<GDoubleParameter> d3(new GDoubleParameter(*d2));
	boost::shared_ptr<GLongParameter> l3(new GLongParameter(*l2));
	boost::shared_ptr<GBoolParameter> b3(new GBoolParameter(*b2));
	boost::shared_ptr<GCharParameter> c3(new GCharParameter(*c2));

	// Test assignment operators
	*d3 = *d1;
	*l3 = *l1;
	*b3 = *b1;
	*c3 = *c1;

	// Check that d3 etc. have the correct values. Note that they
	// had different values before the assignment.
	BOOST_REQUIRE(d3->getParameter() == 1.);
	BOOST_REQUIRE(l3->getParameter() == 2);
	BOOST_REQUIRE(b3->getParameter() == false);
	BOOST_REQUIRE(c3->getParameter() == 'x');

	// Check that no boundaries have been assigned
	BOOST_REQUIRE(!d3->hasBoundaries());
	BOOST_REQUIRE(!l3->hasBoundaries());
	BOOST_REQUIRE(!b3->hasBoundaries());
	BOOST_REQUIRE(!c3->hasBoundaries());

	// Assign new values
	d3->setParameter(3.);
	l3->setParameter(4);
	b3->setParameter(true);
	c3->setParameter('z');

	// Check again values and boundaries
	BOOST_REQUIRE(d3->getParameter() == 3.);
	BOOST_REQUIRE(l3->getParameter() == 4);
	BOOST_REQUIRE(b3->getParameter() == true);
	BOOST_REQUIRE(c3->getParameter() == 'z');

	// Check that no boundaries have been assigned
	BOOST_REQUIRE(!d3->hasBoundaries());
	BOOST_REQUIRE(!l3->hasBoundaries());
	BOOST_REQUIRE(!b3->hasBoundaries());
	BOOST_REQUIRE(!c3->hasBoundaries());

	// Assign new values, this time with boundaries
	d3->setParameter(4.,0.,4.);
	l3->setParameter(5,0,5);
	b3->setParameter(false,false,true);
	c3->setParameter('a',0,127);

	// Check again values and boundaries
	BOOST_REQUIRE(d3->getParameter() == 4.);
	BOOST_REQUIRE(l3->getParameter() == 5);
	BOOST_REQUIRE(b3->getParameter() == false);
	BOOST_REQUIRE(c3->getParameter() == 'a');

	// Check that boundaries have been assigned this time
	BOOST_REQUIRE(d3->hasBoundaries());
	BOOST_REQUIRE(l3->hasBoundaries());
	BOOST_REQUIRE(b3->hasBoundaries());
	BOOST_REQUIRE(c3->hasBoundaries());

	// Check the value of the lower boundaries ...
	BOOST_REQUIRE(d3->getLowerBoundary() == 0.);
	BOOST_REQUIRE(l3->getLowerBoundary() == 0);
	BOOST_REQUIRE(b3->getLowerBoundary() == false);
	BOOST_REQUIRE(c3->getLowerBoundary() == 0);

	// and the value of the upper boundaries
	BOOST_REQUIRE(d3->getUpperBoundary() == 4.);
	BOOST_REQUIRE(l3->getUpperBoundary() == 5);
	BOOST_REQUIRE(b3->getUpperBoundary() == true);
	BOOST_REQUIRE(c3->getUpperBoundary() == 127);
}

/***********************************************************************************/
/**
 * Tests whether GDataExchange objects can be filled with different data types
 * and independent data sets repeatedly.
 */
BOOST_AUTO_TEST_CASE( gdataexchange_datafill_and_reset_no_failure_expected )
{
	GRandom gr;
	boost::shared_ptr<GDataExchange> gde(new GDataExchange());

	// Fill with individual value items
	for(std::size_t gde_counter=0; gde_counter < 10; gde_counter++) {
		for(std::size_t i=0; i<100; i++) gde->append<double>(gr.evenRandom(-10.,10.));
		for(std::size_t i=0; i<100; i++) gde->append<boost::int32_t>(gr.discreteRandom(-10,10));
		for(std::size_t i=0; i<100; i++) gde->append<bool>(gr.boolRandom());
		for(std::size_t i=0; i<100; i++) gde->append<char>(gr.charRandom());

		gde->newDataSet();
	}

	// GDataExchange fills itself with a single data set upon creation. Since we
	// added another 10 data sets, there should now be 11 of them.
	BOOST_REQUIRE(gde->nDataSets() == 11);

	gde->gotoStart();
	gde->resetAll(); // There should now only be one data set remaining
	BOOST_REQUIRE(gde->nDataSets() == 1);

	// Fill with values including boundaries
	for(std::size_t gde_counter=0; gde_counter < 10; gde_counter++) {
		for(std::size_t i=0; i<100; i++) gde->append<double>(gr.evenRandom(-10.,10.),-11.,11.);
		for(std::size_t i=0; i<100; i++) gde->append<boost::int32_t>(gr.discreteRandom(-10,10),-11,11);
		for(std::size_t i=0; i<100; i++) gde->append<bool>(gr.boolRandom(), false, true);
		for(std::size_t i=0; i<100; i++) gde->append<char>(gr.charRandom(false), 0, 127);

		gde->newDataSet();
	}
	BOOST_REQUIRE(gde->nDataSets() == 11);

	gde->gotoStart();
	gde->resetAll(); // There should now only be one data set remaining
	BOOST_REQUIRE(gde->nDataSets() == 1);

	// Fill directly with GParameter objects
	for(std::size_t gde_counter=0; gde_counter < 10; gde_counter++) {
		boost::shared_ptr<GDoubleParameter> d(new GDoubleParameter(gr.evenRandom(-10.,10.)));
		boost::shared_ptr<GLongParameter> l(new GLongParameter(gr.discreteRandom(-10,10)));
		boost::shared_ptr<GBoolParameter> b(new GBoolParameter(gr.boolRandom()));
		boost::shared_ptr<GCharParameter> c(new GCharParameter(gr.charRandom(false)));

		for(std::size_t i=0; i<100; i++) gde->append(d);
		for(std::size_t i=0; i<100; i++) gde->append(l);
		for(std::size_t i=0; i<100; i++) gde->append(b);
		for(std::size_t i=0; i<100; i++) gde->append(c);

		gde->newDataSet();
	}
	BOOST_REQUIRE(gde->nDataSets() == 11);

	gde->gotoStart();
	gde->resetAll(); // There should now only be one data set remaining
	BOOST_REQUIRE(gde->nDataSets() == 1);

	// Need test about file creation and scanning. In this context:
	// Improve fp accuracy of serialized numbers.
}

/***********************************************************************************/

// EOF
