/**
 * @file GIdentityAdaptorT.cpp
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

#include "GIdentityAdaptorT.hpp"

/**
 * Included here so no conflicts occur. See explanation at
 * http://www.boost.org/libs/serialization/doc/special.html#derivedpointers
 */
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_GUID(Gem::GenEvA::GIdentityAdaptorT<boost::int32_t>, "GIdentityAdaptorT_int32_t")
BOOST_CLASS_EXPORT_GUID(Gem::GenEvA::GIdentityAdaptorT<double>, "GIdentityAdaptorT_double")
BOOST_CLASS_EXPORT_GUID(Gem::GenEvA::GIdentityAdaptorT<bool>, "GIdentityAdaptorT_bool")
BOOST_CLASS_EXPORT_GUID(Gem::GenEvA::GIdentityAdaptorT<char>, "GIdentityAdaptorT_char")