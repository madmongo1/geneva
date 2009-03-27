/**
 * @file GIndividualSet.hpp
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

// Boost header files go here

#include <boost/version.hpp>
#include "GGlobalDefines.hpp"
#if BOOST_VERSION < ALLOWED_BOOST_VERSION
#error "Error: Boost has incorrect version !"
#endif /* BOOST_VERSION */

#include <boost/cstdint.hpp>


#ifndef GINDIVIDUALSET_HPP_
#define GINDIVIDUALSET_HPP_

// GenEvA headers go here
#include "GMutableSetT.hpp"
#include "GIndividual.hpp"
#include "GObject.hpp"

namespace Gem {
namespace GenEvA {

class GIndividualSet
	:public GMutableSetT<Gem::GenEvA::GIndividual>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int version){
	  using boost::serialization::make_nvp;
	  ar & make_nvp("GMutableSetT_GIndividual",
			  boost::serialization::base_object<GMutableSetT<Gem::GenEvA::GIndividual> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GIndividualSet();
	/** @brief The copy constructor */
	GIndividualSet(const GIndividualSet&);
	/** @brief The destructor */
	virtual ~GIndividualSet();

	/** @brief Creates a deep clone of this object */
	virtual GObject* clone() = 0;
	/** @brief Loads the data of another GObject */
	virtual void load(const GObject*);

	/**********************************************************************/
	/**
	 * If compiled in debug mode, this function performs all necessary error
	 * checks on the conversion from GIndividual to the desired parameter type.
	 * Note that, if compiled in debug mode, this function will throw. Otherwise
	 * a segfault may be the result if a faulty conversion was attempted. Hence
	 * it is suggested to test your programs in debug mode before using it in a
	 * production environment.
	 *
	 * @param pos The position in our data array that shall be converted
	 * @return A converted version of the GIndividual object, as required by the user
	 */
	template <typename parameter_type>
	inline boost::shared_ptr<parameter_type> individual_cast(const std::size_t& pos){
#ifdef DEBUG
		// Extract data. at() will throw if we have tried to access a position in the
		// vector that does not exist.
		boost::shared_ptr<GIndividual> data_base = data.at(pos);

		// Convert to the desired target type
		boost::shared_ptr<parameter_type> p_load = boost::dynamic_pointer_cast<parameter_type>(data_base);

		// Check that the conversion worked. dynamic_cast emits an empty pointer,
		// if this was not the case.
		if(!p_load){
			std::ostringstream error;
			error << "In GIndividualSet::individual_cast<parameter_type>() : Conversion error!" << std::endl;

			// throw an exception. Add some information so that if the exception
			// is caught through a base object, no information is lost.
			throw geneva_error_condition(error.str());
		}

		return p_load;
#else
		return boost::static_pointer_cast<parameter_type>(data[pos]);
#endif /* DEBUG */
	}

protected:
	/**********************************************************************************/
	/** @brief The actual fitness calculation takes place here */
	virtual double fitnessCalculation() = 0;
};

} /* namespace GenEvA */
} /* namespace Gem */

/**************************************************************************************************/
/**
 * Needed for Boost.Serialization
 */
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Gem::GenEvA::GIndividualSet)

/**************************************************************************************************/

#endif /* GINDIVIDUALSET_HPP_ */
