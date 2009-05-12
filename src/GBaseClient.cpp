/**
 * @file GBaseClient.cpp
 */

/* Copyright (C) 2004-2008 Dr. Ruediger Berlich
 * Copyright (C) 2008 Forschungszentrum Karlsruhe GmbH
 *
 * This file is part of the Geneva library.
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

#include "GBaseClient.hpp"

namespace Gem
{
namespace GenEvA
{

/*********************************************************************************/
/**
 * The default constructor.
 */
GBaseClient::GBaseClient()
	:startTime_(boost::posix_time::microsec_clock::local_time()),
	 maxDuration_(boost::posix_time::microsec(0)),
	 processed_(0),
	 processMax_(0)
{ /* nothing*/ }

/*********************************************************************************/
/**
 * A standard destructor. We have no local, dynamically allocated data, hence it is empty.
 */
GBaseClient::~GBaseClient()
{ /* nothing */ }

/*********************************************************************************/
/**
 * This is the main loop of the client. It will continue to call the process()
 * function (defined by derived classes), until it returns false or the maximum
 * number of processing steps has been reached. All network connectivity is done
 * in process().
 */
void GBaseClient::run(){
	try{
		while(!this->halt() && this->process()){;}
	}
	catch(std::exception& e){
		std::ostringstream error;
		error << "In GBaseClient::run(): Caught std::exception with message" << std::endl
		      << e.what() << std::endl;
		std::cerr << error.str();
		std::terminate();
	}
	catch(boost::exception& e){
		std::ostringstream error;
		error << "In GBaseClient::run(): Caught boost::exception" << std::endl;
		std::cerr << error.str();
		std::terminate();
	}
	catch(...){
		std::ostringstream error;
		error << "In GBaseClient::run(): Caught unknown exception" << std::endl;
		std::cerr << error.str();
		std::terminate();
	}
}

/*********************************************************************************/
/**
 * Allows to set a maximum number of processing steps. If set to 0 or left unset,
 * processing will be done until process() returns false.
 *
 * @param processMax Desired value for the processMax_ variable
 */
void GBaseClient::setProcessMax(const boost::uint32_t& processMax){
	processMax_ = processMax;
}

/*********************************************************************************/
/**
 * Retrieves the value of the processMax_ variable.
 *
 * @return The value of the processMax_ variable
 */
boost::uint32_t GBaseClient::getProcessMax() const{
	return processMax_;
}

/*********************************************************************************/
/**
 * Sets the maximum allowed processing time
 *
 * @param maxDuration The maximum allowed processing time
 */
void GBaseClient::setMaxTime(const boost::posix_time::time_duration& maxDuration) {
	using namespace boost::posix_time;

	// Only allow "real" values
	if(maxDuration.is_special() || maxDuration.is_negative()) {
		std::ostringstream error;
		error << "In GBaseClient::setMaxTime() : Error!" << std::endl
			  << "Invalid maxDuration." << std::endl;

		throw geneva_error_condition(error.str());
	}

	maxDuration_ = maxDuration;
}

/*********************************************************************************/
/**
 * Retrieves the value of the maxDuration_ parameter.
 *
 * @return The maximum allowed processing time
 */
boost::posix_time::time_duration GBaseClient::getMaxTime() {
	return maxDuration_;
}

/*********************************************************************************/
/**
 * Checks whether a halt condition was reached. Either the maximum number of processing
 * steps was reached or the maximum allowed time was reached.
 *
 * @return A boolean indicating whether a halt condition was reached
 */
bool GBaseClient::halt(){
	using namespace boost::posix_time;

	// Maximum number of processing steps reached ?
	if(processMax_ && (processed_++ >= processMax_)) return true;

	// Maximum duration reached ?
	if(maxDuration_.total_microseconds() &&
	  ((microsec_clock::local_time() - startTime_) >= maxDuration_)) return true;

	// Custom halt condition reached ?
	if(customHalt()) return true;

	return false;
}

/*********************************************************************************/
/**
 * In order to allow derived classes to concentrate on network issues, all
 * unpacking, the calculation, and packing is done in the GBaseClient class,
 * which in turn makes use of the facilities provided by GMemberCarrier.
 */
bool GBaseClient::process(){
	 // Store the current serialization mode
	serializationMode serMode;

	// Get an item from the server
	std::string istr;
	std::string serModeStr;
	if(!this->retrieve(istr, serModeStr)) return false;

	// There is a possibility that we have received an unknown command
	// or a timeout command. In this case we want to try again until retrieve
	// returns "false". If we return true here, the next "process" command will
	// be executed.
	if(istr == "empty") return true;

	// Check the serialization mode we need to use
	if(serModeStr == ""){
		std::ostringstream error;
		error << "In GBaseClient::process() : Error!" << std::endl
			  << "Found empty serModeStr" << std::endl;
		std::cerr << error.str();

		return false;
	}

	serMode = boost::lexical_cast<serializationMode>(serModeStr);

	// unpack the data and create a new GIndividual. Note that de-serialization should
	// generally happen through the same type that was used for serialization.
	boost::shared_ptr<GIndividual> target = indptrFromString(istr, serMode);

	// This one line is all it takes to do the processing required for this individual.
	// GIndividual has all required functions on board. GBaseClient does not need to understand
	// what is being done during the processing.
	target->process();

	// We do not want to accidently trigger value calculation if it is not desired by the user
	// - after all we do not know in this class was is done in GIndividual::process() .
	bool isDirty;
	double fitness = target->getCurrentFitness(isDirty);

	std::string portid = target->getAttribute("id");

	if(portid.empty()){ // This is a severe error
		std::ostringstream error;
		error << "In GBaseClient::process() : Error!" << std::endl
			  << "Empty portid found" << std::endl;
		std::cerr << error.str();

		return false;
	}

	// transform target back into a string and submit to the server. The actual
	// actions done by submit are defined by derived classes.
	if(!this->submit(
			indptrToString(target, serMode),
			portid,
			boost::lexical_cast<std::string>(fitness),
			boost::lexical_cast<std::string>(isDirty)
			)
	) return false;

	// Everything worked. Indicate that we want to continue
	return true;
} // boost::shared_ptr<GIndividual> target will cease to exist at this point

/*********************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */
