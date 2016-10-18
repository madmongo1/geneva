/**
 * @file GBaseParChildT.hpp
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here
#include <type_traits>
#include <tuple>

// Boost headers go here
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#ifndef GBASEPARCHILDEAT_HPP_
#define GBASEPARCHILDEAT_HPP_

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GHelperFunctionsT.hpp"
#include "geneva/GOptimizableEntity.hpp"
#include "geneva/GParameterSet.hpp"
#include "geneva/GOptimizationAlgorithmT.hpp"
#include "geneva/GOptimizationEnums.hpp"
#include "geneva/GBaseParChildPersonalityTraits.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The GBaseParChildT<ind_type> class adds the notion of parents and children to
 * the GOptimizationAlgorithmT<ind_type> class. The evolutionary adaptation is realized
 * through the cycle of adaption, evaluation, and sorting, as defined in this
 * class.
 *
 * It forms the base class for either multi populations (i.e. evolutionary algorithms
 * that may act on other optimization algorithms (including themselves), or a hierarchy of
 * algorithms acting on parameter objects.
 *
 * Populations are collections of individuals, which themselves are objects
 * exhibiting at least the GOptimizableEntity class' API, most notably the ind_type::fitness()
 * and GOptimizableEntity::adapt() functions.
 *
 * In order to add parents to an instance of this class use the default constructor,
 * then add at least one GOptimizableEntity-derivative to it, and call setPopulationSizes().
 * The population will then be "filled up" with missing individuals as required, before the
 * optimization starts.
 */
template <typename ind_type>
class GBaseParChildT
	: public GOptimizationAlgorithmT<ind_type>
{
	/////////////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar
		& make_nvp("GOptimizationAlgorithmT_ind_type", boost::serialization::base_object<GOptimizationAlgorithmT<ind_type>>(*this))
		& BOOST_SERIALIZATION_NVP(nParents_)
		& BOOST_SERIALIZATION_NVP(recombinationMethod_)
		& BOOST_SERIALIZATION_NVP(defaultNChildren_)
		& BOOST_SERIALIZATION_NVP(growthRate_)
		& BOOST_SERIALIZATION_NVP(maxPopulationSize_);
	}
	/////////////////////////////////////////////////////////////////////////////

	// Make sure ind_type is a derivative of GOptimizableEntity (or is GOptimizableEntity itself)
	static_assert(
		std::is_base_of<GOptimizableEntity, ind_type>::value
		, "GOptimizableEntity is no base type of ind_type"
	);

