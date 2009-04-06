/**
 * @file GParameterTCollectionT_test.cpp
 */

/* Copyright (C) 2009 Dr. Ruediger Berlich
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

// Geneva header files go here
#include "GenevaExceptions.hpp"
#include "GLogger.hpp"
#include "GLogTargets.hpp"
#include "GRandom.hpp"
#include "GParameterT.hpp"
#include "GParameterTCollectionT.hpp"
#include "GBoolean.hpp"
#include "GBooleanAdaptor.hpp"
#include "GChar.hpp"
#include "GCharFlipAdaptor.hpp"
#include "GInt32.hpp"
#include "GInt32FlipAdaptor.hpp"
#include "GInt32GaussAdaptor.hpp"
#include "GDouble.hpp"
#include "GDoubleGaussAdaptor.hpp"
#include "GBoundedDouble.hpp"
#include "GDouble.hpp"
#include "GChar.hpp"
#include "GInt32.hpp"
#include "GBoolean.hpp"
#include "GBoundedDoubleCollection.hpp"
#include "GDoubleObjectCollection.hpp"
#include "GBooleanObjectCollection.hpp"
#include "GCharObjectCollection.hpp"
#include "GInt32ObjectCollection.hpp"
#include "GStdVectorInterface_test.hpp"

using namespace Gem;
using namespace Gem::Util;
using namespace Gem::GenEvA;
using namespace Gem::GLogFramework;

/***********************************************************************************/
// This test suite checks as much as possible of the functionality provided
// by the GParameterT class. Tests include features of the parent class
// GParameterBaseWithAdaptorsT, as it cannot be instantiated itself.
BOOST_AUTO_TEST_SUITE(GParameterTCollectionTSuite)

// This template allows to create default entries for the collection
// This template allows to create items different of the default item.
// Note that these will not have an adaptor assigned to them and
// can thus not be mutated.
template <typename T> T getTemplateItemNoAdaptor() {
	return T(0);
}

// Specialization for GBoundedDouble
template <> GBoundedDouble getTemplateItemNoAdaptor<GBoundedDouble>() {
	return GBoundedDouble(0.,0.,1.);
}

// This template allows to create items different of the default item.
// Note that these will not have an adaptor assigned to them and
// can thus not be mutated.
template <typename T> T getFindItemNoAdaptor() {
	return T(1);
}

// Specialization for GBoundedDouble
template <> GBoundedDouble getFindItemNoAdaptor<GBoundedDouble>() {
	return GBoundedDouble(1.,0.,1.);
}

/***********************************************************************************/

// This template allows to create default entries for the collection, fully
// equipped with adaptors. As these are different for each type, this
// template should not be called directly. Instead, specializations should
// be used.
template <typename T> T getTemplateItem() {
	// This function should never be called directly
	std::ostringstream error;
	error << "In getTemplateItem<T>(): The template itself should never be called!" << std::endl;
	throw(Gem::GenEvA::geneva_error_condition(error.str()));
}

// Specialization for GBoundedDouble
template <> GBoundedDouble getTemplateItem<GBoundedDouble>() {
	GBoundedDouble gbd(0.,0.,1.);
	gbd.addAdaptor(boost::shared_ptr<GDoubleGaussAdaptor>(new GDoubleGaussAdaptor()));
	return gbd;
}

// Specialization for GDouble
template <> GDouble getTemplateItem<GDouble>() {
	GDouble gbd(0.);
	gbd.addAdaptor(boost::shared_ptr<GDoubleGaussAdaptor>(new GDoubleGaussAdaptor()));
	return gbd;
}

// Specialization for GInt32
template <> GInt32 getTemplateItem<GInt32>() {
	GInt32 gint32(0.);
	gint32.addAdaptor(boost::shared_ptr<GInt32FlipAdaptor>(new GInt32FlipAdaptor()));
	return gint32;
}

// Specialization for GBoolean
template <> GBoolean getTemplateItem<GBoolean>() {
	GBoolean gboolean(false);
	gboolean.addAdaptor(boost::shared_ptr<GBooleanAdaptor>(new GBooleanAdaptor()));
	return gboolean;
}

// Specialization for GChar
template <> GChar getTemplateItem<GChar>() {
	GChar gchar('a');
	gchar.addAdaptor(boost::shared_ptr<GCharFlipAdaptor>(new GCharFlipAdaptor()));
	return gchar;
}

// This template allows to create items different of the default item, , fully
// equipped with adaptors. As these are different for each type, this
// template should not be called directly. Instead, specializations should
// be used.
template <typename T> T getFindItem() {
	// This function should never be called directly
	std::ostringstream error;
	error << "In getFindItem<T>(): The template itself should never be called!" << std::endl;
	throw(Gem::GenEvA::geneva_error_condition(error.str()));
}

// Specialization for GBoundedDouble
template <> GBoundedDouble getFindItem<GBoundedDouble>() {
	GBoundedDouble gbd(1.,0.,1.);
	gbd.addAdaptor(boost::shared_ptr<GDoubleGaussAdaptor>(new GDoubleGaussAdaptor()));
	return gbd;
}

// Specialization for GDouble
template <> GDouble getFindItem<GDouble>() {
	GDouble gbd(1.);
	gbd.addAdaptor(boost::shared_ptr<GDoubleGaussAdaptor>(new GDoubleGaussAdaptor()));
	return gbd;
}

// Specialization for GInt32
template <> GInt32 getFindItem<GInt32>() {
	GInt32 gint32(1.);
	gint32.addAdaptor(boost::shared_ptr<GInt32FlipAdaptor>(new GInt32FlipAdaptor()));
	return gint32;
}

// Specialization for GBoolean
template <> GBoolean getFindItem<GBoolean>() {
	GBoolean gboolean(true);
	gboolean.addAdaptor(boost::shared_ptr<GBooleanAdaptor>(new GBooleanAdaptor()));
	return gboolean;
}

// Specialization for GChar
template <> GChar getFindItem<GChar>() {
	GChar gchar('b');
	gchar.addAdaptor(boost::shared_ptr<GCharFlipAdaptor>(new GCharFlipAdaptor()));
	return gchar;
}

/***********************************************************************************/
// Test features that are expected to work

// typedef boost::mpl::list<GDouble, GBoundedDouble, GChar, GInt32, GBoolean> test_types;
typedef boost::mpl::list<GDouble, GChar, GInt32, GBoolean, GBoundedDouble> test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE( GParameterTCollectionT_no_failure_expected, T, test_types )
{
	GRandom gr;

	// Default construction
	GParameterTCollectionT<T> gptct;

	// Check the vector interface
	T templItem = getTemplateItem<T>();
	T findItem = getFindItem<T>();
	// Make sure both items are indeed different
	BOOST_CHECK(templItem != findItem);
	stdvectorinterfacetestSP(gptct, templItem, findItem);

	// vector functionality of the collection has now been thoroughly tested.
	// Collection items should be remaining in the object. Check.
	BOOST_CHECK(!gptct.empty());

	// Create two copies of the object
	GParameterTCollectionT<T> gptct_cp1, gptct_cp2;
	gptct_cp1 = gptct_cp2 = gptct;

	// Check that they are indeed identical
	BOOST_CHECK(gptct_cp1 == gptct);
	BOOST_CHECK(gptct_cp2 == gptct);

	// Mutate the second copy and check that it has become different from the other two collections
	BOOST_REQUIRE_NO_THROW(gptct_cp2.mutate());
	BOOST_CHECK(gptct_cp2 != gptct);
	BOOST_CHECK(gptct_cp2 != gptct_cp1);

	// Test serialization and loading in different serialization modes
	{ // plain text format
		// Copy construction of a new object
		GParameterTCollectionT<T> gptct_cp3(gptct);

		// Check equalities and inequalities
		BOOST_CHECK(gptct_cp3 == gptct);
		BOOST_CHECK(gptct_cp3 == gptct_cp1);
		BOOST_CHECK(gptct_cp3 != gptct_cp2);

		// Serialize cp2 and load into cp3, check inequalities and similarities
		BOOST_REQUIRE_NO_THROW(gptct_cp3.fromString(gptct_cp2.toString(TEXTSERIALIZATION), TEXTSERIALIZATION));
		BOOST_CHECK(!gptct_cp3.isEqualTo(gptct));
		BOOST_CHECK(!gptct_cp3.isEqualTo(gptct_cp1));
		BOOST_CHECK(gptct_cp3.isSimilarTo(gptct_cp2, exp(-10)));
	}

	{ // XML format
		// Copy construction of a new object
		GParameterTCollectionT<T> gptct_cp3(gptct);

		// Check equalities and inequalities
		BOOST_CHECK(gptct_cp3 == gptct);
		BOOST_CHECK(gptct_cp3 == gptct_cp1);
		BOOST_CHECK(gptct_cp3 != gptct_cp2);

		// Serialize cp2 and load into cp3, check inequalities and similarities
		BOOST_REQUIRE_NO_THROW(gptct_cp3.fromString(gptct_cp2.toString(XMLSERIALIZATION), XMLSERIALIZATION));
		BOOST_CHECK(!gptct_cp3.isEqualTo(gptct));
		BOOST_CHECK(!gptct_cp3.isEqualTo(gptct_cp1));
		BOOST_CHECK(gptct_cp3.isSimilarTo(gptct_cp2, exp(-10)));
	}

	{ // binary test format
		// Copy construction of a new object
		GParameterTCollectionT<T> gptct_cp3(gptct);

		// Check equalities and inequalities
		BOOST_CHECK(gptct_cp3 == gptct);
		BOOST_CHECK(gptct_cp3 == gptct_cp1);
		BOOST_CHECK(gptct_cp3 != gptct_cp2);

		// Serialize cp2 and load into cp3, check inequalities and similarities
		BOOST_REQUIRE_NO_THROW(gptct_cp3.fromString(gptct_cp2.toString(BINARYSERIALIZATION), BINARYSERIALIZATION));
		BOOST_CHECK(!gptct_cp3.isEqualTo(gptct));
		BOOST_CHECK(!gptct_cp3.isEqualTo(gptct_cp1));
		BOOST_CHECK(gptct_cp3.isEqualTo(gptct_cp2));
	}
}

/***********************************************************************************/
// Test features that are expected to fail. Test with one derived class only.
BOOST_AUTO_TEST_CASE_TEMPLATE( GParmeterTCollectionT_failures_expected, T, test_types)
{
	GRandom gr;

	// Default construction
	GParameterTCollectionT<T> gptct;

	// Self assignment should throw
	BOOST_CHECK_THROW(gptct.load(&gptct), Gem::GenEvA::geneva_error_condition);
}
/***********************************************************************************/

BOOST_AUTO_TEST_SUITE_END()
