/**
 * @file GEnums.cpp
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

#include "geneva/GOptimizationEnums.hpp"

namespace Gem {
namespace Geneva {

/*********************************************************************/
/**
 * Puts a Gem::Geneva::recoScheme item into a stream
 *
 * @param o The ostream the item should be added to
 * @param rc the item to be added to the stream
 * @return The std::ostream object used to add the item to
 */
std::ostream& operator<<(std::ostream& o, const Gem::Geneva::recoScheme& rc) {
	boost::uint16_t tmp = static_cast<boost::uint16_t>(rc);
	o << tmp;
	return o;
}

/*********************************************************************/
/**
 * Reads a Gem::Geneva::recoScheme item from a stream
 *
 * @param i The stream the item should be read from
 * @param rc The item read from the stream
 * @return The std::istream object used to read the item from
 */
std::istream& operator>>(std::istream& i, Gem::Geneva::recoScheme& rc) {
	boost::uint16_t tmp;
	i >> tmp;

#ifdef DEBUG
	rc = boost::numeric_cast<Gem::Geneva::recoScheme>(tmp);
#else
	rc = static_cast<Gem::Geneva::recoScheme>(tmp);
#endif /* DEBUG */

	return i;
}

/*********************************************************************/
/**
 * Puts a Gem::Geneva::infoMode item into a stream
 *
 * @param o The ostream the item should be added to
 * @param im the item to be added to the stream
 * @return The std::ostream object used to add the item to
 */
std::ostream& operator<<(std::ostream& o, const Gem::Geneva::infoMode& im) {
	boost::uint16_t tmp = static_cast<boost::uint16_t>(im);
	o << tmp;
	return o;
}

/*********************************************************************/
/**
 * Reads a Gem::Geneva::infoMode item from a stream
 *
 * @param i The stream the item should be read from
 * @param im The item read from the stream
 * @return The std::istream object used to read the item from
 */
std::istream& operator>>(std::istream& i, Gem::Geneva::infoMode& im) {
	boost::uint16_t tmp;
	i >> tmp;

#ifdef DEBUG
	im = boost::numeric_cast<Gem::Geneva::infoMode>(tmp);
#else
	im = static_cast<Gem::Geneva::infoMode>(tmp);
#endif /* DEBUG */

	return i;
}

/*********************************************************************/
/**
 * Puts a Gem::Geneva::adaptorId item into a stream
 *
 * @param o The ostream the item should be added to
 * @param aid the item to be added to the stream
 * @return The std::ostream object used to add the item to
 */
std::ostream& operator<<(std::ostream& o, const Gem::Geneva::adaptorId& aid) {
	boost::uint16_t tmp = static_cast<boost::uint16_t>(aid);
	o << tmp;
	return o;
}

/*********************************************************************/
/**
 * Reads a Gem::Geneva::adaptorId item from a stream
 *
 * @param i The stream the item should be read from
 * @param aid The item read from the stream
 * @return The std::istream object used to read the item from
 */
std::istream& operator>>(std::istream& i, Gem::Geneva::adaptorId& aid) {
	boost::uint16_t tmp;
	i >> tmp;

#ifdef DEBUG
	aid = boost::numeric_cast<Gem::Geneva::adaptorId>(tmp);
#else
	aid = static_cast<Gem::Geneva::adaptorId>(tmp);
#endif /* DEBUG */

	return i;
}

/*********************************************************************/
/**
 * Puts a Gem::Geneva::sortingMode item into a stream
 *
 * @param o The ostream the item should be added to
 * @param smode the item to be added to the stream
 * @return The std::ostream object used to add the item to
 */
std::ostream& operator<<(std::ostream& o, const Gem::Geneva::sortingMode& smode) {
	boost::uint16_t tmp = static_cast<boost::uint16_t>(smode);
	o << tmp;
	return o;
}

/*********************************************************************/
/**
 * Reads a Gem::Geneva::sortingMode item from a stream
 *
 * @param i The stream the item should be read from
 * @param smode The item read from the stream
 * @return The std::istream object used to read the item from
 */
std::istream& operator>>(std::istream& i, Gem::Geneva::sortingMode& smode) {
	boost::uint16_t tmp;
	i >> tmp;

#ifdef DEBUG
	smode = boost::numeric_cast<Gem::Geneva::sortingMode>(tmp);
#else
	smode = static_cast<Gem::Geneva::sortingMode>(tmp);
#endif /* DEBUG */

	return i;
}

/*********************************************************************/
/**
 * Puts a Gem::Geneva::personality item into a stream
 *
 * @param o The ostream the item should be added to
 * @param pers the item to be added to the stream
 * @return The std::ostream object used to add the item to
 */
std::ostream& operator<<(std::ostream& o, const Gem::Geneva::personality& pers) {
	boost::uint16_t tmp = static_cast<boost::uint16_t>(pers);
	o << tmp;
	return o;
}

/*********************************************************************/
/**
 * Reads a Gem::Geneva::personality item from a stream
 *
 * @param i The stream the item should be read from
 * @param pers The item read from the stream
 * @return The std::istream object used to read the item from
 */
std::istream& operator>>(std::istream& i, Gem::Geneva::personality& pers) {
	boost::uint16_t tmp;
	i >> tmp;

#ifdef DEBUG
	pers = boost::numeric_cast<Gem::Geneva::personality>(tmp);
#else
	pers = static_cast<Gem::Geneva::personality>(tmp);
#endif /* DEBUG */

	return i;
}

/*********************************************************************/

} /* namespace Geneva */
} /* namespace Gem */