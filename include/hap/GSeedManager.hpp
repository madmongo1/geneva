/**
 * @file GSeedManager.hpp
 */

/* Copyright (C) Dr. Ruediger Berlich and Karlsruhe Institute of Technology
 * (University of the State of Baden-Wuerttemberg and National Laboratory
 * of the Helmholtz Association)
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Hap library, Gemfony scientific's library of
 * random number routines.
 *
 * Hap is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Hap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Hap library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Hap, visit
 * http://www.gemfony.com .
 */

// Standard headers go here

#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>

// Includes check for correct Boost version(s)
#include "common/GGlobalDefines.hpp"

// Boost headers go here

#include <boost/cstdint.hpp>
#include <boost/random.hpp>
#include <boost/nondet_random.hpp>
#include <boost/date_time.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <boost/cast.hpp>
#include <boost/static_assert.hpp>

/**
 * Check that we have support for threads. This class is useless without this.
 */
#ifndef BOOST_HAS_THREADS
#error "Error: No support for Boost.threads available."
#endif

#ifndef GSEEDMANAGER_HPP_
#define GSEEDMANAGER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Gemfony headers go here
#include "common/GBoundedBufferT.hpp"
#include "common/GCommonEnums.hpp"
#include "common/GExceptions.hpp"
#include "hap/GRandomDefines.hpp"

/****************************************************************************/

namespace Gem {
namespace Hap {

/****************************************************************************/
/**
 * This class manages a set of seeds, making sure they are handed out in
 * pseudo random order themselves. The need for this class became clear when it
 * turned out that random number sequences with successive seeds can be
 * highly correlated. This can only be amended by handing out seeds randomly
 * themselves. A start seed for the seeding sequence is either taken from a
 * non deterministic generator, or provided by the user.
 */
class GSeedManager:
	private boost::noncopyable // prevents this class from being copied
{
public:
	/** @brief The default constructor. */
	GSeedManager();
	/** @brief Initialization with a start seed */
	GSeedManager(const initial_seed_type& startSeed, const std::size_t& seedQueueSize = DEFAULTSEEDQUEUESIZE);
	/** @brief The destructor */
	~GSeedManager();

	/** @brief Allows different objects to retrieve seeds concurrently */
	seed_type getSeed();

	/** @brief Allows different objects to retrieve seeds concurrently, observing a time-out. */
	seed_type getSeed(const boost::posix_time::time_duration&);

	/** @brief Checks whether the global seeding has already started */
	bool checkSeedingIsInitialized() const;

	/** @brief Retrieves the value of the initial start seed */
	initial_seed_type getStartSeed() const;

	/** @brief Retrieves the maximum size of the seed queue */
	std::size_t getQueueSize() const;

private:
	/*************************************************************************/
	/**
	 * Wrapper function that attempts to create a start seed using different methods
	 *
	 * @return A start seed for the random seed sequence
	 */
	static initial_seed_type createStartSeed() {
		initial_seed_type startSeed = 0;

		if(!(startSeed = GSeedManager::createStartSeedDevURandom())) {
			std::cerr << "Warning: Using /dev/urandom to retrieve global seed failed." << std::endl
				      << "Using the current time instead: ";
			startSeed = GSeedManager::createStartSeedCurrentTime();
			std::cerr << startSeed << std::endl;
	 	}
		else {
			std::cout << "Used /dev/urandom to retrieve start seed " << startSeed << std::endl;
		}

		return startSeed;
	}

	/*************************************************************************/
	/**
	 * A private member function that allows to set the global
	 * seed once, using a random number taken from /dev/urandom. As enough entropy
	 * needs to be available and reads from /dev/random will block (on Linux) if this
	 * is not the case, this function should be called only rarely and is meant for
	 * the initialization of the random seed sequence only.
	 *
	 * @return A random number obtained from /dev/urandom
	 */
	static initial_seed_type createStartSeedDevURandom() {
		// Check if /dev/random exists (this might not be a Linux system after all)
		if(!boost::filesystem::exists("/dev/urandom")) return 0;

		// Open the device
		std::ifstream devurandom("/dev/urandom");
		if(!devurandom) return 0;

		// Read in the data
		initial_seed_type startSeed;
		devurandom.read(reinterpret_cast<char*>(&startSeed), sizeof(startSeed));

		// Clean up
		devurandom.close();

		return startSeed;
	}

	/************************************************************************/
	/**
	 * Allows to derive a seed from the current time. Note that, although we try to
	 * add randomness, this might not give good results if many seeds are requested by
	 * different entities in short succession. It should be sufficient for a one-time
	 * retrieval of a seed for the seed random sequence, though. Remote components of
	 * an application needing unique seeds should retrieve them out of the random
	 * sequence instead of this function. Note also that results obtained with this
	 * function might depend on the endianness of your machine (tested on x86_64 only).
	 */
	static initial_seed_type createStartSeedCurrentTime() {
		using namespace boost::posix_time;
		using namespace boost::gregorian;

		// There might be strange systems where this is not the case
		BOOST_STATIC_ASSERT(sizeof(long) >= sizeof(initial_seed_type));

		// Retrieve the time passed since January 1, 1970 in microseconds
		long currentMS = (microsec_clock::local_time() - ptime(date(1970, Jan, 1))).total_microseconds();

		// Attach the char pointer to where the boost::uint32_t inside of the long would start
		char *currentMSStart = reinterpret_cast<char*>(&currentMS);

		// Note: whether the above will give you different values upon each invocation
		// will likely depend on the endianness of your machine. It has been tested on
		// x86_64 only. You might need to use something along the lines of:
		// std::size_t nHops = sizeof(long) - sizeof(boost::uint32_t);
		// currentMSStart+=nHops;
		// Note: This is totally untested.

		// Attach the seed to this point
		initial_seed_type *seed_ptr = 0;
		seed_ptr = reinterpret_cast<initial_seed_type *>(currentMSStart);

		// Let the audience know
		return *seed_ptr;
	}

	/************************************************************************/
	/** @brief Manages the production of seeds */
	void seedProducer();

	/************************************************************************/
	// Variables and data structures

	/** @brief Holds a predefined number of unique seeds */
	Gem::Common::GBoundedBufferT<seed_type> seedQueue_;

	/** @brief Stores the initial start seed */
	initial_seed_type startSeed_;

	/** @brief Holds the producer thread */
	thread_ptr seedThread_;
};

} /* namespace Hap */
} /* namespace Gem */

#endif /* GSEEDMANAGER_HPP_ */