public:
	/***************************************************************************/
	/**
	 * The default constructor, As we do not have any individuals yet, we set the population
	 * size, and number of parents to 0. It is the philosophy of this class not
	 * to provide constructors for each and every use case. Instead, you should set
	 * vital parameters, such as the population size or the parent individuals by hand
	 * or do so through the configuration file.
	 */
	GBaseParChildT() : Gem::Geneva::GOptimizationAlgorithmT<ind_type>()
	{
		// Make sure we start with a valid population size if the user does not supply these values
		this->setPopulationSizes(100,1);
	}

	/***************************************************************************/
	/**
	 * A standard copy constructor. Note that the generation number is reset to 0 and
	 * is not copied from the other object. We assume that a new optimization run will
	 * be started.
	 *
	 * @param cp Another GBaseParChildT<ind_type> object
	 */
	GBaseParChildT(const GBaseParChildT<ind_type>& cp)
		: GOptimizationAlgorithmT<ind_type>(cp)
		, nParents_(cp.nParents_)
		, recombinationMethod_(cp.recombinationMethod_)
		, defaultNChildren_(cp.defaultNChildren_)
		, growthRate_(cp.growthRate_)
		, maxPopulationSize_(cp.maxPopulationSize_)
	{
		// Copying of individuals is done by the parent class
	}

	/***************************************************************************/
	/**
	 * The standard destructor. All work is done in the parent class.
	 */
	virtual ~GBaseParChildT()
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * The standard assignment operator
	 */
	const GBaseParChildT<ind_type>& operator=(const GBaseParChildT<ind_type>& cp) {
		this->load_(&cp);
		return *this;
	}

	/***************************************************************************/
	/**
	 * Checks for equality with another GBaseParChildT<ind_type> object
	 *
	 * @param  cp A constant reference to another GBaseParChildT<ind_type> object
	 * @return A boolean indicating whether both objects are equal
	 */
	bool operator==(const GBaseParChildT<ind_type>& cp) const {
		using namespace Gem::Common;
		try {
			this->compare(cp, Gem::Common::expectation::CE_EQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
			return true;
		} catch(g_expectation_violation&) {
			return false;
		}
	}

	/***************************************************************************/
	/**
	 * Checks for inequality with another GBaseParChildT<ind_type> object
	 *
	 * @param  cp A constant reference to another GBaseParChildT<ind_type> object
	 * @return A boolean indicating whether both objects are inequal
	 */
	bool operator!=(const GBaseParChildT<ind_type>& cp) const {
		using namespace Gem::Common;
		try {
			this->compare(cp, Gem::Common::expectation::CE_INEQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
			return true;
		} catch(g_expectation_violation&) {
			return false;
		}
	}

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

		// Check that we are dealing with a GBaseParChildT<ind_type>  reference independent of this object and convert the pointer
		const GBaseParChildT<ind_type> *p_load = Gem::Common::g_convert_and_compare<GObject, GBaseParChildT<ind_type>>(cp, this);

		GToken token("GBaseParChildT<ind_type>", e);

		// Compare our parent data ...
		Gem::Common::compare_base<GOptimizationAlgorithmT<ind_type>>(IDENTITY(*this, *p_load), token);

		// ... and then the local data
		compare_t(IDENTITY(nParents_, p_load->nParents_), token);
		compare_t(IDENTITY(recombinationMethod_, p_load->recombinationMethod_), token);
		compare_t(IDENTITY(defaultNChildren_, p_load->defaultNChildren_), token);
		compare_t(IDENTITY(maxPopulationSize_, p_load->maxPopulationSize_), token);
		compare_t(IDENTITY(growthRate_, p_load->growthRate_), token);

		// React on deviations from the expectation
		token.evaluate();
	}

	/***************************************************************************/
	/**
	 * Specifies the default size of the population plus the number of parents.
	 * The population will be filled with additional individuals later, as required --
	 * see GBaseParChildT<ind_type>::adjustPopulation() . Also, all error checking is done in
	 * that function.
	 *
	 * @param popSize The desired size of the population
	 * @param nParents The desired number of parents
	 */
	void setPopulationSizes(std::size_t popSize, std::size_t nParents) {
		GOptimizationAlgorithmT<ind_type>::setDefaultPopulationSize(popSize);
		nParents_ = nParents;
	}

	/***************************************************************************/
	/**
	 * Retrieve the number of parents as set by the user. This is a fixed parameter and
	 * should not be changed after it has first been set. Note that, if the size of the
	 * population is smaller than the alleged number of parents, the function will return
	 * the size of the population instead, thus interpreting its individuals as parents.
	 *
	 * @return The number of parents in the population
	 */
	std::size_t getNParents() const {
		return (std::min)(this->size(), nParents_);
	}

	/***************************************************************************/
	/**
	 * Calculates the current number of children from the number of parents and the
	 * size of the vector.
	 *
	 * @return The number of children in the population
	 */
	std::size_t getNChildren() const {
		if(this->size() <= nParents_) {
			// This will happen, when only the default population size has been set,
			// but no individuals have been added yet
			return 0;
		} else {
			return this->size() - nParents_;
		}
	}

	/***************************************************************************/
	/**
	 * Retrieves the defaultNChildren_ parameter. E.g. in GTransferPopulation::adaptChildren() ,
	 * this factor controls when a population is considered to be complete. The corresponding
	 * loop which waits for new arrivals will then be stopped, which in turn allows
	 * a new generation to start.
	 *
	 * @return The defaultNChildren_ parameter
	 */
	std::size_t getDefaultNChildren() const {
		return defaultNChildren_;
	}

	/**************************************************************************/
	/**
	 * Retrieve the number of processible items in the current iteration.
	 *
	 * @return The number of processible items in the current iteration
	 */
	virtual std::size_t getNProcessableItems() const override {
		std::tuple<std::size_t,std::size_t> range = this->getEvaluationRange();

#ifdef DEBUG
      if(std::get<1>(range) <= std::get<0>(range)) {
         glogger
         << "In GBaseParChildT<>::getNProcessableItems(): Error!" << std::endl
         << "Upper boundary of range <= lower boundary: " << std::get<1>(range) << "/" << std::get<0>(range) << std::endl
         << GEXCEPTION;
      }
#endif /* DEBUG */

		return std::get<1>(range) - std::get<0>(range);
	}

	/***************************************************************************/
	/**
	 * Lets the user set the desired recombination method. No sanity checks for the
	 * values are necessary, as we use an enum.
	 *
	 * @param recombinationMethod The desired recombination method
	 */
	void setRecombinationMethod(duplicationScheme recombinationMethod) {
		recombinationMethod_ = recombinationMethod;
	}

	/***************************************************************************/
	/**
	 * Retrieves the value of the recombinationMethod_ variable
	 *
	 * @return The value of the recombinationMethod_ variable
	 */
	duplicationScheme getRecombinationMethod() const {
		return recombinationMethod_;
	}

	/***************************************************************************/
	/**
	 * Loads the state of the class from disc. We do not load the entire population,
	 * but only the best individuals of a former optimization run, as these contain the
	 * "real" information.
	 */
	virtual void loadCheckpoint(const boost::filesystem::path& cpFile) override {
		// Create a vector to hold the best individuals
		std::vector<std::shared_ptr<ind_type>> bestIndividuals;

		// Check that the file indeed exists
		if(!boost::filesystem::exists(cpFile)) {
			glogger
			<< "In GBaseParChildT<ind_type>::loadCheckpoint(const bf::path&)" << std::endl
			<< "Got invalid checkpoint file name " << cpFile.string() << std::endl
			<< GEXCEPTION;
		}

		// Create the input stream and check that it is in good order
		boost::filesystem::ifstream checkpointStream(cpFile);
		if(!checkpointStream) {
			glogger
			<< "In GBaseParChildT<ind_type>::loadCheckpoint(const bf::path&)" << std::endl
			<< "Error: Could not open input file" << std::endl
			<< GEXCEPTION;
		}

		switch(GOptimizationAlgorithmT<ind_type>::getCheckpointSerializationMode()) {
			case Gem::Common::serializationMode::SERIALIZATIONMODE_TEXT:
				// Load the data from disc in text mode
			{
				boost::archive::text_iarchive ia(checkpointStream);
				ia >> boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
			} // note: explicit scope here is essential so the ia-destructor gets called
				break;

			case Gem::Common::serializationMode::SERIALIZATIONMODE_XML:
				// Load the data from disc in xml mode
			{
				boost::archive::xml_iarchive ia(checkpointStream);
				ia >> boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
			} // note: explicit scope here is essential so the ia-destructor gets called
				break;

			case Gem::Common::serializationMode::SERIALIZATIONMODE_BINARY:
				// Load the data from disc in binary mode
			{
				boost::archive::binary_iarchive ia(checkpointStream);
				ia >> boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
			} // note: explicit scope here is essential so the ia-destructor gets called
				break;
		}

		// Make sure the stream is closed again
		checkpointStream.close();

		// Load the individuals into this class
		std::size_t thisSize = this->size();
		std::size_t biSize = bestIndividuals.size();
		if(thisSize >= biSize) { // The most likely case
			for(std::size_t ic=0; ic<biSize; ic++) {
				(*this)[ic]->GObject::load(bestIndividuals[ic]);
			}
		}
		else if(thisSize < biSize) {
			for(std::size_t ic=0; ic<thisSize; ic++) {
				(*this)[ic]->GObject::load(bestIndividuals[ic]);
			}
			for(std::size_t ic=thisSize; ic<biSize; ic++) {
				this->push_back(bestIndividuals[ic]);
			}
		}
	}

	/***************************************************************************/
	/**
	 * Adds the option to increase the population by a given amount per iteration
	 *
	 * @param growthRate The amount of individuals to be added in each iteration
	 * @param maxPopulationSize The maximum allowed size of the population
	 */
	void setPopulationGrowth(
		std::size_t growthRate
		, std::size_t maxPopulationSize
	) {
		growthRate_ = growthRate;
		maxPopulationSize_ = maxPopulationSize;
	}

	/***************************************************************************/
	/**
	 * Allows to retrieve the growth rate of the population
	 *
	 * @return The growth rate of the population per iteration
	 */
	std::size_t getGrowthRate() const {
		return growthRate_;
	}

	/***************************************************************************/
	/**
	 * Allows to retrieve the maximum population size when growth is enabled
	 *
	 * @return The maximum population size allowed, when growth is enabled
	 */
	std::size_t getMaxPopulationSize() const {
		return maxPopulationSize_;
	}

	/***************************************************************************/
	/**
	 * Adds local configuration options to a GParserBuilder object
	 *
	 * @param gpb The GParserBuilder object to which configuration options should be added
	 */
	virtual void addConfigurationOptions (
		Gem::Common::GParserBuilder& gpb
	)  override {
		// Call our parent class'es function
		GOptimizationAlgorithmT<ind_type>::addConfigurationOptions(gpb);

		// Add local data

		gpb.registerFileParameter<std::size_t, std::size_t>(
			"size" // The name of the first variable
			, "nParents" // The name of the second variable
			, DEFAULTEAPOPULATIONSIZE
			, DEFAULTEANPARENTS
			, [this](std::size_t ps, std::size_t np){ this->setPopulationSizes(ps, np); }
			, "population"
		)
		<< "The total size of the population " << Gem::Common::nextComment()
		<< "The number of parents in the population";

		gpb.registerFileParameter<duplicationScheme>(
			"recombinationMethod" // The name of the variable
			, duplicationScheme::DEFAULTDUPLICATIONSCHEME // The default value
			, [this](duplicationScheme d){ this->setRecombinationMethod(d); }
		)
		<< "The recombination method. Options" << std::endl
		<< "0: default" << std::endl
		<< "1: random selection from available parents" << std::endl
		<< "2: selection according to the parent's value";

		gpb.registerFileParameter<std::size_t, std::size_t>(
			"growthRate" // The name of the variable
			, "maxPopulationSize" // The name of the variable
			, 0 // The default value of the first variable
			, 0 // The default value of the second variable
			, [this](std::size_t gr, std::size_t ms){ this->setPopulationGrowth(gr,ms); }
			, "populationGrowth"
		)
		<< "Specifies the number of individuals added per iteration" << Gem::Common::nextComment()
		<< "Specifies the maximum amount of individuals in the population" << std::endl
		<< "if growth is enabled";
	}

	/***************************************************************************/
	/**
	 * Retrieves a specific parent individual and casts it to the desired type. Note that this
	 * function will only be accessible to the compiler if individual_type is a derivative of GOptimizableEntity,
	 * thanks to the magic of the std::enable_if and type_traits.
	 *
	 * @param parent The id of the parent that should be returned
	 * @return A converted shared_ptr to the parent
	 */
	template <typename parent_type>
	std::shared_ptr<parent_type> getParentIndividual(
		std::size_t parentId
		, typename std::enable_if<std::is_base_of<GOptimizableEntity, parent_type>::value>::type *dummy = nullptr
	){
#ifdef DEBUG
      // Check that the parent id is in a valid range
      if(parentId >= this->getNParents()) {
         glogger
         << "In GBaseEA::getParentIndividual<>() : Error" << std::endl
         << "Requested parent id which does not exist: " << parentId << " / " << this->getNParents() << std::endl
         << GEXCEPTION;

         // Make the compiler happy
         return std::shared_ptr<parent_type>();
      }
#endif /* DEBUG */

		// Does error checks on the conversion internally
		return Gem::Common::convertSmartPointer<GOptimizableEntity, parent_type>(*(this->begin() + parentId));
	}

	/***************************************************************************/
	/** @brief Returns the name of this optimization algorithm */
	virtual std::string getAlgorithmName() const override = 0;
	/** @brief Returns information about the type of optimization algorithm */
	virtual std::string getOptimizationAlgorithm() const override = 0;

	/***************************************************************************/
	/**
	 * Emits a name for this class / object
	 */
	virtual std::string name() const override {
		return std::string("GBaseParChildT");
	}

protected:
	/***************************************************************************/
	/** @brief Creates a deep clone of this object */
	virtual GObject *clone_() const override = 0;
	/** @brief Adapts all children of this population */
	virtual void adaptChildren() = 0;
	/** @brief Calculates the fitness of all required individuals; to be re-implemented in derived classes */
	virtual void runFitnessCalculation() override = 0;
	/** @brief Choose new parents, based on the selection scheme set by the user */
	virtual void selectBest() = 0;
	/** @brief Retrieves the evaluation range in a given iteration and sorting scheme */
	virtual std::tuple<std::size_t,std::size_t> getEvaluationRange() const = 0; // Depends on selection scheme
	/** @brief Some error checks related to population sizes */
	virtual void populationSanityChecks() const = 0; // TODO: Take code from old init() function

	/***************************************************************************/
	/**
	 * Loads the data of another GBaseParChildT<ind_type> object, camouflaged as a GObject.
	 *
	 * @param cp A pointer to another GBaseParChildT<ind_type> object, camouflaged as a GObject
	 */
	virtual void load_(const GObject * cp) override {
		// Check that we are dealing with a GBaseParChildT<ind_type>  reference independent of this object and convert the pointer
		const GBaseParChildT<ind_type> *p_load = Gem::Common::g_convert_and_compare<GObject, GBaseParChildT<ind_type>>(cp, this);

		// First load the parent class'es data ...
		GOptimizationAlgorithmT<ind_type>::load_(cp);

		// ... and then our own data
		nParents_ = p_load->nParents_;
		recombinationMethod_ = p_load->recombinationMethod_;
		defaultNChildren_ = p_load->defaultNChildren_;
		maxPopulationSize_ = p_load->maxPopulationSize_;
		growthRate_ = p_load->growthRate_;
	}

	/***************************************************************************/
	/**
	 * This function assigns a new value to each child individual according to the chosen
	 * recombination scheme. Note that this function may be overloaded in derived classes,
	 * to e.g. add features such as cross-over.
	 */
	virtual void doRecombine() {
		std::size_t i;
		std::vector<double> threshold(nParents_);
		double thresholdSum=0.;
		// TODO: Check whether it is sufficient to do this only once
		if(duplicationScheme::VALUEDUPLICATIONSCHEME == recombinationMethod_ && nParents_ > 1) {          // Calculate a weight vector
			for(i=0; i<nParents_; i++) {
				thresholdSum += 1./(static_cast<double>(i)+2.);
			}
			for(i=0; i<nParents_-1; i++) {
				// Normalizing the sum to 1
				threshold[i] = (1./(static_cast<double>(i)+2.)) / thresholdSum;

				// Make sure the subsequent range is in the right position
				if(i>0) threshold[i] += threshold[i-1];
			}
			threshold[nParents_-1] = 1.; // Necessary due to rounding errors
		}

		typename std::vector<std::shared_ptr<ind_type>>::iterator it;
		for(it=GOptimizationAlgorithmT<ind_type>::data.begin()+nParents_; it!= GOptimizationAlgorithmT<ind_type>::data.end(); ++it) {
			switch(recombinationMethod_){
				case duplicationScheme::DEFAULTDUPLICATIONSCHEME: // we want the RANDOMDUPLICATIONSCHEME behavior
				case duplicationScheme::RANDOMDUPLICATIONSCHEME:
				{
					randomRecombine(*it);
				}
					break;

				case duplicationScheme::VALUEDUPLICATIONSCHEME:
				{
					if(nParents_ == 1) {
						(*it)->GObject::load(*(GOptimizationAlgorithmT<ind_type>::data.begin()));
						(*it)->GOptimizableEntity::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setParentId(0);
					} else {
						// A recombination taking into account the value does not make
						// sense in the first iteration, as parents might not have a suitable
						// value. Instead, this function might accidently trigger value
						// calculation. Hence we fall back to random recombination in iteration 0.
						// No value calculation takes place there.
						if(GOptimizationAlgorithmT<ind_type>::inFirstIteration()) {
							randomRecombine(*it);
						} else {
							valueRecombine(*it, threshold);
						}
					}
				}
					break;

				default:
				{
					glogger
					<< "In GBaseParChildT<ind_type>::doRecombine(): Error!" << std::endl
					<< "Got invalid duplication scheme: " << recombinationMethod_ << std::endl
					<< GEXCEPTION;
				}
					break;
			}
		}
	}

	/***************************************************************************/
	/**
	 * This function is called from GOptimizationAlgorithmT<ind_type>::optimize() and performs the
	 * actual recombination, based on the recombination schemes defined by the user.
	 *
	 * Note that, in DEBUG mode, this implementation will enforce a minimum number of children,
	 * as implied by the initial sizes of the population and the number of parents
	 * present. If individuals can get lost in your setting, you must add mechanisms
	 * to "repair" the population.
	 */
	virtual void recombine() {
#ifdef DEBUG
      // We require at this stage that at least the default number of
      // children is present. If individuals can get lost in your setting,
      // you must add mechanisms to "repair" the population.
      if((this->size()-nParents_) < defaultNChildren_){
         glogger
         << "In GBaseParChildT<ind_type>::recombine():" << std::endl
         << "Too few children. Got " << this->size()-nParents_ << "," << std::endl
         << "but was expecting at least " << defaultNChildren_ << std::endl
         << GEXCEPTION;
      }
#endif

		// Do the actual recombination
		doRecombine();

		// Let children know they are children
		markChildren();

		// Tell individuals about their ids
		markIndividualPositions();
	}

	/***************************************************************************/
	/**
	 * Retrieves the adaption range in a given iteration and sorting scheme.
	 *
	 * @return The range inside which adaption should take place
	 */
	std::tuple<std::size_t,std::size_t> getAdaptionRange() const {
		return std::tuple<std::size_t, std::size_t>(nParents_, this->size());
	}

	/***************************************************************************/
	/**
	 * This helper function marks parents as parents and children as children.
	 */
	void markParents() {
		typename std::vector<std::shared_ptr<ind_type>>::iterator it;
		for(it=GOptimizationAlgorithmT<ind_type>::data.begin(); it!=GOptimizationAlgorithmT<ind_type>::data.begin()+nParents_; ++it){
			(*it)->GOptimizableEntity::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setIsParent();
		}
	}

	/***************************************************************************/
	/**
	 * This helper function marks children as children
	 */
	void markChildren() {
		typename std::vector<std::shared_ptr<ind_type>>::iterator it;
		for(it=GOptimizationAlgorithmT<ind_type>::data.begin()+nParents_; it!=GOptimizationAlgorithmT<ind_type>::data.end(); ++it){
			(*it)->GOptimizableEntity::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setIsChild();
		}
	}

	/***************************************************************************/
	/**
	 * This helper function lets all individuals know about their position in the
	 * population.
	 */
	void markIndividualPositions() {
		std::size_t pos = 0;
		typename std::vector<std::shared_ptr<ind_type>>::iterator it;
		for(it=GOptimizationAlgorithmT<ind_type>::data.begin(); it!=GOptimizationAlgorithmT<ind_type>::data.end(); ++it) {
			(*it)->GOptimizableEntity::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setPopulationPosition(pos++);
		}
	}

	/***************************************************************************/
	/**
	 * This function implements the logic that constitutes evolutionary algorithms. The
	 * function is called by GOptimizationAlgorithmT<ind_type> for each cycle of the optimization,
	 *
	 * @return The value of the best individual found
	 */
	virtual std::tuple<double, double> cycleLogic() override {
		// If this is not the first iteration, check whether we need to increase the population
		if(GOptimizationAlgorithmT<ind_type>::afterFirstIteration()) {
			performScheduledPopulationGrowth();
		}

		// create new children from parents
		recombine();

		// adapt children
		adaptChildren();

		// calculate the children's (and possibly their parents' values)
		runFitnessCalculation();

		// Perform post-evaluation updates (mostly of individuals)
		GOptimizationAlgorithmT<ind_type>::postEvaluationWork();

		// find out the best individuals of the population
		selectBest();

#ifdef DEBUG
      // The dirty flag of this individual shouldn't be set
      if(!this->at(0)->isClean()) {
         glogger
         << "In GBaseParChiltT<>::cycleLogic(): Error!" << std::endl
         << "Expected clean individual in best position" << std::endl
         << GEXCEPTION;
      }

#endif /* DEBUG */

		// Return the primary fitness of the best individual in the collection
		return this->at(0)->getFitnessTuple();
	}

	/***************************************************************************/
	/**
	 * The function checks that the population size meets the requirements and does some
	 * tagging. It is called from within GOptimizationAlgorithmT<ind_type>::optimize(), before the
	 * actual optimization cycle starts.
	 */
	virtual void init() override {
		// To be performed before any other action
		GOptimizationAlgorithmT<ind_type>::init();

		// Perform some checks regarding population sizes
		populationSanityChecks();

		// Let parents know they are parents
		markParents();
		// Let children know they are children

		// Make sure derived classes (such as GTransferPopulation) have a way of finding out
		// what the desired number of children is. This is particularly important, if, in a
		// network environment, some individuals might not return and some individuals return
		// late. The factual size of the population then changes and we need to take action.
		defaultNChildren_ = GOptimizationAlgorithmT<ind_type>::getDefaultPopulationSize() - nParents_;
	}

	/***************************************************************************/
	/**
	 * Does any necessary finalization work
	 */
	virtual void finalize() override {
		// Last action
		GOptimizationAlgorithmT<ind_type>::finalize();
	}

	/***************************************************************************/
	/**
	 * The function checks that the population size meets the requirements and resizes the
	 * population to the appropriate size, if required. An obvious precondition is that at
	 * least one individual has been added to the population. Individuals that have already
	 * been added will not be replaced. This function is called once before the optimization
	 * cycle from within GOptimizationAlgorithmT<ind_type>::optimize()
	 */
	virtual void adjustPopulation() override {
		// Has the population size been set at all ?
		if(GOptimizationAlgorithmT<ind_type>::getDefaultPopulationSize() == 0) {
			glogger
			<< "In GBaseParChildT<ind_type>::adjustPopulation() :" << std::endl
			<< "The population size is 0." << std::endl
			<< "Did you call GOptimizationAlgorithmT<ind_type>::setParentsAndPopulationSize() ?" << std::endl
			<< GEXCEPTION;
		}

		// Check how many individuals have been added already. At least one is required.
		std::size_t this_sz = this->size();
		if(this_sz == 0) {
			glogger
			<< "In GBaseParChildT<ind_type>::adjustPopulation() :" << std::endl
			<< "size of population is 0. Did you add any individuals?" << std::endl
			<< "We need at least one local individual" << std::endl
			<< GEXCEPTION;
		}

		// Do the smart pointers actually point to any objects ?
		typename std::vector<std::shared_ptr<ind_type>>::iterator it;
		for(it=GOptimizationAlgorithmT<ind_type>::data.begin(); it!=GOptimizationAlgorithmT<ind_type>::data.end(); ++it) {
			if(!(*it)) { // shared_ptr can be implicitly converted to bool
				glogger
				<< "In GBaseParChildT<ind_type>::adjustPopulation() :" << std::endl
				<< "Found empty smart pointer." << std::endl
				<< GEXCEPTION;
			}
		}

		// Fill up as required. We are now sure we have a suitable number of individuals to do so
		if(this_sz < GOptimizationAlgorithmT<ind_type>::getDefaultPopulationSize()) {
			this->resize_clone(
				GOptimizationAlgorithmT<ind_type>::getDefaultPopulationSize()
				, GOptimizationAlgorithmT<ind_type>::data[0]
			);

			// Randomly initialize new items.
			// (Note: This will currently only have an effect on GParameterSet-derivatives)
			for(it=GOptimizationAlgorithmT<ind_type>::data.begin()+this_sz; it!=GOptimizationAlgorithmT<ind_type>::data.end(); ++it) {
				(*it)->randomInit(activityMode::ACTIVEONLY);
			}
		}
	}

	/***************************************************************************/
	/**
	 * Increases the population size if requested by the user. This will happen until the population size exceeds
	 * a predefined value, set with setPopulationGrowth() .
	 */
	void performScheduledPopulationGrowth() {
		if(
			growthRate_ != 0
			&& (this->getDefaultPopulationSize() + growthRate_ <= maxPopulationSize_)
			&& (this->size() < maxPopulationSize_)
			) {
			// Set a new default population size
			this->setPopulationSizes(this->getDefaultPopulationSize() + growthRate_, this->getNParents());

			// Add missing items as copies of the last individual in the list
			this->resize_clone(GOptimizationAlgorithmT<ind_type>::getDefaultPopulationSize(), GOptimizationAlgorithmT<ind_type>::data[0]);
		}
	}

	/***************************************************************************/
	/**
	 * Saves the state of the class to disc. The function adds the current generation
	 * and the fitness to the base name. We do not save the entire population, but only
	 * the best individuals, as these contain the "real" information. Note that no real
	 * copying of the individual's data takes place here, as we are dealing with
	 * std::shared_ptr objects.
	 */
	virtual void saveCheckpoint() const override {
		// Copy the nParents best individuals to a vector
		std::vector<std::shared_ptr<ind_type>> bestIndividuals;
		typename GBaseParChildT<ind_type>::const_iterator it;
		for(it=this->begin(); it!=this->begin() + getNParents(); ++it)
			bestIndividuals.push_back(*it);

#ifdef DEBUG // Cross check so we do not accidently trigger value calculation
      if(this->at(0)->isDirty()) {
         glogger
         << "In GBaseParChildT<ind_type>::saveCheckpoint():" << std::endl
         << "Error: class member in position " << std::distance(this->begin(),it) << " has the dirty flag set." << std::endl
         << GEXCEPTION;
      }
#endif /* DEBUG */
		double newValue = this->at(0)->fitness(); // The raw fitness -- used for identification only

		// Determine a suitable name for the output file
		std::string outputFile = GOptimizationAlgorithmT<ind_type>::getCheckpointDirectory() + boost::lexical_cast<std::string>(GOptimizationAlgorithmT<ind_type>::getIteration()) + "_"
										 + boost::lexical_cast<std::string>(newValue) + "_" + GOptimizationAlgorithmT<ind_type>::getCheckpointBaseName();

		// Create the output stream and check that it is in good order
		boost::filesystem::ofstream checkpointStream(outputFile);
		if(!checkpointStream) {
			glogger
			<< "In GBaseParChildT<ind_type>::saveCheckpoint()" << std::endl
			<< "Error: Could not open output file" << outputFile.c_str() << std::endl
			<< GEXCEPTION;
		}

		switch(GOptimizationAlgorithmT<ind_type>::getCheckpointSerializationMode()) {
			case Gem::Common::serializationMode::SERIALIZATIONMODE_TEXT:
				// Write the individuals' data to disc in text mode
			{
				boost::archive::text_oarchive oa(checkpointStream);
				oa << boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
			} // note: explicit scope here is essential so the oa-destructor gets called
				break;

			case Gem::Common::serializationMode::SERIALIZATIONMODE_XML:
				// Write the individuals' data to disc in XML mode
			{
				boost::archive::xml_oarchive oa(checkpointStream);
				oa << boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
			} // note: explicit scope here is essential so the oa-destructor gets called
				break;

			case Gem::Common::serializationMode::SERIALIZATIONMODE_BINARY:
				// Write the individuals' data to disc in binary mode
			{
				boost::archive::binary_oarchive oa(checkpointStream);
				oa << boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
			} // note: explicit scope here is essential so the oa-destructor gets called
				break;
		}

		// Make sure the stream is closed again
		checkpointStream.close();
	}

	/***************************************************************************/
	/**
	 * This function implements the RANDOMDUPLICATIONSCHEME scheme. This functions uses BOOST's
	 * numeric_cast function for safe conversion between std::size_t and uint16_t.
	 *
	 * @param pos The position of the individual for which a new value should be chosen
	 */
	void randomRecombine(std::shared_ptr<ind_type>& child) {
		std::size_t parent_pos;

		if(nParents_==1) {
			parent_pos = 0;
		} else {
			// Choose a parent to be used for the recombination. Note that
			// numeric_cast may throw. Exceptions need to be caught in surrounding functions.
			// try/catch blocks would add a non-negligible overhead in this function. uniform_int(max)
			// returns integer values in the range [0,max]. As we want to have values in the range
			// 0,1, ... nParents_-1, we need to subtract one from the argument.
			parent_pos = m_uniform_int_distribution(GOptimizationAlgorithmT<ind_type>::gr, std::uniform_int_distribution<std::size_t>::param_type(0, nParents_-1));
		}

		// Load the parent data into the individual
		child->GObject::load(*(GOptimizationAlgorithmT<ind_type>::data.begin() + parent_pos));

		// Let the individual know the id of the parent
		child->GOptimizableEntity::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setParentId(parent_pos);
	}

	/***************************************************************************/
	/**
	 * This function implements the VALUEDUPLICATIONSCHEME scheme. The range [0.,1.[ is divided
	 * into nParents_ sub-areas with different size (the largest for the first parent,
	 * the smallest for the last). Parents are chosen for recombination according to a
	 * random number evenly distributed between 0 and 1. This way parents with higher
	 * fitness are more likely to be chosen for recombination.
	 *
	 * @param pos The child individual for which a parent should be chosen
	 * @param threshold A std::vector<double> holding the recombination likelihoods for each parent
	 */
	void valueRecombine(
		std::shared_ptr<ind_type>& p
		, const std::vector<double>& threshold
	) {
		bool done=false;
		double randTest // get the test value
            = GOptimizationAlgorithmT<ind_type>::m_uniform_real_distribution(GOptimizationAlgorithmT<ind_type>::gr);

		for(std::size_t par=0; par<nParents_; par++) {
			if(randTest<threshold[par]) {
				// Load the parent's data
				p->GObject::load(*(GOptimizationAlgorithmT<ind_type>::data.begin() + par));
				// Let the individual know the parent's id
				p->GOptimizableEntity::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setParentId(par);
				done = true;

				break;
			}
		}

		if(!done) {
			glogger
			<< "In GBaseParChildT<ind_type>::valueRecombine():" << std::endl
			<< "Could not recombine." << std::endl
			<< GEXCEPTION;
		}
	}

	/***************************************************************************/
	/**
	 * Selection, MUPLUSNU_SINGLEEVAL style. Note that not all individuals of the population (including parents)
	 * are sorted -- only the nParents best individuals are identified. The quality of the population can only
	 * increase, but the optimization will stall more easily in MUPLUSNU_SINGLEEVAL mode.
	 */
	void sortMuPlusNuMode() {
#ifdef DEBUG
      // Check that we do not accidently trigger value calculation
      std::size_t pos = 0;
      for(auto ind_ptr: *this) { // std::shared_ptr may be copied
         if(ind_ptr->isDirty()) {
            glogger
            << "In GBaseParChildT<ind_type>::sortMuplusnuMode(): Error!" << std::endl
            << "In iteration " << GOptimizationAlgorithmT<ind_type>::getIteration() << ": Found individual in position " << pos << std::endl
            << " whose dirty flag is set." << std::endl
            << GEXCEPTION;
         }
         pos++;
      }
#endif /* DEBUG */

		// Only partially sort the arrays
		std::partial_sort(
			GOptimizationAlgorithmT<ind_type>::data.begin()
			, GOptimizationAlgorithmT<ind_type>::data.begin() + nParents_
			, GOptimizationAlgorithmT<ind_type>::data.end()
			, [](std::shared_ptr<ind_type> x, std::shared_ptr<ind_type> y) -> bool {
				return x->minOnly_fitness() < y->minOnly_fitness();
			}
		);
	}

	/***************************************************************************/
	/**
	 * Selection, MUCOMMANU_SINGLEEVAL style. New parents are selected from children only. The quality
	 * of the population may decrease occasionally from generation to generation, but the
	 * optimization is less likely to stall.
	 */
	void sortMuCommaNuMode() {
#ifdef DEBUG
      // Check that we do not accidently trigger value calculation
      typename GBaseParChildT<ind_type>::iterator it;
      for(it=this->begin()+nParents_; it!=this->end(); ++it) {
         if((*it)->isDirty()) {
            glogger
            << "In GBaseParChildT<ind_type>::sortMucommanuMode(): Error!" << std::endl
            << "In iteration " << GOptimizationAlgorithmT<ind_type>::getIteration() << ": Found individual in position " << std::distance(this->begin(),it) << std::endl
            << " whose dirty flag is set." << std::endl
            << GEXCEPTION;
         }
      }
#endif /* DEBUG */

		// Only sort the children
		std::partial_sort(
			GOptimizationAlgorithmT<ind_type>::data.begin() + nParents_
			, GOptimizationAlgorithmT<ind_type>::data.begin() + 2*nParents_
			, GOptimizationAlgorithmT<ind_type>::data.end()
			, [](std::shared_ptr<ind_type> x, std::shared_ptr<ind_type> y) -> bool {
				return x->minOnly_fitness() < y->minOnly_fitness();
			}
		);

		std::swap_ranges(
			GOptimizationAlgorithmT<ind_type>::data.begin()
			, GOptimizationAlgorithmT<ind_type>::data.begin()+nParents_
			, GOptimizationAlgorithmT<ind_type>::data.begin()+nParents_
		);
	}

	/***************************************************************************/
	/**
	 * Selection, MUNU1PRETAIN_SINGLEEVAL style. This is a hybrid between MUPLUSNU_SINGLEEVAL and MUCOMMANU_SINGLEEVAL
	 * mode. If a better child was found than the best parent of the last generation,
	 * all former parents are replaced. If no better child was found than the best
	 * parent of the last generation, then this parent stays in place. All other parents
	 * are replaced by the (nParents_-1) best children. The scheme falls back to MUPLUSNU_SINGLEEVAL
	 * mode, if only one parent is available, or if this is the first generation (so we
	 * do not accidentally trigger value calculation).
	 */
	void sortMunu1pretainMode() {
#ifdef DEBUG
      // Check that we do not accidently trigger value calculation
      typename GBaseParChildT<ind_type>::iterator it;
      for(it=this->begin()+nParents_; it!=this->end(); ++it) {
         if((*it)->isDirty()) {
            glogger
            << "In GBaseParChildT<ind_type>::sortMunu1pretainMode(): Error!" << std::endl
            << "In iteration " << GOptimizationAlgorithmT<ind_type>::getIteration() << ": Found individual in position " << std::distance(this->begin(),it) << std::endl
            << " whose dirty flag is set." << std::endl
            << GEXCEPTION;
         }
      }
#endif /* DEBUG */

		if(nParents_==1 || GOptimizationAlgorithmT<ind_type>::inFirstIteration()) { // Falls back to MUPLUSNU_SINGLEEVAL mode
			sortMuPlusNuMode();
		} else {
			// Sort the children
			std::partial_sort(
				GOptimizationAlgorithmT<ind_type>::data.begin() + nParents_
				, GOptimizationAlgorithmT<ind_type>::data.begin() + 2*nParents_
				, GOptimizationAlgorithmT<ind_type>::data.end()
				, [](std::shared_ptr<ind_type> x, std::shared_ptr<ind_type> y) -> bool {
					return x->minOnly_fitness() < y->minOnly_fitness();
				}
			);

			// Retrieve the best child's and the last generation's best parent's fitness
			double bestTranformedChildFitness_MinOnly  = (*(GOptimizationAlgorithmT<ind_type>::data.begin() + nParents_))->minOnly_fitness();
			double bestTranformedParentFitness_MinOnly = (*(GOptimizationAlgorithmT<ind_type>::data.begin()))->minOnly_fitness();

			// Leave the best parent in place, if no better child was found
			if(bestTranformedChildFitness_MinOnly < bestTranformedParentFitness_MinOnly) { // A better child was found. Overwrite all parents
				std::swap_ranges(
					GOptimizationAlgorithmT<ind_type>::data.begin()
					,GOptimizationAlgorithmT<ind_type>::data.begin()+nParents_
					,GOptimizationAlgorithmT<ind_type>::data.begin()+nParents_
				);
			} else {
				std::swap_ranges(
					GOptimizationAlgorithmT<ind_type>::data.begin()+1
					,GOptimizationAlgorithmT<ind_type>::data.begin()+nParents_
					,GOptimizationAlgorithmT<ind_type>::data.begin()+nParents_
				);
			}
		}
	}

	/***************************************************************************/

	std::size_t nParents_ = 0; ///< The number of parents
	duplicationScheme recombinationMethod_ = duplicationScheme::DEFAULTDUPLICATIONSCHEME; ///< The chosen recombination method
	std::size_t defaultNChildren_ = 0; ///< Expected number of children
	std::size_t growthRate_ = 0; ///< Specifies the amount of individuals added per iteration
	std::size_t maxPopulationSize_ = 0; ///< Specifies the maximum amount of individuals in the population if growth is enabled

private:
   std::uniform_int_distribution<std::size_t> m_uniform_int_distribution; ///< Access to uniformly distributed random numbers

public:
	/***************************************************************************/
	/**
	 * Applies modifications to this object. This is needed for testing purposes
	 *
	 * @return A boolean which indicates whether modifications were made
	 */
	virtual bool modify_GUnitTests() override {
#ifdef GEM_TESTING

      bool result = false;

      // Call the parent class'es function
      if(GOptimizationAlgorithmT<ind_type>::modify_GUnitTests()) result = true;

      return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
		condnotset("GBaseParChildT<ind_type>::modify_GUnitTests", "GEM_TESTING");
		return false;
#endif /* GEM_TESTING */
	}

	/***************************************************************************/
	/**
	 * Performs self tests that are expected to succeed. This is needed for testing purposes
	 */
	virtual void specificTestsNoFailureExpected_GUnitTests() override {
#ifdef GEM_TESTING
      // Call the parent class'es function
      GOptimizationAlgorithmT<ind_type>::specificTestsNoFailureExpected_GUnitTests();

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
		condnotset("GBaseParChildT<ind_type>::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
	}

	/***************************************************************************/
	/**
	 * Performs self tests that are expected to fail. This is needed for testing purposes
	 */
	virtual void specificTestsFailuresExpected_GUnitTests() override {
#ifdef GEM_TESTING
      // Call the parent class'es function
      GOptimizationAlgorithmT<ind_type>::specificTestsFailuresExpected_GUnitTests();

#else /* GEM_TESTING */
		condnotset("GBaseParChildT<ind_type>::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
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
template<typename ind_type>
struct is_abstract<Gem::Geneva::GBaseParChildT<ind_type>> : public boost::true_type {};
template<typename ind_type>
struct is_abstract< const Gem::Geneva::GBaseParChildT<ind_type>> : public boost::true_type {};
}
}

/******************************************************************************/

#endif /* GBASEPARCHILDEAT_HPP_ */
