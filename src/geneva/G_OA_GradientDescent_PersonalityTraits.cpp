/**
 * @file G_OA_GradientDescent_PersonalityTraits.cpp
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
#include "geneva/G_OA_GradientDescent_PersonalityTraits.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::G_OA_GradientDescent_PersonalityTraits)

namespace Gem {
namespace Geneva {

/******************************************************************************/
/** A short identifier suitable for storage in a std::map */
G_API_GENEVA const std::string G_OA_GradientDescent_PersonalityTraits::nickname = "gd";

/******************************************************************************/
/**
 * The default constructor
 */
G_OA_GradientDescent_PersonalityTraits::G_OA_GradientDescent_PersonalityTraits()
	: GPersonalityTraits(), popPos_(0) { /* nothing */ }

/******************************************************************************/
/**
 * The copy contructor
 *
 * @param cp A copy of another GGDPersonalityTraits object
 */
G_OA_GradientDescent_PersonalityTraits::G_OA_GradientDescent_PersonalityTraits(const G_OA_GradientDescent_PersonalityTraits &cp)
	: GPersonalityTraits(cp), popPos_(cp.popPos_) { /* nothing */ }

/******************************************************************************/
/**
 * The standard destructor
 */
G_OA_GradientDescent_PersonalityTraits::~G_OA_GradientDescent_PersonalityTraits() { /* nothing */ }

/***************************************************************************/
/**
 * The standard assignment operator
 */
const G_OA_GradientDescent_PersonalityTraits &G_OA_GradientDescent_PersonalityTraits::operator=(
	const G_OA_GradientDescent_PersonalityTraits &cp
) {
	this->load_(&cp);
	return *this;
}

/******************************************************************************/
/**
 * Checks for equality with another GGDPersonalityTraits object
 *
 * @param  cp A constant reference to another GGDPersonalityTraits object
 * @return A boolean indicating whether both objects are equal
 */
bool G_OA_GradientDescent_PersonalityTraits::operator==(const G_OA_GradientDescent_PersonalityTraits &cp) const {
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
 * Checks for inequality with another GGDPersonalityTraits object
 *
 * @param  cp A constant reference to another GGDPersonalityTraits object
 * @return A boolean indicating whether both objects are inequal
 */
bool G_OA_GradientDescent_PersonalityTraits::operator!=(const G_OA_GradientDescent_PersonalityTraits &cp) const {
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
void G_OA_GradientDescent_PersonalityTraits::compare(
	const GObject &cp, const Gem::Common::expectation &e, const double &limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a G_OA_GradientDescent_PersonalityTraits reference independent of this object and convert the pointer
	const G_OA_GradientDescent_PersonalityTraits *p_load = Gem::Common::g_convert_and_compare<GObject, G_OA_GradientDescent_PersonalityTraits>(cp, this);

	GToken token("G_OA_GradientDescent_PersonalityTraits", e);

	// Compare our parent data ...
	Gem::Common::compare_base<GPersonalityTraits>(IDENTITY(*this, *p_load), token);

	// ... and then the local data
	compare_t(IDENTITY(popPos_, p_load->popPos_), token);

	// React on deviations from the expectation
	token.evaluate();
}

/***********************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string G_OA_GradientDescent_PersonalityTraits::name() const {
	return std::string("G_OA_GradientDescent_PersonalityTraits");
}

/******************************************************************************/
/**
 * Retrieves the mnemonic of the optimization algorithm
 */
std::string G_OA_GradientDescent_PersonalityTraits::getMnemonic() const {
	return G_OA_GradientDescent_PersonalityTraits::nickname;
}

/******************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A clone of this object, camouflaged as a GObject
 */
GObject *G_OA_GradientDescent_PersonalityTraits::clone_() const {
	return new G_OA_GradientDescent_PersonalityTraits(*this);
}

/******************************************************************************/
/**
 * Loads the data of another GGDPersonalityTraits object
 *
 * @param cp A copy of another GGDPersonalityTraits object, camouflaged as a GObject
 */
void G_OA_GradientDescent_PersonalityTraits::load_(const GObject *cp) {
	// Check that we are dealing with a G_OA_GradientDescent_PersonalityTraits reference independent of this object and convert the pointer
	const G_OA_GradientDescent_PersonalityTraits *p_load = Gem::Common::g_convert_and_compare<GObject, G_OA_GradientDescent_PersonalityTraits>(cp, this);

	// Load the parent class'es data
	GPersonalityTraits::load_(cp);

	// and then the local data
	popPos_ = p_load->popPos_;
}

/******************************************************************************/
/**
 * Sets the position of the individual in the population
 *
 * @param popPos The new position of this individual in the population
 */
void G_OA_GradientDescent_PersonalityTraits::setPopulationPosition(const std::size_t &popPos) {
	popPos_ = popPos;
}

/* ----------------------------------------------------------------------------------
 * So far untested
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Retrieves the position of the individual in the population
 *
 * @return The current position of this individual in the population
 */
std::size_t G_OA_GradientDescent_PersonalityTraits::getPopulationPosition(void) const {
	return popPos_;
}

/* ----------------------------------------------------------------------------------
 * So far untested
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool G_OA_GradientDescent_PersonalityTraits::modify_GUnitTests() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if (GPersonalityTraits::modify_GUnitTests()) result = true;

	this->setPopulationPosition(this->getPopulationPosition() + 1);
	result = true;

	return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
   condnotset("G_OA_GradientDescent_PersonalityTraits::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void G_OA_GradientDescent_PersonalityTraits::specificTestsNoFailureExpected_GUnitTests() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent class'es function
	GPersonalityTraits::specificTestsNoFailureExpected_GUnitTests();


	// --------------------------------------------------------------------------

	// --------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("G_OA_GradientDescent_PersonalityTraits::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void G_OA_GradientDescent_PersonalityTraits::specificTestsFailuresExpected_GUnitTests() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent class'es function
	GPersonalityTraits::specificTestsFailuresExpected_GUnitTests();

	// --------------------------------------------------------------------------

	// --------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("G_OA_GradientDescent_PersonalityTraits::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */