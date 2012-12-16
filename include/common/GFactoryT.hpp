/**
 * @file GFactoryT.hpp
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

// Standard header files go here
#include <iostream>
#include <sstream>
#include <string>

// Boost header files go here
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/filesystem.hpp>

#ifndef GFACTORYT_HPP_
#define GFACTORYT_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva header files go here
#include <common/GParserBuilder.hpp>
#include <common/GExceptions.hpp>

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * A factory class that returns objects of type T . The class comprises a framework
 * for reading additional configuration options from a configuration file. The actual setup
 * work needs to be done in functions that are implemented in derived classes for each target
 * object individually, or in specializations of this class.
 */
template <typename T>
class GFactoryT
	:private boost::noncopyable
{
public:
	/***************************************************************************/
	/**
	 * The standard constructor
	 *
	 * @param configFile The name of a configuration file holding information about objects of type T
	 */
	GFactoryT(const std::string& configFile)
		: configFile_(configFile)
		, id_(std::size_t(0))
		, initialized_(false)
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * The destructor
	 */
	virtual ~GFactoryT()
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * Triggers the creation of objects of the desired type
	 *
	 * @return An individual of the desired type
	 */
	boost::shared_ptr<T> operator()() {
	   return this->get();
	}

   /***************************************************************************/
	/**
	 * Allows the creation of objects of the desired type in the old-fashioned way
	 */
	virtual boost::shared_ptr<T> get() {
      // Make sure the initialization code has been executed.
      // This function will do nothing when called more than once
      this->globalInit();

      // Create a parser builder object. It will be destroyed at
      // the end of this function and thus cannot cause trouble
      // due to registered call-backs and references
      Gem::Common::GParserBuilder gpb;

      // Add specific configuration options for the derived factory.
      // These may correspond to local variables
      this->describeLocalOptions_(gpb);

      // Retrieve the actual object. It may, in the process of its
      // creation, add further configuration options and call-backs to
      // the parser
      boost::shared_ptr<T> p = this->getObject_(gpb, id_++);

      // Read the configuration parameters from file
      if(!gpb.parseConfigFile(configFile_)) {
         glogger
         << "In GFactoryT<T>::operator(): Error!" << std::endl
         << "Could not parse configuration file " << configFile_ << std::endl
         << GEXCEPTION;
      }

      // Allow the factory to act on configuration options received
      // in the parsing process.
      this->postProcess_(p);

      // Let the audience know
      return p;
	}

	/***************************************************************************/
	/**
	 * Retrieves an object of the desired type and converts it to a target type
	 */
	template <typename dT> // "dT" stands for "derived type"
	boost::shared_ptr<dT> get() {

#ifdef DEBUG
      boost::shared_ptr<dT> p = boost::dynamic_pointer_cast<dT>(this->get());
      if(p) return p;
      else {
         glogger
         << "In boost::shared_ptr<dT> GFactoryT<T>::get<dT>() :" << std::endl
         << "Invalid conversion" << std::endl
         << GEXCEPTION;

         // Make the compiler happy
         return boost::shared_ptr<dT>();
      }
#else
      return boost::static_pointer_cast<dT>(this->get());
#endif /* DEBUG */
	}

	/***************************************************************************/
	/**
	 * Writes a configuration file to disk
	 *
	 * @param configFile The name of the configuration file to be written
	 * @param header A header to be prepended to the configuration file
	 */
	void writeConfigFile(const std::string& header) {
		// Make sure the initialization code has been executed.
		// This function will do nothing when called more than once
		this->globalInit();

		// Create a parser builder object. It will be destroyed at
		// the end of this function and thus cannot cause trouble
		// due to registered call-backs and references
		Gem::Common::GParserBuilder gpb;

		// Add the user-defined configuration specifications, local to the factory
		this->describeLocalOptions_(gpb);

		// Retrieve an object (will be discarded at the end of this function)
		// Here, further options may be added to the parser builder.
		boost::shared_ptr<T> p = this->getObject_(gpb, id_++);

		// Allow the factory to act on configuration options received
		// in the parsing process.
		this->postProcess_(p);

		// Write out the configuration file, if options have been registered
		if(gpb.numberOfOptions() > 0) {
			gpb.writeConfigFile(configFile_, header, true);
		} else {
			std::cout
				<< "Warning: Tried to write out configuration file " << configFile_ << std::endl
				<< "even though no configuration options were registered. Doing nothing." << std::endl;
		}
	}

protected:
	/***************************************************************************/
	/** @brief Performs necessary initialization work */
	virtual void init_() {}
	/** @brief Allows to describe local configuration options in derived classes */
	virtual void describeLocalOptions_(Gem::Common::GParserBuilder& gpb) {};
	/** @brief Creates individuals of the desired type */
	virtual boost::shared_ptr<T> getObject_(Gem::Common::GParserBuilder&, const std::size_t&) = 0;
	/** @brief Allows to act on the configuration options received from the configuration file */
	virtual void postProcess_(boost::shared_ptr<T>&) = 0;

private:
   /***************************************************************************/
   /**
    * Performs necessary global initialization work. This function is meant for
    * initialization work performed just one prior to the creation of the first
    * item. It will do nothing when called more than once. All real work is done
    * in the "init_()" function, which may be overloaded by the user.
    */
   void globalInit() {
      if(!initialized_) {
         // Perform the user-defined initialization work
         this->init_();
         initialized_ = true;
      }
   }

	/***************************************************************************/
	GFactoryT(); ///< The default constructor. Intentionally private and undefined

	std::string configFile_; ///< The name of the configuration file
	std::size_t id_; ///< The id/number of the individual currently being created
	bool initialized_; ///< Indicates whether the configuration file has already been parsed
};

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

#endif /* GFACTORYT_HPP_ */
