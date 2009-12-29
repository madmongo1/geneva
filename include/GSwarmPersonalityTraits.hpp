/**
 * @file GSwarmPersonalityTraits.hpp
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


// Standard headers go here
#include <string>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost headers go here
#include <boost/cstdint.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>


#ifndef GSWARMPERSONALITYTRAITS_HPP_
#define GSWARMPERSONALITYTRAITS_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// GenEvA headers go here
#include "GPersonalityTraits.hpp"

namespace Gem {
namespace GenEvA {

/*********************************************************************************/
/**
 * This class adds variables and functions to GPersonalityTraits that are specific
 * to evolutionary algorithms.
 */
class GSwarmPersonalityTraits :public GPersonalityTraits
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;
	  ar & make_nvp("GPersonalityTraits", boost::serialization::base_object<GPersonalityTraits>(*this));
	  ar & make_nvp("command_", command_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GSwarmPersonalityTraits();
	/** @brief The copy contructor */
	GSwarmPersonalityTraits(const GSwarmPersonalityTraits&);
	/** @brief The standard destructor */
	virtual ~GSwarmPersonalityTraits();

	/** @brief Checks for equality with another GSwarmPersonalityTraits object */
	bool operator==(const GSwarmPersonalityTraits&) const;
	/** @brief Checks for inequality with another GSwarmPersonalityTraits object */
	bool operator!=(const GSwarmPersonalityTraits&) const;
	/** @brief Checks for equality with another GSwarmPersonalityTraits object */
	virtual bool isEqualTo(const GObject&, const boost::logic::tribool& expected = boost::logic::indeterminate) const;
	/** @brief Checks for similarity with another GSwarmPersonalityTraits object */
	virtual bool isSimilarTo(const GObject&, const double& limit, const boost::logic::tribool& expected = boost::logic::indeterminate) const;

	/** @brief Creates a deep clone of this object */
	virtual GObject* clone() const;
	/** @brief Loads the data of another GSwarmPersonalityTraits object */
	virtual void load(const GObject*);

	/** @brief Sets a command to be performed by a remote client. */
	virtual void setCommand(const std::string&);
	/** @brief Retrieves the command to be performed by a remote client. */
	virtual std::string getCommand() const;

private:
	/** @brief The command to be performed by remote clients */
	std::string command_;
};

} /* namespace GenEvA */
} /* namespace Gem */


#endif /* GSWARMPERSONALITYTRAITS_HPP_ */
