/**
 * @file GCommonEnums.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * This file is part of the Geneva library collection.
 *
 * The following license applies to the code in this file:
 *
 * ***************************************************************************
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * ***************************************************************************
 *
 * NOTE THAT THE BOOST-LICENSE DOES NOT APPLY TO ANY OTHER FILES OF THE
 * GENEVA LIBRARY, UNLESS THIS IS EXPLICITLY STATED IN THE CORRESPONDING FILE!
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) eu
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Boost Software License for more details.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.eu .
 */

#pragma once

#include "common/GGlobalDefines.hpp"

// Standard headers go here
#include <string>
#include <istream>
#include <ostream>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cmath>

// Boost headers go here

#include <boost/cast.hpp>

// Geneva headers go here

/******************************************************************************/
/**
 * We need local signals, so we can act both on Windows and POSIX-OSs
 */
#if defined(_MSC_VER)  && (_MSC_VER >= 1020)
#define G_SIGHUP CTRL_CLOSE_EVENT
#else
#define G_SIGHUP SIGHUP
#endif
/******************************************************************************/

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * By default GBoundedBufferT will have this size. As the buffer
 * dynamically grows and shrinks, we choose a very high value. This
 * is a safeguard against errors like endless loops that might keep
 * filling the buffer until memory is exhausted. In normal work
 * conditions, however, the buffer should never reach its upper
 * limit.
 */
#ifdef GENEVA_COMMON_DEFAULT_BUFFER_SIZE
	const std::size_t DEFAULTBUFFERSIZE = GENEVA_COMMON_DEFAULT_BUFFER_SIZE;
#else
	const std::size_t DEFAULTBUFFERSIZE = 5000;
#endif

/******************************************************************************/
// A basetype used for all enums in Geneva
using ENUMBASETYPE = std::uint16_t;

/******************************************************************************/
/**
 * This enum denotes different dimensions (used particularly by GDecoratorCollection
 */
enum class dimensions : Gem::Common::ENUMBASETYPE {
	Dim1 = 0
	, Dim2 = 1
	, Dim3 = 2
	, Dim4 = 3
};

/** @brief Puts a Gem::Common::dimensions into a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::ostream &operator<<(std::ostream &, const Gem::Common::dimensions &);

/** @brief Reads a Gem::Common::dimensions item from a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::istream &operator>>(std::istream &, Gem::Common::dimensions &);

/******************************************************************************/

/******************************************************************************/
/**
 * The limit for similarity checks of floating point numbers
 */
const double CE_DEF_SIMILARITY_DIFFERENCE = pow(10., -5);

/******************************************************************************/
/**
 * Indicates whether higher or lower values are considered better. Needed e.g.
 * in conjunction with the sorting in priority queues.
 */
enum class sortOrder : Gem::Common::ENUMBASETYPE {
	 LOWERISBETTER = 0
	 , HIGHERISBETTER = 1
};

/** @brief Puts a Gem::Common::sortOrder into a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::ostream &operator<<(std::ostream &, const Gem::Common::sortOrder &);

/** @brief Reads a Gem::Common::sortOrder item from a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::istream &operator>>(std::istream &, Gem::Common::sortOrder &);

/******************************************************************************/
/**
 * Different log and exception types
 */
enum class logType : Gem::Common::ENUMBASETYPE {
	EXCEPTION = 0
	, TERMINATION = 1
	, WARNING = 2
	, LOGGING = 3
	, FILE = 4
	, STDOUT = 5
	, STDERR = 6
};


/** @brief Puts a Gem::Common::logType into a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::ostream &operator<<(std::ostream &, const Gem::Common::logType &);

/** @brief Reads a Gem::Common::logType item from a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::istream &operator>>(std::istream &, Gem::Common::logType &);

/******************************************************************************/
/**
 * The default number of bins in histograms, used in GPlotDesigner
 */
const std::size_t DEFAULTNBINSGPD = 100;

/******************************************************************************/
/**
 * Used in parameter definitions (GParserBuilder)
 */
const bool VAR_IS_ESSENTIAL = true;
const bool VAR_IS_SECONDARY = false;

/******************************************************************************/
/**
 * Needed for the serialization of boost::logic::tribool
 */
enum class triboolStates : Gem::Common::ENUMBASETYPE {
	TBS_FALSE
	, TBS_INDETERMINATE
	, TBS_TRUE
};

/** @brief Puts a Gem::Common::triboolStates into a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::ostream &operator<<(std::ostream &, const Gem::Common::triboolStates &);

/** @brief Reads a Gem::Common::triboolStates item from a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::istream &operator>>(std::istream &, Gem::Common::triboolStates &);

/******************************************************************************/
/**
 * The serialization modes that are currently allowed
 */
enum class serializationMode : Gem::Common::ENUMBASETYPE {
	TEXT = 0
	, XML = 1
	, BINARY = 2
};

/** @brief Puts a Gem::Common::serializationMode into a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::ostream &operator<<(std::ostream &, const Gem::Common::serializationMode &);

/** @brief Reads a Gem::Common::serializationMode item from a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::istream &operator>>(std::istream &, Gem::Common::serializationMode &);

/** @brief Converts a serializationMode to a string representation for debugging purposes */
std::string serModeToString(Gem::Common::serializationMode);

/******************************************************************************/
/**
 * Specification of whether checkExpectation should emit messages
 */
const bool CE_SILENT = false;
const bool CE_WITH_MESSAGES = true;

/******************************************************************************/
/**
 * Needed to express expectations in testing framework. CE stands for "Check expectation".
 */
enum class expectation : Gem::Common::ENUMBASETYPE {
	CE_EQUALITY = 0 // bitwise equality of all checked components
	, CE_FP_SIMILARITY = 1 // equality for non-floating point components, similarity for floating point
	, CE_INEQUALITY = 2// at least one checked component differs
};

/** @brief Puts a Gem::Common::expectation into a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::ostream &operator<<(std::ostream &, const Gem::Common::expectation &);

/** @brief Reads a Gem::Common::expectation item from a stream. Needed also for boost::lexical_cast<> */
G_API_COMMON std::istream &operator>>(std::istream &, Gem::Common::expectation &);

/******************************************************************************/
/**
 * Specification of the default number of threads, when no information about hardware
 * concurrency can be determined.
 */
const unsigned int DEFAULTNHARDWARETHREADS = 2;
/**
 * Specification of the default maximum number of threads
 */
const unsigned int DEFAULTMAXNHARDWARETHREADS = 4;

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */
