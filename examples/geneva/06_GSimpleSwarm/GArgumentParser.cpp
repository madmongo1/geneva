/**
 * @file GArgumentParser.cpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
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
 * http://www.gemfony.com .
 */



#include "GArgumentParser.hpp"

namespace Gem
{
  namespace Geneva
  {
    /************************************************************************************************/
    /**
     * A function that parses the command line for all required parameters
     */
    bool parseCommandLine (
    		 int argc
    	   , char **argv
		   , std::string& configFile
		   , boost::uint16_t& parallelizationMode
		   , bool& serverMode
		   , std::string& ip
		   , unsigned short& port
    ) {
      try{
		// Check the command line options. Uses the Boost program options library.
		po::options_description desc("Usage: evaluator [options]");
		desc.add_options()
		  ("help,h", "emit help message")
		  ("configFile,c", po::value<std::string>(&configFile)->default_value(DEFAULTCONFIGFILE),
		   "The name of the configuration file holding further configuration options")
		  ("parallelizationMode,p", po::value<boost::uint16_t>(&parallelizationMode)->default_value(DEFAULTPARALLELIZATIONMODEAP),
		   "Whether or not to run this optimization in serial mode (0), multi-threaded (1) or networked (2) mode")
		  ("serverMode,s","Whether to run networked execution in server or client mode. The option only gets evaluated if \"--parallelizationMode=2\"")
		  ("ip",po::value<std::string>(&ip)->default_value(DEFAULTIP), "The ip of the server")
		  ("port",po::value<unsigned short>(&port)->default_value(DEFAULTPORT), "The port of the server")
		  ;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		// Emit a help message, if necessary
		if (vm.count("help")) {
		  std::cerr << desc << std::endl;
		  return false;
		}

		serverMode=false;
		if (vm.count("parallelizationMode")) {
		  if(parallelizationMode > 2) {
			std::cout << "Error: the \"-p\" or \"--parallelizationMode\" option may only assume the"<< std::endl
				  << "values 0 (serial), 1 (multi-threaded) or 2 (networked). Leaving ..." << std::endl;
			return false;
		  }

		  if(parallelizationMode == 2) if(vm.count("serverMode")) serverMode = true;
		}

		if(parallelizationMode != DEFAULTPARALLELIZATIONMODE ||  ip != DEFAULTIP  ||  port != DEFAULTPORT){
		  std::string parModeString;
		  switch(parallelizationMode) {
		  case 0:
			parModeString = "serial";
			break;
		  case 1:
			parModeString = "multi-threaded";
			break;
		  case 2:
			parModeString = "networked";
			break;
		  };

		  std::cout << std::endl
				<< "Running with the following command line options:" << std::endl
				<< "configFile = " << configFile << std::endl
				<< "parallelizationMode = " << parModeString << std::endl
				<< "serverMode = " << (serverMode?"true":"false") << std::endl
				<< "ip = " << ip << std::endl
				<< "port = " << port << std::endl
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
    /**
     * A function that parses a config file for further parameters
     */
    bool parseConfigFile (
   		 const std::string& configFile
	   , boost::uint16_t& nProducerThreads
	   , boost::uint16_t& nEvaluationThreads
	   , std::size_t& nNeighborhoods
	   , std::size_t& nNeighborhoodMembers
	   , boost::uint32_t& maxIterations
	   , long& maxMinutes
	   , boost::uint32_t& reportIteration
	   , boost::uint32_t& processingCycles
	   , bool& returnRegardless
	   , boost::uint32_t& nProcessingUnits
	   , std::size_t& parDim
	   , double& minVar
	   , double& maxVar
	   , solverFunction& df
	   , double& cPersonal
	   , double& cNeighborhood
	   , double& cGlobal
	   , double& cVelocity
	   , updateRule& ur
	   , boost::uint16_t& xDim
	   , boost::uint16_t& yDim
	   , bool& followProgress
	   , bool& allRandomInit
    ) {
		  boost::uint16_t evalFunction = 0;
		  bool verbose;

		  // Check the name of the configuation file
		  if(configFile.empty() || configFile == "empty" || configFile == "unknown") {
		std::cerr << "Error: Invalid configuration file name given: \"" << configFile << "\"" << std::endl;
		return false;
      }

      try{
		// Check the configuration file line options. Uses the Boost program options library.
		po::options_description config("Allowed options");
		config.add_options()
		  ("nProducerThreads",po::value<boost::uint16_t>(&nProducerThreads)->default_value(DEFAULTNPRODUCERTHREADS),
		   "The amount of random number producer threads")
		  ("nEvaluationThreads",po::value<boost::uint16_t>(&nEvaluationThreads)->default_value(DEFAULTNEVALUATIONTHREADS),
		   "The amount of threads processing individuals simultaneously")
		  ("nNeighborhoods", po::value<std::size_t>(&nNeighborhoods)->default_value(DEFAULTNNEIGHBORHOODSAP),
		   "The number of neighborhoods in the population")
		  ("nNeighborhoodMembers", po::value<std::size_t>(&nNeighborhoodMembers)->default_value(DEFAULTNNEIGHBORHOODMEMBERSAP),
		   "The default number of members in each neighborhood")
		  ("maxIterations", po::value<boost::uint32_t>(&maxIterations)->default_value(DEFAULTMAXITERATIONS),
		   "Maximum number of iterations in the population")
		  ("maxMinutes", po::value<long>(&maxMinutes)->default_value(DEFAULTMAXMINUTES),
		   "The maximum number of minutes the optimization of the population should run")
		  ("reportIteration",po::value<boost::uint32_t>(&reportIteration)->default_value(DEFAULTREPORTITERATION),
		   "The number of iterations after which information should be emitted in the super-population")
		  ("verbose",po::value<bool>(&verbose)->default_value(DEFAULTVERBOSE),
		   "Whether additional information should be emitted")
		  ("processingCycles", po::value<boost::uint32_t>(&processingCycles)->default_value(DEFAULTPROCESSINGCYCLES),
		   "The maximum number of cycles a client should perform adaptions before it returns without success")
		  ("returnRegardless", po::value<bool>(&returnRegardless)->default_value(DEFAULTRETURNREGARDLESS),
		   "Specifies whether results should be returned even if they are not better than before")
		  ("nProcessingUnits", po::value<boost::uint32_t>(&nProcessingUnits)->default_value(DEFAULTGBTCNPROCUNITS),
		   "Specifies how many processing units are available in networked mode")
		  ("parDim", po::value<std::size_t>(&parDim)->default_value(DEFAULTPARDIM),
		   "The amount of variables in the parabola")
		  ("minVar", po::value<double>(&minVar)->default_value(DEFAULTMINVAR),
		   "The lower boundary for all variables")
		  ("maxVar", po::value<double>(&maxVar)->default_value(DEFAULTMAXVAR),
		   "The upper boundary for all variables")
		  ("evalFunction",po::value<boost::uint16_t>(&evalFunction),
		   "The id of the evaluation function.")
		  ("cPersonal", po::value<double>(&cPersonal)->default_value(DEFAULTCPERSONALAP),
		   "A constant to be multiplied with the personal direction vector")
		  ("cNeighborhood", po::value<double>(&cNeighborhood)->default_value(DEFAULTCNEIGHBORHOODAP),
		   "A constant to be multiplied with the neighborhood direction vector")
		  ("cGlobal", po::value<double>(&cGlobal)->default_value(DEFAULTCGLOBALAP),
		   "A constant to be multiplied with the global direction vector")
		  ("cVelocity", po::value<double>(&cVelocity)->default_value(DEFAULTCVELOCITYAP),
		   "A constant to be multiplied with the old velocity vector")
		  ("ur", po::value<updateRule>(&ur)->default_value(DEFAULTUPDATERULE),
		   "Use linear (0) or classical (1) update rule")
		  ("xDim", po::value<boost::uint16_t>(&xDim)->default_value(DEFAULTXDIMAP),
		   "The x-dimension of the canvas for the result print(s)")
		  ("yDim", po::value<boost::uint16_t>(&yDim)->default_value(DEFAULTYDIMAP),
		   "The y-dimension of the canvas for the result print(s)")
		  ("followProgress", po::value<bool>(&followProgress)->default_value(DEFAULTFOLLOWPROGRESS),
		   "Specifies whether snapshots should be taken in regular intervals")
		  ("allRandomInit", po::value<bool>(&allRandomInit)->default_value(DEFAULTALLRANDOMINIT),
		   "If set, all individuals will be initialized randomly. If 0, all individuals in one neighborhood will have the same start value")
		  ;

		po::variables_map vm;
		std::ifstream ifs(configFile.c_str());
		if(!ifs.good()) {
			std::cerr << "Error accessing configuration file " << configFile;
			return false;
		}

		store(po::parse_config_file(ifs, config), vm);
		notify(vm);

		// Emit a help message, if necessary
		if (vm.count("help")) {
			std::cout << config << std::endl;
			return false;
		}

		// Assign the demo function
		if(evalFunction > (boost::uint16_t)MAXDEMOFUNCTION) {
			std::cout << "Error: Invalid evaluation function: " << evalFunction
					<< std::endl;
			return false;
		}
		df=(solverFunction)evalFunction;

	
		if(verbose){
			std::cout << std::endl
					<< "Running with the following options from " << configFile << ":" << std::endl
					<< "nProducerThreads = " << (boost::uint16_t)nProducerThreads << std::endl // boost::uint8_t not printable on gcc ???
					<< "nNeighborhoods = " << nNeighborhoods << std::endl
					<< "nNeighborhoodMembers = " << nNeighborhoodMembers << std::endl
					<< "maxIterations = " << maxIterations << std::endl
					<< "maxMinutes = " << maxMinutes << std::endl
					<< "reportIteration = " << reportIteration << std::endl
					<< "processingCycles = " << processingCycles << std::endl
					<< "returnRegardless = " << (returnRegardless?"true":"false") << std::endl
					<< "nProcessingUnits = " << nProcessingUnits << std::endl
					<< "parDim = " << parDim << std::endl
					<< "minVar = " << minVar << std::endl
					<< "maxVar = " << maxVar << std::endl
					<< std::endl << "evalFunction = " << df << std::endl
					<< "cPersonal = " << cPersonal << std::endl
					<< "cNeighborhood = " << cNeighborhood << std::endl
					<< "cGlobal = " << cGlobal << std::endl
					<< "cVelocity = " << cVelocity << std::endl
					<< "updateRule = " << ur << std::endl
					<< "xDim = " << xDim << std::endl
					<< "yDim = " << yDim << std::endl
					<< "followProgress = " << (followProgress?"true":"false") << std::endl
					<< "allRandomInit = " << allRandomInit << std::endl
					<< std::endl;
		}
      }
      catch(...){
    	  std::cout << "Error parsing the configuration file " << configFile << std::endl;
    	  return false;
      }

      return true;
    }

    /************************************************************************************************/

  } /* namespace Geneva */
} /* namespace Gem */
