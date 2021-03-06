/********************************************************************************
 *
 * This file is part of the Geneva library collection. The following license
 * applies to this file:
 *
 * ------------------------------------------------------------------------------
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ------------------------------------------------------------------------------
 *
 * Note that other files in the Geneva library collection may use a different
 * license. Please see the licensing information in each file.
 *
 ********************************************************************************
 *
 * Geneva was started by Dr. Rüdiger Berlich and was later maintained together
 * with Dr. Ariel Garcia under the auspices of Gemfony scientific. For further
 * information on Gemfony scientific, see http://www.gemfomy.eu .
 *
 * The majority of files in Geneva was released under the Apache license v2.0
 * in February 2020.
 *
 * See the NOTICE file in the top-level directory of the Geneva library
 * collection for a list of contributors and copyright information.
 *
 ********************************************************************************/

#include "geneva/G_OptimizationAlgorithm_ParChild.hpp"


namespace Gem {
namespace Geneva {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor, As we do not have any individuals yet, we set the population
 * size, and number of parents to 0. It is the philosophy of this class not
 * to provide constructors for each and every use case. Instead, you should set
 * vital parameters, such as the population size or the parent individuals by hand
 * or do so through the configuration file.
 */
G_OptimizationAlgorithm_ParChild::G_OptimizationAlgorithm_ParChild()
	: Gem::Geneva::G_OptimizationAlgorithm_Base()
{
	// Make sure we start with a valid population size if the user does not supply these values
	this->setPopulationSizes(
		DEFPARCHILDPOPSIZE // overall population size
		, DEFPARCHILDNPARENTS // number of parents
	);
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
void G_OptimizationAlgorithm_ParChild::compare_(
	const GObject& cp
	, const Gem::Common::expectation& e
	, const double& limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a G_OptimizationAlgorithm_ParChild  reference independent of this object and convert the pointer
	const G_OptimizationAlgorithm_ParChild *p_load = Gem::Common::g_convert_and_compare<GObject, G_OptimizationAlgorithm_ParChild>(cp, this);

	GToken token("G_OptimizationAlgorithm_ParChild", e);

	// Compare our parent data ...
	Gem::Common::compare_base_t<G_OptimizationAlgorithm_Base>(*this, *p_load, token);

	// ... and then the local data
	compare_t(IDENTITY(m_n_parents, p_load->m_n_parents), token);
	compare_t(IDENTITY(m_recombination_method, p_load->m_recombination_method), token);
	compare_t(IDENTITY(m_default_n_children, p_load->m_default_n_children), token);
	compare_t(IDENTITY(m_max_population_size, p_load->m_max_population_size), token);
	compare_t(IDENTITY(m_growth_rate, p_load->m_growth_rate), token);
	compare_t(IDENTITY(m_amalgamationLikelihood, p_load->m_amalgamationLikelihood), token);

	// React on deviations from the expectation
	token.evaluate();
}

/******************************************************************************/
/**
 * Resets the settings of this population to what was configured when
 * the optimize()-call was issued
 */
void G_OptimizationAlgorithm_ParChild::resetToOptimizationStart_() {
	// There is nothing to reset here, so we simply call the
	// function of the parent class
	G_OptimizationAlgorithm_Base::resetToOptimizationStart_();
}

/******************************************************************************/
/**
 * Specifies the default size of the population plus the number of parents.
 * The population will be filled with additional individuals later, as required --
 * see GParChildT::adjustPopulation() . Also, all error checking is done in
 * that function.
 *
 * @param popSize The desired size of the population
 * @param nParents The desired number of parents
 */
void G_OptimizationAlgorithm_ParChild::setPopulationSizes(std::size_t popSize, std::size_t nParents) {
	G_OptimizationAlgorithm_Base::setDefaultPopulationSize(popSize);
	m_n_parents = nParents;
}

/******************************************************************************/
/**
 * Retrieve the number of parents as set by the user. This is a fixed parameter and
 * should not be changed after it has first been set. Note that, if the size of the
 * population is smaller than the alleged number of parents, the function will return
 * the size of the population instead, thus interpreting its individuals as parents.
 *
 * @return The number of parents in the population
 */
std::size_t G_OptimizationAlgorithm_ParChild::getNParents() const {
	return (std::min)(this->size(), m_n_parents);
}

/******************************************************************************/
/**
 * Calculates the current number of children from the number of parents and the
 * size of the vector.
 *
 * @return The number of children in the population
 */
std::size_t G_OptimizationAlgorithm_ParChild::getNChildren() const {
	if(this->size() <= m_n_parents) {
		// This will happen, when only the default population size has been set,
		// but no individuals have been added yet
		return 0;
	} else {
		return this->size() - m_n_parents;
	}
}

/******************************************************************************/
/**
 * Retrieves the defaultNChildren_ parameter. E.g. in GTransferPopulation::adaptChildren() ,
 * this factor controls when a population is considered to be complete. The corresponding
 * loop which waits for new arrivals will then be stopped, which in turn allows
 * a new generation to start.
 *
 * @return The defaultNChildren_ parameter
 */
std::size_t G_OptimizationAlgorithm_ParChild::getDefaultNChildren() const {
	return m_default_n_children;
}

/**************************************************************************/
/**
 * Retrieve the number of processible items in the current iteration.
 *
 * @return The number of processible items in the current iteration
 */
std::size_t G_OptimizationAlgorithm_ParChild::getNProcessableItems_() const {
	std::tuple<std::size_t,std::size_t> range = this->getEvaluationRange_();

#ifdef DEBUG
	if(std::get<1>(range) <= std::get<0>(range)) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In G_OptimizationAlgorithm_ParChild<>::getNProcessableItems(): Error!" << std::endl
				<< "Upper boundary of range <= lower boundary: " << std::get<1>(range) << "/" << std::get<0>(range) << std::endl
		);
	}
#endif /* DEBUG */

