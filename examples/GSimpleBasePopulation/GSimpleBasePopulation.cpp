/**
 * @file GSimpleBasePopulation.cpp
 */

/* Copyright (C) 2004-2008 Dr. Ruediger Berlich
 * Copyright (C) 2007-2008 Forschungszentrum Karlsruhe GmbH
 *
 * This file is part of Geneva, Gemfony scientific's optimization library.
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

// Standard header files go here
#include <iostream>
#include <cmath>
#include <sstream>

// Boost header files go here

// GenEvA header files go here
#include "GRandom.hpp"
#include "GBasePopulation.hpp"
#include "GBoostThreadPopulation.hpp"

// The individual that should be optimized
// This is a simple parabola
#include "GParabolaIndividual.hpp"

// Declares a function to parse the command line
#include "GCommandLineParser.hpp"

using namespace Gem::GenEvA;
using namespace Gem::Util;

/************************************************************************************************/
/**
 * The main function. We search for the minimum of a parabola. This example demonstrates the use
 * of the GBasePopulation class or (at your choice) of the GBoostThreadPopulation class. Note that
 * a number of command line options are available. Call the executable with the "-h" switch
 * to get an overview.
 */

int main(int argc, char **argv){
	// Variables for the command line parsing
	 std::size_t parabolaDimension;
	 std::size_t populationSize, nParents;
	 double parabolaMin, parabolaMax;
	 boost::uint16_t nProducerThreads;
	 boost::uint32_t maxGenerations, reportGeneration;
	 boost::uint32_t maxStallGenerations;
	 boost::uint32_t adaptionThreshold;
	 long maxMinutes;
	 bool parallel;
	 bool verbose;
	 bool maximize;
	 recoScheme rScheme;
	 bool sortingScheme;
	 double qualityThreshold;
	 std::size_t arraySize;
	 bool productionPlace;

	// Parse the command line
	if(!parseCommandLine(argc, argv,
		  			     parabolaDimension,
						 parabolaMin,
						 parabolaMax,
						 adaptionThreshold,
						 nProducerThreads,
						 populationSize,
						 nParents,
						 maxGenerations,
						 maxStallGenerations,
						 qualityThreshold,
						 maxMinutes,
						 reportGeneration,
						 rScheme,
						 sortingScheme,
						 parallel,
						 maximize,
						 arraySize,
						 productionPlace,
						 verbose))
	{ exit(1); }

	// Random numbers are our most valuable good. Set the number of threads
	GRANDOMFACTORY->setNProducerThreads(nProducerThreads);
	GRANDOMFACTORY->setArraySize(arraySize);


	// Set up a single parabola individual
	boost::shared_ptr<GParabolaIndividual> parabolaIndividual(new GParabolaIndividual());

	// Set up a GDoubleCollection with parabolaDimension values, each initialized
	// with a random number in the range [min,max[
	boost::shared_ptr<GDoubleCollection> gdc_ptr(new GDoubleCollection(parabolaDimension,parabolaMin,parabolaMax));

	// Set up and register an adaptor for the collection, so it
	// knows how to be mutated. We want a sigma of 1, a sigma-adaption of 0.001, a
	// minimum sigma of 0.000001 and a maximum sigma of 5.
	boost::shared_ptr<GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(1.,0.001,0.000001,5));
	gdga_ptr->setAdaptionThreshold(adaptionThreshold);
	if(productionPlace) // Factory means "true"
		gdga_ptr->setRnrGenerationMode(Gem::Util::RNRFACTORY);
	else // Local means "false"
		gdga_ptr->setRnrGenerationMode(Gem::Util::RNRLOCAL);
	gdc_ptr->addAdaptor(gdga_ptr);

	// Make the parameter collection known to this individual
	parabolaIndividual->push_back(gdc_ptr);

	if(parallel) {
	  // Now we've got our first individual and can create a simple population with parallel execution.
	  GBoostThreadPopulation pop_par;
	  pop_par.setNThreads(10);

	  pop_par.push_back(parabolaIndividual);

	  // Specify some population settings
	  pop_par.setPopulationSize(populationSize,nParents);
	  pop_par.setMaxGeneration(maxGenerations);
	  pop_par.setMaxStallGeneration(maxStallGenerations);
	  pop_par.setMaxTime(boost::posix_time::minutes(maxMinutes)); // Calculation should be finished after 5 minutes
	  pop_par.setReportGeneration(reportGeneration); // Emit information during every generation
	  pop_par.setRecombinationMethod(rScheme); // The best parents have higher chances of survival
	  pop_par.setSortingScheme(sortingScheme); // Determines whether sorting is done in MUPLUSNU or MUCOMMANU mode
	  pop_par.setMaximize(maximize); // Specifies whether we want to do maximization or minimization

	  if(productionPlace) // Factory means "true"
		  pop_par.setRnrGenerationMode(Gem::Util::RNRFACTORY);
	  else // Local means "false"
		  pop_par.setRnrGenerationMode(Gem::Util::RNRLOCAL);

	  if(qualityThreshold > 0.) pop_par.setQualityThreshold(qualityThreshold);

	  // Do the actual optimization
	  pop_par.optimize();
	}
	else {
	  // Now we've got our first individual and can create a simple population with serial execution.
	  GBasePopulation pop_ser;

	  pop_ser.push_back(parabolaIndividual);

	  // Specify some population settings
	  pop_ser.setPopulationSize(populationSize,nParents);
	  pop_ser.setMaxGeneration(maxGenerations);
	  pop_ser.setMaxStallGeneration(maxStallGenerations);
	  pop_ser.setMaxTime(boost::posix_time::minutes(maxMinutes)); // Calculation should be finished after 5 minutes
	  pop_ser.setReportGeneration(reportGeneration); // Emit information during every generation
	  pop_ser.setRecombinationMethod(rScheme); // The best parents have higher chances of survival
	  pop_ser.setSortingScheme(sortingScheme); // Determines whether sorting is done in MUPLUSNU or MUCOMMANU mode
	  pop_ser.setMaximize(maximize); // Specifies whether we want to do maximization or minimization

	  if(productionPlace) // Factory means "true"
		  pop_ser.setRnrGenerationMode(Gem::Util::RNRFACTORY);
	  else // Local means "false"
		  pop_ser.setRnrGenerationMode(Gem::Util::RNRLOCAL);

	  if(qualityThreshold > 0.) pop_ser.setQualityThreshold(qualityThreshold);

	  // Do the actual optimization
	  pop_ser.optimize();
	}

	std::cout << "Done ..." << std::endl;

	return 0;
}
