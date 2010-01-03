/**
 * @file GEvolutionaryAlgorithm.hpp
 */

/* Copyright (C) Dr. Ruediger Berlich and Karlsruhe Institute of Technology
 * (University of the State of Baden-Wuerttemberg and National Laboratory
 * of the Helmholtz Association)
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Geneva library, Gemfony scientific's optimization
 * library.
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
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.com .
 */


// Standard headers go here
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cfloat>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost headers go here

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/cast.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>
#include <boost/cast.hpp>
#include <boost/filesystem.hpp>

/**
 * Check that we have support for threads
 */
#ifndef BOOST_HAS_THREADS
#error "Error: Support for multi-threading does not seem to be available."
#endif

#ifndef GEVOLUTIONARYALGORITHM_HPP_
#define GEVOLUTIONARYALGORITHM_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// GenEvA headers go here
#include "GIndividual.hpp"
#include "GOptimizationAlgorithm.hpp"
#include "GenevaExceptions.hpp"
#include "GEnums.hpp"

namespace Gem {
namespace GenEvA {

/**
 * The default sorting mode
 */
const sortingMode DEFAULTSMODE=MUPLUSNU;

/**
 * The default number of generations without improvement after which
 * a micro-training should be started. A value of 0 means that no
 * micro-training will take place.
 */
const boost::uint32_t DEFAULTMICROTRAININGINTERVAL=0;

/*********************************************************************************/
/**
 * The GEvolutionaryAlgorithm class adds the notion of parents and children to
 * the GOptimizationAlgorithm class. The evolutionary adaptation is realized
 * through the cycle of mutation, evaluation, and sorting, as defined in this
 * class.
 *
 * Populations are collections of individuals, which themselves are objects
 * exhibiting the GIndividual class' API, most notably the GIndividual::fitness() and
 * GIndividual::mutate() functions. Individuals can thus themselves be populations,
 * which can again contain populations, and so on.
 *
 * In order to add parents to an instance of this class use the default constructor,
 * then add at least one GIndividual to it, and call setPopulationSize(). The population
 * will then be "filled up" with missing individuals as required, before the optimization
 * starts. Note that this class will enforce a minimum, default number of children,
 * as implied by the population size and the number of parents set at the beginning.
 */
class GEvolutionaryAlgorithm
	:public GOptimizationAlgorithm
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar & make_nvp("GOptimizationAlgorithm",	boost::serialization::base_object<GOptimizationAlgorithm>(*this));
		ar & make_nvp("nParents_", nParents_);
		ar & make_nvp("microTrainingInterval_", microTrainingInterval_);
		ar & make_nvp("recombinationMethod_", recombinationMethod_);
		ar & make_nvp("smode_", smode_);
		ar & make_nvp("defaultNChildren_", defaultNChildren_);
		ar & make_nvp("oneTimeMuCommaNu_", oneTimeMuCommaNu_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GEvolutionaryAlgorithm();
	/** @brief A standard copy constructor */
	GEvolutionaryAlgorithm(const GEvolutionaryAlgorithm&);
	/** @brief The destructor */
	virtual ~GEvolutionaryAlgorithm();

	/** @brief A standard assignment operator */
	const GEvolutionaryAlgorithm& operator=(const GEvolutionaryAlgorithm&);

	/** @brief Loads the data of another population */
	virtual void load(const GObject *);
	/** @brief Creates a deep clone of this object */
	virtual GObject *clone() const;

	/** @brief Checks for equality with another GEvolutionaryAlgorithm object */
	bool operator==(const GEvolutionaryAlgorithm&) const;
	/** @brief Checks for inequality with another GEvolutionaryAlgorithm object */
	bool operator!=(const GEvolutionaryAlgorithm&) const;
	/** @brief Checks for equality with another GEvolutionaryAlgorithm object */
	virtual bool isEqualTo(const GObject&,  const boost::logic::tribool& expected = boost::logic::indeterminate) const;
	/** @brief Checks for similarity with another GEvolutionaryAlgorithm object */
	virtual bool isSimilarTo(const GObject&, const double&, const boost::logic::tribool& expected = boost::logic::indeterminate) const;

	/** @brief Emits information specific to this population */
	virtual void doInfo(const infoMode&);

	/** @brief Registers a function to be called when emitting information from doInfo */
	void registerInfoFunction(boost::function<void (const infoMode&, GEvolutionaryAlgorithm * const)>);

	/** @brief Sets population size and number of parents */
	void setPopulationSize(const std::size_t&, const std::size_t&);

	/** @brief Retrieve the number of parents in this population */
	std::size_t getNParents() const;
	/** @brief Retrieve the number of children in this population */
	std::size_t getNChildren() const;
	/** @brief Retrieves the defaultNChildren_ parameter */
	std::size_t getDefaultNChildren() const;

	/** @brief Set the sorting scheme for this population */
	void setSortingScheme(const sortingMode&);
	/** @brief Retrieve the current sorting scheme for this population */
	sortingMode getSortingScheme() const;

	/** @brief Specify, what recombination mode should be used */
	void setRecombinationMethod(const recoScheme&);
	/** @brief Find out, what recombination mode is being used */
	recoScheme getRecombinationMethod() const;

	/** @brief Loads a checkpoint from disk */
	virtual void loadCheckpoint(const std::string&);

	//------------------------------------------------------------------------------------------
	// Settings specific to micro-training
	/** @brief Set the interval in which micro training should be performed */
	void setMicroTrainingInterval(const boost::uint32_t&);
	/** @brief Retrieve the interval in which micro training should be performed */
	boost::uint32_t getMicroTrainingInterval() const;

	/**************************************************************************************************/
	/**
	 * Retrieves the best individual of the population and casts it to the desired type.
	 *
	 * @return A converted shared_ptr to the best (i.e. first) individual of the population
	 */
	template <typename individual_type>
	inline boost::shared_ptr<individual_type> getBestIndividual(){
		// Get a copy of the best individual
		boost::shared_ptr<GIndividual> p_base = this->data.at(0);

#ifdef DEBUG
		// Convert to the desired target type
		boost::shared_ptr<individual_type> p_load = boost::dynamic_pointer_cast<individual_type>(p_base);

		// Check that the conversion worked. dynamic_cast emits an empty pointer,
		// if this was not the case.
		if(!p_load){
			std::ostringstream error;
			error << "In GEvolutionaryAlgorithm::getBestIndividual<individual_type>() : Conversion error!" << std::endl;

			// throw an exception. Add some information so that if the exception
			// is caught through a base object, no information is lost.
			throw geneva_error_condition(error.str());
		}

		return p_load;
#else
		return boost::static_pointer_cast<individual_type>(p_base);
#endif /* DEBUG */
	}

	/**************************************************************************************************/
	/**
	 * Emits information about the population it has been given, using a simple format.  This is
	 * used in the micro training environment. Note that we are using a static member function in order
	 * to avoid storing a local "this" pointer in this function when registering it in the
	 * boost::function object. This might otherwise be problematic when copying the boost::function object.
	 *
	 * Far more sophisticated setups than this information function are possible, and in general
	 * it is recommended to register function objects instead of this function.
	 *
	 * @param im Indicates the information mode
	 * @param gbp A pointer to the population information should be emitted about
	 */
	static void simpleInfoFunction(const infoMode& im, GEvolutionaryAlgorithm * const gbp) {
		std::ostringstream information;

		switch(im){
		case INFOINIT: // nothing
			break;

		case INFOPROCESSING:
			{
				bool isDirty = false;

				information << std::setprecision(10) << "In iteration "<< gbp->getIteration() << ": " << gbp->data.at(0)->getCurrentFitness(isDirty);

				if(isDirty) {
					information << " (dirty flag is set)";
				}
				information << std::endl;
			}
			break;

		case INFOEND: // nothing
			break;
		}

		// Let the audience know
		std::cout << information.str();
	}

	/**************************************************************************************************/

protected:
	/** @brief Allows to set the personality type of the individuals */
	virtual void setIndividualPersonalities();

	/** @brief user-defined recombination scheme */
	void doRecombine();

	/** @brief Creates children from parents according to a predefined recombination scheme */
	virtual void recombine();
	/** @brief Mutates all children of this population */
	virtual void mutateChildren();
	/** @brief Selects the best children of the population */
	virtual void select();

	/** @brief Marks parents as parents and children as children */
	void markParents();
	/** @brief Lets individuals know about their position in the population */
	void markIndividualPositions();

	/** @brief The actual business logic to be performed during each iteration. Returns the best achieved fitness */
	virtual double cycleLogic();
	/** @brief Does some preparatory work before the optimization starts */
	virtual void init();

private:
	/** @brief Enforces a one-time selection policy of MUCOMMANU */
	void setOneTimeMuCommaNu();
	/** @brief Updates the parent's structure */
	bool updateParentStructure();

	/** @brief Saves the state of the class to disc. Private, as we do not want to accidently trigger value calculation  */
	virtual void saveCheckpoint() const;

	/** @brief Implements the RANDOMRECOMBINE recombination scheme */
	void randomRecombine(boost::shared_ptr<GIndividual>&);
	/** @brief Implements the VALUERECOMBINE recombination scheme */
	void valueRecombine(boost::shared_ptr<GIndividual>&, const std::vector<double>&);

	/** @brief Selection, MUPLUSNU style */
	void sortMuplusnuMode();
	/** @brief Selection, MUCOMMANU style */
	void sortMucommanuMode();
	/** @brief Selection, MUNU1PRETAIN style */
	void sortMunu1pretainMode();

	std::size_t nParents_; ///< The number of parents
	boost::uint32_t microTrainingInterval_; ///< The number of generations without improvements after which a micro training should be started
	recoScheme recombinationMethod_; ///< The chosen recombination method
	sortingMode smode_; ///< The chosen sorting scheme
	std::size_t defaultNChildren_; ///< Expected number of children
	bool oneTimeMuCommaNu_; ///< Specifies whether a one-time selection scheme of MUCOMMANU should be used

	boost::function<void (const infoMode&, GEvolutionaryAlgorithm * const)> infoFunction_; ///< Used to emit information with doInfo()
};

/*********************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */

#endif /* GEVOLUTIONARYALGORITHM_HPP_ */