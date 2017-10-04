/**
 * @file GSerialSwarm.hpp
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here

// Boost headers go here

#ifndef GSERIALSWARM_HPP_
#define GSERIALSWARM_HPP_

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GCommonHelperFunctions.hpp"
#include "geneva/GBaseSwarm.hpp"
#include "geneva/GOptimizableEntity.hpp"
#include "geneva/GObject.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * A serial swarm, no multithreading is used directly by this class
 */
class GSerialSwarm
	: public GBaseSwarm
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar
			& BOOST_SERIALIZATION_BASE_OBJECT_NVP(GBaseSwarm);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor. Intentionally empty, as it is only needed for de-serialization purposes */
	G_API_GENEVA GSerialSwarm();
	/** @brief The default constructor */
	G_API_GENEVA GSerialSwarm(const std::size_t&, const std::size_t&);
	/** @brief A standard copy constructor */
	G_API_GENEVA GSerialSwarm(const GSerialSwarm&);
	/** @brief The destructor */
	virtual G_API_GENEVA ~GSerialSwarm();

	/** @brief The standard assignment operator */
	G_API_GENEVA const GSerialSwarm& operator=(const GSerialSwarm&);

	/** @brief Checks for equality with another GSerialSwarm object */
	G_API_GENEVA bool operator==(const GSerialSwarm&) const;
	/** @brief Checks for inequality with another GSerialSwarm object */
	G_API_GENEVA bool operator!=(const GSerialSwarm&) const;

	/** @brief Searches for compliance with expectations with respect to another object of the same type */
	virtual G_API_GENEVA void compare(
		const GObject& // the other object
		, const Gem::Common::expectation& // the expectation for this object, e.g. equality
		, const double& // the limit for allowed deviations of floating point types
	) const override;

	/** @brief Adds local configuration options to a GParserBuilder object */
	virtual G_API_GENEVA void addConfigurationOptions (
		Gem::Common::GParserBuilder& gpb
	) override;

	/** @brief Emits a name for this class / object */
	virtual G_API_GENEVA std::string name() const override;

protected:
	/** @brief Loads the data of another population */
	virtual G_API_GENEVA void load_(const GObject *) override;
	/** @brief Creates a deep clone of this object */
	virtual G_API_GENEVA GObject *clone_() const override;

	/** @brief Does some preparatory work before the optimization starts */
	virtual G_API_GENEVA void init() override;
	/** @brief Does any necessary finalization work */
	virtual G_API_GENEVA void finalize() override;

	/** @brief Updates the fitness of all individuals */
	virtual G_API_GENEVA void runFitnessCalculation() override;

public:
	/***************************************************************************/
	/** @brief Applies modifications to this object. This is needed for testing purposes */
	virtual G_API_GENEVA bool modify_GUnitTests() override;
	/** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	virtual G_API_GENEVA void specificTestsNoFailureExpected_GUnitTests() override;
	/** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	virtual G_API_GENEVA void specificTestsFailuresExpected_GUnitTests() override;
};

} /* namespace Geneva */
} /* namespace Gem */

#ifdef GEM_TESTING
// Tests of this class (and parent classes)
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * As Gem::Geneva::GSerialSwarm has a protected default constructor, we need to provide a
 * specialization of the factory function that creates objects of this type.
 */
template <>
inline G_API_GENEVA std::shared_ptr<Gem::Geneva::GSerialSwarm> TFactory_GUnitTests<Gem::Geneva::GSerialSwarm>() {
   using namespace Gem::Tests;
   const std::size_t NNEIGHBORHOODS=2;
   const std::size_t NNEIGHBORHOODMEMBERS=3;
   std::shared_ptr<Gem::Geneva::GSerialSwarm> p;
   BOOST_CHECK_NO_THROW(p= std::shared_ptr<Gem::Geneva::GSerialSwarm>(new Gem::Geneva::GSerialSwarm(NNEIGHBORHOODS, NNEIGHBORHOODMEMBERS)));
   for(std::size_t i=0; i<NNEIGHBORHOODS*NNEIGHBORHOODMEMBERS; i++) {
      p->push_back(std::shared_ptr<GTestIndividual1>(new GTestIndividual1()));
   }
   return p;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
#endif /* GEM_TESTING */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GSerialSwarm)

#endif /* GSERIALSWARM_HPP_ */
