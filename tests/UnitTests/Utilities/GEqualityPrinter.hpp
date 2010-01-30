/**
 * @file GEqualityPrinter.hpp
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
#include <boost/mpl/has_xxx.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>

#ifndef GEQUALITYPRINTER_HPP_
#define GEQUALITYPRINTER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

/*************************************************************************************************/
/**
 * Define a check whether we are dealing with an object which has a given function.
 * See "Beyond the C++ Standard Library" by Bjoern Karlsson, p. 99 for an explanation
 */
BOOST_MPL_HAS_XXX_TRAIT_DEF(checkRelationshipWithFunction)


/*************************************************************************************************/
/**
 * This is a simple convenience class to facilitate comparisons in Geneva's test framework.
 */
class GEqualityPrinter
	: boost::noncopyable
{
public:
	/*********************************************************************************************/
	/**
	 * The only constructor. The default constructor has been disabled.
	 *
	 * @param caller The name of the calling entity
	 * @param limit Used in floating point comparisons to check similarity
	 * @param emitMessages Determines whether messages should be emitted upon error
	 */
	GEqualityPrinter (
		  const std::string& caller
		, const double& limit
		, const bool& emitMessages
	)
		: caller_(caller)
		, limit_(limit)
		, emitMessages_(emitMessages)
	{ /* nothing */ }

	/*********************************************************************************************/
	/**
	 * Checks for equality, optionally emitting a message. The compared entities must have the
	 * Geneva interface.
	 *
	 * @param x The first parameter to compare
	 * @oaram y The second parameter to compare
	 * @return A boolean indicating whether both parameters are equal
	 */
	template <typename geneva_type>
	bool isEqual(
			  const geneva_type& x
			, const geneva_type& y
			, typename boost::enable_if<has_checkRelationshipWithFunction<geneva_type> >::type* dummy = 0
	) const	{
		using namespace Gem::Util;

		boost::optional<std::string> o =
				x.checkRelationshipWith(
						y
					  , Gem::Util::CE_EQUALITY
					  , 0.
					  , caller_
					  , "y"
					  , emitMessages_?CE_WITH_MESSAGES:CE_SILENT);

		if(o) { // The expectation was not met
			std::cout
			<< "\n=========================================\n"
			<< *o
			<< "\n=========================================\n";
			return false;
		}
		else {
			return true;
		}
	}

	/*********************************************************************************************/
	/**
	 * Checks for inequality, optionally emitting a message. The compared entities must have the
	 * Geneva interface.
	 *
	 * @param x The first parameter to compare
	 * @oaram y The second parameter to compare
	 * @return A boolean indicating whether both parameters are inequal
	 */
	template <typename geneva_type>
	bool isInEqual(
			  const geneva_type& x
			, const geneva_type& y
			, typename boost::enable_if<has_checkRelationshipWithFunction<geneva_type> >::type* dummy = 0
	) const	{
		using namespace Gem::Util;

		boost::optional<std::string> o =
				x.checkRelationshipWith(
						y
					  , Gem::Util::CE_INEQUALITY
					  , 0.
					  , caller_
					  , "y"
					  , emitMessages_?CE_WITH_MESSAGES:CE_SILENT);

		if(o) { // The expectation was not met
			std::cout
			<< "\n=========================================\n"
			<< *o
			<< "\n=========================================\n";
			return false;
		}
		else {
			return true;
		}
	}

	/*********************************************************************************************/
	/**
	 * Checks for similarity, optionally emitting a message. The compared entities must have the
	 * Geneva interface.
	 *
	 * @param x The first parameter to compare
	 * @oaram y The second parameter to compare
	 * @param limit A limit used to determine similarity in fp comparisons
	 * @return A boolean indicating whether both parameters are similar
	 */
	template <typename geneva_type>
	bool isSimilar(
			  const geneva_type& x
			, const geneva_type& y
			, double limit = limit_
			, typename boost::enable_if<has_checkRelationshipWithFunction<geneva_type> >::type* dummy = 0
	) const	{
		using namespace Gem::Util;

		boost::optional<std::string> o =
				x.checkRelationshipWith(
						y
					  , Gem::Util::CE_FP_SIMILARITY
					  , limit
					  , caller_
					  , "y"
					  , emitMessages_?CE_WITH_MESSAGES:CE_SILENT);

		if(o) { // The expectation was not met
			std::cout
			<< "\n=========================================\n"
			<< *o
			<< "\n=========================================\n";
			return false;
		}
		else {
			return true;
		}
	}

	/*********************************************************************************************/

private:
	GEqualityPrinter(); ///< Default constructor intentionally left undefined

	std::string caller_; ///< Holds the name of the calling entity
	double limit_; ///< A limit used to determine similarity in fp comparisons
	bool emitMessages_; ///< Specifies whether messages should be emitted if expectations were not met
};

/*************************************************************************************************/

#endif /* GEQUALITYPRINTER_HPP_ */
