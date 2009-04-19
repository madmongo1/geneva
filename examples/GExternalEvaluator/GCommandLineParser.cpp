/**
  * @file GCommandLineParser.cpp
  */

/* Copyright (C) 2004-2008 Dr. Ruediger Berlich
 * Copyright (C) 2008 Forschungszentrum Karlsruhe GmbH
 *
 * This file is part of the Geneva library.
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
 */

#include "GCommandLineParser.hpp"

namespace Gem
{
namespace GenEvA
{

/************************************************************************************************/
/**
 * A function that parses the command line for all required parameters
 */
bool parseCommandLine(int argc, char **argv,
					  std::string& program,
					  std::string& externalArguments,
					  std::size_t& popSize,
					  std::size_t& nParents,
					  boost::uint32_t& adaptionThreshold,
					  boost::uint16_t& nProducerThreads,
					  boost::uint32_t& maxGenerations,
					  long& maxMinutes,
					  boost::uint32_t& reportGeneration,
					  recoScheme& rScheme,
					  boost::uint16_t& parallel,
					  bool& serverMode,
					  std::string& ip,
					  unsigned short& port,
					  double& sigma,
					  double& sigmaSigma,
					  double& minSigma,
					  double& maxSigma,
					  boost::uint32_t& nEvaluations,
					  Gem::GenEvA::dataExchangeMode& exchangeMode,
					  bool&sortingScheme,
					  bool& verbose)
{
	boost::uint16_t recombinationScheme=0;

	try{
		// Check the command line options. Uses the Boost program options library.
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "emit help message")
			("program,P",po::value<std::string>(&program)->default_value(DEFAULTPROGRAM),
					"the name of a file holding the evaluation executable")
			("externalArguments,e",po::value<std::string>(&externalArguments)->default_value(DEFAULTEXTERNALARGUMENTS),
					"Arguments to be handed to programs called through the \"system()\" call")
			("popSize,z",po::value<std::size_t>(&popSize)->default_value(DEFAULTPOPSIZE),
					"The envisaged size of the population")
			("nParents,Z",po::value<std::size_t>(&nParents)->default_value(DEFAULTNPARENTS),
					"The envisaged number of parents")
			("adaptionThreshold,a", po::value<boost::uint32_t>(&adaptionThreshold)->default_value(DEFAULTADAPTIONTHRESHOLD),
					"Number of calls to mutate after which mutation parameters should be adapted")
			("nProducerThreads,n",po::value<boost::uint16_t>(&nProducerThreads)->default_value(DEFAULTNPRODUCERTHREADS),
					"The amount of random number producer threads")
			("maxGenerations,G", po::value<boost::uint32_t>(&maxGenerations)->default_value(DEFAULTMAXGENERATIONS),
					"maximum number of generations in the population")
			("maxMinutes,X", po::value<long>(&maxMinutes)->default_value(DEFAULTMAXMINUTES),
					"The maximum number of minutes the optimization of the population should run")
			("reportGeneration,R",po::value<boost::uint32_t>(&reportGeneration)->default_value(DEFAULTREPORTGENERATION),
					"The number of generations after which information should be emitted in the super-population")
			("rScheme,E",po::value<boost::uint16_t>(&recombinationScheme)->default_value(DEFAULTRSCHEME),
					"The recombination scheme for the super-population")
			("parallel,p", po::value<boost::uint16_t>(&parallel)->default_value(DEFAULTPARALLEL),
					"Whether or not to run this optimization in serial (0), multi-threaded (1) or networked (2) mode")
			("serverMode,d","Whether to run networked execution in server or client mode. The option only gets evaluated if \"--parallel=2\"")
			("ip",po::value<std::string>(&ip)->default_value(DEFAULTIP), "The ip of the server")
			("port",po::value<unsigned short>(&port)->default_value(DEFAULTPORT), "The port of the server")
			("sigma,s", po::value<double>(&sigma)->default_value(DEFAULTSIGMA),
					"The width of the gaussian used for the adaption of double values")
			("sigmaSigma,S", po::value<double>(&sigmaSigma)->default_value(DEFAULTSIGMASIGMA),
					"The adaption rate of sigma")
			("minSigma,m", po::value<double>(&minSigma)->default_value(DEFAULTMINSIGMA),
					"The minimum allowed value for sigma")
			("maxSigma,M", po::value<double>(&maxSigma)->default_value(DEFAULTMAXSIGMA),
					"The maximum allowed value for sigma")
			("nEvaluations,V", po::value<boost::uint32_t>(&nEvaluations)->default_value(DEFAULTNEVALUATIONS),
					"The amount of evaluations each external program shall perform")
			("exchangeMode,x", po::value<Gem::GenEvA::dataExchangeMode>(&exchangeMode)->default_value(DEFAULTEXCHANGEMODE),
					"Determines whether data exchange should be done in binary mode (0) or in text mode(1)")
			("sortingScheme,o", po::value<bool>(&sortingScheme)->default_value(DEFAULTSORTINGSCHEME),
					"Determines whether sorting is done in MUPLUSNU or MUCOMMANU mode")
			("verbose,v",po::value<bool>(&verbose)->default_value(DEFAULTVERBOSE),
					"Whether additional information should be emitted")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		// Emit a help message, if necessary
		if (vm.count("help")) {
			 std::cout << desc << std::endl;
			 return false;
		}

		// Workaround for assigment problem with rScheme
		if(recombinationScheme==(boost::uint16_t)VALUERECOMBINE)
			rScheme=VALUERECOMBINE;
		else if(recombinationScheme==(boost::uint16_t)RANDOMRECOMBINE)
			rScheme=RANDOMRECOMBINE;
		else if(recombinationScheme==(boost::uint16_t)DEFAULTRECOMBINE)
			rScheme=DEFAULTRECOMBINE;
		else {
			std::cout << "Error: Invalid recombination scheme in population: " << recombinationScheme << std::endl;
			return false;
		}

		serverMode=false;
		if (vm.count("parallel")) {
			if(parallel > 2) {
				std::cout << "Error: the \"-p\" or \"--parallel\" option may only assume the"<< std::endl
						  << "values 0 (seriel), 1 (multi-threaded) or 2 (networked). Leaving ..." << std::endl;
				return false;
			}

			if(parallel == 2) if(vm.count("serverMode")) serverMode = true;
		}

		if(verbose){
			std::cout << std::endl
				      << "Running with the following options:" << std::endl
				      << "program = " << program << std::endl
					  << "externalArguments = " << externalArguments << std::endl
					  << "popSize = " << popSize << std::endl
					  << "nParents = " << nParents << std::endl
					  << "adaptionThreshold = " << adaptionThreshold << std::endl
					  << "nProducerThreads = " << (boost::uint16_t)nProducerThreads << std::endl // boost::uint8_t not printable on gcc ???
					  << "maxGenerations = " << maxGenerations << std::endl
					  << "maxMinutes = " << maxMinutes << std::endl
					  << "reportGeneration = " << reportGeneration << std::endl
					  << "rScheme = " << (boost::uint16_t)rScheme << std::endl
					  << "parallel = " << parallel << std::endl
					  << "serverMode = " << (serverMode?"true":"false") << std::endl
					  << "ip = " << ip << std::endl
					  << "port = " << port << std::endl
					  << "sigma = " << sigma << std::endl
					  << "sigmaSigma = " << sigmaSigma << std::endl
					  << "minSigma = " << minSigma << std::endl
					  << "maxSigma = " << maxSigma << std::endl
					  << "nEvaluations = " << nEvaluations << std::endl
					  << "exchangeMode = " << (exchangeMode==0?"binary mode":"text mode") << std::endl
					  << "sortingScheme = " << (sortingScheme?"MUPLUSNU":"MUCOMMANU") << std::endl
					  << std::endl;
		}
	}
	catch(...){
		std::cout << "Error parsing the command line" << std::endl;
		return false;
	}

	return true;
}

/************************************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */
