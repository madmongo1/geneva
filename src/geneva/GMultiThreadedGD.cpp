/**
 * @file GMultiThreadedGD.cpp
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

#include "geneva/GMultiThreadedGD.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GMultiThreadedGD)

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The default constructor
 */
GMultiThreadedGD::GMultiThreadedGD()
	: GBaseGD(), nThreads_(
	boost::numeric_cast<std::uint16_t>(Gem::Common::getNHardwareThreads(DEFAULTNSTDTHREADS))) { /* nothing */ }

/******************************************************************************/
/**
 * Initialization with the number of starting points and the size of the finite step
 */
GMultiThreadedGD::GMultiThreadedGD(
	const std::size_t &nStartingPoints, const double &finiteStep, const double &stepSize
)
	: GBaseGD(nStartingPoints, finiteStep, stepSize), nThreads_(
	boost::numeric_cast<std::uint16_t>(Gem::Common::getNHardwareThreads(DEFAULTNSTDTHREADS))) { /* nothing */ }

/******************************************************************************/
/**
 * A standard copy constructor
 */
GMultiThreadedGD::GMultiThreadedGD(const GMultiThreadedGD &cp)
	: GBaseGD(cp), nThreads_(cp.nThreads_) { /* nothing */ }

/******************************************************************************/
/**
 * The destructor. We clear remaining work items in the
 * thread pool and wait for active tasks to finish.
 */
GMultiThreadedGD::~GMultiThreadedGD() { /* nothing */ }

/***************************************************************************/
/**
 * The standard assignment operator
 */
const GMultiThreadedGD &GMultiThreadedGD::operator=(const GMultiThreadedGD &cp) {
	this->load_(&cp);
	return *this;
}

/******************************************************************************/
/**
 * Checks for equality with another GMultiThreadedGD object
 *
 * @param  cp A constant reference to another GMultiThreadedGD object
 * @return A boolean indicating whether both objects are equal
 */
bool GMultiThreadedGD::operator==(const GMultiThreadedGD &cp) const {
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
 * Checks for inequality with another GMultiThreadedGD object
 *
 * @param  cp A constant reference to another GMultiThreadedGD object
 * @return A boolean indicating whether both objects are inequal
 */
bool GMultiThreadedGD::operator!=(const GMultiThreadedGD &cp) const {
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
void GMultiThreadedGD::compare(
	const GObject &cp, const Gem::Common::expectation &e, const double &limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a GMultiThreadedGD reference independent of this object and convert the pointer
	const GMultiThreadedGD *p_load = Gem::Common::g_convert_and_compare<GObject, GMultiThreadedGD>(cp, this);

	GToken token("GMultiThreadedGD", e);

	// Compare our parent data ...
	Gem::Common::compare_base<GBaseGD>(IDENTITY(*this, *p_load), token);

	// ... and then the local data
	compare_t(IDENTITY(nThreads_, p_load->nThreads_), token);

	// React on deviations from the expectation
	token.evaluate();
}

/***********************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string GMultiThreadedGD::name() const {
	return std::string("GMultiThreadedGD");
}

/******************************************************************************/
/**
 * Sets the number of threads for this population. If nThreads is set
 * to 0, an attempt will be made to set the number of threads to the
 * number of hardware threading units (e.g. number of cores or hyperthreading
 * units).
 *
 * @param nThreads The number of threads this class uses
 */
void GMultiThreadedGD::setNThreads(std::uint16_t nThreads) {
	if (nThreads == 0) {
		nThreads_ = boost::numeric_cast<std::uint16_t>(Gem::Common::getNHardwareThreads(DEFAULTNSTDTHREADS));
	}
	else {
		nThreads_ = nThreads;
	}
}

/******************************************************************************/
/**
 * Retrieves the number of threads this population uses.
 *
 * @return The maximum number of allowed threads
 */
std::uint16_t GMultiThreadedGD::getNThreads() const {
	return nThreads_;
}

/******************************************************************************/
/**
 * Loads the data from another GMultiThreadedGD object.
 *
 * @param vp Pointer to another GMultiThreadedGD object, camouflaged as a GObject
 */
void GMultiThreadedGD::load_(const GObject *cp) {
	// Check that we are dealing with a GMultiThreadedGD reference independent of this object and convert the pointer
	const GMultiThreadedGD *p_load = Gem::Common::g_convert_and_compare<GObject, GMultiThreadedGD>(cp, this);

	// First load our parent class'es data ...
	GBaseGD::load_(cp);

	// ... and then our own
	nThreads_ = p_load->nThreads_;
}

/******************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A deep copy of this object, camouflaged as a GObject
 */
GObject *GMultiThreadedGD::clone_() const {
	return new GMultiThreadedGD(*this);
}

/******************************************************************************/
/**
 * Necessary initialization work before the start of the optimization
 */
void GMultiThreadedGD::init() {
	// GBaseGD sees exactly the environment it would when called from its own class
	GBaseGD::init();

	// Initialize our thread pool
	tp_ptr_.reset(new Gem::Common::GThreadPool(nThreads_));
}

/******************************************************************************/
/**
 * Necessary clean-up work after the optimization has finished
 */
void GMultiThreadedGD::finalize() {
	// Check whether there were any errors during thread execution
	if (tp_ptr_->hasErrors()) {
		std::ostringstream oss;

		std::vector<std::string> errors;
		errors = tp_ptr_->getErrors();

		oss
		<< "========================================================================" << std::endl
		<< "In GMultiThreadedGD::finalize(): WARNING" << std::endl
		<< "There were errors during thread execution:" << std::endl
		<< std::endl;

		for (std::vector<std::string>::iterator it = errors.begin(); it != errors.end(); ++it) {
			oss << *it << std::endl;
		}

		oss << std::endl
		<< "========================================================================" << std::endl;

		glogger // We cannot currently interrupt glogger input, all input must be transferred in one go
		<< oss.str()
		<< GWARNING;
	}

	// Terminate our thread pool
	tp_ptr_.reset();

	// GBaseGD sees exactly the environment it would when called from its own class
	GBaseGD::finalize();
}

/******************************************************************************/
/**
 * Adds local configuration options to a GParserBuilder object
 *
 * @param gpb The GParserBuilder object to which configuration options should be added
 */
void GMultiThreadedGD::addConfigurationOptions(
	Gem::Common::GParserBuilder &gpb
) {
	// Call our parent class'es function
	GBaseGD::addConfigurationOptions(gpb);

	// add local data
	gpb.registerFileParameter<std::uint16_t>(
		"nEvaluationThreads" // The name of the variable
		, 0 // The default value
		, [this](std::uint16_t nt) { this->setNThreads(nt); }
	)
	<< "The number of evaluation threads" << std::endl
	<< "0 means: determine automatically;";
}

/******************************************************************************/
/**
 * Triggers fitness calculation of a number of individuals. This function performs the same task as done
 * in GBaseGD, albeit multi-threaded.
 */
void GMultiThreadedGD::runFitnessCalculation() {
	GMultiThreadedGD::iterator it; // An iterator that allows us to loop over the collection
	for (it = this->begin(); it != this->end(); ++it) {
#ifdef DEBUG
      // Make sure the evaluated individuals have the dirty flag set
      if(afterFirstIteration() && !(*it)->isDirty()) {
         glogger
         << "In GMultiThreadedGD::runFitnessCalculation():" << std::endl
         << "Found individual in position " << std::distance(this->begin(), it) << " whose dirty flag isn't set" << std::endl
         << GEXCEPTION;
      }
#endif /* DEBUG */

		// Do the actual scheduling
		tp_ptr_->async_schedule(
			[it]() { (*it)->process(); }
		);
	}

	// wait for the pool to run out of tasks
	tp_ptr_->wait();
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool GMultiThreadedGD::modify_GUnitTests() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if (GBaseGD::modify_GUnitTests()) result = true;

	return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GMultiThreadedGD::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GMultiThreadedGD::specificTestsNoFailureExpected_GUnitTests() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	GBaseGD::specificTestsNoFailureExpected_GUnitTests();

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GMultiThreadedGD::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GMultiThreadedGD::specificTestsFailuresExpected_GUnitTests() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	GBaseGD::specificTestsFailuresExpected_GUnitTests();

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GMultiThreadedGD::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

