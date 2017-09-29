/**
 * @file GSerialGD.cpp
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

#include "geneva/GSerialGD.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GSerialGD)

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The default constructor
 */
GSerialGD::GSerialGD()
	: GBaseGD() { /* nothing */ }

/******************************************************************************/
/**
 * Initialization with the number of starting points and the size of the finite step
 */
GSerialGD::GSerialGD(
	const std::size_t &nStartingPoints, const double &finiteStep, const double &stepSize
)
	: GBaseGD(nStartingPoints, finiteStep, stepSize) { /* nothing */ }

/******************************************************************************/
/**
 * A standard copy constructor
 */
GSerialGD::GSerialGD(const GSerialGD &cp)
	: GBaseGD(cp) { /* nothing */ }

/******************************************************************************/
/**
 * The destructor.
 */
GSerialGD::~GSerialGD() { /* nothing */ }

/***************************************************************************/
/**
 * The standard assignment operator
 */
const GSerialGD &GSerialGD::operator=(const GSerialGD &cp) {
	this->load_(&cp);
	return *this;
}

/******************************************************************************/
/**
 * Checks for equality with another GSerialGD object
 *
 * @param  cp A constant reference to another GSerialGD object
 * @return A boolean indicating whether both objects are equal
 */
bool GSerialGD::operator==(const GSerialGD &cp) const {
	using namespace Gem::Common;
	try {
		this->compare(cp, Gem::Common::expectation::CE_EQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
		return true;
	} catch (g_expectation_violation &) {
		return false;
	}
}

/******************************************************************************/
/**
 * Checks for inequality with another GSerialGD object
 *
 * @param  cp A constant reference to another GSerialGD object
 * @return A boolean indicating whether both objects are inequal
 */
bool GSerialGD::operator!=(const GSerialGD &cp) const {
	using namespace Gem::Common;
	try {
		this->compare(cp, Gem::Common::expectation::CE_INEQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
		return true;
	} catch (g_expectation_violation &) {
		return false;
	}
}

/******************************************************************************/
/**
 * Searches for compliance with expectations with respect to another object
 * of the same type
 *
 * @param cp A constant reference to another GObject object
 * @param e The expected outcome of the comparison
 * @param limit The maximum deviation for floating point values (important for similarity checks)
 */
void GSerialGD::compare(
	const GObject &cp, const Gem::Common::expectation &e, const double &limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a GSerialGD reference independent of this object and convert the pointer
	const GSerialGD *p_load = Gem::Common::g_convert_and_compare<GObject, GSerialGD>(cp, this);

	GToken token("GSerialGD", e);

	// Compare our parent data ...
	Gem::Common::compare_base<GBaseGD>(IDENTITY(*this, *p_load), token);

	// ... no local data

	// React on deviations from the expectation
	token.evaluate();
}

/******************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string GSerialGD::name() const {
	return std::string("GSerialGD");
}

/******************************************************************************/
/**
 * Loads the data from another GSerialGD object.
 *
 * @param vp Pointer to another GSerialGD object, camouflaged as a GObject
 */
void GSerialGD::load_(const GObject *cp) {
	// Convert the pointer to our target type and check for self-assignment
	const GSerialGD * p_load = Gem::Common::g_convert_and_compare<GObject, GSerialGD>(cp, this);

	// First load our parent class'es data ...
	GBaseGD::load_(cp);

	// ... no local data
}

/******************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A deep copy of this object, camouflaged as a GObject
 */
GObject *GSerialGD::clone_() const {
	return new GSerialGD(*this);
}

/******************************************************************************/
/**
 * Necessary initialization work before the start of the optimization
 */
void GSerialGD::init() {
	// GSerialGD sees exactly the environment it would when called from its own class
	GBaseGD::init();

	// Add local configuration code here
}

/******************************************************************************/
/**
 * Necessary clean-up work after the optimization has finished
 */
void GSerialGD::finalize() {
	// Add local finalization code here

	// GSerialGD sees exactly the environment it would when called from its own class
	GBaseGD::finalize();
}

/******************************************************************************/
/**
 * Adds local configuration options to a GParserBuilder object
 *
 * @param gpb The GParserBuilder object to which configuration options should be added
 */
void GSerialGD::addConfigurationOptions(
	Gem::Common::GParserBuilder &gpb
) {
	// Call our parent class'es function
	GBaseGD::addConfigurationOptions(gpb);

	// no local data
}

/******************************************************************************/
/**
 * Triggers fitness calculation of a number of individuals. This function can be overloaded to perform
 * fitness calculation in parallel, using threads or network communication.
 */
void GSerialGD::runFitnessCalculation() {
	// Trigger value calculation for all individuals (including parents)
	GSerialGD::iterator it; // An iterator that allows us to loop over the collection
	for (it = this->begin(); it != this->end(); ++it) {
#ifdef DEBUG
      // Make sure the evaluated individuals have the dirty flag set
      if(afterFirstIteration() && !(*it)->isDirty()) {
         glogger
         << "In GSerialGD::runFitnessCalculation():" << std::endl
         << "In iteration " << this->getIteration() << ": Found individual in position " << std::distance(this->begin(), it) << " whose dirty flag isn't set" << std::endl
         << GEXCEPTION;
      }
#endif /* DEBUG */

		// Perform the actual evaluation
		(*it)->process();
	}
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 */
bool GSerialGD::modify_GUnitTests() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if (GBaseGD::modify_GUnitTests()) result = true;

	return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GSerialGD::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GSerialGD::specificTestsNoFailureExpected_GUnitTests() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	GBaseGD::specificTestsNoFailureExpected_GUnitTests();

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GSerialGD::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GSerialGD::specificTestsFailuresExpected_GUnitTests() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	GBaseGD::specificTestsFailuresExpected_GUnitTests();

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GSerialGD::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */
