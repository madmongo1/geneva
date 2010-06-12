/**
 * @file GRandom.cpp
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

#include "GRandom.hpp"

// Included here so no conflicts occur. See explanation at
// http://www.boost.org/libs/serialization/doc/special.html#derivedpointers
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(Gem::Util::GRandom)

namespace Gem {
namespace Util {

/*************************************************************************/
/**
 * The standard constructor. It stores the GRANDOMFACTORY
 * boost::shared_ptr<GRandomFactory> locally and in this way
 * makes sure that the factory doesn't get destroyed while it
 * is still needed.
 */
GRandom::GRandom()
	: rnrGenerationMode_(Gem::Util::RNRFACTORY)
	, currentPackageSize_(DEFAULTARRAYSIZE)
	, current01_(1) // position 0 holds the array size
	, grf_(GRANDOMFACTORY)
	, initialSeed_(GRANDOMFACTORY->getSeed())
	, linCongr_(boost::numeric_cast<boost::uint64_t>(initialSeed_))
	, gaussCache_(0.)
	, gaussCacheAvailable_(false)
{ /* nothing */ }

/*************************************************************************/
/**
 * Initialization with the random number generation mode
 *
 * @param rnrGenMode The random number generation mode (local vs. factory)
 */
GRandom::GRandom(const Gem::Util::rnrGenerationMode& rnrGenMode)
	: rnrGenerationMode_(rnrGenMode)
	, currentPackageSize_(DEFAULTARRAYSIZE)
	, current01_(1) // position 0 holds the array size
	, initialSeed_(GRANDOMFACTORY->getSeed())
	, linCongr_(boost::numeric_cast<boost::uint64_t>(initialSeed_))
	, gaussCache_(0.)
	, gaussCacheAvailable_(false)
{
    switch(rnrGenerationMode_) {
    case Gem::Util::RNRFACTORY:
    	// Make sure we have a local pointer to the factory
    	grf_ = GRANDOMFACTORY;
    	break;

    case Gem::Util::RNRLOCAL:
    	// Reset all other random number generation modes
    	p01_.reset();
    	grf_.reset();
    	break;
    };
}

/*************************************************************************/
/**
 * The copy constructor. Note that we acquire our own seed and do not copy
 * it from the other object. This way we do not have two generators that produce
 * the same numbers.
 *
 * @param cp A copy of another GRandom object
 */
GRandom::GRandom(const GRandom& cp)
	: rnrGenerationMode_(cp.rnrGenerationMode_)
	, currentPackageSize_(DEFAULTARRAYSIZE)
	, current01_(1) // position 0 holds the array size
	, initialSeed_(GRANDOMFACTORY->getSeed()) // We do not want to use the other generator's start seed
	, linCongr_(boost::numeric_cast<boost::uint64_t>(initialSeed_))
	, gaussCache_(0.)
	, gaussCacheAvailable_(false)
{
    switch(rnrGenerationMode_) {
    case Gem::Util::RNRFACTORY:
    	// Make sure we have a local pointer to the factory
    	grf_ = GRANDOMFACTORY;
    	break;

    case Gem::Util::RNRLOCAL:
    	// Reset all other random number generation modes
    	p01_.reset();
    	grf_.reset();
    	break;
    };
}

/*************************************************************************/
/**
 * A standard destructor
 */
GRandom::~GRandom() {
	p01_.reset();
	grf_.reset();
}

/*************************************************************************/
/**
 * A standard assignment operator.
 *
 * @param cp A copy of another GRandom object
 */
GRandom& GRandom::operator=(const GRandom& cp) {
	GRandom::load(cp);
	return *this;
}

/*************************************************************************/
/**
 * Loads the data of another GRandom object
 *
 * @param cp A copy of another GRandom object
 */
void GRandom::load(const GRandom& cp) {
	// Then our own data
	rnrGenerationMode_ = cp.rnrGenerationMode_;
	switch(rnrGenerationMode_) {
	case RNRFACTORY:
		setRNRFactoryMode();
		break;
	case RNRLOCAL:
		setRNRLocalMode();
		break;
	};

	// Other class-local variables do not need to be loaded, as they have
	// either been initialized at construction time or are temporary variables
}

/*************************************************************************/
/**
 * Sets the seed of the local random number generator
 *
 * @param seed A user-defined seed for the local random number generator
 */
void  GRandom::setSeed(const boost::uint32_t& seed) {
	initialSeed_ = seed;

    // Make sure we continue with the correct seed
    linCongr_.seed(boost::numeric_cast<boost::uint64_t>(initialSeed_));
}

/*************************************************************************/
/**
 * Retrieves the current seed value of the local generator.
 * Mostly useful for testing purposes.
 *
 * @return The current value of the initialSeed_ variable
 */
