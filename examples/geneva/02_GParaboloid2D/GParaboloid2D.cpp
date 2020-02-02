/**
 * @file GParaboloid2D.cpp
 */

/********************************************************************************
 *
 * This file is part of the Geneva library collection. The following license
 * applies to this file:
 *
 * ------------------------------------------------------------------------------
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ------------------------------------------------------------------------------
 *
 * Note that other files in the Geneva library collection may use a different
 * license. Please see the licensing information in each file.
 *
 ********************************************************************************
 *
 * Geneva was started by Dr. Rüdiger Berlich and was later maintained together
 * with Dr. Ariel Garcia under the auspices of Gemfony scientific. For further
 * information on Gemfony scientific, see http://www.gemfomy.eu .
 *
 * The majority of files in Geneva was released under the Apache license v2.0
 * in February 2020.
 *
 * See the NOTICE file in the top-level directory of the Geneva library
 * collection for a list of contributors and copyright information.
 *
 ********************************************************************************/

// Standard header files go here
#include <iostream>

// Boost header files go here

// Geneva header files go here
#include "geneva/Go2.hpp"

// The individual that should be optimized
#include "GParaboloidIndividual2D.hpp"

using namespace Gem::Geneva;

int main(int argc, char **argv) {
	Go2 go(argc, argv, "config/Go2.json");

	//---------------------------------------------------------------------
	// Initialize a client, if requested
	if(go.clientMode()) {
		return go.clientRun();
	}

	//---------------------------------------------------------------------
	// Add individuals and algorithms and perform the actual optimization cycle

	// Make an individual known to the optimizer
	std::shared_ptr<GParaboloidIndividual2D> p(new GParaboloidIndividual2D());
	go.push_back(p);

	// Add an evolutionary algorithm to the Go2 class.
	go & "ea";

	// Perform the actual optimization
	std::shared_ptr<GParaboloidIndividual2D>
		bestIndividual_ptr = go.optimize()->getBestGlobalIndividual<GParaboloidIndividual2D>();

	// Do something with the best result
}
