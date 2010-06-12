/**
 * @file GSerializableI.hpp
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

#include <string>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost header files go here

#ifndef GSERIALIZABLEI_HPP_
#define GSERIALIZABLEI_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva header files go here
#include "GEnums.hpp" // For the serialization mode

namespace Gem {
namespace GenEvA {

/**************************************************************************************************/
/**
 * This is an interface class that specifies the operations available
 * for "serializable" classes.
 */
class GSerializableI {
public:
	/** @brief The standard destructor */
	virtual ~GSerializableI() { /* nothing */ }

	/** @brief Create a text representation from this class */
	virtual std::string toString(const serializationMode&) = 0;

	/** @brief Initialize this class from a text representation */
	virtual void fromString(const std::string&,const serializationMode&) = 0;
};

} /* namespace GenEvA */
} /* namespace Gem */

/**************************************************************************************************/

#endif /* GSERIALIZABLEI_HPP_ */