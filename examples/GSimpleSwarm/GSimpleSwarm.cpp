/**
 * @file GSimpleSwarm.cpp
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

// Standard header files go here
#include <iostream>
#include <cmath>
#include <sstream>

// Boost header files go here
#include <boost/lexical_cast.hpp>

// GenEvA header files go here
#include "GRandom.hpp"
#include "GSwarm.hpp"
#include "GBrokerEA.hpp"
#include "GIndividualBroker.hpp"
#include "GAsioTCPConsumer.hpp"
#include "GAsioTCPClient.hpp"
#include "GAsioHelperFunctions.hpp"

// The individual that should be optimized
#include "GStartIndividual.hpp"

// Declares a function to parse the command line
#include "GArgumentParser.hpp"

// Information retrieval and printing
#include "GInfoFunction.hpp"

using namespace Gem::GenEvA;
using namespace Gem::Util;

/************************************************************************************************/
/**
 * The main function.
 */
int main(int argc, char **argv){
  std::string configFile;		  
  boost::uint16_t parallelizationMode;
  bool serverMode;
  std::string ip;
  unsigned short port;
  boost::uint16_t nProducerThreads;
  boost::uint16_t nEvaluationThreads;
  std::size_t populationSize;
  std::size_t nParents;
  boost::uint32_t maxIterations;
  long maxMinutes;
  boost::uint32_t reportIteration;
  recoScheme rScheme;
  std::size_t arraySize;
  std::size_t parDim;
  double minVar;
  double maxVar;
  sortingMode smode;
  boost::uint32_t processingCycles;
  bool returnRegardless;
  boost::uint32_t waitFactor;
  std::size_t nNeighborhoods;
  std::size_t nNeighborhoodMembers;

  if(!parseCommandLine(argc, argv,
		       configFile,			  
		       parallelizationMode,
		       serverMode,
		       ip,
		       port)
     ||
     !parseConfigFile(configFile,
		      nProducerThreads,
		      nEvaluationThreads,
		      nNeighborhoods,
			  nNeighborhoodMembers,
		      maxIterations,
		      maxMinutes,
		      reportIteration,
		      arraySize,
		      processingCycles,
		      returnRegardless,
		      waitFactor,
		      parDim,
		      minVar,
		      maxVar))
    { exit(1); }

  // Random numbers are our most valuable good. Set the number of threads
  GRANDOMFACTORY->setNProducerThreads(nProducerThreads);
  GRANDOMFACTORY->setArraySize(arraySize);
  
  //***************************************************************************
  // If this is a client in networked mode, we can just start the listener and
  // return when it has finished
  if(parallelizationMode==2 && !serverMode) {
    boost::shared_ptr<GAsioTCPClient> p(new GAsioTCPClient(ip, boost::lexical_cast<std::string>(port)));

    p->setMaxStalls(0); // An infinite number of stalled data retrievals
    p->setMaxConnectionAttempts(100); // Up to 100 failed connection attempts

    // Prevent return of unsuccessful mutation attempts to the server
    p->returnResultIfUnsuccessful(returnRegardless);

    // Start the actual processing loop
    p->run();

    return 0;
  }

  //***************************************************************************
  // Create an instance of our optimization monitor, telling it to output information in given intervals
  std::ofstream resultSummary("./result.C");
  boost::shared_ptr<optimizationMonitor> om(new optimizationMonitor(nParents, resultSummary));

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // We can now start creating populations. We refer to them through the base class

  // This smart pointer will hold the different population types
  boost::shared_ptr<GSwarm> pop_ptr;

  // We need to know the population size in advance. It is calculated from the number of neighborhoods and
  // the number of individuals inside of them.
  pop_ptr->setPopulationSize(nNeighborhoods, nNeihborhoodMembers);

  // Create the actual populations
  switch (parallelizationMode) {
    //-----------------------------------------------------------------------------------------------------
  case 0: // Serial execution
    // Create an empty population
    pop_ptr = boost::shared_ptr<GEvolutionaryAlgorithm>(new GEvolutionaryAlgorithm());
    break;

    //-----------------------------------------------------------------------------------------------------
  case 1: // Multi-threaded execution
    {
      // Create the multi-threaded population
      boost::shared_ptr<GMultiThreadedEA> popPar_ptr(new GMultiThreadedEA());

      // Population-specific settings
      popPar_ptr->setNThreads(nEvaluationThreads);

      // Assignment to the base pointer
      pop_ptr = popPar_ptr;
    }
    break;

    //-----------------------------------------------------------------------------------------------------
  case 2: // Networked execution (server-side)
    {
      // Create a network consumer and enrol it with the broker
      boost::shared_ptr<GAsioTCPConsumer> gatc(new GAsioTCPConsumer(port));
      GINDIVIDUALBROKER->enrol(gatc);

      // Create the actual broker population
      boost::shared_ptr<GBrokerEA> popBroker_ptr(new GBrokerEA());
      popBroker_ptr->setWaitFactor(waitFactor);

      // Assignment to the base pointer
      pop_ptr = popBroker_ptr;
    }
    break;
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Now we have suitable populations and can fill them with data

  // Add individuals to the population
  for(std::size_t p = 0 ; p<pop_ptr->getDefaultPopulationSize(); p++) {
	  boost::shared_ptr<GStartIndividual> gdii_ptr(new GStartIndividual(parDim, minVar, maxVar));
	  gdii_ptr->setProcessingCycles(processingCycles);
	  pop_ptr->push_back(gdii_ptr);
  }
 
  // Specify some general population settings
  pop_ptr->setMaxIteration(maxIterations);
  pop_ptr->setMaxTime(boost::posix_time::minutes(maxMinutes));
  pop_ptr->setReportIteration(reportIteration);
  pop_ptr->registerInfoFunction(boost::bind(&optimizationMonitor::informationFunction, om, _1, _2));
  
  // Do the actual optimization
  pop_ptr->optimize();

  //--------------------------------------------------------------------------------------------

  // Make sure we close the result file
  resultSummary.close();

  std::cout << "Done ..." << std::endl;
  return 0;
}