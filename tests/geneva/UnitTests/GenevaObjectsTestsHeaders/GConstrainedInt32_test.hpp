/**
 * @file GConstrainedInt32_test.cpp
 */

/* Copyright (C) Dr. Ruediger Berlich and Karlsruhe Institute of Technology
 * (University of the State of Baden-Wuerttemberg and National Laboratory
 * of the Helmholtz Association)
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Geneva library, Gemfony scientific's optimization
 * library.
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
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.com .
 */

// Standard header files go here
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Boost header files go here

#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>

#ifndef GBOUNDEDINT32_TEST_HPP_
#define GBOUNDEDINT32_TEST_HPP_

// Geneva header files go here
#include "common/GExceptions.hpp"
#include "hap/GRandomT.hpp"
#include "geneva/GConstrainedInt32.hpp"
#include "geneva/GInt32FlipAdaptor.hpp"
#include "GEqualityPrinter.hpp"

using namespace Gem::Hap;
using namespace Gem::Geneva;

using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

/********************************************************************************************/
// The actual unit tests for this class
class GConstrainedInt32_test {
public:
	/***********************************************************************************/
	// The default constructor
	GConstrainedInt32_test():
		NADAPTIONS(10000)
	{ /* empty*/ }

	/***********************************************************************************/
	// Test features that are expected to work
	void no_failure_expected() {
		// Prepare printing of error messages in object comparisons
		GEqualityPrinter gep("GConstrainedInt32_test::no_failure_expected()",
							 pow(10,-10),
							 Gem::Common::CE_WITH_MESSAGES);

		// Test instantiation in different modes
		GConstrainedInt32 gbi0;
		GConstrainedInt32 gbi1(-10,10);
		GConstrainedInt32 gbi2(1,-10,10);
		GConstrainedInt32 gbi7(3); // has maximum boundaries
		GConstrainedInt32 gbi3(gbi2);

		BOOST_CHECK(gbi3 == gbi2);
		if(gbi1.value() != 1) BOOST_CHECK(gbi2 != gbi1); // gbi1 may randomly be assigned the value, which would result in an error being erroneously reported
		BOOST_CHECK(gbi2 != gbi0);
		BOOST_CHECK(gbi1 != gbi0);
		BOOST_CHECK(gbi7 != gbi0);

		// Check that value calculation works repeatedly. Internal value should be == external value for gbi7
		const std::size_t NCHECKS=10000;
		for(std::size_t i=0; i<NCHECKS; i++) {
			boost::int32_t in=boost::int32_t(-5000.+10000.*double(i)/double(NCHECKS)), out = 0;
			BOOST_CHECK_NO_THROW(out = gbi7.transfer(in));
			BOOST_CHECK(in==out);
		}
		// Try resetting the boundaries to a finite value (which includes the current external value)
		BOOST_CHECK_NO_THROW(gbi7.setBoundaries(-6000,6000));
		BOOST_CHECK_NO_THROW(gbi7 = 10);
		BOOST_CHECK_NO_THROW(gbi7.setBoundaries(-10,10));

		// (Repeated) assignment
		GConstrainedInt32 gbi3_2;
		gbi3_2 = gbi3 = gbi0;
		BOOST_CHECK(gbi3 != gbi2);
		BOOST_CHECK(gbi3 == gbi0);
		BOOST_CHECK(gbi3_2 != gbi2);
		BOOST_CHECK(gbi3_2 == gbi0);

		// Cloning and loading
		GConstrainedInt32 gbi5;
		{
			boost::shared_ptr<GObject> gbi4;
			BOOST_CHECK_NO_THROW(gbi4 = gbi3.GObject::clone());
			BOOST_CHECK_NO_THROW(gbi5.GObject::load(gbi4));
		}
		BOOST_CHECK(gbi5 == gbi3);

		// Value assignment
		gbi5=gbi1;
		BOOST_CHECK(gbi5 == gbi1);
		gbi5 = 2;
		BOOST_CHECK(gbi5.value() == 2);
		BOOST_CHECK(gep.isInEqual(gbi5, gbi3));

		BOOST_CHECK(gbi5.getLowerBoundary() ==  -10);
		BOOST_CHECK(gbi5.getUpperBoundary() ==  10);

		// Test automatic conversion to boost::int32_t
		boost::int32_t val=0;
		val = gbi5;
		BOOST_CHECK(val == 2);

		// Attach an adaptor to the integer and check that no values outside of the
		// value range occur
		GConstrainedInt32 mutTest(2, 1, 5);
		boost::shared_ptr<GInt32FlipAdaptor> gifa(new GInt32FlipAdaptor);
		mutTest.addAdaptor(gifa);
		for(std::size_t i=0; i<NADAPTIONS; i++) {
			mutTest.adapt();
			BOOST_CHECK(mutTest.value() >= 1 && mutTest.value() <= 5);
		}

		// Check that input of bounded int leads to the desired output (in this
		// case same number of entries in the vector
		std::vector<boost::int32_t> iVec(20,0);
		GConstrainedInt32 cc(-10,9); // cc == "checkConversion; 20 values
		for(boost::int32_t i=0; i<1000; i++) {
			val =  cc.transfer(-10+i%20);
			iVec[boost::numeric_cast<std::size_t>(10 + val)] += 1;
		}
		for(std::size_t i=1; i<20; i++) {
			BOOST_CHECK(iVec[i] == iVec[i-1]);
		}

		// Test serialization and loading in different serialization modes
		{ // plain text format
			// Copy construction of a new object
			GConstrainedInt32 gbi6(0,-10,10);
			GConstrainedInt32 gbi6_cp(gbi6);

			// Check equalities and inequalities
			BOOST_CHECK(gbi6_cp == gbi6);
			// Re-assign a new value to gbi6_cp
			gbi6_cp = 1;
			BOOST_CHECK(gbi6_cp.value() == 1);
			BOOST_CHECK(gbi6_cp != gbi6);

			// Serialize gbi6 and load into gbi6_co, check equalities and similarities
			BOOST_REQUIRE_NO_THROW(gbi6_cp.fromString(gbi6.toString(Gem::Common::SERIALIZATIONMODE_TEXT), Gem::Common::SERIALIZATIONMODE_TEXT));
			BOOST_CHECK(gep.isSimilar(gbi6_cp, gbi6));
		}

		{ // XML format
			// Copy construction of a new object
			GConstrainedInt32 gbi6(0,-10,10);
			GConstrainedInt32 gbi6_cp(gbi6);

			// Check equalities and inequalities
			BOOST_CHECK(gbi6_cp == gbi6);
			// Re-assign a new value to gbi6_cp
			gbi6_cp = 1;
			BOOST_CHECK(gbi6_cp.value() == 1);
			BOOST_CHECK(gbi6_cp != gbi6);

			// Serialize gbi6 and load into gbi6_co, check equalities and similarities
			BOOST_REQUIRE_NO_THROW(gbi6_cp.fromString(gbi6.toString(Gem::Common::SERIALIZATIONMODE_XML), Gem::Common::SERIALIZATIONMODE_XML));
			BOOST_CHECK(gep.isSimilar(gbi6_cp, gbi6));
		}

		{ // binary test format
			// Copy construction of a new object
			GConstrainedInt32 gbi6(0,-10,10);
			GConstrainedInt32 gbi6_cp(gbi6);

			// Check equalities and inequalities
			BOOST_CHECK(gbi6_cp == gbi6);
			// Re-assign a new value to gbi6_cp
			gbi6_cp = 1;
			BOOST_CHECK(gbi6_cp.value() == 1);
			BOOST_CHECK(gbi6_cp != gbi6);

			// Serialize gbi6 and load into gbi6_co, check equalities and similarities
			BOOST_REQUIRE_NO_THROW(gbi6_cp.fromString(gbi6.toString(Gem::Common::SERIALIZATIONMODE_BINARY), Gem::Common::SERIALIZATIONMODE_BINARY));
			BOOST_CHECK(gep.isEqual(gbi6_cp, gbi6));
		}
	}

	/***********************************************************************************/
	// Test features that are expected to fail
	void failures_expected() {
		{
			// Assignment of value outside of the allowed range
			GConstrainedInt32 gbi(-10,10);
			BOOST_CHECK_THROW(gbi=11, Gem::Common::gemfony_error_condition);
		}

		{
			// Self assignment should throw in DEBUG mode
#ifdef DEBUG
			boost::shared_ptr<GConstrainedInt32> gbi_ptr(new GConstrainedInt32(-10,10));
			BOOST_CHECK_THROW(gbi_ptr->load(gbi_ptr), Gem::Common::gemfony_error_condition);
#endif /* DEBUG */
		}
	}

	/***********************************************************************************/
private:
	GRandomT<RANDOMLOCAL> gr;
	const std::size_t NADAPTIONS;
};

/********************************************************************************************/
// This test suite checks as much as possible of the functionality provided
// by the GConstrainedInt32 class. Please also have a look at the manual test,
// as it gives a graphical representation of the mapping.
class GConstrainedInt32Suite: public test_suite
{
public:
	GConstrainedInt32Suite() :test_suite("GConstrainedInt32Suite") {
	  // create an instance of the test cases class
	  boost::shared_ptr<GConstrainedInt32_test> instance(new GConstrainedInt32_test());

	  test_case* no_failure_expected_test_case = BOOST_CLASS_TEST_CASE(&GConstrainedInt32_test::no_failure_expected, instance);
	  test_case* failures_expected_test_case = BOOST_CLASS_TEST_CASE(&GConstrainedInt32_test::failures_expected, instance);

	  add(no_failure_expected_test_case);
	  add(failures_expected_test_case);
	}
};

#endif /* GBOUNDEDINT32_TEST_HPP_ */