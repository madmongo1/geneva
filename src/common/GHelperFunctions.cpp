/**
 * @file GHelperFunctions.cpp
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

#include "common/GHelperFunctions.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * This function retrieves the number of CPU cores on the system, which is regarded as a suitable
 * default number for the amount of threads of a specific kind. The function is thread-safe. When called
 * multiple times, it will read the number of hardware threads from a local cache.
 *
 * @param defaultNThreads The default number of threads to be returned if hardware concurrency cannot be determined
 * @return A guess at a suitable number of threads for this architecture, used as a fallback
 */

std::mutex g_hwt_read_mutex;
std::atomic<bool> g_hwt_read(false); // global in this file
std::atomic<unsigned int> g_nHardwareThreads(Gem::Common::DEFAULTNHARDWARETHREADS); // global in this file

unsigned int getNHardwareThreads(const unsigned int &defaultNThreads) {
	if (false == g_hwt_read.load()) {
		std::unique_lock<std::mutex>(g_hwt_read_mutex);
		if (false == g_hwt_read.load()) {
			g_hwt_read = true;
			g_nHardwareThreads.store(std::thread::hardware_concurrency());
		}
	} // exclusive access ends

	if (g_nHardwareThreads.load() > 0) {
		return g_nHardwareThreads;
	} else {
		if(0==defaultNThreads) {
#ifdef DEBUG
			glogger
				<< "Could not get information regarding suitable number of threads." << std::endl
				<< "Using the default value DEFAULTNHARDWARETHREADS = " << Gem::Common::DEFAULTNHARDWARETHREADS << " instead." << std::endl
				<< GWARNING;
#endif

			return Gem::Common::DEFAULTNHARDWARETHREADS;
		} else {
#ifdef DEBUG
			glogger
				<< "Could not get information regarding suitable number of threads." << std::endl
				<< "Using the default value defaultNThreads = " << defaultNThreads << " instead." << std::endl
				<< GWARNING;
#endif

			return defaultNThreads;
		}
	}
}

/******************************************************************************/
/**
 * This function loads textual (ASCII) data from an external file. Note that this function is
 * not currently meant to be used with binary data, although it should work with that as well
 * (at least in theory -- this is untested). There is no check whether the data loaded represents
 * a text.
 *
 * @param p The name of the file to be loaded
 * @return The data contained in the file
 */
std::string loadTextDataFromFile(const boost::filesystem::path &p) {
	// Check that the file exists
	if (!boost::filesystem::exists(p)) {
		glogger
		<< "In loadTextDataFromFile(): Error!" << std::endl
		<< "Tried to load data from file " << p.string() << std::endl
		<< "which does not exist" << std::endl
		<< GEXCEPTION;
	}

	boost::filesystem::ifstream sourceFileStream(p);
	std::string sourceFile(
		std::istreambuf_iterator<char>(sourceFileStream), (std::istreambuf_iterator<char>())
	);
	return sourceFile;
}

/******************************************************************************/
/**
 * Execute an external command, reacting to possible errors.
 *
 * @param command The command to be executed (possibly including errors)
 * @param arguments The list of arguments to be added to the command
 * @param commandOutputFileName The name of a file to which information should be piped
 * @param fullCommand Allows the caller to find out about the full command
 * @return The error code
 */
int runExternalCommand(
	const boost::filesystem::path &program, const std::vector<std::string> &arguments,
	const boost::filesystem::path &commandOutputFileName, std::string &fullCommand
) {
	// Convert slashes to backslashes on Windows
	boost::filesystem::path p_program = program;
	std::string localCommand = (p_program.make_preferred()).string();

	// Add command line arguments
	std::vector<std::string>::const_iterator cit;
	for (cit = arguments.begin(); cit != arguments.end(); ++cit) {
		localCommand += (std::string(" ") + *cit);
	}

	// If requested by the user, we want to send the command to an external file
	if (boost::filesystem::path() != commandOutputFileName) {
		boost::filesystem::path p_commandOutputFileName = commandOutputFileName;
		std::string localcommandOutputFileName = (p_commandOutputFileName.make_preferred()).string();

		localCommand =
			std::string("(") + localCommand + std::string(") > ") + localcommandOutputFileName + std::string(" 2>&1");
	}

	// MOstly for external debugging
#ifdef GEM_COMMON_PRINT_COMMANDLINE
   std::cout << "Executing external command \"" << localCommand << "\" ...";
#endif /* GEM_COMMON_PRINT_COMMANDLINE */

	// Assign the full command (mostly needed for external error-evaluation)
	fullCommand = localCommand;

	// Run the actual command. T
	int errorCode = system(localCommand.c_str());

#ifdef GEM_COMMON_PRINT_COMMANDLINE
   std::cout << "... done." << std::endl;
#endif /* GEM_COMMON_PRINT_COMMANDLINE */

	// The error code will be returned as the function valiue
	return errorCode;
}

