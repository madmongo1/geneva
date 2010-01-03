/**
 * @file GOptimizationAlgorithm.hpp
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

// Standard header files go here
#include <sstream>
#include <cmath>
#include <cfloat>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost header files go here

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

#ifndef GOPTIMIZATIONALGORITHM_HPP_
#define GOPTIMIZATIONALGORITHM_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// GenEvA headers go here
#include "GMutableSetT.hpp"
#include "GIndividual.hpp"
#include "GObject.hpp"
#include "GRandom.hpp"

namespace Gem {
namespace GenEvA {

/******************************************************************************************/
/**
 * The default base name used for check-pointing. Derivatives of this
 * class can build distinguished filenames from this e.g. by adding
 * the current generation.
 */
const std::string DEFAULTCPBASENAME = "geneva.cp";

/******************************************************************************************/
/**
 * The default directory used for check-pointing. We choose a directory
 * that will always exist.
 */
const std::string DEFAULTCPDIR = "./";

/******************************************************************************************/
/**
 * This class implements basic operations found in iteration-based optimization algorithms.
 * E.g., one might want to stop the optimization after a given number of cycles, or after
 * a given amount of time. The class also defines the interface functions found in these
 * algorithms, such as a general call to "optimize()".
 */
class GOptimizationAlgorithm
	:public GMutableSetT<Gem::GenEvA::GIndividual>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;
	  ar & make_nvp("GMutableSetT_GIndividual", boost::serialization::base_object<GMutableSetT<Gem::GenEvA::GIndividual> >(*this));
	  ar & make_nvp("gr", gr);
	  ar & make_nvp("iteration_", iteration_);
	  ar & make_nvp("maxIteration_", maxIteration_);
	  ar & make_nvp("maxStallIteration_", maxStallIteration_);
	  ar & make_nvp("reportIteration_", reportIteration_);
	  ar & make_nvp("defaultPopulationSize_", defaultPopulationSize_);
	  ar & make_nvp("bestPastFitness_", bestPastFitness_);
	  ar & make_nvp("maximize_", maximize_);
	  ar & make_nvp("stallCounter_", stallCounter_);
	  ar & make_nvp("cpInterval_", cpInterval_);
	  ar & make_nvp("cpBaseName_", cpBaseName_);
	  ar & make_nvp("cpDirectory_", cpDirectory_);
	  ar & make_nvp("qualityThreshold_", qualityThreshold_);
	  ar & make_nvp("hasQualityThreshold_", hasQualityThreshold_);
	  ar & make_nvp("maxDuration_", maxDuration_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GOptimizationAlgorithm();
	/** @brief The copy constructor */
	GOptimizationAlgorithm(const GOptimizationAlgorithm&);
	/** @brief The destructor */
	virtual ~GOptimizationAlgorithm();

	/** @brief Performs the necessary administratory work of doing check-pointing */
	void checkpoint(const bool&) const;
	/** @brief Loads the state of the class from disc */
	virtual void loadCheckpoint(const std::string&) = 0;

	/** @brief Check whether a better solution was found and update the stall counter as necessary */
	bool ifProgress(const double&);

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

	/** @brief Checks for equality with another GOptimizationAlgorithm object */
	bool operator==(const GOptimizationAlgorithm&) const;
	/** @brief Checks for inequality with another GOptimizationAlgorithm object */
	bool operator!=(const GOptimizationAlgorithm&) const;
	/** @brief Checks for equality with another GOptimizationAlgorithm object */
	virtual bool isEqualTo(const GObject&, const boost::logic::tribool& expected = boost::logic::indeterminate) const;
	/** @brief Checks for similarity with another GOptimizationAlgorithm object */
	virtual bool isSimilarTo(const GObject&, const double& limit, const boost::logic::tribool& expected = boost::logic::indeterminate) const;

	/** @brief Creates a deep clone of this object */
	virtual GObject* clone() const = 0;
	/** @brief Loads the data of another GObject */
	virtual void load(const GObject*);

	/** @brief Triggers the business logic of the optimization algorithm */
	virtual void optimize(const boost::uint32_t& startIteration = 0);

	/** @brief Emits information in regular intervals */
	virtual void doInfo(const infoMode& im);

	/** @brief Sets the nominal size of the population */
	virtual void setPopulationSize(const std::size_t&);
	/** @brief Retrieves the default population size */
	std::size_t getDefaultPopulationSize() const;
	/** @brief Retrieve the current population size */
	std::size_t getPopulationSize() const;

	/** @brief Set the number of iterations after which the optimization should be stopped */
	void setMaxIteration(const boost::uint32_t&);
	/** @brief Retrieve the number of iterations after which optimization should be stopped */
	boost::uint32_t getMaxIteration() const;

	/** @brief Set the number of iterations after which sorting should be stopped */
	void setMaxStallIteration(const boost::uint32_t&);
	/** @brief Retrieve the number of iterations after which sorting should be stopped */
	boost::uint32_t getMaxStallIteration() const;

	/** @brief Sets the maximum allowed processing time */
	void setMaxTime(const boost::posix_time::time_duration&);
	/** @brief Retrieves the maximum allowed processing time */
	boost::posix_time::time_duration getMaxTime() const;

	/** @brief Sets a quality threshold beyond which optimization is expected to stop */
	void setQualityThreshold(const double&);
	/** @brief Retrieves the current value of the quality threshold */
	double getQualityThreshold(bool&) const;
	/** @brief Removes the quality threshold */
	void unsetQualityThreshold();
	/** @brief Checks whether a quality threshold has been set */
	bool hasQualityThreshold() const;

	/** @brief Retrieve the current iteration of the optimization run */
	boost::uint32_t getIteration() const;

	/** @brief Sets the number of iterations after which the algorithm should
	 * report about its inner state. */
	void setReportIteration(const boost::uint32_t&);
	/** @brief Returns the number of iterations after which the algorithm should
	 * report about its inner state. */
	boost::uint32_t getReportIteration() const;

	/** @brief Retrieve the current number of failed optimization attempts */
	boost::uint32_t getStallCounter() const;
	/** @brief Gives access to the best known fitness so far */
	double getBestFitness() const;

	/** @brief Specify whether we want to work in maximization or minimization mode */
	void setMaximize(const bool&);
	/** @brief Find out whether we work in maximization or minimization mode */
	bool getMaximize() const;

	/** @brief Determines whether production of random numbers should happen remotely (RNRFACTORY) or locally (RNRLOCAL) */
	void setRnrGenerationMode(const Gem::Util::rnrGenerationMode&);
	/** @brief Retrieves the random number generators current generation mode. */
	Gem::Util::rnrGenerationMode getRnrGenerationMode() const;

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
	boost::shared_ptr<parameter_type> individual_cast(const std::size_t& pos){
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
			error << "In GOptimizationAlgorithm::individual_cast<parameter_type>() : Conversion error!" << std::endl;

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
	/** @brief Allows derived classes to set the personality type of the individuals */
	virtual void setIndividualPersonalities() = 0;
	/** @brief Resets the personality type of all individuals */
	void resetIndividualPersonalities();

	/** @brief Saves the state of the class to disc */
	virtual void saveCheckpoint() const = 0;

	/** @brief The actual business logic to be performed during each iteration. Returns the best achieved fitness */
	virtual double cycleLogic() = 0;

	/** @brief user-defined halt-criterion for the optimization */
	virtual bool customHalt() const;
	/** @brief The mutation scheme for this population */
	virtual void customMutations();
	/** @brief The evaluation scheme for this population */
	virtual double fitnessCalculation();

	/** @brief Allows derived classes to reset the stallCounter */
	void resetStallCounter();

	/** @brief Helper function that determines whether a new value is better than an older one */
	bool isBetter(double, const double&) const;

	/** @brief Allows derived classes to perform initialization work before the optimization clycle starts */
	virtual void init();
	/** @brief Allows derived classes to perform any remaining work after the optimization cycle has finished */
	virtual void finalize();

	/***********************************************************************************/
    /**
     * A random number generator. Note that the actual calculation is possibly
     * done in a random number server. GRandom also has a local generator
     * in case the factory is unreachable, or local storage of random
     * number containers requires too much memory.
     */
	Gem::Util::GRandom gr;

	/**********************************************************************************/
private:
	/** @brief Emits true once a given time has passed */
	bool timedHalt() const;
	/** @brief Emits true once the quality is below or above a given threshold (depending on the optimization mode) */
	bool qualityHalt() const;
	/** @brief Determines when to stop the optimization */
	bool halt(const boost::uint32_t&) const;

	/** @brief Sets the maximization mode of all individuals */
	void setIndividualMaxMode();
	/** @brief Lets individuals know about the current iteration */
	void markIteration();
	/** @brief Marks the globally best known fitness in all individuals */
	void markBestFitness();
	/** @brief Marks the number of stalled optimization attempts in all individuals */
	void markNStalls();

	/** @brief Resizes the population to the desired level and does some error checks */
	void adjustPopulation();

	boost::uint32_t iteration_; ///< The current iteration
	boost::uint32_t maxIteration_; ///< The maximum number of iterations
	boost::uint32_t maxStallIteration_; ///< The maximum number of generations without improvement, after which optimization is stopped
	boost::uint32_t reportIteration_; ///< The number of generations after which a report should be issued
	std::size_t defaultPopulationSize_; ///< The nominal size of the population
	double bestPastFitness_; ///< Records the best fitness found in past generations
	bool maximize_; ///< The optimization mode (minimization/false vs. maximization/true)
	boost::uint32_t stallCounter_; ///< Counts the number of iterations without improvement
	boost::int32_t cpInterval_; ///< Number of generations after which a checkpoint should be written. -1 means: Write whenever an improvement was encountered
	std::string cpBaseName_; ///< The base name of the checkpoint file
	std::string cpDirectory_; ///< The directory where checkpoint files should be stored
	double qualityThreshold_; ///< A threshold beyond which optimization is expected to stop
	bool hasQualityThreshold_; ///< Specifies whether a qualityThreshold has been set
	boost::posix_time::time_duration maxDuration_; ///< Maximum time frame for the optimization
	mutable boost::posix_time::ptime startTime_; ///< Used to store the start time of the optimization. Declared mutable so the halt criteria can be const
};

} /* namespace GenEvA */
} /* namespace Gem */

/**************************************************************************************************/
/**
 * Needed for Boost.Serialization
 */
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Gem::GenEvA::GOptimizationAlgorithm)

/**************************************************************************************************/

#endif /* GOPTIMIZATIONALGORITHM_HPP_ */