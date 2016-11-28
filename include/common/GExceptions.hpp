/**
 * @file GExceptions.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * This file is part of the Geneva library collection.
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard header files go here
#include <string>
#include <sstream>
#include <exception>

// Boost header files go here

#ifndef GEXCEPTIONS_HPP_
#define GEXCEPTIONS_HPP_

// Geneva header files go here

namespace Gem {
namespace Common {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
// Exceptions and related definitions

/**
 * General error class to be thrown in the case of severe errors
 */
class gemfony_error_condition : public std::exception
{
public:
	 /** @brief The standard constructor */
	 G_API_COMMON gemfony_error_condition(const std::string&) throw();
	 /** @brief The copy constructor */
	 G_API_COMMON gemfony_error_condition(const gemfony_error_condition&) throw();
	 /** @brief The destructor */
	 virtual G_API_COMMON ~gemfony_error_condition() throw();

	 /** @brief Allows to add further information to the exception */
	 virtual G_API_COMMON void add(const std::string&) throw();
	 /** @brief Allows to add further informtion, automatically terminated through a '\n' */
	 G_API_COMMON gemfony_error_condition& operator()(const std::string&) throw();

	 /** @brief Emits information when thrown */
	 virtual G_API_COMMON const char* what() const throw();
	 /** @brief Allows to check whether any information is stored in this class */
	 G_API_COMMON bool empty() const throw();

protected:
	 /** @brief The default constructor: Intentionally private and undefined */
	 gemfony_error_condition() throw() = delete;

	 // Data
	 std::string m_description; ///< Holds the actual error description
};

/******************************************************************************/
/** @brief This function allows to output a gemfony_error_condition to a stream */
G_API_COMMON std::ostream& operator<<(std::ostream&, const Gem::Common::gemfony_error_condition&);

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An exception to be thrown in case of an expectation violation. This is used
 * in the compare infrastructure, but listed here to resolve a circular dependency.
 */
class g_expectation_violation
	: public gemfony_error_condition
{
public:
	 /** @brief The standard constructor */
	 G_API_COMMON g_expectation_violation(const std::string &) throw();

	 /** @brief The destructor */
	 virtual G_API_COMMON ~g_expectation_violation() throw();

	 /** @brief Allows to add further informtion, automatically terminated through a '\n' */
	 G_API_COMMON g_expectation_violation &operator()(const std::string &) throw();

protected:
	 /** @brief The default constructor: Intentionally private and undefined */
	 g_expectation_violation() = delete;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

/******************************************************************************/
/**
 * This define allows easy access to throwing exceptions.
 */
#define raiseException(E)                                                                                \
  {                                                                                                      \
    std::ostringstream error;                                                                            \
    error                                                                                                \
       << std::endl                                                                                      \
       << "================================================" << std::endl                                \
       << "ERROR" << std::endl                                                                           \
       << "in file " << __FILE__ << std::endl                                                            \
       << "near line " << __LINE__ << " with description:" << std::endl                                  \
       << std::endl                                                                                      \
       << E << std::endl                                                                                 \
       << std::endl                                                                                      \
       << "If you suspect that this error is due to Geneva," << std::endl                                \
       << "then please consider filing a bug via" << std::endl                                           \
       << "http://www.gemfony.eu (link \"Bug Reports\") or" << std::endl                                 \
       << "through http://www.launchpad.net/geneva" << std::endl                                         \
       << std::endl                                                                                      \
       << "We appreciate your help!" << std::endl                                                        \
       << "The Geneva team" << std::endl                                                                 \
       << "================================================" << std::endl;                               \
    throw(Gem::Common::gemfony_error_condition(error.str()));                                            \
  }                                                                                                      \

/******************************************************************************/
/** @brief Raise an exception if a given define wasn't set */
G_API_COMMON void condnotset(const std::string&, const std::string&);

/******************************************************************************/

#endif /* GEXCEPTIONS_HPP_ */