/******************************************************************************/
/**
 * Returns a string for a given serialization mode
 *
 * @param s The serialization mode which should be translated to a string
 * @return A string for a given serialization mode
 */
std::string serializationModeToString(const serializationMode &s) {
	switch (s) {
		case Gem::Common::serializationMode::SERIALIZATIONMODE_TEXT:
			return std::string("text mode");
			break;
		case Gem::Common::serializationMode::SERIALIZATIONMODE_XML:
			return std::string("XML mode");
			break;
		case Gem::Common::serializationMode::SERIALIZATIONMODE_BINARY:
			return std::string("binary mode");
			break;
		default: {
			glogger
			<< "In serializationModeToString(): Error!" << std::endl
			<< "Incorrect serialization mode requested: " << s << std::endl
			<< GEXCEPTION;
			break;
		}
	}

	// Make the compiler happy
	return std::string("");
}

/******************************************************************************/
/**
 * Splits a string into a vector of strings, according to a seperator character.
 * Any trailing or leading white spaces are removed from the result strings.
 *
 * @param str The string to be split
 * @param sep The separator character
 * @return A std::vector holding the fragments
 */
std::vector<std::string> splitString(const std::string &str, const char *sep) {
	std::vector<std::string> result;

#ifdef DEBUG
   if(1 != std::string(sep).size()) {
      glogger
      << "In splitString(): Error!" << std::endl
      << "Supplied separator \"" << sep << "\" has invalid size " << std::string(sep).size() << std::endl
      << GEXCEPTION;
   }
#endif /* DEBUG */

	typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
	boost::char_separator<char> sep_char(sep);
	tokenizer oaTokenizer(str, sep_char);
	for (tokenizer::iterator oa = oaTokenizer.begin(); oa != oaTokenizer.end(); ++oa) {
		std::string frag = *oa;
		boost::trim(frag); // Remove any leading or trailing white spaces
		if (frag.empty()) continue; // Ignore empty strings
		result.push_back(frag);
	}

	return result;
}

/******************************************************************************/
/**
 * Splits a string into a vector of unsigned int, if possible, or throws
 * an exception. The list must at least contain one entry and must be
 * comma-separated.
 */
std::vector<unsigned int> stringToUIntVec(const std::string &raw) {
	using namespace boost::spirit;

	std::vector<unsigned int> result;
	bool success = false;

	std::string::const_iterator from = raw.begin();
	std::string::const_iterator to = raw.end();

	// Do the actual parsing
	success = qi::phrase_parse(
		from, to, (uint_ % ','), qi::space, result
	);

	if (from != to || !success) {
		std::string rest(from, to);
		glogger
		<< "In stringToUIntVec(const std::string& raw): Error!" << std::endl
		<< "Parsing failed." << std::endl
		<< "Stopped at: \": " << rest << "\"" << std::endl
		<< GEXCEPTION;
	}

	return result;
}

/******************************************************************************/
/**
 * Splits a string into a vector of double values, if possible, or throws
 * an exception. The list must at least contain one entry and must be
 * comma-separated.
 */
std::vector<double> stringToDoubleVec(const std::string &raw) {
	using namespace boost::spirit;

	std::vector<double> result;
	bool success = false;

	std::string::const_iterator from = raw.begin();
	std::string::const_iterator to = raw.end();

	// Do the actual parsing
	success = qi::phrase_parse(
		from, to, (double_ % ','), qi::space, result
	);

	if (from != to || !success) {
		std::string rest(from, to);
		glogger
		<< "In stringToDoubleVec(const std::string& raw): Error!" << std::endl
		<< "Parsing failed." << std::endl
		<< "Stopped at: \": " << rest << "\"" << std::endl
		<< GEXCEPTION;
	}

	return result;
}


/******************************************************************************/
/**
 * Splits a string into a vector of unsigned int-tuples, if possible, or
 * throws an exception. The string should have the form "(1,2), (3,4)" etc.
 */
std::vector<std::tuple<unsigned int, unsigned int>> stringToUIntTupleVec(const std::string &raw) {
	using namespace boost::spirit;

	typedef std::string::const_iterator cit_type;
	typedef std::vector<std::tuple<unsigned int, unsigned int>> res_type;

	std::vector<std::tuple<unsigned int, unsigned int>> result;
	bool success = false;

	std::string::const_iterator from = raw.begin();
	std::string::const_iterator to = raw.end();

	// Do the actual parsing
	success = qi::phrase_parse(
		from, to, (('(' >> uint_ >> ',' >> uint_ >> ')') % ','), qi::space, result
	);

	if (from != to || !success) {
		std::string rest(from, to);
		glogger
		<< "In stringToUIntTupleVec(const std::string& raw): Error!" << std::endl
		<< "Parsing failed." << std::endl
		<< "Stopped at: \"" << rest << "\"" << std::endl
		<< GEXCEPTION;
	}

	return result;
}

/******************************************************************************/


} /* namespace Common */
} /* namespace Gem */
