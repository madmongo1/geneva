/**
 * @file GMultiThreadedEA_test.cpp
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

#ifndef GMULTITHREADEDEA_TEST_HPP_
#define GMULTITHREADEDEA_TEST_HPP_

// Geneva header files go here
#include "GenevaExceptions.hpp"
#include "GRandom.hpp"
#include "GMultiThreadedEA.hpp"
#include "GEqualityPrinter.hpp"

using namespace Gem;
using namespace Gem::Util;
using namespace Gem::GenEvA;

using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

/********************************************************************************************/
// The actual unit tests for this class
class GMultiThreadedEA_test {
public:
	/***********************************************************************************/
	// Test features that are expected to work
	void no_failure_expected() {
		/* empty */
	}

	/***********************************************************************************/
	// Test features that are expected to fail
	void failures_expected() {
		/* empty */
	}

	/***********************************************************************************/
private:
	GRandom gr;
};

/********************************************************************************************/
// This test suite checks as much as possible of the functionality provided
// by the GMultiThreadedEA class.
class GMultiThreadedEASuite: public test_suite
{
public:
	GMultiThreadedEASuite() :test_suite("GMultiThreadedEASuite") {
	  // create an instance of the test cases class
	  boost::shared_ptr<GMultiThreadedEA_test> instance(new GMultiThreadedEA_test());

	  test_case* no_failure_expected_test_case = BOOST_CLASS_TEST_CASE(&GMultiThreadedEA_test::no_failure_expected, instance);
	  test_case* failures_expected_test_case = BOOST_CLASS_TEST_CASE(&GMultiThreadedEA_test::failures_expected, instance);

	  add(no_failure_expected_test_case);
	  add(failures_expected_test_case);
	}
};

/********************************************************************************************/

#endif /* GMULTITHREADEDEA_TEST_HPP_ */
