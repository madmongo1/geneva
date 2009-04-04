/**
 * @file GParameterT.hpp
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
#include <sstream>
#include <cmath>

// Boost header files go here

#include <boost/version.hpp>
#include "GGlobalDefines.hpp"
#if BOOST_VERSION < ALLOWED_BOOST_VERSION
#error "Error: Boost has incorrect version !"
#endif /* BOOST_VERSION */

#include <boost/cstdint.hpp>

#ifndef GPARAMETERT_HPP_
#define GPARAMETERT_HPP_

// GenEvA header files go here
#include "GParameterBaseWithAdaptorsT.hpp"
#include "GObject.hpp"
#include "GLogger.hpp"
#include "GenevaExceptions.hpp"

namespace Gem {
namespace GenEvA {

/*******************************************************************************************/
/**
 * A class holding a single, mutable parameter - usually just an atomic value (double, long,
 * GenEvA::bit, ...). The class is non-virtual, so that it is possible to store simple values
 * in this class without too much fuss.
 */
template <typename T>
class GParameterT
	:public GParameterBaseWithAdaptorsT<T>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version){
	  using boost::serialization::make_nvp;
	  ar & make_nvp("GParameterBaseWithAdaptors_T", boost::serialization::base_object<GParameterBaseWithAdaptorsT<T> >(*this));
	  ar & make_nvp("val_", val_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/*******************************************************************************************/
	/**
	 * The default constructor
	 */
	GParameterT()
		:GParameterBaseWithAdaptorsT<T>(),
		 val_((T)NULL)
	{ /* nothing */ }

	/*******************************************************************************************/
	/**
	 * The copy constructor
	 *
	 * @param cp A copy of another GParameterT<T> object
	 */
	GParameterT(const GParameterT<T>& cp)
		:GParameterBaseWithAdaptorsT<T>(cp),
		 val_(cp.val_)
	{ /* nothing */	}

	/*******************************************************************************************/
	/**
	 * Initialization by contained value
	 *
	 * @param val The new value of val_
	 */
	explicit GParameterT(const T& val)
		:GParameterBaseWithAdaptorsT<T>(),
		 val_(val)
	{ /* nothing */	}

	/*******************************************************************************************/
	/**
	 * The standard destructor
	 */
	virtual ~GParameterT()
	{ /* nothing */ }

	/*******************************************************************************************/
	/**
	 * A standard assignment operator.
	 *
	 * @param cp A copy of another GParameterT object
	 * @return A constant reference to this object
	 */
	const GParameterT<T>& operator=(const GParameterT<T>& cp){
		GParameterT<T>::load(&cp);
		return *this;
	}

	/*******************************************************************************************/
	/**
	 * An assignment operator that allows us to set val_
	 *
	 * @param val The new value for val_
	 * @return The new value of val_
	 */
	virtual T operator=(const T& val){
		val_ = val;
		return val_;
	}

	/*******************************************************************************************/
	/**
	 * Automatic conversion to the target type
	 */
	operator T(){
		return this->value();
	}

	/*******************************************************************************************/
	/**
	 * Retrieval of the value
	 *
	 * @return The value of val_
	 */
	inline T value(){
		return val_;
	}

	/*******************************************************************************************/
	/**
	 * Creates a deep clone of this object.
	 *
	 * @return A copy of this object, camouflaged as a GObject
	 */
	virtual GObject* clone(){
		return new GParameterT<T>(*this);
	}

	/*******************************************************************************************/
	/**
	 * Checks for equality with another GParameterT<T> object
	 *
	 * @param  cp A constant reference to another GParameterT<T> object
	 * @return A boolean indicating whether both objects are equal
	 */
	bool operator==(const GParameterT<T>& cp) const {
		return GParameterT<T>::isEqualTo(cp);
	}

	/*******************************************************************************************/
	/**
	 * Checks for inequality with another GParameterT<T> object
	 *
	 * @param  cp A constant reference to another GParameterT<T> object
	 * @return A boolean indicating whether both objects are inequal
	 */
	bool operator!=(const GParameterT<T>& cp) const {
		return !GParameterT<T>::isEqualTo(cp);
	}

	/*******************************************************************************************/
	/**
	 * Checks for equality with another GParameterT<T> object.  If T is an object type,
	 * then it must implement operator!= .
	 *
	 * @param  cp A constant reference to another GParameterT<T> object
	 * @return A boolean indicating whether both objects are equal
	 */
	bool isEqualTo(const GParameterT<T>& cp) const {
		// Check equality of the parent class
		if(!GParameterBaseWithAdaptorsT<T>::isEqualTo(cp)) return false;

		// Check the local data
		if(val_ != cp.val_) return false;

		return true;
	}

	/*******************************************************************************************/
	/**
	 * Checks for similarity with another GParameterT<T> object.  As we do not know the
	 * type of T, we need to create a specialization of this function for typeof(T)==double
	 *
	 * @param  cp A constant reference to another GParameterT<T> object
	 * @param limit A double value specifying the acceptable level of differences of floating point values
	 * @return A boolean indicating whether both objects are similar to each other
	 */
	bool isSimilarTo(const GParameterT<T>& cp, const double& limit=0) const {
		// Check similarity of the parent class
		if(!GParameterBaseWithAdaptorsT<T>::isSimilarTo(cp, limit)) return false;

		// Check the local data
		if(val_ != cp.val_) return false;

		return true;
	}

	/*******************************************************************************************/
	/**
	 * Loads the data of another GObject
	 *
	 * @param cp A copy of another GParameterT<T> object, camouflaged as a GObject
	 */
	virtual void load(const GObject* cp){
		// Convert cp into local format
		const GParameterT<T> *gpt = conversion_cast(cp, this);

		// Load our parent class'es data ...
		GParameterBaseWithAdaptorsT<T>::load(cp);

		// ... and then our own data
		val_ = gpt->val_;
	}

	/*******************************************************************************************/
	/**
	 * Allows to mutate the value stored in this class. If more than one adaptor was registered,
	 * all will be applied to the value.
	 */
	virtual void mutate(){
		if(this->numberOfAdaptors() == 1){
			GParameterBaseWithAdaptorsT<T>::applyFirstAdaptor(val_);
		}
		else {
			GParameterBaseWithAdaptorsT<T>::applyAllAdaptors(val_);
		}
	}

protected:
	/*******************************************************************************************/
	/**
	 * Allows derivatives to set the internal value. Note that we assume here that T has an
	 * operator=() or is a basic value type, such as double or int.
	 *
	 * @param val The new T value stored in this class
	 */
	void setValue(const T& val) throw() {
		val_ = val;
	}

private:
	/*******************************************************************************************/

	T val_; ///< The internal representation of our value. It is protected as it needs to accessible to derived classes.
};


/*********************************************************************************************/
// Declaration of specializations for various types
/** @brief Checks similarity of this object, if typeof(T) == typeof(double) */
template<> bool GParameterT<double>::isSimilarTo(const GParameterT<double>& cp, const double& limit) const;
/** @brief A default constructor for bool, needed due to call to GRandom::boolRandom() during construction */
template <> GParameterT<bool>::GParameterT();
/** @brief A default constructor for bool, needed due to call to GRandom::charRandom() during construction */
template <> GParameterT<char>::GParameterT();
/** @brief A default constructor for boost::int32_t, needed due to call to GRandom::discreteRandom() during construction */
template <> GParameterT<boost::int32_t>::GParameterT();

/*********************************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */

#endif /* GPARAMETERT_HPP_ */
