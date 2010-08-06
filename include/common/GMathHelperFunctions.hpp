/**
 * @file GMathHelperFunctions.hpp
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
#include <cmath>

// Boost headers go here

#ifndef GMATHHELPERFUNCTIONS_HPP_
#define GMATHHELPERFUNCTIONS_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here

namespace Gem {
namespace Common {

/************************************************************************************/

/** Calculates the floor value of a float value */
float GFloor(const float&);
/** @brief Calculates the floor value of a double value */
double GFloor(const double&);
/** @brief Calculates the floor value of a long double value */
long double GFloor(const long double&);

/** @brief Calculates the fabs value of a float value */
float GFabs(const float&);
/** @brief Calculates the fabs value of a double value */
double GFabs(const double&);
/** @brief Calculates the fabs value of a long double value */
long double GFabs(const long double&);

/** @brief Calculates the sqrt value of a float value */
float GSqrt(const float&);
/** @brief Calculates the sqrt value of a double value */
double GSqrt(const double&);
/** @brief Calculates the sqrt value of a long double value */
long double GSqrt(const long double&);

/** @brief Calculates the sin value of a float value */
float GSin(const float&);
/** @brief Calculates the sin value of a double value */
double GSin(const double&);
/** @brief Calculates the sin value of a long double value */
long double GSin(const long double&);

/** @brief Calculates the cos value of a float value */
float GCos(const float&);
/** @brief Calculates the cos value of a double value */
double GCos(const double&);
/** @brief Calculates the cos value of a long double value */
long double GCos(const long double&);

/** @brief Calculates the log value of a float value */
float GLog(const float&);
/** @brief Calculates the log value of a double value */
double GLog(const double&);
/** @brief Calculates the log value of a long double value */
long double GLog(const long double&);

/************************************************************************************/

} /* namespace Common */
} /* namespace Gem */

#endif /* GMATHHELPERFUNCTIONS_HPP_ */