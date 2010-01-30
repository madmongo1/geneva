/**
 * @file GGaussAdaptorT_test.cpp
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
#include <cmath>
#include <algorithm>

// Boost header files go here

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

#ifndef GGAUSSADAPTORT_TEST_HPP_
#define GGAUSSADAPTORT_TEST_HPP_

// Geneva header files go here
#include "GenevaExceptions.hpp"
#include "GRandom.hpp"
#include "GGaussAdaptorT.hpp"
#include "GDoubleGaussAdaptor.hpp"
#include "GInt32GaussAdaptor.hpp"
#include "GEqualityPrinter.hpp"

using namespace Gem;
using namespace Gem::Util;
using namespace Gem::GenEvA;

using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

/********************************************************************************************/
// The actual unit tests for this class

// Test features that are expected to work
BOOST_TEST_CASE_TEMPLATE_FUNCTION( GGaussAdaptorT_no_failure_expected, T )
{
	// Prepare printing of error messages in object comparisons
	GEqualityPrinter gep("GGaussAdaptorT_no_failure_expected",
						 exp(-10),
						 Gem::Util::CE_WITH_MESSAGES);

	// A local random number generator
	GRandom gr;

	// Test simple instantiation
	T ggat0;
	// An id should have been set automatically
	BOOST_CHECK(ggat0.getAdaptorId() == Gem::GenEvA::GDOUBLEGAUSSADAPTOR || ggat0.getAdaptorId() == Gem::GenEvA::GINT32GAUSSADAPTOR);

	// Instantiation with an intentionally long sigma
	T ggat1(0.202030405060708,0.001, 0., 1.); // intentionally long

	// Instantiation with sigma, sigmaSigma, minSigma and maxSigma
	T ggat2(0.1, 0.001, 0., 1.);
	BOOST_CHECK(ggat2 != ggat1);

	// Copy construction
	T ggat3(ggat2);
	BOOST_CHECK(ggat3 == ggat2);

	// Assignment
	boost::shared_ptr<T> ggat4_ptr = ggat1.GObject::clone<T>();
	*ggat4_ptr = ggat3;
	BOOST_CHECK(*ggat4_ptr == ggat3 && *ggat4_ptr == ggat2);

	// ... and loading
	T ggat5;
	ggat5.load(ggat4_ptr.get());
	BOOST_CHECK(ggat5 == ggat3 && *ggat4_ptr == ggat2);

	// Check (de-)serialization in different modes
	BOOST_CHECK_NO_THROW(ggat5.fromString(ggat1.toString(TEXTSERIALIZATION),TEXTSERIALIZATION));
	BOOST_CHECK(gep.isSimilar(ggat5, ggat1));

	ggat5 = ggat3; // reset
	BOOST_CHECK(gep.isInEqual(ggat5, ggat1));

	BOOST_CHECK_NO_THROW(ggat5.fromString(ggat1.toString(XMLSERIALIZATION),XMLSERIALIZATION));
	BOOST_CHECK(gep.isSimilar(ggat5, ggat1));
	ggat5 = ggat3; // reset
	BOOST_CHECK_NO_THROW(ggat5.fromString(ggat1.toString(BINARYSERIALIZATION),BINARYSERIALIZATION));
	BOOST_CHECK(gep.isInEqual(ggat5, ggat1));

	// Check that we can set and retrieve sigma
	BOOST_CHECK_NO_THROW(ggat5.setSigma(0.5));
	BOOST_CHECK(ggat5.getSigma() == 0.5);

	// Check that we can set and retrieve the sigma range
	BOOST_CHECK_NO_THROW(ggat5.setSigmaRange(0.,2.));
	BOOST_CHECK(ggat5.getSigma() == 0.5);

	// Check that sigma actually gets adapted, if we move the lower boundary
	BOOST_CHECK_NO_THROW(ggat5.setSigmaRange(0.6,2.));
	BOOST_CHECK(ggat5.getSigma() == 0.6);

	// Check the range
	BOOST_CHECK(ggat5.getSigmaRange().first == 0.6 && ggat5.getSigmaRange().second == 2.);

	// Set and retrieve the adaption rate
	BOOST_CHECK_NO_THROW(ggat5.setSigmaAdaptionRate(0.001));
	BOOST_CHECK(ggat5.getSigmaAdaptionRate() == 0.001);

	// Finally set all parameters in one go
	BOOST_CHECK_NO_THROW(ggat5.setAll(0.1,0.001,0.,2.));
	BOOST_CHECK(ggat5.getSigma() == 0.1);
	BOOST_CHECK(ggat5.getSigmaAdaptionRate() == 0.001);
	BOOST_CHECK(ggat5.getSigmaRange().first == DEFAULTMINSIGMA && ggat5.getSigmaRange().second == 2.);

	// Perform mutations with varying mutation parameters
	typename T::mutant_type mutationTarget = typename T::mutant_type(0);
	std::size_t NMUTATIONS=10000;
	ggat5.setAdaptionThreshold(1);
	for(std::size_t p=0; p<20; p++) {
		BOOST_CHECK_NO_THROW(ggat5.setAll(gr.evenRandom(DEFAULTMINSIGMA,2.), 0.001, 0., 2.));
		for(std::size_t m=0; m<NMUTATIONS; m++)
			BOOST_CHECK_NO_THROW(ggat5.mutate(mutationTarget));
	}
}

/***********************************************************************************/
// Test features that are expected to fail
BOOST_TEST_CASE_TEMPLATE_FUNCTION( GGaussAdaptorT_failures_expected, T )
{
	GRandom gr;

	{
		T ggat0(0.1, 0.001, 0., 1.);
		BOOST_CHECK_THROW(ggat0.setSigma(1.1), Gem::GenEvA::geneva_error_condition); // outside of the allowed range
	}

	{ // not sure what state ggat0 is in after it has thrown. Hence we recreate it for the next test
		T ggat0;
		BOOST_CHECK_THROW(ggat0.setSigmaRange(-1.,1.), Gem::GenEvA::geneva_error_condition); // outside of the allowed range
	}

	{ // not sure what state ggat0 is in after it has thrown. Hence we recreate it for the next test
		T ggat0;
		BOOST_CHECK_THROW(ggat0.setSigmaAdaptionRate(0.), Gem::GenEvA::geneva_error_condition); // 0. is not an allowed value
	}

	{
		// Self assignment should throw in DEBUG mode
#ifdef DEBUG
		T ggat0;
		BOOST_CHECK_THROW(ggat0.load(&ggat0), Gem::GenEvA::geneva_error_condition);
#endif /* DEBUG */
	}
}

/***********************************************************************************/


/***********************************************************************************/
// This test suite checks as much as possible of the functionality provided
// by the GGaussAdaptorT class. The template is instantiated for all types
// defined in an mpl::list . Note that a lot of functionality of this class has
// already been covered as GBooleanAdaptor has been used as a vehicle to
// test GObject and GAdaotorT.
class GGaussAdaptorTSuite: public test_suite
{
public:
	GGaussAdaptorTSuite() :test_suite("GGaussAdaptorTSuite") {
		typedef boost::mpl::list<GInt32GaussAdaptor, GDoubleGaussAdaptor> test_types;

		add( BOOST_TEST_CASE_TEMPLATE( GGaussAdaptorT_no_failure_expected, test_types ) );
		add( BOOST_TEST_CASE_TEMPLATE( GGaussAdaptorT_failures_expected, test_types ) );
	}
};

#endif /* GGAUSSADAPTORT_TEST_HPP_ */