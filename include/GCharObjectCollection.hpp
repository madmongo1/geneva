/**
 * @file GCharObjectCollection.hpp
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

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost header files go here

#ifndef GCHAROBJECTCOLLECTION_HPP_
#define GCHAROBJECTCOLLECTION_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// GenEvA header files go here
#include "GChar.hpp"
#include "GParameterTCollectionT.hpp"

namespace Gem {
namespace GenEvA {

/*************************************************************************/
/**
 * A collection of GChar objects, ready for use in a
 * GParameterSet derivative.
 */
class GCharObjectCollection
	:public GParameterTCollectionT<GChar>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;

	  ar & make_nvp("GParameterTCollectionT_gcoc",
			  boost::serialization::base_object<GParameterTCollectionT<GChar> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GCharObjectCollection();
	/** @brief The copy constructor */
	GCharObjectCollection(const GCharObjectCollection&);
	/** @brief The destructor */
	virtual ~GCharObjectCollection();

	/** @brief A standard assignment operator */
	const GCharObjectCollection& operator=(const GCharObjectCollection&);

	/** @brief Checks for equality with another GCharObjectCollection object */
	bool operator==(const GCharObjectCollection&) const;
	/** @brief Checks for inequality with another GCharObjectCollection object */
	bool operator!=(const GCharObjectCollection&) const;

	/** @brief Checks whether this object fulfills a given expectation in relation to another object */
	virtual boost::optional<std::string> checkRelationshipWith(const GObject&, const Gem::Util::expectation&, const double&, const std::string&, const std::string&, const bool&) const;

protected:
	/** @brief Loads the data of another GObject */
	virtual void load_(const GObject*);
	/** @brief Creates a deep clone of this object. */
	virtual GObject* clone_() const;
};

/*************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */

#endif /* GCHAROBJECTCOLLECTION_HPP_ */
