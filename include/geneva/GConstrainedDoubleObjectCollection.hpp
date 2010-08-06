/**
 * @file GConstrainedDoubleObjectCollection.hpp
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
#include <sstream>
#include <vector>
#include <algorithm>

// Includes check for correct Boost version(s)
#include "common/GGlobalDefines.hpp"

// Boost header files go here

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#ifndef GCONSTRAINEDDOUBLECOLLECTION_HPP_
#define GCONSTRAINEDDOUBLECOLLECTION_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva header files go here
#include "GConstrainedDouble.hpp"
#include "GParameterTCollectionT.hpp"

namespace Gem {
namespace Geneva {

/*************************************************************************/
/**
 * A collection of GConstrainedDouble objects, ready for use in a
 * GParameterSet derivative.
 */
class GConstrainedDoubleObjectCollection
	:public GParameterTCollectionT<GConstrainedDouble>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;

	  ar & make_nvp("GParameterTCollectionT_gbd",
			  boost::serialization::base_object<GParameterTCollectionT<GConstrainedDouble> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GConstrainedDoubleObjectCollection();
	/** @brief The copy constructor */
	GConstrainedDoubleObjectCollection(const GConstrainedDoubleObjectCollection&);
	/** @brief The destructor */
	virtual ~GConstrainedDoubleObjectCollection();

	/** @brief A standard assignment operator */
	const GConstrainedDoubleObjectCollection& operator=(const GConstrainedDoubleObjectCollection&);

	/** @brief Checks for equality with another GConstrainedDoubleObjectCollection object */
	bool operator==(const GConstrainedDoubleObjectCollection&) const;
	/** @brief Checks for inequality with another GConstrainedDoubleObjectCollection object */
	bool operator!=(const GConstrainedDoubleObjectCollection&) const;

	/** @brief Checks whether this object fulfills a given expectation in relation to another object */
	virtual boost::optional<std::string> checkRelationshipWith(const GObject&, const Gem::Common::expectation&, const double&, const std::string&, const std::string&, const bool&) const;

protected:
	/** @brief Loads the data of another GObject */
	virtual void load_(const GObject*);
	/** @brief Creates a deep clone of this object. */
	virtual GObject* clone_() const;

#ifdef GENEVATESTING
public:
	/** @brief Applies modifications to this object. This is needed for testing purposes */
	virtual bool modify_GUnitTests();
	/** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	virtual void specificTestsNoFailureExpected_GUnitTests();
	/** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	virtual void specificTestsFailuresExpected_GUnitTests();
#endif /* GENEVATESTING */
};

/*************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

#endif /* GCONSTRAINEDDOUBLECOLLECTION_HPP_ */