/**
 * @file GBasePopulation.hpp
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

#ifndef GBASEPOPULATION_HPP_
#define GBASEPOPULATION_HPP_

// GenEvA headers go here

#include "GIndividual.hpp"
#include "GIndividualSet.hpp"
#include "GenevaExceptions.hpp"
#include "GEnums.hpp"

namespace Gem {
namespace GenEvA {

/**
 * The two const variables MAXIMIZE and MINIMIZE determine, whether the library
 * should work in maximization or minimization mode.
 */
const bool MAXIMIZE = true;
const bool MINIMIZE = false;

/**
 * The number of generations after which information should be
 * emitted about the inner state of the population.
 */
const boost::uint32_t DEFAULTREPORTGEN = 10;

/**
 * The number of generations after which a checkpoint should be written.
 * 0 means that no checkpoints are written at all.
 */
const boost::uint32_t DEFAULTCHECKPOINTGEN = 0;

/**
 * The default maximum number of generations
 */
const boost::uint32_t DEFAULTMAXGEN = 1000;

/**
 * The default maximum number of generations without improvement. 0 means: ignore
 */
const boost::uint32_t DEFAULMAXTSTALLGEN = 0;

/**
 * The default maximization mode
 */
const bool DEFAULTMAXMODE = false;

/**
 * A 0 time period . timedHalt will not trigger if this duration is set
 */
const std::string EMPTYDURATION = "00:00:00.000"; // 0 - no duration

/**
 * The default maximum duration of the calculation.
 */
const std::string DEFAULTDURATION = EMPTYDURATION;

/**
 * The default quality threshold
 */
const double DEFAULTQUALITYTHRESHOLD=0.;

/**
 * The default sorting mode
 */
const sortingMode DEFAULTSMODE=MUPLUSNU;

/*********************************************************************************/
/**
 * The GBasePopulation class adds the notion of parents and children to
 * the GIndividualSet class. The evolutionary adaptation is realized
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
class GBasePopulation
	:public GIndividualSet
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar & make_nvp("GIndividualSet",
				boost::serialization::base_object<GIndividualSet>(*this));
		ar & make_nvp("nParents_", nParents_);
		ar & make_nvp("popSize_", popSize_);
		ar & make_nvp("generation_", generation_);
		ar & make_nvp("maxGeneration_", maxGeneration_);
		ar & make_nvp("maxStallGeneration_", maxStallGeneration_);
		ar & make_nvp("reportGeneration_", reportGeneration_);
		ar & make_nvp("cpInterval_", cpInterval_);
		ar & make_nvp("cpBaseName_", cpBaseName_);
		ar & make_nvp("cpDirectory_", cpDirectory_);
		ar & make_nvp("recombinationMethod_", recombinationMethod_);
		ar & make_nvp("smode_", smode_);
		ar & make_nvp("maximize_", maximize_);
		ar & make_nvp("maxDuration_", maxDuration_);
		ar & make_nvp("defaultNChildren_", defaultNChildren_);
		ar & make_nvp("qualityThreshold_", qualityThreshold_);
		ar & make_nvp("hasQualityThreshold_", hasQualityThreshold_);

		// Note that id and firstId_ are not serialized as we need the id
		// to be recalculated for de-serialized objects. Likewise, startTime_
		// doesn't need to be serialized.
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GBasePopulation();
	/** @brief A standard copy constructor */
	GBasePopulation(const GBasePopulation&);
	/** @brief The destructor */
	virtual ~GBasePopulation();

	/** @brief A standard assignment operator */
	const GBasePopulation& operator=(const GBasePopulation&);

	/** @brief Loads the data of another population */
	virtual void load(const GObject *);
	/** @brief Creates a deep clone of this object */
	virtual GObject *clone() const;

	/** @brief Checks for equality with another GBasePopulation object */
	bool operator==(const GBasePopulation&) const;
	/** @brief Checks for inequality with another GBasePopulation object */
	bool operator!=(const GBasePopulation&) const;
	/** @brief Checks for equality with another GBasePopulation object */
	virtual bool isEqualTo(const GObject&,  const boost::logic::tribool& expected = boost::logic::indeterminate) const;
	/** @brief Checks for similarity with another GBasePopulation object */
	virtual bool isSimilarTo(const GObject&, const double&, const boost::logic::tribool& expected = boost::logic::indeterminate) const;

	/** @brief Loads the state of the class from disc */
	virtual void loadCheckpoint(const std::string&);

	/** @brief Allows to set the number of generations after which a checkpoint should be written */
	void setCheckpointInterval(const boost::int32_t&);
	/** @brief Allows to retrieve the number of generations after which a checkpoint should be written */
	boost::uint32_t getCheckpointInterval() const;

	/** @brief Allows to set the base name of the checkpoint file */
	void setCheckpointBaseName(const std::string&, const std::string&);
	/** @brief Allows to retrieve the base name of the checkpoint file */
	std::string getCheckpointBaseName() const;
	/** @brief Allows to retrieve the directory where checkpoint files should be stored */
	std::string getCheckpointDirectory() const;

	/** @brief The core function of the entire GenEvA library.
	 * Triggers the optimization of a population. */
	virtual void optimize();

	/** @brief Emits information specific to this population */
	virtual void doInfo(const infoMode&);

	/** @brief Registers a function to be called when emitting information from doInfo */
	void registerInfoFunction(boost::function<void (const infoMode&, GBasePopulation * const)>);

	/** @brief Sets population size and number of parents */
	void setPopulationSize(const std::size_t&, const std::size_t&);

	/** @brief Retrieve the number of parents in this population */
	std::size_t getNParents() const;
	/** @brief Retrieve the number of children in this population */
	std::size_t getNChildren() const;
	/** @brief Retrieve the current population size */
	std::size_t getPopulationSize() const;
	/** @brief Retrieves the defaultNChildren_ parameter */
	std::size_t getDefaultNChildren() const;
	/** @brief Retrieves the default population size */
	std::size_t getDefaultPopulationSize() const;

	/** @brief Set the sorting scheme for this population */
	void setSortingScheme(const sortingMode&);
	/** @brief Retrieve the current sorting scheme for this population */
	sortingMode getSortingScheme() const;

	/** @brief Set the number of generations after which sorting should be stopped */
	void setMaxGeneration(const boost::uint32_t&);
	/** @brief Retrieve the number of generations after which sorting should be stopped */
	boost::uint32_t getMaxGeneration() const;

	/** @brief Set the number of generations after which sorting should be stopped */
	void setMaxStallGeneration(const boost::uint32_t&);
	/** @brief Retrieve the number of generations after which sorting should be stopped */
	boost::uint32_t getMaxStallGeneration() const;

	/** @brief Get information about the current generation */
	boost::uint32_t getGeneration() const;

	/** @brief Sets the maximum allowed processing time */
	void setMaxTime(const boost::posix_time::time_duration&);
	/** @brief Retrieves the maximum allowed processing time */
	boost::posix_time::time_duration getMaxTime();

	/** @brief Sets a quality threshold beyond which optimization is expected to stop */
	void setQualityThreshold(const double&);
	/** @brief Retrieves the current value of the quality threshold */
	double getQualityThreshold(bool&) const;
	/** @brief Removes the quality threshold */
	void unsetQualityThreshold();
	/** @brief Checks whether a quality threshold has been set */
	bool hasQualityThreshold() const;

	/** @brief Specify whether we want to work in maximization or minimization mode */
	void setMaximize(const bool& val);
	/** @brief Find out whether we work in maximization or minimization mode */
	bool getMaximize() const;

	/** @brief Specify, what recombination mode should be used */
	void setRecombinationMethod(const recoScheme&);
	/** @brief Find out, what recombination mode is being used */
	recoScheme getRecombinationMethod() const;

	/** @brief Sets the number of generations after which the population should
	 * report about its inner state. */
	void setReportGeneration(const boost::uint32_t&);
	/** @brief Returns the number of generations after which the population should
	 * report about its inner state. */
	boost::uint32_t getReportGeneration() const;

	/** @brief Retrieve the id of this class */
	std::string getId();

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
			error << "In GBasePopulation::getBestIndividual<individual_type>() : Conversion error!" << std::endl;

			// throw an exception. Add some information so that if the exception
			// is caught through a base object, no information is lost.
			throw geneva_error_condition(error.str());
		}

		return p_load;
#else
		return boost::static_pointer_cast<individual_type>(p_base);
#endif
	}

	/**************************************************************************************************/
	/**
	 * Emits information about the population it has been given. This is the default
	 * information function provided for all populations. Information is emitted in the
	 * format of the ROOT analysis toolkit (see http://root.cern.ch). Note that we are
	 * using a static member function in order to avoid storing a local "this" pointer in
	 * this function when registering it in the boost::function object. This might otherwise
	 * be problematic when copying the boost::function object.
	 *
	 * @param im Indicates the information mode
	 * @param gbp A pointer to the population information should be emitted about
	 */
	static void defaultInfoFunction(const infoMode& im, GBasePopulation * const gbp) {
		std::ostringstream information;

		switch(im){
		case INFOINIT:
			information << "{" << std::endl
						<< "  TH1F *h" << gbp << " = new TH1F(\"h"
						<< gbp << "\",\"h" << gbp << "\"," << gbp->getMaxGeneration()+1
						<< ",0," << gbp->getMaxGeneration() << ");" << std::endl << std::endl;
			break;

		case INFOPROCESSING:
			{
				bool isDirty = false;

				information << std::setprecision(10) << "  h" << gbp
							<< "->Fill(" << gbp->getGeneration() << ", "
							<< gbp->data.at(0)->getCurrentFitness(isDirty) << ");";

				if(isDirty) {
					information << "// dirty!";
				}
				information << std::endl;
			}
			break;

		case INFOEND:
			information << std::endl
						<< "  h"<< gbp << "->Draw();" << std::endl
						<< "}" << std::endl;
			break;
		}

		// Let the audience know
		std::cout << information.str();
	}

	/**************************************************************************************************/

protected:
	/** @brief user-defined halt-criterium for the optimization */
	virtual bool customHalt();
	/** @brief user-defined recombination scheme */
	virtual void customRecombine();

	/** @brief Creates children from parents according to a predefined recombination scheme */
	virtual void recombine();
	/** @brief Mutates all children of this population */
	virtual void mutateChildren();
	/** @brief Selects the best children of the population */
	virtual void select();

	/** @brief The mutation scheme for this population */
	virtual void customMutations();
	/** @brief The evaluation scheme for this population */
	virtual double fitnessCalculation();

	/** @brief Marks parents as parents and children as children */
	void markParents();

	/** @brief Lets individuals know about the current generation */
	void markGeneration();

	/** @brief Lets individuals know about their position in the population */
	void markIndividualPositions();

private:
	/** @brief Saves the state of the class to disc. Private, as we do not want to accidently trigger value calculation  */
	virtual void saveCheckpoint() const;

	/** @brief Adjusts the actual population size to the desired value */
	void adjustPopulation();

	/** @brief Emits true once a given time has passed */
	bool timedHalt();
	/** @brief Emits true once the quality is below or above a given threshold */
	bool qualityHalt();
	/** @brief Determines when to stop the optimization */
	bool halt();

	/** @brief Helper function that determines whether a new value is better than an older one */
	bool isBetter(double, const double&) const;

	/** @brief Implements the RANDOMRECOMBINE recombination scheme */
	void randomRecombine(boost::shared_ptr<GIndividual>&);
	/** @brief Implements the VALUERECOMBINE recombination scheme */
	void valueRecombine(boost::shared_ptr<GIndividual>&, const std::vector<double>&);

	std::size_t nParents_; ///< The number of parents
	std::size_t popSize_; ///< The size of the population. Only used in adjustPopulation()
	boost::uint32_t generation_; ///< The current generation
	boost::uint32_t maxGeneration_; ///< The maximum number of generations
	boost::uint32_t maxStallGeneration_; ///< The maximum number of generations without improvement
	boost::uint32_t reportGeneration_; ///< Number of generations after which a report should be issued
	boost::int32_t cpInterval_; ///< Number of generations after which a checkpoint should be written. -1 means: Write whenever an improvement was encountered
	std::string cpBaseName_; ///< The base name of the checkpoint file
	std::string cpDirectory_; ///< The directory where checkpoint files should be stored
	recoScheme recombinationMethod_; ///< The chosen recombination method
	sortingMode smode_; ///< The chosen sorting scheme
	bool maximize_; ///< The optimization mode (minimization/false vs. maximization/true)
	std::string id_; ///< A unique id, used in networking contexts
	bool firstId_; ///< Is this the first call to getId() ?
	boost::posix_time::time_duration maxDuration_; ///< Maximum time frame for the optimization
	boost::posix_time::ptime startTime_; ///< Used to store the start time of the optimization
	std::size_t defaultNChildren_; ///< Expected number of children
	double qualityThreshold_; ///< A threshold beyond which optimization is expected to stop
	bool hasQualityThreshold_; ///< Specifies whether a qualityThreshold has been set

	boost::function<void (const infoMode&, GBasePopulation * const)> infoFunction_; ///< Used to emit information with doInfo()
};

/*********************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */

#endif /* GBASEPOPULATION_HPP_ */
