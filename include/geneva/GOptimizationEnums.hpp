/**
 * @file GOptimizationEnums.hpp
 */

/*
 * Copyright (C) Authors of the Geneva library collection and Karlsruhe
 * Institute of Technology (University of the State of Baden-Wuerttemberg
 * and National Laboratory of the Helmholtz Association).
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Geneva library collection
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
#include <ostream>
#include <istream>

// Includes check for correct Boost version(s)
#include "common/GGlobalDefines.hpp"

// Boost headers go here

#include <boost/cstdint.hpp>

#ifndef GOPTIMIZATIONENUMS_HPP_
#define GOPTIMIZATIONENUMS_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "common/GCommonEnums.hpp"

namespace Gem {
namespace Geneva {

/**********************************************************************************************/
/**
 * The two const variables MAXIMIZE and MINIMIZE determine, whether the library
 * should work in maximization or minimization mode.
 */
const bool MAXIMIZE = true;
const bool MINIMIZE = false;

/**********************************************************************************************/
/**
 * The number of iterations after which information should be
 * emitted about the inner state of the optimization algorithm.
 */
const boost::uint32_t DEFAULTREPORTITER = 1;

/**********************************************************************************************/
/**
 * The number of iterations after which a checkpoint should be written.
 * 0 means that no checkpoints are written at all.
 */
const boost::uint32_t DEFAULTCHECKPOINTIT = 0;

/**********************************************************************************************/
/**
 * The default maximum number of iterations
 */
const boost::uint32_t DEFAULTMAXIT = 1000;

/**********************************************************************************************/
/**
 * The default maximum number of iterations without improvement. 0 means: ignore
 */
const boost::uint32_t DEFAULMAXTSTALLIT = 0;

/**********************************************************************************************/
/**
 * The default maximization mode
 */
const bool DEFAULTMAXMODE = false; // means: "minimization"

/**********************************************************************************************/
/**
 * A 0 time period . timedHalt will not trigger if this duration is set
 */
const std::string EMPTYDURATION = "00:00:00.000"; // 0 - no duration

/**********************************************************************************************/
/**
 * The default maximum duration of the calculation.
 */
const std::string DEFAULTDURATION = EMPTYDURATION;

/**********************************************************************************************/
/**
 * The default quality threshold
 */
const double DEFAULTQUALITYTHRESHOLD=0.;

/**********************************************************************************************/
/**
 * Currently three types of recombination schemes are supported:
 * - DEFAULTRECOMBINE defaults to RANDOMRECOMBINE
 * - RANDOMRECOMBINE chooses the parents to be replicated randomly from all parents
 * - VALUERECOMBINE prefers parents with a higher fitness
 */
enum recoScheme {
	  DEFAULTRECOMBINE = 0
	, RANDOMRECOMBINE = 1
	, VALUERECOMBINE = 2
};

/**********************************************************************************************/
/**
 * The info function can be called in these three modes
 */
enum infoMode {
	  INFOINIT
	, INFOPROCESSING
	, INFOEND
};

/**********************************************************************************************/
/**
 * Ids that are assigned to adaptors and which should (by convention!) be unique for these
 */
enum adaptorId {
	  GIDENTITYADAPTOR
	, GSWARMADAPTOR
	, GDOUBLEGAUSSADAPTOR
	, GINT32GAUSSADAPTOR
	, GBOOLEANADAPTOR
	, GINT32FLIPADAPTOR
};

/**********************************************************************************************/
/**
 * The selection mode in populations. MUPLUSNU means that new parents are selected from old
 * parents and their children. MUCOMMNU means that new parents are selected from children only.
 * MUNU1PRETAIN means that the best parent of the last generation will also become a new parent
 * (unless a better child was found). All other parents are selected from children only.
 */
enum sortingMode {
	  MUPLUSNU = 0
	, MUCOMMANU = 1
	, MUNU1PRETAIN = 2
};

/**********************************************************************************************/
/**
 * Different optimization algorithms need to assign different information to individuals. They
 * can thus assume different personalities, resulting in different data structures to be stored
 * in them (see the GPersonalityTraits classes).
 */
enum personality {
	  NONE
	, EA
	, GD
	, SWARM
};

/**********************************************************************************************/
/**
 * The default value for the GEvolutionaryAlgorithm::markOldParents_ flag. This is used mostly
 * for logging purposes. If set, the algorithm will keep a copy of the parents from which the
 * children originated and will mark their id in the individual's personality traits.
 */
const bool DEFAULTMARKOLDPARENTS = false;

/**********************************************************************************************/

const double DEFAULTSIGMA = 1; ///< Default start value for sigma_
const double DEFAULTSIGMASIGMA = 0.001; ///< Default width of the gaussian used for sigma adaption
const double DEFAULTMINSIGMA = 0.0000001; ///< Default minimum allowed value for sigma_
const double DEFAULTMAXSIGMA = 5; ///< Default maximum allowed value for sigma_
const double DEFAULTBITADPROB = 0.05; // 5 percent adaption probability for bits
const double DEFAULTADPROB = 1.0; // 100 percent adaption probability for all other cases

/**********************************************************************************************/

const double DEFAULTCLOCAL = 2.; ///< Default multiplier for local distances (swarm)
const double DEFAULTCGLOBAL = 2.; ///< Default multiplier for global distances (swarm)
const double DEFAULTCDELTA = 0.4; ///< Default multiplier for deltas (swarm)
const double CLOCALRANGEDISABLED = -1.; ///< A value < 0 means that the local range is disabled
const double CGLOBALRANGEDISABLED = -1.; ///< A value < 0 means that the global range is disabled
const double CDELTARANGEDISABLED = -1.; ///< A value < 0 means that the delta range is disabled

/**********************************************************************************************/
/**
 * Specifies different update rules in swarms
 */
enum updateRule {
	  LINEAR
	, CLASSIC
};

/**********************************************************************************************/

const updateRule DEFAULTUPDATERULE = CLASSIC; ///< The default update rule in swarms

/**********************************************************************************************/
/**
 * The default factor applied to the turn-around time
 * of the first individual in the first generation. Used to
 * find a suitable timeout-value for following individuals.
 * Used in conjunction with optimization algorithms that
 * communicate via the "courtier" broker infrastructure.
 */
const boost::uint32_t DEFAULTBROKERWAITFACTOR = 20;

/**********************************************************************************************/
/**
 * The default allowed time in seconds for the first individual
 * in generation 0 to return. Set it to 0 to disable this timeout.
 * Used in conjunction with optimization algorithms that
 * communicate via the "courtier" broker infrastructure.
 */
const std::string DEFAULTBROKERFIRSTTIMEOUT = EMPTYDURATION;

/**********************************************************************************************/

/** @brief Puts a Gem::Geneva::recoScheme into a stream. Needed also for boost::lexical_cast<> */
std::ostream& operator<<(std::ostream&, const Gem::Geneva::recoScheme&);

/** @brief Reads a Gem::Geneva::recoScheme item from a stream. Needed also for boost::lexical_cast<> */
std::istream& operator>>(std::istream&, Gem::Geneva::recoScheme&);

/** @brief Puts a Gem::Geneva::infoMode into a stream. Needed also for boost::lexical_cast<> */
std::ostream& operator<<(std::ostream&, const Gem::Geneva::infoMode&);

/** @brief Reads a Gem::Geneva::infoMode item from a stream. Needed also for boost::lexical_cast<> */
std::istream& operator>>(std::istream&, Gem::Geneva::infoMode&);

/** @brief Puts a Gem::Geneva::adaptorId into a stream. Needed also for boost::lexical_cast<> */
std::ostream& operator<<(std::ostream&, const Gem::Geneva::adaptorId&);

/** @brief Reads a Gem::Geneva::adaptorId item from a stream. Needed also for boost::lexical_cast<> */
std::istream& operator>>(std::istream&, Gem::Geneva::adaptorId&);

/** @brief Puts a Gem::Geneva::sortingMode into a stream. Needed also for boost::lexical_cast<> */
std::ostream& operator<<(std::ostream&, const Gem::Geneva::sortingMode&);

/** @brief Reads a Gem::Geneva::sortingMode from a stream. Needed also for boost::lexical_cast<> */
std::istream& operator>>(std::istream&, Gem::Geneva::sortingMode&);

/** @brief Puts a Gem::Geneva::personality into a stream. Needed also for boost::lexical_cast<> */
std::ostream& operator<<(std::ostream&, const Gem::Geneva::personality&);

/** @brief Reads a Gem::Geneva::personality from a stream. Needed also for boost::lexical_cast<> */
std::istream& operator>>(std::istream&, Gem::Geneva::personality&);

/** @brief Puts a Gem::Geneva::updateRule into a stream. Needed also for boost::lexical_cast<> */
std::ostream& operator<<(std::ostream&, const Gem::Geneva::updateRule&);

/** @brief Reads a Gem::Geneva::updateRule from a stream. Needed also for boost::lexical_cast<> */
std::istream& operator>>(std::istream&, Gem::Geneva::updateRule&);

/**********************************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

#endif /* GOPTIMIZATIONENUMS_HPP_ */
