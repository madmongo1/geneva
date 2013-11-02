/**
 * @file GParameterPropertyParser.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) eu
 *
 * This file is part of the Geneva library collection.
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * Geneva is free software: you can redistribute and/or modify it under
 * the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Geneva library. If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.eu .
 */

// Standard headers go here
#include <string>
#include <iostream>
#include <vector>

// Boost headers go here
#include <boost/tuple/tuple.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/tuple.hpp>
#include <boost/fusion/include/boost_tuple.hpp>
#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/variant.hpp>
#include <boost/cstdint.hpp>

#ifndef GPARAMETERPROPERTYPARSER_HPP_
#define GPARAMETERPROPERTYPARSER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "common/GLogger.hpp"
#include "common/GExceptions.hpp"

namespace Gem {
namespace Geneva {

const bool ISINDEX = false;
const bool ISNAME  = true;

/******************************************************************************/
/** @brief Converts the nameOrId parameter into the desired target type */
boost::variant<std::size_t, std::string> getNameOrId(bool &type, const std::string& nameOrId);

/******************************************************************************/
/**
 * This struct holds common properties of supported parameters in the context
 * of parameter scans. This is targeted at float, double, integer and boolean
 * parameter types. Note that particularly the nSteps variable can have different
 * meanings for different types. E.g., for double variables it may stand for the
 * number of steps from the lower (inclusive) to the upper (exclusive) boundary
 * OR the number of random values picked from this range, whereas for booleans
 * it may only signify the latter.
 */
template <typename par_type>
struct parPropSpec {
   std::string nameOrId; ///< Either a unique variable name or the index of a vector of parameter objects
   par_type lowerBoundary; ///< The lower boundary for the parameter scan
   par_type upperBoundary; ///< The upper boundary for the parameter scan
   std::size_t nSteps;   ///< The number of steps from the lower boundary to the upper boundary (or possibly the number of random values from this parameter range, depending on the scan mode and parameter type)
};

/******************************************************************************/
/**
 * A simple output operator, mostly for debugging purposes
 *
 * @param o A reference to the output stream
 * @param s The object to be emitted
 * @return A reference to the output stream
 */
template <typename par_type>
std::ostream& operator<<(std::ostream& o, const parPropSpec<par_type>& s) {
   bool noi = ISINDEX;
   boost::variant<std::size_t, std::string> v = getNameOrId(noi, s.nameOrId);

   if(ISINDEX==noi) {
      o << "id = " << boost::get<std::size_t>(v) << std::endl;
   } else {
      o << "Name = " << boost::get<std::string>(v) << std::endl;
   }

   o
   << "lowerBoundary = " << s.lowerBoundary << std::endl
   << "upperBoundary = " << s.upperBoundary << std::endl
   << "nSteps = "        << s.nSteps << std::endl;

   return o;
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

/******************************************************************************/
// Make our structs accessible to boost.fusion. These macros must be
// called in the global namespace

/** @brief Makes the struct boost.fusion-compatible */
BOOST_FUSION_ADAPT_STRUCT(
      Gem::Geneva::parPropSpec<double>,
      (std::string, nameOrId)
      (double, lowerBoundary)
      (double, upperBoundary)
      (std::size_t, nSteps)
)

/** @brief Makes the struct boost.fusion-compatible */
BOOST_FUSION_ADAPT_STRUCT(
      Gem::Geneva::parPropSpec<float>,
      (std::string, nameOrId)
      (float, lowerBoundary)
      (float, upperBoundary)
      (std::size_t, nSteps)
)

/** @brief Makes the struct boost.fusion-compatible */
BOOST_FUSION_ADAPT_STRUCT(
      Gem::Geneva::parPropSpec<boost::int32_t>,
      (std::string, nameOrId)
      (boost::int32_t, lowerBoundary)
      (boost::int32_t, upperBoundary)
      (std::size_t, nSteps)
)

/** @brief Makes the struct boost.fusion-compatible */
BOOST_FUSION_ADAPT_STRUCT(
      Gem::Geneva::parPropSpec<bool>,
      (std::string, nameOrId)
      (bool, lowerBoundary)
      (bool, upperBoundary)
      (std::size_t, nSteps)
)

/******************************************************************************/

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * This class accepts a "raw" parameter description, parses it and provides
 * functions to access individual parameter properties. Note that this class
 * is meant for setup purposes only and thus cannot be serialized (nor can it
 * be copied).
 */
class GParameterPropertyParser: boost::noncopyable // Make sure this class cannot be copied
{
public:
   /** @brief The standard constructor -- assignment of the "raw" paramter property string */
   GParameterPropertyParser(const std::string&);

   /** @brief Retrieves the raw parameter description */
   std::string getRawParameterDescription() const;
   /** @brief Allows to check whether parsing has already taken place */
   bool isParsed() const;

   /** @brief Allows to reset the internal structures and to parse a new parameter string */
   void setNewParameterDescription(std::string);

   /** @brief Initiates parsing of the raw string */
   void parse();

   /***************************************************************************/
   /**
    * This function returns a set of const_iterators that allow to retrieve
    * the information from the parsers. Note that these iterators may go out
    * of scope, if a new parameter description is supplied to this class.
    *
    * The first tuple-entry allows you to access all parameter entries. When the
    * function is called, it is set to the start of the vector. The second tuple
    * entry is set to the vector end.
    *
    * The function will throw if parsing hasn't happened yet.
    *
    * Note that this implementation is a trap. Use one of the overloads for
    * supported types instead.
    */
   template <typename par_type>
   boost::tuple<typename std::vector<parPropSpec<par_type> >::const_iterator, typename std::vector<parPropSpec<par_type> >::const_iterator> getIterators() const {
      boost::tuple<typename std::vector<parPropSpec<par_type> >::const_iterator, typename std::vector<parPropSpec<par_type> >::const_iterator> result;

      glogger
      << "In generic GParameterPropertyParser::getIterators<par_type>() function: Error!" << std::endl
      << "Function was called for an unsupported type" << std::endl
      << GEXCEPTION;

      // Make the compiler happy
      return result;
   }

private:
   /***************************************************************************/
   /** @brief The default constructor -- intentionally private and undefined */
   GParameterPropertyParser();

   boost::spirit::qi::rule<std::string::const_iterator, std::string(), boost::spirit::ascii::space_type> varSpec;
   boost::spirit::qi::rule<std::string::const_iterator, boost::tuple<char, std::string>(), boost::spirit::ascii::space_type> varString;

   boost::spirit::qi::rule<std::string::const_iterator, parPropSpec<double>()         , boost::spirit::ascii::space_type> doubleStringParser;
   boost::spirit::qi::rule<std::string::const_iterator, parPropSpec<float>()          , boost::spirit::ascii::space_type> floatStringParser;
   boost::spirit::qi::rule<std::string::const_iterator, parPropSpec<boost::int32_t>() , boost::spirit::ascii::space_type> intStringParser;
   boost::spirit::qi::rule<std::string::const_iterator, parPropSpec<bool>()           , boost::spirit::ascii::space_type> boolStringParser;

   std::string raw_; ///< Holds the "raw" parameter description
   bool parsed_;     ///< Indicates whether the raw_ string has already been parsed

   std::vector<parPropSpec<double> >         dSpecVec; ///< Holds parameter specifications for double values
   std::vector<parPropSpec<float> >          fSpecVec; ///< Holds parameter specifications for float values
   std::vector<parPropSpec<boost::int32_t> > iSpecVec; ///< Holds parameter specifications for integer values
   std::vector<parPropSpec<bool> >           bSpecVec; ///< Holds parameter specifications for boolean values
};

/******************************************************************************/
// Overloads of the getIterators() function for supported types
template <> boost::tuple<std::vector<parPropSpec<double> >::const_iterator, std::vector<parPropSpec<double> >::const_iterator> GParameterPropertyParser::getIterators<double>() const;
template <> boost::tuple<std::vector<parPropSpec<float> >::const_iterator, std::vector<parPropSpec<float> >::const_iterator> GParameterPropertyParser::getIterators<float>() const;
template <> boost::tuple<std::vector<parPropSpec<boost::int32_t> >::const_iterator, std::vector<parPropSpec<boost::int32_t> >::const_iterator> GParameterPropertyParser::getIterators<boost::int32_t>() const;
template <> boost::tuple<std::vector<parPropSpec<bool> >::const_iterator, std::vector<parPropSpec<bool> >::const_iterator> GParameterPropertyParser::getIterators<bool>() const;

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

#endif /* GPARAMETERPROPERTYPARSER_HPP_ */