/**
 * @file GStdVectorInterface_test.hpp
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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

// Boost header files go here

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

#ifndef GSTDVECTORINTERFACE_TEST_HPP_
#define GSTDVECTORINTERFACE_TEST_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "common/GHelperFunctionsT.hpp"
#include "geneva/GStdSimpleVectorInterfaceT.hpp"
#include "geneva/GStdPtrVectorInterfaceT.hpp"
#include "geneva/GObject.hpp"
#include "GEqualityPrinter.hpp"

/*************************************************************************************************/
/**
 * Tests whether an object of type "vi" has the std::vector<items> interface. Items are
 * assumed to be basic types.
 */
template <typename vi, typename item>
void stdvectorinterfacetest(vi& vectorObject, const item& templItem, const item& findItem) {
	// Prepare printing of error messages in object comparisons
	GEqualityPrinter gep("stdvectorinterfacetest()",
						 pow(10,-10),
						 Gem::Common::CE_WITH_MESSAGES);

	const std::size_t NITEMS = 100;

	// Make sure the object is empty
	BOOST_CHECK_NO_THROW(vectorObject.clear());

	// Attach items
	for(std::size_t i=0; i<NITEMS; i++) {
		BOOST_CHECK_NO_THROW(vectorObject.push_back(templItem));
	}

	BOOST_CHECK(vectorObject.size() == NITEMS);

	BOOST_CHECK(!vectorObject.empty());
	BOOST_CHECK(vectorObject.max_size() != 0);
	BOOST_CHECK(vectorObject.capacity() != 0);
	BOOST_CHECK_NO_THROW(vectorObject.reserve(NITEMS));

	// Attach again items fillin the reserved space
	for(std::size_t i=0; i<NITEMS; i++) {
		BOOST_CHECK_NO_THROW(vectorObject.push_back(templItem));
	}
	BOOST_CHECK(vectorObject.size() == 2*NITEMS);

	// Count objects in the collection
	BOOST_CHECK_NO_THROW(vectorObject.at(2) = findItem);
	BOOST_CHECK(vectorObject.count(findItem)==1);

	// Find items in the collection
	typename vi::const_iterator begin_cit = vectorObject.begin(), find_cit;
	find_cit = vectorObject.find(findItem);
	BOOST_CHECK(find_cit - begin_cit == 2);

	// Create two copies of vectorObject and retrieve its data.
	vi vectorObject_cp1, vectorObject_cp2;
	BOOST_CHECK_NO_THROW(vectorObject_cp2 = vectorObject_cp1 = vectorObject);

	// Check that all objects are equal
	BOOST_CHECK(gep.isEqual(vectorObject_cp1, vectorObject));
	BOOST_CHECK(gep.isEqual(vectorObject_cp2, vectorObject));

	// Assign a different value to the first position of the first copy to create different data sets
	vectorObject_cp1[0] = findItem;

	// Check that now cp1 is different from the others
	BOOST_CHECK(gep.isInEqual(vectorObject_cp1, vectorObject));
	BOOST_CHECK(gep.isInEqual(vectorObject_cp1, vectorObject_cp2));

	// Swap the data of first and second copy
	BOOST_CHECK_NO_THROW(vectorObject_cp2.swap(vectorObject_cp1));

	// Now it should be vectorObject_cp2 that is different
	BOOST_CHECK(gep.isInEqual(vectorObject_cp2, vectorObject));
	BOOST_CHECK(gep.isInEqual(vectorObject_cp2, vectorObject_cp1));


	// Same procedure, this time with a std::vector as target
	std::vector<item> vec_cp1, vec_cp2;
	typename vi::const_iterator obj_cit;
	for(obj_cit=vectorObject.begin(); obj_cit != vectorObject.end(); ++obj_cit) {
		vec_cp1.push_back(*obj_cit);
		vec_cp2.push_back(*obj_cit);
	}

	// Holds possible error messages
	boost::optional<std::string> o;

	// o = vectorObject.GStdSimpleVectorInterfaceT<item>::checkRelationshipWith(vec_cp1, Gem::Common::CE_EQUALITY, 0., "stdvectorinterfacetest", "vec_cp1", Gem::Common::CE_WITH_MESSAGES);
	// BOOST_CHECK_MESSAGE(!o, std::string(o?"\n\n"+*o+"\n":""));
	// o = vectorObject.GStdSimpleVectorInterfaceT<item>::checkRelationshipWith(vec_cp2, Gem::Common::CE_EQUALITY, 0., "stdvectorinterfacetest", "vec_cp2", Gem::Common::CE_WITH_MESSAGES);
	// BOOST_CHECK_MESSAGE(!o, std::string(o?"\n\n"+*o+"\n":""));

	BOOST_CHECK(gep.isEqual(vectorObject, vec_cp1));
	BOOST_CHECK(gep.isEqual(vectorObject, vec_cp2));

	// Assign a different value to one position
	vectorObject.at(vectorObject.size()-1) = findItem;
	// o = vectorObject.GStdSimpleVectorInterfaceT<item>::checkRelationshipWith(vec_cp1, Gem::Common::CE_INEQUALITY, 0., "stdvectorinterfacetest", "vec_cp1", Gem::Common::CE_WITH_MESSAGES);
	// BOOST_CHECK_MESSAGE(!o, std::string(o?"\n\n"+*o+"\n":""));
	// o = vectorObject.GStdSimpleVectorInterfaceT<item>::checkRelationshipWith(vec_cp2, Gem::Common::CE_INEQUALITY, 0., "stdvectorinterfacetest", "vec_cp2", Gem::Common::CE_WITH_MESSAGES);
	// BOOST_CHECK_MESSAGE(!o, std::string(o?"\n\n"+*o+"\n":""));

	BOOST_CHECK(gep.isInEqual(vectorObject, vec_cp1));
	BOOST_CHECK(gep.isInEqual(vectorObject, vec_cp2));

	// Swap the data with a cp1
	vectorObject.GStdSimpleVectorInterfaceT<item>::swap(vec_cp1);
	// o = vectorObject.GStdSimpleVectorInterfaceT<item>::checkRelationshipWith(vec_cp1, Gem::Common::CE_INEQUALITY, 0., "stdvectorinterfacetest", "vec_cp1", Gem::Common::CE_WITH_MESSAGES);
	// BOOST_CHECK_MESSAGE(!o, std::string(o?"\n\n"+*o+"\n":""));
	// o = vectorObject.GStdSimpleVectorInterfaceT<item>::checkRelationshipWith(vec_cp2, Gem::Common::CE_EQUALITY, 0., "stdvectorinterfacetest", "vec_cp2", Gem::Common::CE_WITH_MESSAGES);
	// BOOST_CHECK_MESSAGE(!o, std::string(o?"\n\n"+*o+"\n":""));

	BOOST_CHECK(gep.isInEqual(vectorObject, vec_cp1));
	BOOST_CHECK(gep.isEqual(vectorObject, vec_cp2));

	// Swap back again
	vectorObject.GStdSimpleVectorInterfaceT<item>::swap(vec_cp1);
	// o = vectorObject.GStdSimpleVectorInterfaceT<item>::checkRelationshipWith(vec_cp1, Gem::Common::CE_INEQUALITY, 0., "stdvectorinterfacetest", "vec_cp1", Gem::Common::CE_WITH_MESSAGES);
	// BOOST_CHECK_MESSAGE(!o, std::string(o?"\n\n"+*o+"\n":""));
	// o = vectorObject.GStdSimpleVectorInterfaceT<item>::checkRelationshipWith(vec_cp2, Gem::Common::CE_INEQUALITY, 0., "stdvectorinterfacetest", "vec_cp2", Gem::Common::CE_WITH_MESSAGES);
	// BOOST_CHECK_MESSAGE(!o, std::string(o?"\n\n"+*o+"\n":""));

	BOOST_CHECK(gep.isInEqual(vectorObject, vec_cp1));
	BOOST_CHECK(gep.isInEqual(vectorObject, vec_cp2));

	/*
	BOOST_CHECK(vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::checkIsEqualTo(vec_cp1, false)); // No failure expected
	BOOST_CHECK(vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::checkIsEqualTo(vec_cp2, false));

	// Assign a different value to one position
	vectorObject.at(vectorObject.size()-1) = findItem;
	BOOST_CHECK(!vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::checkIsEqualTo(vec_cp1, true)); // Failures expected
	BOOST_CHECK(!vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::checkIsEqualTo(vec_cp2, true));

	// Swap the data with a cp1
	vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::swap(vec_cp1);

	// Now vectorObject should be in the old state again
	BOOST_CHECK(!vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::checkIsEqualTo(vec_cp1, true)); // Failures expected
	BOOST_CHECK(vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::checkIsEqualTo(vec_cp2, false)); // No failures expected

	// Swap back again
	vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::swap(vec_cp1);
	BOOST_CHECK(!vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::checkIsEqualTo(vec_cp1, true)); // Failures expected
	BOOST_CHECK(!vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::checkIsEqualTo(vec_cp2, true));
	 */

	// Check that the front and back elements can be accessed
	BOOST_CHECK(vectorObject[0] ==  vectorObject.front());
	BOOST_CHECK(vectorObject.back() == findItem);

	// Count the number of findItems again, this time from the end to the beginning.
	std::size_t nFindItems = std::count_if(vectorObject.rbegin(), vectorObject.rend(), boost::bind(std::equal_to<item>(), findItem,  _1));
	BOOST_CHECK(nFindItems == 2);

	// Insert another copy
	BOOST_CHECK_NO_THROW(vectorObject.insert(vectorObject.begin()+3, findItem));
	// and count again
	nFindItems = std::count_if(vectorObject.rbegin(), vectorObject.rend(), boost::bind(std::equal_to<item>(), findItem, _1));
	BOOST_CHECK(nFindItems == 3);

	BOOST_CHECK_NO_THROW(vectorObject.insert(vectorObject.begin()+4, findItem));
	// and count again
	nFindItems = std::count_if(vectorObject.rbegin(), vectorObject.rend(), boost::bind(std::equal_to<item>(), findItem, _1));
	BOOST_CHECK(nFindItems == 4);

	// Inser a number of items
	const std::size_t NINSERT=5;
	BOOST_CHECK_NO_THROW(vectorObject.insert(vectorObject.begin()+3, NINSERT, findItem));
	// and count again
	nFindItems = std::count_if(vectorObject.rbegin(), vectorObject.rend(), boost::bind(std::equal_to<item>(), findItem, _1));
	BOOST_CHECK(nFindItems == 9);

	// Insert a number of items with a different method
	BOOST_CHECK_NO_THROW(vectorObject.insert(vectorObject.begin()+4, NINSERT, findItem));
	// and count again
	nFindItems = std::count_if(vectorObject.rbegin(), vectorObject.rend(), boost::bind(std::equal_to<item>(), findItem, _1));
	BOOST_CHECK(nFindItems == 14);

	std::size_t currentSize = vectorObject.size();
	vectorObject.erase(vectorObject.begin() + 7);
	vectorObject.erase(vectorObject.begin() + 7, vectorObject.begin() + 10);
	BOOST_CHECK(vectorObject.size() == currentSize -4);

	currentSize = vectorObject.size();
	std::size_t NPOPBACK=10;
	for(std::size_t i=0; i<NPOPBACK; i++) vectorObject.pop_back();
	BOOST_CHECK(vectorObject.size() == currentSize -10);

	// Resize
	// vectorObject.resize(NITEMS);
	// BOOST_CHECK(vectorObject.size() == NITEMS);

	// Clear, then resize again to NITEMS, filling up with templItem
	vectorObject.clear();
	BOOST_CHECK(vectorObject.size() == 0);
	vectorObject.resize(NITEMS, templItem);
	BOOST_CHECK(vectorObject.size() == NITEMS);

	// Add another NITEMS copies of findItem, then count their number
	vectorObject.resize(2*NITEMS, findItem);
	nFindItems = std::count_if(vectorObject.begin(), vectorObject.end(), boost::bind(std::equal_to<item>(), findItem, _1));
	BOOST_CHECK(nFindItems == NITEMS);

	// Add another NITEMS copies of findItem, using a different method,, then count their number
	vectorObject.resize(3*NITEMS, findItem);
	nFindItems = std::count_if(vectorObject.begin(), vectorObject.end(), boost::bind(std::equal_to<item>(), findItem, _1));
	BOOST_CHECK(nFindItems == 2*NITEMS);

	std::vector<item> vec_obj;
	for(std::size_t i=0; i<NITEMS; i++) {
		vec_obj.push_back(findItem);
	}
	// Assign the vector
	BOOST_CHECK_NO_THROW(vectorObject.Gem::Geneva::GStdSimpleVectorInterfaceT<item>::operator=(vec_obj));
	nFindItems = std::count_if(vectorObject.begin(), vectorObject.end(), boost::bind(std::equal_to<item>(), findItem, _1));
	BOOST_CHECK(nFindItems == NITEMS);
}

/*************************************************************************************************/
/**
 * Tests whether an object of type "vi" has the std::vector<boost:shared_ptr<items> > interface. Items are
 * assumed to have the Geneva interface. vectorObject is assumed to store boost::shared_ptr
 * wrappers around pointers to the items. Hence the SP in the name, It stands for smart
 * pointers.
 */
template <typename vi, typename item>
void stdvectorinterfacetestSP(vi& vectorObject,
								  boost::shared_ptr<item> templItem,
								  boost::shared_ptr<item> findItem) {
	// Prepare printing of error messages in object comparisons
	GEqualityPrinter gep("stdvectorinterfacetestSP()",
						 pow(10,-10),
						 Gem::Common::CE_WITH_MESSAGES);

	const std::size_t NITEMS = 100;

	// Make sure the object is empty
	BOOST_CHECK_NO_THROW(vectorObject.clear());

	// Attach items
	for(std::size_t i=0; i<NITEMS; i++) {
		BOOST_CHECK_NO_THROW(vectorObject.push_back(templItem->Gem::Geneva::GObject::clone<item>()));
	}

	BOOST_CHECK(vectorObject.size() == NITEMS);

	BOOST_CHECK(!vectorObject.empty());
	BOOST_CHECK(vectorObject.max_size() != 0);
	BOOST_CHECK(vectorObject.capacity() != 0);
	BOOST_CHECK_NO_THROW(vectorObject.reserve(NITEMS));

	// Attach again items using another method
	for(std::size_t i=0; i<NITEMS; i++) {
		BOOST_CHECK_NO_THROW(vectorObject.push_back_clone(templItem));
	}
	BOOST_CHECK(vectorObject.size() == 2*NITEMS);

	// Count objects in the collection
	BOOST_CHECK_NO_THROW((*vectorObject.at(2)) = *findItem);
	BOOST_CHECK(vectorObject.count(findItem)==1);

	// Find items in the collection
	typename vi::const_iterator begin_cit = vectorObject.begin(), find_cit;
	find_cit = vectorObject.find(findItem);
	BOOST_CHECK(find_cit - begin_cit == 2);

	/***********************************************************************************/
	// Create two copies of vectorObject and retrieve its data.
	vi vectorObject_cp1, vectorObject_cp2;
	BOOST_CHECK_NO_THROW(vectorObject_cp2 = vectorObject_cp1 = vectorObject);

	// Check that all objects are equal
	BOOST_CHECK(vectorObject_cp1== vectorObject);
	BOOST_CHECK(vectorObject_cp2== vectorObject);

	// Assign a different value to the first position of the first copy to create different data sets
	(*vectorObject_cp1[0]) = *findItem;

	// Check that now cp1 is different from the others
	BOOST_CHECK(gep.isInEqual(vectorObject_cp1, vectorObject));
	BOOST_CHECK(gep.isInEqual(vectorObject_cp1, vectorObject_cp2));

	// Swap the data of first and second copy
	BOOST_CHECK_NO_THROW(vectorObject_cp2.swap(vectorObject_cp1));

	// Now it should be vectorObject_cp2 that is different
	BOOST_CHECK(vectorObject_cp2 != vectorObject);
	BOOST_CHECK(vectorObject_cp2 != vectorObject_cp1);

	// Check that the front and back elements can be accessed
	BOOST_CHECK_NO_THROW(boost::shared_ptr<item> frontItem = boost::dynamic_pointer_cast<item>(vectorObject.front()));
	BOOST_CHECK_NO_THROW(*boost::dynamic_pointer_cast<item>(vectorObject.back()) = *findItem);
	BOOST_CHECK(*boost::dynamic_pointer_cast<item>(vectorObject.back()) == *findItem);

	// Count the number of findItems again, this time from the end to the beginning.
	std::size_t nFindItems = vectorObject.count(findItem);
	BOOST_CHECK(nFindItems == 2);

	// Insert another copy
	BOOST_CHECK_NO_THROW(vectorObject.insert_clone(vectorObject.begin()+3, findItem));
	// and count again
	nFindItems = vectorObject.count(findItem);
	BOOST_CHECK(nFindItems == 3);

	BOOST_CHECK_NO_THROW(vectorObject.insert_clone(vectorObject.begin()+4, findItem));
	// and count again
	nFindItems = vectorObject.count(findItem);
	BOOST_CHECK(nFindItems == 4);

	// Insert a number of items (will clone)
	const std::size_t NINSERT=5;
	BOOST_CHECK_NO_THROW(vectorObject.insert(vectorObject.begin()+3, NINSERT, findItem));
	// and count again
	nFindItems = vectorObject.count(findItem);
	BOOST_CHECK(nFindItems == 9);

	// Insert a number of items with a different method
	BOOST_CHECK_NO_THROW(vectorObject.insert_clone(vectorObject.begin()+4, NINSERT, findItem));
	// and count again
	nFindItems = vectorObject.count(findItem);
	BOOST_CHECK(nFindItems == 14);

	std::size_t currentSize = vectorObject.size();
	vectorObject.erase(vectorObject.begin() + 7);
	vectorObject.erase(vectorObject.begin() + 7, vectorObject.begin() + 10);
	BOOST_CHECK(vectorObject.size() == currentSize - 4);

	currentSize = vectorObject.size();
	std::size_t NPOPBACK=10;
	for(std::size_t i=0; i<NPOPBACK; i++) vectorObject.pop_back();
	BOOST_CHECK(vectorObject.size() == currentSize - 10);

	// Clear, then resize to NITEMS, filling up with templItem
	vectorObject.clear();
	BOOST_CHECK(vectorObject.size() == 0);
	vectorObject.resize(NITEMS, templItem);
	BOOST_CHECK(vectorObject.size() == NITEMS);

	// Add another NITEMS copies of findItem, then count their number
	vectorObject.resize(2*NITEMS, findItem);
	nFindItems = vectorObject.count(findItem);
	BOOST_CHECK(nFindItems == NITEMS);

	// Add another NITEMS copies of findItem, using a different method,, then count their number
	vectorObject.resize_clone(3*NITEMS, findItem);
	nFindItems = vectorObject.count(findItem);
	BOOST_CHECK(nFindItems == 2*NITEMS);
}

/*************************************************************************************************/

#endif /* GSTDVECTORINTERFACE_TEST_HPP_ */