/**
 * @file GDoubleObject.cpp
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
#include "geneva/GDoubleObject.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GDoubleObject)

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The default constructor
 */
GDoubleObject::GDoubleObject()
{ /* nothing */ }

/******************************************************************************/
/**
 * The copy constructor
 *
 * @param cp A copy of another GDoubleObject object
 */
GDoubleObject::GDoubleObject(const GDoubleObject& cp)
	: GNumFPT<double>(cp)
{ /* nothing */ }

/******************************************************************************/
/**
 * Initialization by contained value
 *
 * @param val A value used for the initialization
 */
GDoubleObject::GDoubleObject(const double& val)
	: GNumFPT<double>(val)
{ /* nothing */ }

/******************************************************************************/
/**
 * Initialization by random number in a given range
 *
 * @param lowerBoundary The lower boundary for the random number used in the initialization
 * @param upperBoundary The upper boundary for the random number used in the initialization
 */
GDoubleObject::GDoubleObject(
		const double& lowerBoundary
		, const double& upperBoundary
)
	: GNumFPT<double>(lowerBoundary, upperBoundary)
{ /* nothing */ }

/******************************************************************************/
/**
 * Initialization with a fixed value and the range for random initialization
 *
 * @param val The value to be assigned to the object
 * @param lowerBoundary The lower boundary for random initialization
 * @param upperBoundary The upper boundary for random initialization
 */
GDoubleObject::GDoubleObject(
		const double& val
		, const double& lowerBoundary
		, const double& upperBoundary
)
	: GNumFPT<double>(val, lowerBoundary, upperBoundary)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GDoubleObject::~GDoubleObject()
{ /* nothing */ }

/******************************************************************************/
/**
 * An assignment operator
 *
 * @param val The value to be assigned to this object
 * @return The value that was just assigned to this object
 */
double GDoubleObject::operator=(const double& val) {
	return GNumFPT<double>::operator=(val);
}

/******************************************************************************/
/**
 * A standard assignment operator.
 *
 * @param cp A copy of another GDoubleObject object
 * @return A constant reference to this object
 */
const GDoubleObject& GDoubleObject::operator=(const GDoubleObject& cp){
	GDoubleObject::load_(&cp);
	return *this;
}

/******************************************************************************/
/**
 * Creates a deep clone of this object.
 *
 * @return A copy of this object, camouflaged as a GObject
 */
GObject* GDoubleObject::clone_() const {
	return new GDoubleObject(*this);
}

/******************************************************************************/
/**
 * Checks for equality with another GDoubleObject object
 *
 * @param  cp A constant reference to another GDoubleObject object
 * @return A boolean indicating whether both objects are equal
 */
bool GDoubleObject::operator==(const GDoubleObject& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of equality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_EQUALITY, 0.,"GDoubleObject::operator==","cp", CE_SILENT);
}

/******************************************************************************/
/**
 * Checks for inequality with another GDoubleObject object
 *
 * @param  cp A constant reference to another GDoubleObject object
 * @return A boolean indicating whether both objects are inequal
 */
bool GDoubleObject::operator!=(const GDoubleObject& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of inequality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_INEQUALITY, 0.,"GDoubleObject::operator!=","cp", CE_SILENT);
}

/******************************************************************************/
/**
 * Checks whether a given expectation for the relationship between this object and another object
 * is fulfilled.
 *
 * @param cp A constant reference to another object, camouflaged as a GObject
 * @param e The expected outcome of the comparison
 * @param limit The maximum deviation for floating point values (important for similarity checks)
 * @param caller An identifier for the calling entity
 * @param y_name An identifier for the object that should be compared to this one
 * @param withMessages Whether or not information should be emitted in case of deviations from the expected outcome
 * @return A boost::optional<std::string> object that holds a descriptive string if expectations were not met
 */
boost::optional<std::string> GDoubleObject::checkRelationshipWith(const GObject& cp,
   const Gem::Common::expectation& e
   , const double& limit
   , const std::string& caller
   , const std::string& y_name
   , const bool& withMessages
) const {
    using namespace Gem::Common;

    // Check that we are not accidently assigning this object to itself
    GObject::selfAssignmentCheck<GDoubleObject>(&cp);

	// Will hold possible deviations from the expectation, including explanations
    std::vector<boost::optional<std::string> > deviations;

	// Check our parent class'es data ...
	deviations.push_back(GNumFPT<double>::checkRelationshipWith(cp, e, limit, "GDoubleObject", y_name, withMessages));

	// no local data ...

	return evaluateDiscrepancies("GDoubleObject", caller, deviations, e);
}

/***********************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string GDoubleObject::name() const {
   return std::string("GDoubleObject");
}

/******************************************************************************/
/**
 * Attach our local value to the vector. This is used to collect all parameters of this type
 * in the sequence in which they were registered.
 *
 * @param parVec The vector to which the local value should be attached
 */
void GDoubleObject::doubleStreamline(
   std::vector<double>& parVec
   , const activityMode& am
) const {
	parVec.push_back(this->value());
}

/******************************************************************************/
/**
 * Attach our local value to the map.
 *
 * @param parVec The map to which the local value should be attached
 */
void GDoubleObject::doubleStreamline(
   std::map<std::string, std::vector<double> >& parVec
   , const activityMode& am
) const {
#ifdef DEBUG
   if((this->getParameterName()).empty()) {
      glogger
      << "In GDoubleObject::doubleStreamline(std::map<std::string, std::vector<double> >& parVec) const: Error!" << std::endl
      << "No name was assigned to the object" << std::endl
      << GEXCEPTION;
   }
#endif /* DEBUG */

   std::vector<double> parameters;
   parameters.push_back(this->value());
   parVec[this->getParameterName()] = parameters;
}



/******************************************************************************/
/**
 * Attach boundaries of type double to the vectors. Since this is an unbounded type,
 * we use the initialization boundaries as a replacement.
 *
 * @param lBndVec A vector of lower double parameter boundaries
 * @param uBndVec A vector of upper double parameter boundaries
 */
void GDoubleObject::doubleBoundaries(
   std::vector<double>& lBndVec
   , std::vector<double>& uBndVec
   , const activityMode& am
) const {
	lBndVec.push_back(this->getLowerInitBoundary());
	uBndVec.push_back(this->getUpperInitBoundary());
}

/******************************************************************************/
/**
 * Tell the audience that we own a double value
 *
 * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
 * @return The number 1, as we own a single double parameter
 */
std::size_t GDoubleObject::countDoubleParameters(
   const activityMode& am
) const {
   return 1;
}

/******************************************************************************/
/**
 * Assigns part of a value vector to the parameter
 */
void GDoubleObject::assignDoubleValueVector(
   const std::vector<double>& parVec
   , std::size_t& pos
   , const activityMode& am
) {
#ifdef DEBUG
	// Do we have a valid position ?
	if(pos >= parVec.size()) {
	   glogger
	   << "In GDoubleObject::assignDoubleValueVector(const std::vector<double>&, std::size_t&):" << std::endl
      << "Tried to access position beyond end of vector: " << parVec.size() << "/" << pos << std::endl
      << GEXCEPTION;
	}
#endif

	this->setValue(parVec[pos]);
	pos++;
}

/******************************************************************************/
/**
 * Assigns part of a value map to the parameter
 */
void GDoubleObject::assignDoubleValueVectors(
   const std::map<std::string, std::vector<double> >& parMap
   , const activityMode& am
) {
   this->setValue((Gem::Common::getMapItem(parMap,this->getParameterName())).at(0));
}

/******************************************************************************/
/**
 * Loads the data of another GObject
 *
 * @param cp A copy of another GDoubleObject object, camouflaged as a GObject
 */
void GDoubleObject::load_(const GObject* cp){
    // Check that we are not accidently assigning this object to itself
    GObject::selfAssignmentCheck<GDoubleObject>(cp);

	// Load our parent class'es data ...
	GNumFPT<double>::load_(cp);

	// ... no local data
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool GDoubleObject::modify_GUnitTests() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if(GNumFPT<double>::modify_GUnitTests()) result = true;

	return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GDoubleObject::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GDoubleObject::specificTestsNoFailureExpected_GUnitTests() {
#ifdef GEM_TESTING
	// A few settings
	const std::size_t nTests = 10000;

	// Make sure we have an appropriate adaptor loaded when performing these tests
	bool adaptorStored = false;
	boost::shared_ptr<GAdaptorT<double> > storedAdaptor;

	if(this->hasAdaptor()) {
		storedAdaptor = this->getAdaptor();
		adaptorStored = true;
	}

	boost::shared_ptr<GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(0.025, 0.1, 0., 0.5, 1.0));
	gdga_ptr->setAdaptionThreshold(0); // Make sure the adaptor's internal parameters don't change through the adaption
	gdga_ptr->setAdaptionMode(true); // Always adapt
	this->addAdaptor(gdga_ptr);

	// Call the parent class'es function
	GNumFPT<double>::specificTestsNoFailureExpected_GUnitTests();

	// --------------------------------------------------------------------------

	{ // Test of GParameterT<T>'s methods for setting and retrieval of values
		boost::shared_ptr<GDoubleObject> p_test = this->clone<GDoubleObject>();

		for(double d=0.; d<10; d+=0.01) {
			BOOST_CHECK_NO_THROW((*p_test) = d); // Setting using operator=()
			BOOST_CHECK(p_test->value() == d); // Retrieval through the value() function
			BOOST_CHECK_NO_THROW(p_test->setValue(d)); // Setting using the setValue() function
			BOOST_CHECK(p_test->value() == d); // Retrieval through the value() function
			BOOST_CHECK_NO_THROW(p_test->setValue_(d)); // Setting using the protected constant setValue_() function
			BOOST_CHECK(p_test->value() == d); // Retrieval through the value() function
		}
	}

	// --------------------------------------------------------------------------

	{ // Test automatic conversion to the target type, using GParameterT<T>'s operator T()
		boost::shared_ptr<GDoubleObject> p_test = this->clone<GDoubleObject>();

		double target = -1.;
		for(double d=0.; d<10; d+=0.01) {
			BOOST_CHECK_NO_THROW(p_test->setValue(d)); // Setting using the setValue() function
			BOOST_CHECK_NO_THROW(target = *p_test); // Automatic conversion
			BOOST_CHECK(target == d); // Cross-check
		}
	}

	// --------------------------------------------------------------------------

	{ // Test the GParameterT<T>::adaptImpl() implementation
		boost::shared_ptr<GDoubleObject> p_test = this->clone<GDoubleObject>();

		if(p_test->hasAdaptor()) {
			BOOST_CHECK_NO_THROW(*p_test = 1.);
			double origVal = *p_test;
			BOOST_CHECK(*p_test == 1.);
			BOOST_CHECK(origVal == 1.);

			for(std::size_t i=0; i<nTests; i++) {
				BOOST_CHECK_NO_THROW(p_test->adaptImpl());
				BOOST_CHECK(origVal != *p_test); // Should be different
				BOOST_CHECK_NO_THROW(origVal = *p_test);
			}
		}
	}

	// --------------------------------------------------------------------------

	{ // Test resetting, adding and retrieval of adaptors in GParameterBaseWithAdaptorsT<T>
		boost::shared_ptr<GDoubleObject> p_test = this->clone<GDoubleObject>();

		// Reset the local adaptor to its pristine condition
		BOOST_CHECK_NO_THROW(p_test->resetAdaptor());

		// Add a new adaptor. This should clone the adaptor
		BOOST_CHECK_NO_THROW(p_test->addAdaptor(gdga_ptr));

		// Check that we indeed have an adaptor (should always be the case)
		BOOST_CHECK(p_test->hasAdaptor() == true);

		// Retrieve a pointer to the adaptor
		boost::shared_ptr<GAdaptorT<double> > p_adaptor_base;
		BOOST_CHECK(!p_adaptor_base);
		BOOST_CHECK_NO_THROW(p_adaptor_base = p_test->getAdaptor());

		// Check that we have indeed received an adaptor
		BOOST_CHECK(p_adaptor_base);

		// Retrieve another, converted pointer to the adaptor
		boost::shared_ptr<GDoubleGaussAdaptor> gdga_clone_ptr;
		BOOST_CHECK(!gdga_clone_ptr);
		BOOST_CHECK_NO_THROW(gdga_clone_ptr = p_test->getAdaptor<GDoubleGaussAdaptor>());

		// Check that we have indeed received an adaptor
		BOOST_CHECK(gdga_clone_ptr);

		// The address of the original adaptor and of this one should differ
		BOOST_CHECK(gdga_clone_ptr.get() != gdga_ptr.get());

		// The adaptors should otherwise be identical
		BOOST_CHECK(*gdga_clone_ptr == *gdga_ptr);
	}

	// --------------------------------------------------------------------------

	{ // Test that retrieval of adaptor doesn't throw in GParameterBaseWithAdaptorsT<T>::getAdaptor() after calling resetAdaptor() (Note: This is the non-templated version of the function)
		boost::shared_ptr<GDoubleObject> p_test = this->clone<GDoubleObject>();

		// Make sure the adaptor is in pristine condition
		BOOST_CHECK_NO_THROW(p_test->resetAdaptor());
		BOOST_CHECK(p_test->hasAdaptor() == true);
		BOOST_CHECK_NO_THROW(p_test->getAdaptor());
	}

	// --------------------------------------------------------------------------

	{ // Test that retrieval of an adaptor doesn't throw in GParameterBaseWithAdaptorsT<T>::getAdaptor<>() after calling resetAdaptor() (Note: This is the templated version of the function)
		boost::shared_ptr<GDoubleObject> p_test = this->clone<GDoubleObject>();

		// Make sure no adaptor is present
		BOOST_CHECK_NO_THROW(p_test->resetAdaptor());
		BOOST_CHECK(p_test->hasAdaptor() == true);
		BOOST_CHECK_NO_THROW(p_test->getAdaptor<GDoubleGaussAdaptor>());
	}

   // Remove the test adaptor
   this->resetAdaptor();

   // Load the old adaptor, if needed
   if(adaptorStored) {
      this->addAdaptor(storedAdaptor);
   }


   // --------------------------------------------------------------------------

   { // Check that construction with initialization boundaries leads to random content

      double previous = -1.;
      for(std::size_t i=0; i<10; i++) {
         GDoubleObject p(0., 10000000.);
         BOOST_CHECK(p.value() != previous);
         previous = p.value();
      }
   }

   // --------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GDoubleObject::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GDoubleObject::specificTestsFailuresExpected_GUnitTests() {
#ifdef GEM_TESTING
	// Make sure we have an appropriate adaptor loaded when performing these tests
	bool adaptorStored = false;
	boost::shared_ptr<GAdaptorT<double> > storedAdaptor;

	if(this->hasAdaptor()) {
		storedAdaptor = this->getAdaptor();
		adaptorStored = true;
	}

	boost::shared_ptr<GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(0.025, 0.1, 0., 0.5, 1.0));
	gdga_ptr->setAdaptionThreshold(0); // Make sure the adaptor's internal parameters don't change through the adaption
	gdga_ptr->setAdaptionMode(true); // Always adapt
	this->addAdaptor(gdga_ptr);

	// Call the parent class'es function
	GNumFPT<double>::specificTestsFailuresExpected_GUnitTests();

	// --------------------------------------------------------------------------

	{ // Test of GParameterBaseWithAdaptorsT<T>::addAdaptor() in case of an empty adaptor pointer
		boost::shared_ptr<GDoubleObject> p_test = this->clone<GDoubleObject>();

		// Make sure the object is in pristine condition
		BOOST_CHECK_NO_THROW(p_test->resetAdaptor());

		// Add an empty boost::shared_ptr<GDoubleGaussAdaptor>. This should throw
		BOOST_CHECK_THROW(p_test->addAdaptor(boost::shared_ptr<GDoubleGaussAdaptor>()), Gem::Common::gemfony_error_condition);
	}

	// --------------------------------------------------------------------------

	// Remove the test adaptor
	this->resetAdaptor();

	// Load the old adaptor, if needed
	if(adaptorStored) {
		this->addAdaptor(storedAdaptor);
	}

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GDoubleObject::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */
