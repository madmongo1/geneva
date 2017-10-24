/**
 * @file GCourtierHelperFunctions.hpp
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <cfloat>
#include <climits>

// Boost headers go here
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

#ifndef GCOURTIERHELPERFUNCTIONS_HPP_
#define GCOURTIERHELPERFUNCTIONS_HPP_

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GLogger.hpp"
#include "courtier/GCourtierEnums.hpp"

namespace Gem {
namespace Courtier {

/******************************************************************************/
/** @brief Assembles a query string from a given command */
G_API_COURTIER std::string assembleQueryString(const std::string &, const std::size_t &);

/** @brief Extracts the size of ASIO's data section from a C string. */
G_API_COURTIER std::size_t extractDataSize(const char *, const std::size_t &);

/** @brief Cleanly shuts down a socket */
G_API_COURTIER void disconnect(boost::asio::ip::tcp::socket &);

/** @brief Create a boolean mask */
G_API_COURTIER std::vector<bool> getBooleanMask(
	std::size_t vecSize
	, std::size_t start
	, std::size_t end
);

/******************************************************************************/

} /* namespace Courtier */
} /* namespace Gem */

#endif /* GCOURTIERHELPERFUNCTIONS_HPP_ */