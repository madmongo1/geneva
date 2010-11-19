/**
 * @file GParserBuilder.hpp
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
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost headers go here
#include <boost/thread/thread.hpp>
#include <boost/utility.hpp>
#include <boost/program_options.hpp>
#include <boost/any.hpp>

/**
 * Check that we have support for threads. This collection of classes is useless
 * without this.
 */
#ifndef BOOST_HAS_THREADS
#error "Error: No support for Boost.threads available"
#endif

#ifndef GPARSERBUILDER_HPP_
#define GPARSERBUILDER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

namespace Gem {
namespace Common {

/************************************************************************/
/**
 * This class implements a "parser builder", that allows to easily specify
 * the options that the parser should search for in a configuration file.
 * Results of the parsing process will be written directly into the supplied
 * variables. If not found, a default value will be used. Note that this
 * class assumes that the parameter_type can be streamed.
 */
class GParserBuilder
	:boost::noncopyable
{
public:
	/** @brief The standard constructor */
	explicit GParserBuilder(const std::string&);

	/** @brief Tries to parse a given configuration file for a set of options */
	bool parse();

	/********************************************************************/
	/**
	 * Adds a parameter with a specific type to the collection
	 *
	 * @param optionName The name of the option
	 * @param parameter The parameter into which the value will be written
	 * @param default_value The value to be used if the corresponding parameter was not found in the configuration file
	 */
	template <typename parameter_type>
	void registerParameter(
			const std::string& optionName
			, parameter_type& parameter
			, const parameter_type& def_val
	) {
	    namespace po = boost::program_options;

		config_.add_options()
				(optionName.c_str(), po::value<parameter_type>(&parameter)->default_value(def_val))
		;

		variables_.push_back(optionName);
	}

	/********************************************************************/
private:
	/** @brief The default constructor. Intentionally private and undefined */
	GParserBuilder();

	std::string configurationFile_; ///< The name and path of the configuration file to be parsed
	boost::program_options::options_description config_; ///< Will hold a description of the program options
	std::vector<std::string> variables_; ///< The names of the variables to be parsed
};

} /* namespace Common */
} /* namespace Gem */

#endif /* GPARSERBUILDER_HPP_ */