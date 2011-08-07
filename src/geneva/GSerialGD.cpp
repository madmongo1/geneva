/**
 * @file GSerialGD.cpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
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
 * http://www.gemfony.com .
 */

#include "geneva/GSerialGD.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GSerialGD)
BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GSerialGD::GGDOptimizationMonitor)

namespace Gem {
namespace Geneva {

/************************************************************************************************************/
/**
 * The default constructor
 */
GSerialGD::GSerialGD()
	: GOptimizationAlgorithmT<GParameterSet>()
	, nStartingPoints_(DEFAULTGDSTARTINGPOINTS)
	, nFPParmsFirst_(0)
	, finiteStep_(DEFAULTFINITESTEP)
	, stepSize_(DEFAULTSTEPSIZE)
{
	GOptimizationAlgorithmT<GParameterSet>::setOptimizationAlgorithm(GD);

	// Register the default optimization monitor
	this->registerOptimizationMonitor(
			boost::shared_ptr<GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT>(
					new GGDOptimizationMonitor()
			)
	);
}

/************************************************************************************************************/
/**
 * Initialization with the number of starting points and other parameters
 *
 * @param nStartingPoints The number of simultaneous starting points for the gradient descent
 * @param finiteStep The desired size of the incremental adaption process
 * @param stepSize The size of the multiplicative factor of the adaption process
 */
GSerialGD::GSerialGD(
		const std::size_t& nStartingPoints
		, const float& finiteStep
		, const float& stepSize
)
	: GOptimizationAlgorithmT<GParameterSet>()
	, nStartingPoints_(nStartingPoints)
	, nFPParmsFirst_(0)
	, finiteStep_(finiteStep)
	, stepSize_(stepSize)
{
	GOptimizationAlgorithmT<GParameterSet>::setOptimizationAlgorithm(GD);

	// Register the default optimization monitor
	this->registerOptimizationMonitor(
			boost::shared_ptr<GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT>(
					new GGDOptimizationMonitor()
			)
	);
}

/************************************************************************************************************/
/**
 * A standard copy constructor.
 *
 * @param cp A copy of another GRadientDescent object
 */
GSerialGD::GSerialGD(const GSerialGD& cp)
	: GOptimizationAlgorithmT<GParameterSet>(cp)
	, nStartingPoints_(cp.nStartingPoints_)
	, nFPParmsFirst_(cp.nFPParmsFirst_)
	, finiteStep_(cp.finiteStep_)
	, stepSize_(cp.stepSize_)
{
	// Copying / setting of the optimization algorithm id is done by the parent class. The same
	// applies to the copying of the optimization monitor.
}

/************************************************************************************************************/
/**
 * The destructor
 */
GSerialGD::~GSerialGD()
{ /* nothing */ }

/************************************************************************************************************/
/**
 * Retrieves the number of starting points of the algorithm
 *
 * @return The number of simultaneous starting points of the gradient descent
 */
std::size_t GSerialGD::getNStartingPoints() const {
	return nStartingPoints_;
}

/************************************************************************************************************/
/**
 * Allows to set the number of starting points for the gradient descent
 *
 * @param nStartingPoints The desired number of starting points for the gradient descent
 */
void GSerialGD::setNStartingPoints(const std::size_t& nStartingPoints) {
	// Do some error checking
	if(nStartingPoints == 0) {
		raiseException(
				"In GSerialGD::setNStartingPoints(const std::size_t&):" << std::endl
				<< "Got invalid number of starting points."
		);
	}

	nStartingPoints_ = nStartingPoints;
}

/************************************************************************************************************/
/**
 * Set the size of the finite step of the adaption process
 *
 * @param finiteStep The desired size of the adaption
 */
void GSerialGD::setFiniteStep(const float& finiteStep) {
	// Do some error checking
	if(finiteStep <= 0.) {
		raiseException(
				"In GSerialGD::setFiniteStep(const float&):" << std::endl
				<< "Got invalid finite step size: " << finiteStep
		);
	}

	finiteStep_ = finiteStep;
}

/************************************************************************************************************/
/**
 * Retrieve the size of the finite step of the adaption process
 *
 * @return The current finite step size
 */
float GSerialGD::getFiniteStep() const {
	return finiteStep_;
}

/************************************************************************************************************/
/**
 * Sets a multiplier for the adaption process
 *
 * @param stepSize A multiplicative factor for the adaption process
 */
void GSerialGD::setStepSize(const float& stepSize) {
	// Do some error checking
	if(stepSize <= 0.) {
		raiseException(
				"In GSerialGD::setStepSize(const float&):" << std::endl
				<< "Got invalid step size: " << stepSize
		);
	}

	stepSize_ = stepSize;
}

/************************************************************************************************************/
/**
 * Retrieves the current step size
 *
 * @return The current valze of the step size
 */
float GSerialGD::getStepSize() const {
	return stepSize_;
}

/************************************************************************************************************/
/**
 * Retrieve the number of processable items in the current iteration.
 *
 * @return The number of processable items in the current iteration
 */
std::size_t GSerialGD::getNProcessableItems() const {
	return this->size(); // Evaluation always needs to be done for the entire population
}


/************************************************************************************************************/
/**
 * A standard assignment operator
 *
 * @param cp A copy of another GRadientDescent object
 */
const GSerialGD& GSerialGD::operator=(const GSerialGD& cp) {
	GSerialGD::load_(&cp);
	return *this;
}

/************************************************************************************************************/
/**
 * Checks for equality with another GSerialGD object
 *
 * @param cp A copy of another GRadientDescent object
 */
bool GSerialGD::operator==(const GSerialGD& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of equality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_EQUALITY, 0.,"GSerialGD::operator==","cp", CE_SILENT);
}

/************************************************************************************************************/
/**
 * Checks for inequality with another GSerialGD object
 *
 * @param cp A copy of another GRadientDescent object
 */
bool GSerialGD::operator!=(const GSerialGD& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of inequality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_INEQUALITY, 0.,"GSerialGD::operator!=","cp", CE_SILENT);
}

/************************************************************************************************************/
/**
 * Checks whether this object fulfills a given expectation in relation to another object
 *
 * @param cp A constant reference to another object, camouflaged as a GObject
 * @param e The expected outcome of the comparison
 * @param limit The maximum deviation for floating point values (important for similarity checks)
 * @param caller An identifier for the calling entity
 * @param y_name An identifier for the object that should be compared to this one
 * @param withMessages Whether or not information should be emitted in case of deviations from the expected outcome
 * @return A boost::optional<std::string> object that holds a descriptive string if expectations were not met
 */
boost::optional<std::string> GSerialGD::checkRelationshipWith(
		const GObject& cp
		, const Gem::Common::expectation& e
		, const double& limit
		, const std::string& caller
		, const std::string& y_name
		, const bool& withMessages
) const {
    using namespace Gem::Common;

	// Check that we are indeed dealing with a GParamterBase reference
	const GSerialGD *p_load = GObject::conversion_cast<GSerialGD>(&cp);

	// Will hold possible deviations from the expectation, including explanations
    std::vector<boost::optional<std::string> > deviations;

	// Check our parent class'es data ...
    deviations.push_back(GOptimizationAlgorithmT<GParameterSet>::checkRelationshipWith(cp, e, limit, "GOptimizationAlgorithmT<GParameterSet>", y_name, withMessages));

	// ... and then our local data
	deviations.push_back(checkExpectation(withMessages, "GSerialGD", nStartingPoints_, p_load->nStartingPoints_, "nStartingPoints_", "p_load->nStartingPoints_", e , limit));
	deviations.push_back(checkExpectation(withMessages, "GSerialGD", nFPParmsFirst_, p_load->nFPParmsFirst_, "nFPParmsFirst_", "p_load->nFPParmsFirst_", e , limit));
	deviations.push_back(checkExpectation(withMessages, "GSerialGD", finiteStep_, p_load->finiteStep_, "finiteStep_", "p_load->finiteStep_", e , limit));
	deviations.push_back(checkExpectation(withMessages, "GSerialGD", stepSize_, p_load->stepSize_, "stepSize_", "p_load->stepSize_", e , limit));

	return evaluateDiscrepancies("GSerialGD", caller, deviations, e);
}

/************************************************************************************************************/
/**
 * Loads a checkpoint from disk
 *
 * @param cpFile The name of the file the checkpoint should be loaded from
 */
void GSerialGD::loadCheckpoint(const std::string& cpFile) {
	// Create a vector to hold the best individuals
	std::vector<boost::shared_ptr<Gem::Geneva::GParameterSet> > bestIndividuals;

	// Check that the file indeed exists
	if(!boost::filesystem::exists(cpFile)) {
		raiseException(
				"In GSerialGD::loadCheckpoint(const std::string&)" << std::endl
				<< "Got invalid checkpoint file name " << cpFile
		);
	}

	// Create the input stream and check that it is in good order
	std::ifstream checkpointStream(cpFile.c_str());
	if(!checkpointStream) {
		raiseException(
				"In GSerialGD::loadCheckpoint(const std::string&)" << std::endl
				<< "Error: Could not open input file"
		);
	}

	switch(getCheckpointSerializationMode()) {
	case Gem::Common::SERIALIZATIONMODE_TEXT:
		// Load the data from disc in text mode
		{
			boost::archive::text_iarchive ia(checkpointStream);
		    ia >> boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
		} // note: explicit scope here is essential so the ia-destructor gets called
		break;

	case Gem::Common::SERIALIZATIONMODE_XML:
		// Load the data from disc in xml mode
		{
			boost::archive::xml_iarchive ia(checkpointStream);
		    ia >> boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
		} // note: explicit scope here is essential so the ia-destructor gets called
		break;

	case Gem::Common::SERIALIZATIONMODE_BINARY:
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

/************************************************************************************************************/
/**
 * Loads the data of another population
 *
 * @param cp A pointer to another GSerialGD object, camouflaged as a GObject
 */
void GSerialGD::load_(const GObject *cp) {
	// Make a note of the current iteration (needed for a check below).
	// The information would otherwise be lost after the load call below
	boost::uint32_t currentIteration = this->getIteration();

	const GSerialGD *p_load = this->conversion_cast<GSerialGD>(cp);

	// First load the parent class'es data.
	// This will also take care of copying all individuals.
	GOptimizationAlgorithmT<GParameterSet>::load_(cp);

	// ... and then our own data
	nStartingPoints_ = p_load->nStartingPoints_;
	nFPParmsFirst_ = p_load->nFPParmsFirst_;
	finiteStep_ = p_load->finiteStep_;
	stepSize_ = p_load->stepSize_;
}

/************************************************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A deep copy of this object, camouflaged as a pointer to a GObject
 */
GObject *GSerialGD::clone_() const {
	return new GSerialGD(*this);
}

/************************************************************************************************************/
/**
 * Sets the individual's personality types to GradientDescent
 */
void GSerialGD::setIndividualPersonalities() {
	for(GSerialGD::iterator it=this->begin(); it!=this->end(); ++it) {
		(*it)->setPersonality(GD);
	}
}

/************************************************************************************************************/
/**
 * The actual business logic to be performed during each iteration. Returns the best achieved fitness
 *
 * @return The value of the best individual found
 */
double GSerialGD::cycleLogic() {
	if(this->getIteration() > this->getOffset()) {
		// Update the parameters of the parent individuals. This
		// only makes sense once the individuals have been evaluated
		this->updateParentIndividuals();
	}

	// Update the individual parameters in each dimension of the "children"
	this->updateChildParameters();

	// Trigger value calculation for all individuals (including parents)
	double bestFitness = doFitnessCalculation(this->size());

	return bestFitness;
}

/************************************************************************************************************/
/**
 * Updates the individual parameters of children
 */
void GSerialGD::updateChildParameters() {
	// Loop over all starting points
	for(std::size_t i=0; i<nStartingPoints_; i++) {
		// Extract the fp vector
		std::vector<double> parmVec;
		this->at(i)->streamline(parmVec);

		// Loop over all directions
		for(std::size_t j=0; j<nFPParmsFirst_; j++) {
			// Calculate the position of the child
			std::size_t childPos = nStartingPoints_ + i*nFPParmsFirst_ + j;

			// Load the current "parent" into the "child"
			this->at(childPos)->load(this->at(i));

			// Update the child's position in the population
			this->at(childPos)->getPersonalityTraits<GGDPersonalityTraits>()->setPopulationPosition(childPos);

			// Make a note of the current parameter's value
			double origParmVal = parmVec[j];

			// Add the finite step to the feature vector's current parameter
			parmVec[j] += finiteStep_;

			// Attach the feature vector to the child individual
			this->at(childPos)->assignValueVector(parmVec);

			// Restore the original value in the feature vector
			parmVec[j] = origParmVal;
		}
	}
}

/**********************************************************************************************************/
/**
 * Performs a step of the parent individuals
 */
void GSerialGD::updateParentIndividuals() {
	for(std::size_t i=0; i<nStartingPoints_; i++) {
		// Extract the fp vector
		std::vector<double> parmVec;
		this->at(i)->streamline(parmVec);

#ifdef DEBUG
		// Make sure the parents are clean
		if(this->at(i)->isDirty()) {
			raiseException(
					"In GSerialGD::updateParentIndividuals():" << std::endl
					<< "Found individual in position " << i << " with active dirty flag"
			);
		}
#endif /* DEBUG*/

		// Retrieve the fitness of the individual again
		double parentFitness = this->at(i)->fitness(0);

		// Calculate the adaption of each parameter
		double gradient = 0.;
		//std::cout << "==============" << std::endl;
		for(std::size_t j=0; j<nFPParmsFirst_; j++) {
			// Calculate the position of the child
			std::size_t childPos = nStartingPoints_ + i*nFPParmsFirst_ + j;

			// Calculate the step to be performed in a given direction
			gradient = (1./finiteStep_) * (this->at(childPos)->fitness(0) - parentFitness);

			/*
			std::cout << "parmVec[" << j << "] = " << parmVec[j] << std::endl
					  << "gradient = " << gradient << std::endl
					  << "stepSize_ = " << stepSize_ << std::endl
					  << "adaption = " << stepSize_*gradient << std::endl;
	        */

			if(this->getMaxMode()) {
				parmVec[j] += stepSize_*gradient;
			}
			else { // Minimization
				parmVec[j] -= stepSize_*gradient;
			}
		}
		// std::cout << "==============" << std::endl;

		// Load the parameter vector back into the parent
		this->at(i)->assignValueVector(parmVec);
	}
}

/************************************************************************************************************/
/**
 * Retrieves the best individual of the population and returns it in Gem::Geneva::GIndividual format.
 * Note that this protected function will return the item itself. Direct usage of this function should
 * be avoided even by derived classes. We suggest to use the function
 * GOptimizableI::getBestIndividual<individual_type>() instead, which internally uses
 * this function and returns copies of the best individual, converted to the desired target type.
 *
 * @return A shared_ptr to the best individual of the population
 */
boost::shared_ptr<GIndividual> GSerialGD::getBestIndividual(){
#ifdef DEBUG
	// Check that data is present at all
	if(data.size() < nStartingPoints_) {
		raiseException(
				"In GSerialGD::getBestIndividual() : Error!" << std::endl
				<< "Population has fewer individuals than starting points: " << data.size() << " / " << nStartingPoints_
		);
	}

	// Check that no parent is in "dirty" state
	for(std::size_t i=0; i<nStartingPoints_; i++) {
		if(data.at(i)->isDirty()) {
			raiseException(
					"In GSerialGD::getBestIndividual() : Error!" << std::endl
					<< "Found dirty parent at position : " << i
			);
		}
	}
#endif /* DEBUG */

	// Loop over all "parent" individuals and find out which one is the best
	std::size_t pos_best=0;
	for(std::size_t i=1; i<nStartingPoints_; i++) {
		if(isBetter(data.at(i)->fitness(0), data.at(i-1)->fitness(0))) {
			pos_best=i;
		}
	}

	// There will be an implicit downcast here, as data holds
	// boost::shared_ptr<GParameterSet> objects
	return data[pos_best];
}

/************************************************************************************************************/
/**
 * Does some preparatory work before the optimization starts
 */
void GSerialGD::init() {
	// To be performed before any other action
	GOptimizationAlgorithmT<GParameterSet>::init();

	// Tell individuals about their position in the population
	markIndividualPositions();
}

/************************************************************************************************************/
/**
 * Does any necessary finalization work
 */
void GSerialGD::finalize() {
	// Last action
	GOptimizationAlgorithmT<GParameterSet>::finalize();
}

/************************************************************************************************************/
/**
 * Performs final optimization work. In the case of (networked) gradient descents, the starting points need
 * to be re-evaluated at the end of the optimization cycle, before the connection to the broker is cut.
 * doFitnessCalculation is overloaded in GBrokerGD.
 */
// void GSerialGD::optimizationFinalize() {
	// Make sure the fitness of the parent individuals is calculated in the final iteration
//	checkpoint(ifProgress(doFitnessCalculation(nStartingPoints_)));
// }

/************************************************************************************************************/
/**
 * Triggers fitness calculation of a number of individuals. This function can be overloaded to perform
 * fitness calculation in parallel, using threads or network communication.
 *
 * @param finalPos The position in the vector up to which the fitness calculation should be performed
 * @return The best fitness found amongst all parents
 */
double GSerialGD::doFitnessCalculation(const std::size_t& finalPos) {
	double bestFitness = getWorstCase(); // Holds the best fitness found so far

#ifdef DEBUG
	if(finalPos > this->size()) {
		raiseException(
				"In GSerialGD::doFitnessCalculation(const std::size_t&):" << std::endl
				<< "Got invalid final position: " << finalPos << "/" << this->size()
		);
	}

	if(finalPos < nStartingPoints_) {
		raiseException(
				"In GSerialGD::doFitnessCalculation(const std::size_t&):" << std::endl
				<< "We require finalPos to be at least " << nStartingPoints_ << ", but got " << finalPos
		);
	}
#endif

	// Trigger value calculation for all individuals (including parents)
	double fitnessFound = 0.;
	for(std::size_t i=0; i<finalPos; i++) {
#ifdef DEBUG
		// Make sure the evaluated individuals have the dirty flag set
		if(!this->at(i)->isDirty()) {
			raiseException(
					"In GSerialGD::doFitnessCalculation(const std::size_t&):" << std::endl
					<< "Found individual in position " << i << " whose dirty flag isn't set"
			);
		}
#endif /* DEBUG*/

		fitnessFound = this->at(i)->fitness(0);

		// Update the best fitness value found
		if(i<nStartingPoints_) {
			if(isBetter(fitnessFound, bestFitness)) {
				bestFitness = fitnessFound;
			}
		}
	}

	return bestFitness;
}

/************************************************************************************************************/
/**
 * Resizes the population to the desired level and does some error checks.
 */
void GSerialGD::adjustPopulation() {
	// Check how many individuals we already have
	std::size_t nStart = this->size();

	// Do some error checking ...

	// We need at least one individual
	if(nStart == 0) {
		raiseException(
				"In GSerialGD::adjustPopulation():" << std::endl
				<< "You didn't add any individuals to the collection. We need at least one."
		);
	}

	// Update the number of floating point parameters in the individuals
	nFPParmsFirst_ = this->at(0)->countParameters<double>();

	// Check that the first individual has floating point parameters (double for the moment)
	if(nFPParmsFirst_ == 0) {
		raiseException(
				"In GSerialGD::adjustPopulation():" << std::endl
				<< "No floating point parameters in individual."
		);
	}

	// Check that all individuals currently available have the same amount of parameters
	for(std::size_t i=1; i<this->size(); i++) {
		if(this->at(i)->countParameters<double>() != nFPParmsFirst_) {
			raiseException(
					"In GSerialGD::adjustPopulation():" << std::endl
					<< "Found individual in position " <<  i << " with different" << std::endl
					<< "number of floating point parameters than the first one: " << this->at(i)->countParameters<double>() << "/" << nFPParmsFirst_
			);
		}
	}

	// Set the default size of the population
	GOptimizationAlgorithmT<GParameterSet>::setDefaultPopulationSize(nStartingPoints_*(nFPParmsFirst_+1));

	// First create a suitable number of start individuals and initialize them as required
	if(nStart < nStartingPoints_) {
		for(std::size_t i=0; i<(nStartingPoints_-nStart); i++) {
			// Create a copy of the first individual
			this->push_back(this->at(0)->clone<GParameterSet>());
			// Make sure our start values differ
			this->back()->randomInit();
		}
	} else {
		// Start with a defined size. This will remove surplus items.
		this->resize(nStartingPoints_);
	}

	// Add the required number of clones for each starting point. These will be
	// used for the calculation of the difference quotient for each parameter
	for(std::size_t i=0; i<nStartingPoints_; i++) {
		for(std::size_t j=0; j<nFPParmsFirst_; j++) {
			this->push_back(this->at(i)->clone<GParameterSet>());
		}
	}

	// We now should have nStartingPoints_ sets of individuals,
	// each of size nFPParmsFirst_.
#ifdef DEBUG
	if(this->size() != nStartingPoints_*(nFPParmsFirst_ + 1)) {
		raiseException(
				"In GSerialGD::adjustPopulation():" << std::endl
				<< "Population size is " << this->size() << std::endl
				<< "but expected " << nStartingPoints_*(nFPParmsFirst_ + 1)
		);
	}
#endif /* DEBUG */
}

/************************************************************************************************************/
/**
 * Saves the state of the class to disc.
 */
void GSerialGD::saveCheckpoint() const {
	// Copy the parent individuals to a vector
	std::vector<boost::shared_ptr<Gem::Geneva::GParameterSet> > bestIndividuals;
	GSerialGD::const_iterator it;
	for(it=this->begin(); it!=this->begin() + nStartingPoints_; ++it) {
		bestIndividuals.push_back(*it);
	}

	// Determine a suitable name for the output file
	std::string outputFile =
			getCheckpointDirectory() +
			(this->halted()?"final":boost::lexical_cast<std::string>(getIteration())) +
			"_"	+
			boost::lexical_cast<std::string>(getBestFitness()) +
			"_"	+
			getCheckpointBaseName();

	// Create the output stream and check that it is in good order
	std::ofstream checkpointStream(outputFile.c_str());
	if(!checkpointStream) {
		raiseException(
				"In GSerialGD::saveCheckpoint()" << std::endl
				<< "Error: Could not open output file " << outputFile.c_str()
		);
	}

	switch(getCheckpointSerializationMode()) {
	case Gem::Common::SERIALIZATIONMODE_TEXT:
		// Write the individuals' data to disc in text mode
		{
			boost::archive::text_oarchive oa(checkpointStream);
			oa << boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
		} // note: explicit scope here is essential so the oa-destructor gets called
		break;

	case Gem::Common::SERIALIZATIONMODE_XML:
		// Write the individuals' data to disc in XML mode
		{
			boost::archive::xml_oarchive oa(checkpointStream);
			oa << boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
		} // note: explicit scope here is essential so the oa-destructor gets called
		break;

	case Gem::Common::SERIALIZATIONMODE_BINARY:
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

/************************************************************************************************************/
/**
 * This helper function lets all individuals know about their position in the
 * population.
 */
void GSerialGD::markIndividualPositions() {
	for(std::size_t pos=0; pos<this->size(); pos++) {
		this->at(pos)->getPersonalityTraits<GGDPersonalityTraits>()->setPopulationPosition(pos);
	}
}

#ifdef GENEVATESTING
/************************************************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 */
bool GSerialGD::modify_GUnitTests() {
	bool result = false;

	// Call the parent class'es function
	if(GOptimizationAlgorithmT<GParameterSet>::modify_GUnitTests()) result = true;

	return result;
}

/************************************************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GSerialGD::specificTestsNoFailureExpected_GUnitTests() {
	// Call the parent class'es function
	GOptimizationAlgorithmT<GParameterSet>::specificTestsNoFailureExpected_GUnitTests();
}

/************************************************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GSerialGD::specificTestsFailuresExpected_GUnitTests() {
	// Call the parent class'es function
	GOptimizationAlgorithmT<GParameterSet>::specificTestsFailuresExpected_GUnitTests();
}
#endif /* GENEVATESTING */

/**********************************************************************************/
/**
 * The default constructor
 */
GSerialGD::GGDOptimizationMonitor::GGDOptimizationMonitor()
{ /* nothing */ }

/**********************************************************************************/
/**
 * The copy constructor
 *
 * @param cp A copy of another GGDOptimizationMonitor object
 */
GSerialGD::GGDOptimizationMonitor::GGDOptimizationMonitor(const GSerialGD::GGDOptimizationMonitor& cp)
	: GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT(cp)
  { /* nothing */ }

/**********************************************************************************/
/**
 * The destructor
 */
GSerialGD::GGDOptimizationMonitor::~GGDOptimizationMonitor()
{ /* nothing */ }

/**********************************************************************************/
/**
 * A standard assignment operator.
 *
 * @param cp A copy of another GGDOptimizationMonitor object
 * @return A constant reference to this object
 */
const GSerialGD::GGDOptimizationMonitor& GSerialGD::GGDOptimizationMonitor::operator=(const GSerialGD::GGDOptimizationMonitor& cp){
	GSerialGD::GGDOptimizationMonitor::load_(&cp);
	return *this;
}

/**********************************************************************************/
/**
 * Checks for equality with another GParameter Base object
 *
 * @param  cp A constant reference to another GGDOptimizationMonitor object
 * @return A boolean indicating whether both objects are equal
 */
bool GSerialGD::GGDOptimizationMonitor::operator==(const GSerialGD::GGDOptimizationMonitor& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of equality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_EQUALITY, 0.,"GSerialGD::GGDOptimizationMonitor::operator==","cp", CE_SILENT);
}

/**********************************************************************************/
/**
 * Checks for inequality with another GGDOptimizationMonitor object
 *
 * @param  cp A constant reference to another GGDOptimizationMonitor object
 * @return A boolean indicating whether both objects are inequal
 */
bool GSerialGD::GGDOptimizationMonitor::operator!=(const GSerialGD::GGDOptimizationMonitor& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of inequality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_INEQUALITY, 0.,"GSerialGD::GGDOptimizationMonitor::operator!=","cp", CE_SILENT);
}

/**********************************************************************************/
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
boost::optional<std::string> GSerialGD::GGDOptimizationMonitor::checkRelationshipWith(
		const GObject& cp
		, const Gem::Common::expectation& e
		, const double& limit
		, const std::string& caller
		, const std::string& y_name
		, const bool& withMessages
) const {
	using namespace Gem::Common;

	// Check that we are indeed dealing with a GParamterBase reference
	const GSerialGD::GGDOptimizationMonitor *p_load = GObject::conversion_cast<GSerialGD::GGDOptimizationMonitor >(&cp);

	// Will hold possible deviations from the expectation, including explanations
	std::vector<boost::optional<std::string> > deviations;

	// Check our parent class'es data ...
	deviations.push_back(GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT::checkRelationshipWith(cp, e, limit, "GSerialGD::GGDOptimizationMonitor", y_name, withMessages));

	// ... there is no local data

	return evaluateDiscrepancies("GSerialGD::GGDOptimizationMonitor", caller, deviations, e);
}

/**********************************************************************************/
/**
 * Allows to set the dimensions of the canvas
 *
 * @param xDim The desired dimension of the canvas in x-direction
 * @param yDim The desired dimension of the canvas in y-direction
 */
void GSerialGD::GGDOptimizationMonitor::setDims(const boost::uint16_t& xDim, const boost::uint16_t& yDim) {
	xDim_ = xDim;
	yDim_ = yDim;
}

/**********************************************************************************/
/**
 * Retrieves the dimension of the canvas in x-direction
 *
 * @return The dimension of the canvas in x-direction
 */
boost::uint16_t GSerialGD::GGDOptimizationMonitor::getXDim() const {
	return xDim_;
}

/**********************************************************************************/
/**
 * Retrieves the dimension of the canvas in y-direction
 *
 * @return The dimension of the canvas in y-direction
 */
boost::uint16_t GSerialGD::GGDOptimizationMonitor::getYDim() const {
	return yDim_;
}

/**********************************************************************************/
/**
 * A function that is called once before the optimization starts
 *
 * @param goa A pointer to the current optimization algorithm for which information should be emitted
 * @return A string containing information to written to the output file (if any)
 */
std::string GSerialGD::GGDOptimizationMonitor::firstInformation(GOptimizationAlgorithmT<GParameterSet> * const goa) {
	// This should always be the first statement in a custom optimization monitor
	std::cout << GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT::firstInformation(goa);

	// Perform the conversion to the target algorithm
#ifdef DEBUG
	if(goa->getOptimizationAlgorithm() != GD) {
		raiseException(
				"In GSerialGD::GGDOptimizationMonitor::firstInformation():" << std::endl
				<< "Provided optimization algorithm has wrong type: " << goa->getOptimizationAlgorithm()
		);
	}
#endif /* DEBUG */
	GSerialGD * const gd = static_cast<GSerialGD * const>(goa);

	// Output the header to the summary stream
	return gdFirstInformation(gd);
}

/**********************************************************************************/
/**
 * A function that is called during each optimization cycle. It is possible to
 * extract quite comprehensive information in each iteration. For examples, see
 * the standard overloads provided for the various optimization algorithms.
 *
 * @param goa A pointer to the current optimization algorithm for which information should be emitted
 * @return A string containing information to written to the output file (if any)
 */
std::string GSerialGD::GGDOptimizationMonitor::cycleInformation(GOptimizationAlgorithmT<GParameterSet> * const goa) {
	// Let the audience know what the parent has to say
	std::cout << GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT::cycleInformation(goa);

	// Perform the conversion to the target algorithm
#ifdef DEBUG
	if(goa->getOptimizationAlgorithm() != GD) {
		raiseException(
				"In GSerialGD::GGDOptimizationMonitor::cycleInformation():" << std::endl
				<< "Provided optimization algorithm has wrong type: " << goa->getOptimizationAlgorithm()
		);
	}
#endif /* DEBUG */
	GSerialGD * const gd = static_cast<GSerialGD * const>(goa);

	return gdCycleInformation(gd);
}

/**********************************************************************************/
/**
 * A function that is called once at the end of the optimization cycle
 *
 * @param goa A pointer to the current optimization algorithm for which information should be emitted
 * @return A string containing information to written to the output file (if any)
 */
std::string GSerialGD::GGDOptimizationMonitor::lastInformation(GOptimizationAlgorithmT<GParameterSet> * const goa) {

	// Perform the conversion to the target algorithm
#ifdef DEBUG
	if(goa->getOptimizationAlgorithm() != GD) {
		raiseException(
				"In GSerialGD::GGDOptimizationMonitor::lastInformation():" << std::endl
				<< "Provided optimization algorithm has wrong type: " << goa->getOptimizationAlgorithm()
		);
	}
#endif /* DEBUG */
	GSerialGD * const gd = static_cast<GSerialGD * const>(goa);

	// Do the actual information gathering
	std::ostringstream result;
	result << gdLastInformation(gd);

	// This should always be the last statement in a custom optimization monitor
	std::cout << GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT::lastInformation(goa);

	return result.str();
}


/**********************************************************************************/
/**
 * A function that is called once before the optimization starts
 *
 * @param gd The object for which information should be collected
 */
std::string GSerialGD::GGDOptimizationMonitor::gdFirstInformation(GSerialGD * const gd) {
	std::ostringstream result;

	// Output the header to the summary stream
	result << "{" << std::endl
		   << "  gROOT->Reset();" << std::endl
		   << "  gStyle->SetOptTitle(0);" << std::endl
		   << "  TCanvas *cc = new TCanvas(\"cc\",\"cc\",0,0," << xDim_ << "," << yDim_ << ");" << std::endl
		   << std::endl
		   << "  std::vector<long> iteration;" << std::endl
		   << "  std::vector<double> evaluation;" << std::endl
		   << std::endl;

	return result.str();
}

/**********************************************************************************/
/**
 * A function that is called during each optimization cycle
 *
 * @param gd The object for which information should be collected
 */
std::string GSerialGD::GGDOptimizationMonitor::gdCycleInformation(GSerialGD * const gd) {
	std::ostringstream result;
	bool isDirty = false;
	double currentEvaluation = 0.;


	result << "  iteration.push_back(" << gd->getIteration() << ");" << std::endl
		   << "  evaluation.push_back(" <<  gd->getBestFitness() << ");" << std::endl
	       << std::endl; // Improves readability when following the output with "tail -f"

	return result.str();
}

/**********************************************************************************/
/**
 * A function that is called once at the end of the optimization cycle
 *
 * @param gd The object for which information should be collected
 */
std::string GSerialGD::GGDOptimizationMonitor::gdLastInformation(GSerialGD * const gd) {
	std::ostringstream result;

	// Output final print logic to the stream
	result << "  // Transfer the vectors into arrays" << std::endl
		   << "  double iteration_arr[iteration.size()];" << std::endl
		   << "  double evaluation_arr[evaluation.size()];" << std::endl
		   << std::endl
		   << "  for(std::size_t i=0; i<iteration.size(); i++) {" << std::endl
		   << "     iteration_arr[i] = (double)iteration[i];" << std::endl
		   << "     evaluation_arr[i] = evaluation[i];" << std::endl
		   << "  }" << std::endl
		   << std::endl
		   << "  // Create a TGraph object" << std::endl
		   << "  TGraph *evGraph = new TGraph(evaluation.size(), iteration_arr, evaluation_arr);" << std::endl
		   << std::endl
		   << "  // Set the axis titles" << std::endl
		   << "  evGraph->GetXaxis()->SetTitle(\"Iteration\");" << std::endl
		   << "  evGraph->GetYaxis()->SetTitleOffset(1.1);" << std::endl
		   << "  evGraph->GetYaxis()->SetTitle(\"Fitness\");" << std::endl
		   << std::endl
		   << "  // Do the actual drawing" << std::endl
		   << "  evGraph->Draw(\"APL\");" << std::endl
		   << "}" << std::endl;

	return result.str();
}

/**********************************************************************************/
/**
 * Loads the data of another object
 *
 * cp A pointer to another GGDOptimizationMonitor object, camouflaged as a GObject
 */
void GSerialGD::GGDOptimizationMonitor::load_(const GObject* cp) {
	const GSerialGD::GGDOptimizationMonitor *p_load = conversion_cast<GSerialGD::GGDOptimizationMonitor>(cp);

	// Load the parent classes' data ...
	GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT::load_(cp);

	// no local data
}

/**********************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A deep clone of this object
 */
GObject* GSerialGD::GGDOptimizationMonitor::clone_() const {
	return new GSerialGD::GGDOptimizationMonitor(*this);
}

#ifdef GENEVATESTING
/**********************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 */
bool GSerialGD::GGDOptimizationMonitor::modify_GUnitTests() {
	bool result = false;

	// Call the parent class'es function
	if(GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT::modify_GUnitTests()) result = true;

	return result;
}

/**********************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GSerialGD::GGDOptimizationMonitor::specificTestsNoFailureExpected_GUnitTests() {
	// Call the parent class'es function
	GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT::specificTestsNoFailureExpected_GUnitTests();
}

/**********************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GSerialGD::GGDOptimizationMonitor::specificTestsFailuresExpected_GUnitTests() {
	// Call the parent class'es function
	GOptimizationAlgorithmT<GParameterSet>::GOptimizationMonitorT::specificTestsFailuresExpected_GUnitTests();
}

/**********************************************************************************/
#endif /* GENEVATESTING */

} /* namespace Geneva */
} /* namespace Gem */


/************************************************************************************************************/
#ifdef GENEVATESTING
/**
 * A factory function that emits a GSerialGD object
 */
template <> boost::shared_ptr<Gem::Geneva::GSerialGD> TFactory_GUnitTests<Gem::Geneva::GSerialGD>() {
	using namespace Gem::Tests;
	boost::shared_ptr<Gem::Geneva::GSerialGD> p;
	BOOST_CHECK_NO_THROW(p= boost::shared_ptr<Gem::Geneva::GSerialGD>(new Gem::Geneva::GSerialGD()));
	p->push_back(boost::shared_ptr<GTestIndividual1>(new GTestIndividual1()));
	return p;
}
#endif /* GENEVATESTING */