	return std::get<1>(range) - std::get<0>(range);
}

/******************************************************************************/
/**
 * Lets the user set the desired recombination method. No sanity checks for the
 * values are necessary, as we use an enum.
 *
 * @param recombinationMethod The desired recombination method
 */
void G_OptimizationAlgorithm_ParChild::setRecombinationMethod(duplicationScheme recombinationMethod) {
	m_recombination_method = recombinationMethod;
}

/******************************************************************************/
/**
 * Retrieves the value of the recombinationMethod_ variable
 *
 * @return The value of the recombinationMethod_ variable
 */
duplicationScheme G_OptimizationAlgorithm_ParChild::getRecombinationMethod() const {
	return m_recombination_method;
}

/******************************************************************************/
/**
 * Adds the option to increase the population by a given amount per iteration
 *
 * @param growthRate The amount of individuals to be added in each iteration
 * @param maxPopulationSize The maximum allowed size of the population
 */
void G_OptimizationAlgorithm_ParChild::setPopulationGrowth(
	std::size_t growthRate
	, std::size_t maxPopulationSize
) {
	m_growth_rate = growthRate;
	m_max_population_size = maxPopulationSize;
}

/******************************************************************************/
/**
 * Allows to retrieve the growth rate of the population
 *
 * @return The growth rate of the population per iteration
 */
std::size_t G_OptimizationAlgorithm_ParChild::getGrowthRate() const {
	return m_growth_rate;
}

/******************************************************************************/
/**
 * Allows to retrieve the maximum population size when growth is enabled
 *
 * @return The maximum population size allowed, when growth is enabled
 */
std::size_t G_OptimizationAlgorithm_ParChild::getMaxPopulationSize() const {
	return m_max_population_size;
}

/******************************************************************************/
/**
 * Adds local configuration options to a GParserBuilder object
 *
 * @param gpb The GParserBuilder object to which configuration options should be added
 */
void G_OptimizationAlgorithm_ParChild::addConfigurationOptions_ (
	Gem::Common::GParserBuilder& gpb
)  {
	// Call our parent class'es function
	G_OptimizationAlgorithm_Base::addConfigurationOptions_(gpb);

	// Add local data

	gpb.registerFileParameter<double>(
		"amalgamationLikelihood" // The name of the variable
		, DEFAULTAMALGAMATIONLIKELIHOOD // The default value
		, [this](double al) { this->setAmalgamationLikelihood(al); }
	)
		<< "The likelihood for parent individuals to be \"fused\" together" << std::endl
		<< "rather than \"just\" being created through duplication schemes";

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

/******************************************************************************/
/**
 * Allows to set the likelihood for amalgamation of two units to be
 * performed instead of "just" duplication.
 */
void G_OptimizationAlgorithm_ParChild::setAmalgamationLikelihood(double amalgamationLikelihood) {
	if (amalgamationLikelihood < 0. || amalgamationLikelihood > 1.) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In setCrossOverLikelihood(" << amalgamationLikelihood << "): Error!" << std::endl
				<< "Received invalid likelihood for amalgamation. Must be in the range [0:1]." << std::endl
		);
	}

	m_amalgamationLikelihood = amalgamationLikelihood;
}

/******************************************************************************/
/**
 * Allows to retrieve the likelihood for amalgamation of two units to be
 * performed instead of "just" duplication.
 */
double G_OptimizationAlgorithm_ParChild::getAmalgamationLikelihood() const {
	return m_amalgamationLikelihood;
}

/******************************************************************************/
/**
 * This function assigns a new value to each child individual according to the chosen
 * recombination scheme.
 */
void G_OptimizationAlgorithm_ParChild::doRecombine() {
	std::size_t i;
	std::vector<double> threshold(m_n_parents);
	double thresholdSum = 0.;
	// Calculate a weight vector
	// TODO: Check whether it is sufficient to do this only once
	if (duplicationScheme::VALUEDUPLICATIONSCHEME == m_recombination_method && m_n_parents > 1) {
		for (i = 0; i < m_n_parents; i++) {
			thresholdSum += 1. / (static_cast<double>(i) + 2.);
		}
		for (i = 0; i < m_n_parents - 1; i++) {
			// Normalizing the sum to 1
			threshold[i] = (1. / (static_cast<double>(i) + 2.)) / thresholdSum;

			// Make sure the subsequent range is in the right position
			if (i > 0) threshold[i] += threshold[i - 1];
		}
		threshold[m_n_parents - 1] = 1.; // Necessary due to rounding errors
	}

	std::vector<std::shared_ptr<GParameterSet>>::iterator it;
	std::bernoulli_distribution amalgamationWanted(m_amalgamationLikelihood); // true with a likelihood of m_amalgamation_likelihood
	for (it = G_OptimizationAlgorithm_Base::m_data_cnt.begin() + m_n_parents;
		  it != G_OptimizationAlgorithm_Base::m_data_cnt.end(); ++it) {
		// Retrieve a random number so we can decide whether to perform cross-over or duplication
		// If we do perform cross-over, we always cross the best individual with another random parent
		if (m_n_parents > 1 && amalgamationWanted(this->m_gr)) { // Create individuals using a cross-over scheme
			std::shared_ptr <GParameterSet> bestParent = this->front();
			std::shared_ptr <GParameterSet> combiner = (m_n_parents > 2) ? (*(this->begin() + this->m_uniform_int_distribution(this->m_gr, std::uniform_int_distribution<std::size_t>::param_type(1, m_n_parents - 1)))) : (*(this->begin() + 1));

			(*it)->GObject::load(bestParent->crossOverWith(combiner));
		} else { // Just perform duplication
			switch (m_recombination_method) {
				case duplicationScheme::DEFAULTDUPLICATIONSCHEME: // we want the RANDOMDUPLICATIONSCHEME behavior
				case duplicationScheme::RANDOMDUPLICATIONSCHEME: {
					randomRecombine(*it);
				}
					break;

				case duplicationScheme::VALUEDUPLICATIONSCHEME: {
					if (m_n_parents == 1) {
						(*it)->GObject::load(*(G_OptimizationAlgorithm_Base::m_data_cnt.begin()));
						(*it)->GParameterSet::getPersonalityTraits<GBaseParChildPersonalityTraits> ()->setParentId(0);
					} else {
						// A recombination taking into account the value does not make
						// sense in the first iteration, as parents might not have a suitable
						// value. Instead, this function might accidentaly trigger value
						// calculation. Hence we fall back to random recombination in iteration 0.
						// No value calculation takes place there.
						if (G_OptimizationAlgorithm_Base::inFirstIteration()) {
							randomRecombine(*it);
						} else {
							valueRecombine(*it, threshold);
						}
					}
				}
					break;
			}
		}
	}
}

/******************************************************************************/
/**
 * Gives individuals an opportunity to update their internal structures. Here
 * we just trigger an update of the adaptors. We only do so for parents, as
 * they will be replicated in the next iteration. We leave the best parent
 * untouched, so that otherwise successful adaptor settings may survive.
 */
void G_OptimizationAlgorithm_ParChild::actOnStalls_() {
	if (this->getNParents() > 1) {
		// Update parent individuals. We leave the best parent untouched
		for(auto it = this->begin() + 1; it != this->begin() + this->getNParents(); ++it) {
			(*it)->updateAdaptorsOnStall(this->getStallCounter());
		}
	}
}

/******************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string G_OptimizationAlgorithm_ParChild::name_() const {
	return std::string("G_OptimizationAlgorithm_ParChild");
}

/******************************************************************************/
/**
 * Loads the data of another GParChildT object, camouflaged as a GObject.
 *
 * @param cp A pointer to another GParChildT object, camouflaged as a GObject
 */
void G_OptimizationAlgorithm_ParChild::load_(const GObject * cp) {
	// Check that we are dealing with a G_OptimizationAlgorithm_ParChild  reference independent of this object and convert the pointer
	const G_OptimizationAlgorithm_ParChild *p_load = Gem::Common::g_convert_and_compare<GObject, G_OptimizationAlgorithm_ParChild>(cp, this);

	// First load the parent class'es data ...
	G_OptimizationAlgorithm_Base::load_(cp);

	// ... and then our own data
	m_n_parents = p_load->m_n_parents;
	m_recombination_method = p_load->m_recombination_method;
	m_default_n_children = p_load->m_default_n_children;
	m_max_population_size = p_load->m_max_population_size;
	m_growth_rate = p_load->m_growth_rate;
	m_amalgamationLikelihood = p_load->m_amalgamationLikelihood;
}

/******************************************************************************/
/**
 * This function is called from G_OptimizationAlgorithm_Base::optimize() and performs the
 * actual recombination, based on the recombination schemes defined by the user.
 *
 * Note that, in DEBUG mode, this implementation will enforce a minimum number of children,
 * as implied by the initial sizes of the population and the number of parents
 * present. If individuals can get lost in your setting, you must add mechanisms
 * to "repair" the population.
 */
void G_OptimizationAlgorithm_ParChild::recombine() {
#ifdef DEBUG
	// We require at this stage that at least the default number of
	// children is present. If individuals can get lost in your setting,
	// you must add mechanisms to "repair" the population.
	if((this->size()-m_n_parents) < m_default_n_children){
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In G_OptimizationAlgorithm_ParChild::recombine():" << std::endl
				<< "Too few children. Got " << this->size()-m_n_parents << "," << std::endl
				<< "but was expecting at least " << m_default_n_children << std::endl
		);
	}
#endif

	// Do the actual recombination
	doRecombine();

	// Let children know they are children
	markChildren();

	// Tell individuals about their ids
	markIndividualPositions();
}

/******************************************************************************/
/**
 * Retrieves the adaption range in a given iteration and sorting scheme.
 *
 * @return The range inside which adaption should take place
 */
std::tuple<std::size_t,std::size_t> G_OptimizationAlgorithm_ParChild::getAdaptionRange() const {
	return std::tuple<std::size_t,std::size_t>{m_n_parents, this->size()};
}

/******************************************************************************/
/**
 * This helper function marks parents as parents and children as children.
 */
void G_OptimizationAlgorithm_ParChild::markParents() {
	typename std::vector<std::shared_ptr<GParameterSet>>::iterator it;
	for(it=G_OptimizationAlgorithm_Base::m_data_cnt.begin(); it!=G_OptimizationAlgorithm_Base::m_data_cnt.begin()+m_n_parents; ++it){
		(*it)->GParameterSet::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setIsParent();
	}
}

/******************************************************************************/
/**
 * This helper function marks children as children
 */
void G_OptimizationAlgorithm_ParChild::markChildren() {
	typename std::vector<std::shared_ptr<GParameterSet>>::iterator it;
	for(it=G_OptimizationAlgorithm_Base::m_data_cnt.begin()+m_n_parents; it!=G_OptimizationAlgorithm_Base::m_data_cnt.end(); ++it){
		(*it)->GParameterSet::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setIsChild();
	}
}

/******************************************************************************/
/**
 * This helper function lets all individuals know about their position in the
 * population.
 */
void G_OptimizationAlgorithm_ParChild::markIndividualPositions() {
	std::size_t pos = 0;
	typename std::vector<std::shared_ptr<GParameterSet>>::iterator it;
	for(it=G_OptimizationAlgorithm_Base::m_data_cnt.begin(); it!=G_OptimizationAlgorithm_Base::m_data_cnt.end(); ++it) {
		(*it)->GParameterSet::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setPopulationPosition(pos++);
	}
}

/******************************************************************************/
/**
 * This function implements the logic that constitutes evolutionary algorithms. The
 * function is called by G_OptimizationAlgorithm_Base for each cycle of the optimization,
 *
 * @return The value of the best individual found
 */
std::tuple<double, double> G_OptimizationAlgorithm_ParChild::cycleLogic_() {
	// If this is not the first iteration, check whether we need to increase the population
	if(G_OptimizationAlgorithm_Base::afterFirstIteration()) {
		performScheduledPopulationGrowth();
	}

	// create new children from parents
	recombine();

	// adapt children
	adaptChildren_();

	// calculate the children's (and possibly their parents' values)
	runFitnessCalculation_();

	// find out the best individuals of the population
	selectBest_();

#ifdef DEBUG
	// The dirty flag of this individual shouldn't be set
	if(not this->at(0)->is_processed()) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In G_OptimizationAlgorithm_ParChild::cycleLogic(): Error!" << std::endl
				<< "Expected clean individual in best position" << std::endl
		);
	}

#endif /* DEBUG */

	// Return the primary fitness of the best individual in the collection
	return this->at(0)->getFitnessTuple();
}

/******************************************************************************/
/**
 * The function checks that the population size meets the requirements and does some
 * tagging. It is called from within G_OptimizationAlgorithm_Base::optimize(), before the
 * actual optimization cycle starts.
 */
void G_OptimizationAlgorithm_ParChild::init() {
	// To be performed before any other action
	G_OptimizationAlgorithm_Base::init();

	// Perform some checks regarding population sizes
	populationSanityChecks_();

	// Let parents know they are parents
	markParents();
	// Let children know they are children

	// Make sure derived classes (such as GTransferPopulation) have a way of finding out
	// what the desired number of children is. This is particularly important, if, in a
	// network environment, some individuals might not return and some individuals return
	// late. The factual size of the population then changes and we need to take action.
	m_default_n_children = G_OptimizationAlgorithm_Base::getDefaultPopulationSize() - m_n_parents;
}

/******************************************************************************/
/**
 * Does any necessary finalization work
 */
void G_OptimizationAlgorithm_ParChild::finalize() {
	// Last action
	G_OptimizationAlgorithm_Base::finalize();
}

/******************************************************************************/
/**
 * The function checks that the population size meets the requirements and resizes the
 * population to the appropriate size, if required. An obvious precondition is that at
 * least one individual has been added to the population. Individuals that have already
 * been added will not be replaced. This function is called once before the optimization
 * cycle from within G_OptimizationAlgorithm_Base::optimize()
 */
void G_OptimizationAlgorithm_ParChild::adjustPopulation_() {
	// Has the population size been set at all ?
	if(G_OptimizationAlgorithm_Base::getDefaultPopulationSize() == 0) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In G_OptimizationAlgorithm_ParChild::adjustPopulation() :" << std::endl
				<< "The population size is 0." << std::endl
				<< "Did you call G_OptimizationAlgorithm_Base::setParentsAndPopulationSize() ?" << std::endl
		);
	}

	// Check how many individuals have been added already. At least one is required.
	std::size_t this_sz = this->size();
	if(this_sz == 0) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In G_OptimizationAlgorithm_ParChild::adjustPopulation() :" << std::endl
				<< "size of population is 0. Did you add any individuals?" << std::endl
				<< "We need at least one local individual" << std::endl
		);
	}

	// Do the smart pointers actually point to any objects ?
	typename std::vector<std::shared_ptr<GParameterSet>>::iterator it;
	for(it=G_OptimizationAlgorithm_Base::m_data_cnt.begin(); it!=G_OptimizationAlgorithm_Base::m_data_cnt.end(); ++it) {
		if(not (*it)) { // shared_ptr can be implicitly converted to bool
			throw gemfony_exception(
				g_error_streamer(DO_LOG,  time_and_place)
					<< "In G_OptimizationAlgorithm_ParChild::adjustPopulation() :" << std::endl
					<< "Found empty smart pointer." << std::endl
			);
		}
	}

	// Fill up as required. We are now sure we have a suitable number of individuals to do so
	if(this_sz < G_OptimizationAlgorithm_Base::getDefaultPopulationSize()) {
		this->resize_clone(
			G_OptimizationAlgorithm_Base::getDefaultPopulationSize()
			, G_OptimizationAlgorithm_Base::m_data_cnt[0]
		);

		// Randomly initialize new items
		for(it=G_OptimizationAlgorithm_Base::m_data_cnt.begin()+this_sz; it!=G_OptimizationAlgorithm_Base::m_data_cnt.end(); ++it) {
			(*it)->randomInit(activityMode::ACTIVEONLY);
		}
	}
}

/******************************************************************************/
/**
 * Increases the population size if requested by the user. This will happen until the population size exceeds
 * a predefined value, set with setPopulationGrowth() .
 */
void G_OptimizationAlgorithm_ParChild::performScheduledPopulationGrowth() {
	if(
		m_growth_rate != 0
		&& (this->getDefaultPopulationSize() + m_growth_rate <= m_max_population_size)
		&& (this->size() < m_max_population_size)
		) {
		// Set a new default population size
		this->setPopulationSizes(this->getDefaultPopulationSize() + m_growth_rate, this->getNParents());

		// Add missing items as copies of the last individual in the list
		this->resize_clone(G_OptimizationAlgorithm_Base::getDefaultPopulationSize(), G_OptimizationAlgorithm_Base::m_data_cnt[0]);
	}
}

/******************************************************************************/
/**
 * This function implements the RANDOMDUPLICATIONSCHEME scheme. This functions uses BOOST's
 * numeric_cast function for safe conversion between std::size_t and uint16_t.
 *
 * @param pos The position of the individual for which a new value should be chosen
 */
void G_OptimizationAlgorithm_ParChild::randomRecombine(std::shared_ptr<GParameterSet>& child) {
	std::size_t parent_pos;

	if(m_n_parents==1) {
		parent_pos = 0;
	} else {
		// Choose a parent to be used for the recombination. Note that
		// numeric_cast may throw. Exceptions need to be caught in surrounding functions.
		// try/catch blocks would add a non-negligible overhead in this function. uniform_int(max)
		// returns integer values in the range [0,max]. As we want to have values in the range
		// 0,1, ... m_n_parents-1, we need to subtract one from the argument.
		parent_pos = m_uniform_int_distribution(this->m_gr, std::uniform_int_distribution<std::size_t>::param_type(0, m_n_parents-1));
	}

	// Load the parent data into the individual
	child->GObject::load(*(G_OptimizationAlgorithm_Base::m_data_cnt.begin() + parent_pos));

	// Let the individual know the id of the parent
	child->GParameterSet::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setParentId(parent_pos);
}

/******************************************************************************/
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
void G_OptimizationAlgorithm_ParChild::valueRecombine(
	std::shared_ptr<GParameterSet>& p
	, const std::vector<double>& threshold
) {
	bool done=false;
	double randTest // get the test value
		= G_OptimizationAlgorithm_Base::m_uniform_real_distribution(this->m_gr);

	for(std::size_t par=0; par<m_n_parents; par++) {
		if(randTest<threshold[par]) {
			// Load the parent's data
			p->GObject::load(*(G_OptimizationAlgorithm_Base::m_data_cnt.begin() + par));
			// Let the individual know the parent's id
			p->GParameterSet::template getPersonalityTraits<GBaseParChildPersonalityTraits>()->setParentId(par);
			done = true;

			break;
		}
	}

	if(not done) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In G_OptimizationAlgorithm_ParChild::valueRecombine():" << std::endl
				<< "Could not recombine." << std::endl
		);
	}
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool G_OptimizationAlgorithm_ParChild::modify_GUnitTests_() {
#ifdef GEM_TESTING

	bool result = false;

	// Call the parent class'es function
	if(G_OptimizationAlgorithm_Base::modify_GUnitTests_()) result = true;

	return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("G_OptimizationAlgorithm_ParChild::modify_GUnitTests", "GEM_TESTING");
		 return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void G_OptimizationAlgorithm_ParChild::specificTestsNoFailureExpected_GUnitTests_() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	G_OptimizationAlgorithm_Base::specificTestsNoFailureExpected_GUnitTests_();

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("G_OptimizationAlgorithm_ParChild::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void G_OptimizationAlgorithm_ParChild::specificTestsFailuresExpected_GUnitTests_() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	G_OptimizationAlgorithm_Base::specificTestsFailuresExpected_GUnitTests_();

#else /* GEM_TESTING */
	Gem::Common::condnotset("G_OptimizationAlgorithm_ParChild::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */
