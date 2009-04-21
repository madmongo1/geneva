/**
 * @file GSingleton.hpp
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

// Boost headers go here

#include <boost/version.hpp>
#include "GGlobalDefines.hpp"
#if BOOST_VERSION < ALLOWED_BOOST_VERSION
#error "Error: Boost has incorrect version !"
#endif /* BOOST_VERSION */

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

/**
 * Check that we have support for threads. This collection of classes is useless
 * without this.
 */
#ifndef BOOST_HAS_THREADS
#error "Error: No support for Boost.threads available."
#endif

#ifndef GSINGLETON_HPP_
#define GSINGLETON_HPP_

namespace Gem {
namespace Util {

/************************************************************************/
/**
 * This class implements a singleton pattern, augmented so that it returns
 * a boost::shared_ptr. This is allows other singletons to store a copy of
 * T, so that it only gets destroyed once it is no longer needed. Note that
 * the static shared_ptr may long have vanished at that time.
 */
template<typename T>
class GSingleton
	:private boost::noncopyable
{
public:
	/*******************************************************************/
	/**
	 * If called for the first time, the function creates a boost::shared_ptr
	 * of T and returns it to the caller. Subsequent calls to this function
	 * will return the stored copy of the shared_ptr. Other classes can store
	 * the pointer, so that T doesn't get deleted while it is still needed.
	 */
	static boost::shared_ptr<T>& getInstance() {
		static bool first=true;
		static boost::shared_ptr<T> p;
		static boost::mutex creation_mutex;

		if(first) {
			// Prevent concurrent "first access"
			boost::mutex::scoped_lock lk(creation_mutex);

			p = boost::shared_ptr<T>(new T());
			first=false;
		}

		return p;
	}

	/*******************************************************************/

private:
	GSingleton(); ///< Intentionally left undefined
	~GSingleton(); ///< Intentionally left undefined
};

/************************************************************************/

} /* namespace Util */
} /* namespace Gem */

#endif /* GSINGLETON_HPP_ */
