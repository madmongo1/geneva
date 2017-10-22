/**
 * @file G_OA_SwarmAlgorithmFactory.cpp
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

#include "geneva/G_OA_SwarmAlgorithmFactory.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The default constructor
 */
GSwarmAlgorithmFactory::GSwarmAlgorithmFactory()
	: GOptimizationAlgorithmFactoryT<GOptimizationAlgorithmT<Gem::Courtier::GBrokerExecutorT<GParameterSet>>>(
	"./config/GSwarmAlgorithm.json")
{ /* nothing */ }

/******************************************************************************/
/**
 * Initialization with the name of the config file
 */
GSwarmAlgorithmFactory::GSwarmAlgorithmFactory(
	const std::string &configFile
)
	: GOptimizationAlgorithmFactoryT<GOptimizationAlgorithmT<Gem::Courtier::GBrokerExecutorT<GParameterSet>>>(configFile)
{ /* nothing */ }

/******************************************************************************/
/**
 * A constructor with the ability to switch the parallelization mode and
 * to add a content creator. It initializes a target item as needed.
 */
GSwarmAlgorithmFactory::GSwarmAlgorithmFactory(
	const std::string &configFile
	, std::shared_ptr <Gem::Common::GFactoryT<GParameterSet>> contentCreatorPtr
)
	: GOptimizationAlgorithmFactoryT<GOptimizationAlgorithmT<Gem::Courtier::GBrokerExecutorT<GParameterSet>>>(configFile, contentCreatorPtr)
{ /* nothing */ }

/******************************************************************************/
/**
 * The copy constructor
 */
GSwarmAlgorithmFactory::GSwarmAlgorithmFactory(const GSwarmAlgorithmFactory& cp)
	: GOptimizationAlgorithmFactoryT<GOptimizationAlgorithmT<Gem::Courtier::GBrokerExecutorT<GParameterSet>>>(cp)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GSwarmAlgorithmFactory::~GSwarmAlgorithmFactory()
{ /* nothing */ }

/******************************************************************************/
/**
 * Gives access to the mnemonics / nickname describing an algorithm
 */
std::string GSwarmAlgorithmFactory::getMnemonic() const {
	return GSwarmPersonalityTraits::nickname;
}

/******************************************************************************/
/**
 * Gives access to a clear-text description of the algorithm
 */
std::string GSwarmAlgorithmFactory::getAlgorithmName() const {
	return std::string("Swarm Algorithm");
}

/******************************************************************************/
/**
 * Creates items of this type
 *
 * @return Items of the desired type
 */
std::shared_ptr<GOptimizationAlgorithmT<Gem::Courtier::GBrokerExecutorT<GParameterSet>>> GSwarmAlgorithmFactory::getObject_(
	Gem::Common::GParserBuilder &gpb
	, const std::size_t &id
) {
	std::shared_ptr<GSwarmAlgorithm> target(
		new GSwarmAlgorithm()
	);

	// Make the local configuration options known (up to the level of GSwarmAlgorithm)
	target->GSwarmAlgorithm::addConfigurationOptions(gpb);

	return target;
}

/******************************************************************************/
/**
 * Allows to act on the configuration options received from the configuration file. Here
 * we can add the options described in describeLocalOptions to the object.
 *
 * @param p A smart-pointer to be acted on during post-processing
 */
void GSwarmAlgorithmFactory::postProcess_(
	std::shared_ptr<GOptimizationAlgorithmT<Gem::Courtier::GBrokerExecutorT<GParameterSet>>>& p_base
) {
	// Call our parent class'es function
	GOptimizationAlgorithmFactoryT<GOptimizationAlgorithmT<Gem::Courtier::GBrokerExecutorT<GParameterSet>>>::postProcess_(p_base);
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */
