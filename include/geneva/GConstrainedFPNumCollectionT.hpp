/**
 * @file
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

#pragma once

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard header files go here
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <type_traits>

// Boost header files go here
#include <boost/math/special_functions/next.hpp> // Needed so we can calculate the next representable value smaller than a given upper boundary

// Geneva header files go here
#include "common/GExceptions.hpp"
#include "geneva/GDoubleGaussAdaptor.hpp"
#include "geneva/GObject.hpp"
#include "geneva/GConstrainedNumCollectionT.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * This class represents a collection of floating point values with common
 * boundaries, all modified using the same algorithm. The most likely types to
 * be stored in this class are double values. Note: If you want
 * to access or set the transformed value, use the value() and setValue()
 * functions. Using the subscript operator or at() function, or the
 * native iterator, will give you the "raw" data only.
 */
template <typename fp_type>
class GConstrainedFPNumCollectionT
	: public GConstrainedNumCollectionT<fp_type>
{
	 ///////////////////////////////////////////////////////////////////////
	 friend class boost::serialization::access;

	 template<typename Archive>
	 void serialize(Archive & ar, const unsigned int) {
		 using boost::serialization::make_nvp;
		 ar
		 & make_nvp("GConstrainedNumCollectionT",
			 boost::serialization::base_object<GConstrainedNumCollectionT<fp_type>>(*this));
	 }
	 ///////////////////////////////////////////////////////////////////////

	 // Make sure this class can only be instantiated if fp_type really is a floating point type
	 static_assert(
		 std::is_floating_point<fp_type>::value
		 , "fp_type must be a floating point type"
	 );

public:
	 /** @brief Specifies the type of parameters stored in this collection */
	 using collection_type = fp_type;

	 /***************************************************************************/
	 /**
	  * Initialize the lower and upper boundaries for data members of this class.
	  * Then set all positions to random values.
	  *
	  * @param size The desired size of the collection
	  * @param lowerBoundary The lower boundary for data members
	  * @param upperBoundary The upper boundary for data members
	  */
	 GConstrainedFPNumCollectionT (
		 const std::size_t& size
		 , const fp_type& lowerBoundary
		 , const fp_type& upperBoundary
	 )
		 : GConstrainedNumCollectionT<fp_type> (size, lowerBoundary, boost::math::float_prior<fp_type>(upperBoundary)) // Note that we define the upper boundary as "open"
	 {
		 Gem::Hap::GRandomT<Gem::Hap::RANDFLAVOURS::RANDOMLOCAL> gr;
		 typename std::uniform_real_distribution<fp_type> uniform_real_distribution(lowerBoundary,upperBoundary);

		 // Assign random values to each position
		 typename GConstrainedFPNumCollectionT<fp_type>::iterator it;
		 for(it=this->begin(); it!=this->end(); ++it) {
			 *it = uniform_real_distribution(gr);
		 }
	 }

	 /***************************************************************************/
	 /**
	  * Initialize the lower and upper boundaries for data members of this class.
	  * Set all positions to the same value. Note that we take the liberty to adapt val,
	  * if it is equal to the unmodified upper boundary. Otherwise you will get an
	  * error, where what you likely really meant was to start with the
	  * upper boundary.
	  *
	  * @param size The desired size of the collection
	  * @param val The value to be assigned to all positions
	  * @param lowerBoundary The lower boundary for data members
	  * @param upperBoundary The upper boundary for data members
	  */
	 GConstrainedFPNumCollectionT (
		 const std::size_t& size
		 , const fp_type& val
		 , const fp_type& lowerBoundary
		 , const fp_type& upperBoundary
	 )
		 : GConstrainedNumCollectionT<fp_type>(
		 size
		 , (val==upperBoundary?boost::math::float_prior<fp_type>(val):val)
		 , lowerBoundary
		 , boost::math::float_prior<fp_type>(upperBoundary)
	 ) // Note that we define the upper boundary as "open"
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * The standard copy constructor
	  */
	 GConstrainedFPNumCollectionT(const GConstrainedFPNumCollectionT<fp_type>& cp)
		 : GConstrainedNumCollectionT<fp_type> (cp)
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * The standard destructor
	  */
	 virtual ~GConstrainedFPNumCollectionT()
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * Searches for compliance with expectations with respect to another object
	  * of the same type
	  *
	  * @param cp A constant reference to another GObject object
	  * @param e The expected outcome of the comparison
	  * @param limit The maximum deviation for floating point values (important for similarity checks)
	  */
	 virtual void compare(
		 const GObject& cp
		 , const Gem::Common::expectation& e
		 , const double& limit
	 ) const override {
		 using namespace Gem::Common;

		 // Check that we are dealing with a GConstrainedFPNumCollectionT<fp_type>  reference independent of this object and convert the pointer
		 const GConstrainedFPNumCollectionT<fp_type> *p_load = Gem::Common::g_convert_and_compare<GObject, GConstrainedFPNumCollectionT<fp_type>>(cp, this);

		 GToken token("GConstrainedNumCollectionT<fp_type>", e);

		 // Compare our parent data ...
		 Gem::Common::compare_base<GConstrainedNumCollectionT<fp_type>>(IDENTITY(*this, *p_load), token);

		 // ... no local data

		 // React on deviations from the expectation
		 token.evaluate();
	 }

	 /****************************************************************************/
	 /**
	  * The transfer function needed to calculate the externally visible value.
	  *
	  * @param val The value to which the transformation should be applied
	  * @return The transformed value
	  */
	 fp_type transfer(const fp_type& val) const override {
		 fp_type lowerBoundary = GConstrainedNumCollectionT<fp_type>::getLowerBoundary();
		 fp_type upperBoundary = GConstrainedNumCollectionT<fp_type>::getUpperBoundary();

		 if(val >= lowerBoundary && val < upperBoundary) {
			 return val;
		 }
		 else {
			 // Find out which region the value is in (compare figure transferFunction.pdf
			 // that should have been delivered with this software). Note that boost::numeric_cast<>
			 // may throw - exceptions must be caught in surrounding functions.
			 std::int32_t region = 0;

#ifdef DEBUG
			 region =	boost::numeric_cast<std::int32_t>(Gem::Common::gfloor((fp_type(val) - fp_type(lowerBoundary)) / (fp_type(upperBoundary) - fp_type(lowerBoundary))));
#else
			 region =	static_cast<std::int32_t>(Gem::Common::gfloor((fp_type(val) - fp_type(lowerBoundary)) / (fp_type(upperBoundary) - fp_type(lowerBoundary))));
#endif

			 // Check whether we are in an odd or an even range and calculate the
			 // external value accordingly
			 fp_type mapping = fp_type(0.);
			 if(region%2 == 0) { // can it be divided by 2 ? Region 0,2,... or a negative even range
				 mapping = val - fp_type(region) * (upperBoundary - lowerBoundary);
			 } else { // Range 1,3,... or a negative odd range
				 mapping = -val + (fp_type(region-1)*(upperBoundary - lowerBoundary) + 2*upperBoundary);
			 }

			 return mapping;
		 }

		 // Make the compiler happy
		 return fp_type(0.);
	 }

protected:
	 /***************************************************************************/
	 /**
	  * Loads the data of another GConstrainedFPNumCollectionT<fp_type> object,
	  * camouflaged as a GObject. We have no local data, so
	  * all we need to do is to the standard identity check,
	  * preventing that an object is assigned to itself.
	  *
	  * @param cp A copy of another GConstrainedFPNumCollectionT<fp_type> object, camouflaged as a GObject
	  */
	 void load_(const GObject *cp) override {
		 // Check that we are dealing with a GConstrainedFPNumCollectionT<fp_type>  reference independent of this object and convert the pointer
		 const GConstrainedFPNumCollectionT<fp_type> *p_load = Gem::Common::g_convert_and_compare<GObject, GConstrainedFPNumCollectionT<fp_type>>(cp, this);

		 // Load our parent class'es data ...
		 GConstrainedNumCollectionT<fp_type>::load_(cp);

		 // ... no local data
	 }

	 /***************************************************************************/
	 /**
	  * Triggers random initialization of the parameter collection
	  */
	 virtual bool randomInit_(
		 const activityMode&
		 , Gem::Hap::GRandomBase& gr
	 ) override {
		 typename std::uniform_real_distribution<fp_type> uniform_real_distribution(
			 GConstrainedNumCollectionT<fp_type>::getLowerBoundary()
			 , GConstrainedNumCollectionT<fp_type>::getUpperBoundary()
		 );
		 for(std::size_t pos=0; pos<this->size(); pos++) {
			 this->setValue(pos, uniform_real_distribution(gr));
		 }

		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * The default constructor. Intentionally protected, as it is only
	  * needed for de-serialization and as the basis for derived class'es
	  * default constructors.
	  */
	 GConstrainedFPNumCollectionT()
		 : GConstrainedNumCollectionT<fp_type> ()
	 { /* nothing */ }

private:
	 /***************************************************************************/
	 /**
	  * Emits a name for this class / object
	  */
	 std::string name_() const override {
		 return std::string("GConstrainedFPNumCollectionT");
	 }
	 /***************************************************************************/
	 /** @brief Creates a deep copy of this object */
	 GObject *clone_() const override = 0;

public:
	 /***************************************************************************/
	 /**
	  * Applies modifications to this object. This is needed for testing purposes
	  *
	  * @return A boolean which indicates whether modifications were made
	  */
	 bool modify_GUnitTests() override {
#ifdef GEM_TESTING
		 bool result = false;

		 // Call the parent classes' functions
		 if(GConstrainedNumCollectionT<fp_type>::modify_GUnitTests()) result = true;

		 return result;
#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
		 Gem::Common::condnotset("GConstrainedFPNumCollectionT<>::modify_GUnitTests", "GEM_TESTING");
		return false;
#endif /* GEM_TESTING */
	 }

	 /***************************************************************************/
	 /**
	  * Performs self tests that are expected to succeed. This is needed for testing purposes
	  */
	 void specificTestsNoFailureExpected_GUnitTests() override {
#ifdef GEM_TESTING
		 // Call the parent classes' functions
		 GConstrainedNumCollectionT<fp_type>::specificTestsNoFailureExpected_GUnitTests();
#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
		 Gem::Common::condnotset("GConstrainedFPNumCollectionT<>::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
	 }

	 /***************************************************************************/
	 /**
	  * Performs self tests that are expected to fail. This is needed for testing purposes
	  */
	 void specificTestsFailuresExpected_GUnitTests() override {
#ifdef GEM_TESTING
		 // Call the parent classes' functions
		 GConstrainedNumCollectionT<fp_type>::specificTestsFailuresExpected_GUnitTests();
#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
		 Gem::Common::condnotset("GConstrainedFPNumCollectionT<>::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
	 }
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

/******************************************************************************/
// The content of BOOST_SERIALIZATION_ASSUME_ABSTRACT(T)

namespace boost {
namespace serialization {
template<typename fp_type>
struct is_abstract<Gem::Geneva::GConstrainedFPNumCollectionT<fp_type>> : public boost::true_type {};
template<typename fp_type>
struct is_abstract< const Gem::Geneva::GConstrainedFPNumCollectionT<fp_type>> : public boost::true_type {};
}
}
/******************************************************************************/