boost::uint32_t GRandom::getSeed() {
	return initialSeed_;
}

/*************************************************************************/
/**
 * Sets the random number generation mode to a new value
 *
 * @param rnrGenMode The new desired random number generation mode
 */

void GRandom::setRnrGenerationMode(const Gem::Util::rnrGenerationMode& rnrGenMode) {
	switch (rnrGenMode) {
	case Gem::Util::RNRFACTORY:
		setRNRFactoryMode();
		break;
	case Gem::Util::RNRLOCAL:
		setRNRLocalMode();
		break;
	}
}

/*************************************************************************/
/**
 * Retrieves the current random number generation mode
 *
 * @return The current random number generation scheme
 */
Gem::Util::rnrGenerationMode  GRandom::getRnrGenerationMode () const {
	return rnrGenerationMode_;
}

/*************************************************************************/
/**
 * Switches to factory production mode and disables all other modes
 */
void  GRandom::setRNRFactoryMode() {
	// Do nothing if the mode has already been set
	if(rnrGenerationMode_ == Gem::Util::RNRFACTORY) return;

	// Get access to the factory and set the mode
	rnrGenerationMode_ = Gem::Util::RNRFACTORY;
	grf_ = GRANDOMFACTORY;
}

/*************************************************************************/
/**
 * Switches to local production mode, using GRandomFactory::getSeed() for seeding,
 *  and disables all other modes
 */
void  GRandom::setRNRLocalMode() {
	// Do nothing if the mode has already been set
	if(rnrGenerationMode_ == Gem::Util::RNRLOCAL) return;

	// Reset all other random number generation modes
	p01_.reset();
	grf_.reset();

	// Switch the mode
	rnrGenerationMode_ = Gem::Util::RNRLOCAL;
}

/*************************************************************************/
/**
 * Switches to local production mode, using the supplied seed value,
 *  and disables all other modes
 *
 * @param seed A user-defined seed for the local random number generator
 */
void  GRandom::setRNRLocalMode(const boost::uint32_t& seed) {
	// If the mode has already been set, just reset the seed
	if(rnrGenerationMode_ == Gem::Util::RNRLOCAL) {
		setSeed(seed);
	}
	else { // Otherwise take the required steps
		// Reset all other random number generation modes
		p01_.reset();
		grf_.reset();

		// Set the seed
		setSeed(seed);

		// Switch the mode
		rnrGenerationMode_ = RNRLOCAL;
	}
}

/*************************************************************************/
/**
 * This function emits evenly distributed random numbers in the range [0,max[ .
 *
 * @param max The maximum (excluded) value of the range
 * @return Random numbers evenly distributed in the range [0,max[
 */
double GRandom::evenRandom(const double& max) {
#ifdef DEBUG
	// Check that min and max have appropriate values
	assert(max>0.);
#endif
	return GRandom::evenRandom() * max;
}

/*************************************************************************/
/**
 * This function produces evenly distributed random numbers in the range [min,max[ .
 *
 * @param min The minimum value of the range
 * @param max The maximum (excluded) value of the range
 * @return Random numbers evenly distributed in the range [min,max[
 */
double GRandom::evenRandom(const double& min, const double& max) {
#ifdef DEBUG
	// Check that min and max have appropriate values
	assert(min<=max);
#endif
	return GRandom::evenRandom() * (max - min) + min;
}

/*************************************************************************/
/**
 * Gaussian-distributed random numbers form the core of Evolutionary Strategies.
 * This function provides an easy means of producing such random numbers with
 * mean "mean" and sigma "sigma".
 *
 * @param mean The mean value of the Gaussian
 * @param sigma The sigma of the Gaussian
 * @return double random numbers with a gaussian distribution
 */
double GRandom::gaussRandom(const double& mean, const double& sigma) {
	if(gaussCacheAvailable_) {
		gaussCacheAvailable_ = false;
		return sigma*gaussCache_ + mean;
	}
	else {
#ifdef USEBOXMULLER
		double y1 = 0.;
		double rnr1 = evenRandom();
		double rnr2 = evenRandom();
		y1 = sigma * sqrt(fabs(-2. * log(1. - rnr1))) * sin(2. * M_PI	* rnr2) + mean;
		gaussCache_ = sqrt(fabs(-2. * log(1. - rnr1))) * cos(2. * M_PI	* rnr2);
		gaussCacheAvailable_ = true;
		return y1;
#else // USEBOXMULLERPOLAR, see here: http://de.wikipedia.org/wiki/Normalverteilung#Polar-Methode ; faster than USEBOXMULLER
		double q, u1, u2, y1;
        do {
        	u1 = 2.* evenRandom() - 1.;
        	u2 = 2.* evenRandom() - 1.;
        	q = u1*u1 + u2*u2;
        } while (q > 1.0);
        q = sqrt((-2.*log(q))/q);
        y1 = sigma * u1 * q + mean;
        gaussCache_ = u2 * q;
        gaussCacheAvailable_ = true;
		return y1;
#endif
	}
}

/*************************************************************************/
/**
 * This function adds two gaussians with sigma "sigma" and a distance
 * "distance" from each other of distance, centered around mean.
 *
 * @param mean The mean value of the entire distribution
 * @param sigma The sigma of both gaussians
 * @param distance The distance between both peaks
 * @return Random numbers with a double-gaussian shape
 */
double GRandom::doubleGaussRandom(const double& mean, const double& sigma, const double& distance) {
	if (GRandom::boolRandom())
		return GRandom::gaussRandom(mean - fabs(distance / 2.), sigma);
	else
		return GRandom::gaussRandom(mean + fabs(distance / 2.), sigma);
}

/*************************************************************************/
/**
 * This function returns true with a probability "probability", otherwise false.
 *
 * @param p The probability for the value "true" to be returned
 * @return A boolean value, which will be true with a user-defined likelihood
 */
bool GRandom::boolRandom(const double& probability) {
#ifdef DEBUG
	assert(probability>=0. && probability<=1.);
#endif
	return ((GRandom::evenRandom()<probability)?true:false);
}

/*************************************************************************/
/**
 * This function produces boolean values with a 50% likelihood each for
 * true and false.
 *
 * @return Boolean values with a 50% likelihood for true/false respectively
 */
bool GRandom::boolRandom() {
	return boolRandom(0.5);
}

/*************************************************************************/
/**
 * This function produces random ASCII characters. Please note that that
 * includes also non-printable characters, if "printable" is set to false
 * (default is true).
 *
 * @param printable A boolean variable indicating whether only printable characters should be produced
 * @return Random ASCII characters
 */
char GRandom::charRandom(const bool& printable) {
	if (!printable) {
		return (char) discreteRandom<boost::uint8_t>(0, 128);
	} else {
		return (char) discreteRandom<boost::uint8_t>(33, 127);
	}
}

/*************************************************************************/
/**
 * In cases where GRandomFactory was not able to supply us with a suitable
 * array of [0,1[ random numbers we need to produce our own.
 */
void GRandom::fillContainer01() {
	boost::shared_array<double> p(new double[currentPackageSize_+1]);
	double *local_p = p.get();
	local_p[0] = static_cast<double>(currentPackageSize_);

	for (std::size_t i = 1; i <= currentPackageSize_; i++) {
		local_p[i] = evenRandomLocalProduction();
	}

	p01_ = p;
}

/*************************************************************************/
/**
 * (Re-)Initialization of p01_. Checks that a valid GRandomFactory still
 * exists, then retrieves a new container.
 */
void GRandom::getNewP01() {
	if(grf_) {
		p01_ = grf_->new01Container();
	}

	if (!grf_ || !p01_) {
		// Something went wrong with the retrieval of the
		// random number container. We need to create
		// our own instead. This will be done with the last
		// known array size
		GRandom::fillContainer01();
	}

	// We should now have a valid p01_ in any case
	p_raw_ = p01_.get();

	// Extract the array size for later use
	currentPackageSize_ = static_cast<std::size_t>(p_raw_[0]);
}

/*************************************************************************/
/**
 * Puts a Gem::Util::rnrGenerationMode item into a stream
 *
 * @param o The ostream the item should be added to
 * @param rnrgen the item to be added to the stream
 * @return The std::ostream object used to add the item to
 */
std::ostream& operator<<(std::ostream& o, const Gem::Util::rnrGenerationMode& rnrgen){
	boost::uint16_t tmp = static_cast<boost::uint16_t>(rnrgen);
	o << tmp;
	return o;
}

/*************************************************************************/
/**
 * Reads a Gem::Util::rnrGenerationMode item from a stream
 *
 * @param i The stream the item should be read from
 * @param rnrgen The item read from the stream
 * @return The std::istream object used to read the item from
 */
std::istream& operator>>(std::istream& i, Gem::Util::rnrGenerationMode& rnrgen){
	boost::uint16_t tmp;
	i >> tmp;

#ifdef DEBUG
	rnrgen = boost::numeric_cast<Gem::Util::rnrGenerationMode>(tmp);
#else
	rnrgen = static_cast<Gem::Util::rnrGenerationMode>(tmp);
#endif /* DEBUG */

	return i;
}

/*************************************************************************/

} /* namespace Util */
} /* namespace Gem */