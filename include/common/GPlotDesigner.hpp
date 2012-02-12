/**
 * @file GPlotDesigner.hpp
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
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// Boost headers go here
#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#ifndef GPLOTDESIGNER_HPP_
#define GPLOTDESIGNER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "common/GExceptions.hpp"

namespace Gem {
namespace Common {

/*********************************************************************************/
/**
 * Determines whether a scatter plot or a curve should be recorded
 */
enum graphPlotMode {
	SCATTER = 0
	, CURVE = 1
};

//Some default values

const boost::uint32_t DEFCXDIM = 1024;
const boost::uint32_t DEFCYDIM =  768;

const boost::uint32_t DEFCXDIV =    1;
const boost::uint32_t DEFCYDIV =    1;

const graphPlotMode        DEFPLOTMODE = CURVE;

// Easier access to the header-, body- and footer-data
typedef boost::tuple<std::string,std::string,std::string> plotData;

// Forward declaration in order to allow a friend statement in GBasePlotter
class GPlotDesigner;

/*********************************************************************************/
/**
 * An abstract base class that defines functions for plots. Concrete plotters
 * derive from this class. They can be added to a master canvas, which takes care
 * to plot them into sub-pads.
 */
class GBasePlotter {
	friend class GPlotDesigner;

public:
	/** @brief The default constructor */
	GBasePlotter();
	/** @brief A copy constructor */
	GBasePlotter(const GBasePlotter&);
	/** @brief The destructor */
	virtual ~GBasePlotter();

	/** @brief The assignment operator */
	void operator=(const GBasePlotter&);

	//----------------------------------------------------------------------------
	// Functions to be specified in derived classes

	/** @brief Retrieve specific header settings for this plot */
	virtual std::string headerData() const = 0;
	/** @brief Retrieves the actual data sets */
	virtual std::string bodyData() const = 0;
	/** @brief Retrieves specific draw commands for this plot */
	virtual std::string footerData() const = 0;

	//----------------------------------------------------------------------------

	/** @brief Allows to set the drawing arguments for this plot */
	virtual void setDrawingArguments(std::string);
	/** @brief Retrieve the current drawing arguments */
	std::string drawingArguments() const;

	/** @brief Sets the label for the x-axis */
	void setXAxisLabel(std::string);
	/** @brief Retrieve the x-axis label */
	std::string xAxisLabel() const;
	/** @brief Sets the label for the y-axis */
	void setYAxisLabel(std::string);
	/** @brief Retrieve the y-axis label */
	std::string yAxisLabel() const;
	/** @brief Sets the label for the z-axis */
	void setZAxisLabel(std::string);
	/** @brief Retrieve the z-axis label */
	std::string zAxisLabel() const;

	/** @brief Allows to assign a label to the entire plot */
	void setPlotLabel(std::string);
	/** @brief Allows to retrieve the plot label */
	std::string plotLabel() const;

	/** @brief Allows to assign a marker to data structures */
	void setDataStructureMarker(std::string);
	/** @brief Allows to retrieve the data structure marker */
	std::string dsMarker() const;

protected:
	std::string drawingArguments_; ///< Holds the drawing arguments for this plot

	std::string x_axis_label_; ///< A label for the x-axis
	std::string y_axis_label_; ///< A label for the y-axis
	std::string z_axis_label_; ///< A label for the z-axis (if available)

	std::string plot_label_;   ///< A label to be assigned to the entire plot
	std::string dsMarker_;     ///< A marker to make the origin of data structures clear in the output file

	/** @brief Allows to retrieve the id of this object */
	std::size_t id() const;

private:
	/** @brief Sets the id of the object */
	void setId(const std::size_t&);

	std::size_t id_; ///< The id of this object
};

/*********************************************************************************/
/**
 * A data collector for 1-d data of user-defined type. This will usually be
 * data of a histogram type.
 */
template <typename x_type>
class GDataCollector1T :public GBasePlotter
{
public:
	/*****************************************************************************/
	/**
	 * The default constructor
	 */
	GDataCollector1T()
		: GBasePlotter()
		, data_()
	{ /* nothing */ }

	/*****************************************************************************/
	/**
	 * A copy constructor
	 *
	 * @param cp A copy of another GDataCollector1T<x_type> object
	 */
	GDataCollector1T(const GDataCollector1T<x_type>& cp)
		: GBasePlotter(cp)
		, data_(cp.data_)
	{ /* nothing */ }

	/*****************************************************************************/
	/**
	 * The destructor
	 */
	virtual ~GDataCollector1T() {
		data_.clear();
	}

	/*****************************************************************************/
	/**
	 * The assignment operator
	 */
	void operator=(const GDataCollector1T<x_type>& cp) {
		// Assign our parent class'es data
		GBasePlotter::operator=(cp);

		// and then our own
		data_ = cp.data_;
	}

	/*****************************************************************************/
	/**
	 * Allows to add data
	 *
	 * @param x_undet The data item to be added to the collection
	 */
	template <typename x_type_undet>
	void operator&(const x_type_undet& x_undet) {
		using boost::numeric::bad_numeric_cast;

		x_type x = x_type(0);

		// Make sure the data can be converted to doubles
		try {
			x=boost::numeric_cast<x_type>(x_undet);
		}
		catch(bad_numeric_cast &e) {
			raiseException(
				"In GDataCollector1T::operator&(): Error!" << std::endl
				<< "Encountered invalid cast with boost::numeric_cast," << std::endl
				<< "with the message " << std::endl
				<< e.what() << std::endl
			);
		}

		// Add the converted data to our collection
		data_.push_back(x);
	}

protected:
	/*****************************************************************************/

	std::vector<x_type> data_; ///< Holds the actual data
};

/*********************************************************************************/
/**
 * A data collector for 2-d data of user-defined type
 */
template <typename x_type, typename y_type>
class GDataCollector2T :public GBasePlotter
{
public:
	/*****************************************************************************/
	/**
	 * The default constructor
	 */
	GDataCollector2T()
		: GBasePlotter()
		, data_()
	{ /* nothing */ }

	/*****************************************************************************/
	/**
	 * A copy constructor
	 *
	 * @param cp A copy of another GDataCollector1T<x_type> object
	 */
	GDataCollector2T(const GDataCollector2T<x_type, y_type>& cp)
		: GBasePlotter(cp)
		, data_(cp.data_)
	{ /* nothing */ }

	/*****************************************************************************/
	/**
	 * The destructor
	 */
	virtual ~GDataCollector2T() {
		data_.clear();
	}

	/*****************************************************************************/
	/**
	 * The assignment operator
	 */
	void operator=(const GDataCollector2T<x_type, y_type>& cp) {
		// Assign our parent class'es data
		GBasePlotter::operator=(cp);

		// and then our own
		data_ = cp.data_;
	}

	/*****************************************************************************/
	/**
	 * Allows to add data to the collection in an intuitive way.
	 *
	 * @param point_undet The data item to be added to the collection
	 */
	template <typename x_type_undet, typename y_type_undet>
	void operator&(const boost::tuple<x_type_undet,y_type_undet>& point_undet) {
		using boost::numeric::bad_numeric_cast;

		x_type x = x_type(0);
		y_type y = y_type(0);

		// Make sure the data can be converted to doubles
		try {
			x=boost::numeric_cast<x_type>(boost::get<0>(point_undet));
			y=boost::numeric_cast<y_type>(boost::get<1>(point_undet));
		}
		catch(bad_numeric_cast &e) {
			raiseException(
				"In GDataCollector2T::operator&(): Error!" << std::endl
				<< "Encountered invalid cast with boost::numeric_cast," << std::endl
				<< "with the message " << std::endl
				<< e.what() << std::endl
			);
		}

		data_.push_back(boost::tuple<x_type,y_type>(x,y));
	}

protected:
	/*****************************************************************************/

	std::vector<boost::tuple<x_type, y_type> > data_; ///< Holds the actual data
};


/*********************************************************************************/
/**
 * A wrapper for a ROOT TGraph object (2d data and curve-like structures)
 */
class GGraph2D : public GDataCollector2T<double,double> {
public:
	/** @brief The default constructor */
	GGraph2D();

	/** @brief A copy constructor */
	GGraph2D(const GGraph2D&);

	/** @brief The destructor */
	~GGraph2D();

	/** @brief The assignment operator */
	GGraph2D &operator=(const GGraph2D&);

	/** @brief Determines whether a scatter plot or a curve is created */
	void setPlotMode(graphPlotMode);
	/** @brief Allows to retrieve the current plotting mode */
	graphPlotMode getPlotMode() const;

	/** @brief Retrieve specific header settings for this plot */
	virtual std::string headerData() const;
	/** @brief Retrieves the actual data sets */
	virtual std::string bodyData() const;
	/** @brief Retrieves specific draw commands for this plot */
	virtual std::string footerData() const;

private:
	graphPlotMode pM_; ///< Whether to create scatter plots or a curve, connected by lines
};

/*********************************************************************************/
/**
 * A wrapper for ROOT's TH1D class (1-d double data)
 */

/*********************************************************************************/
/**
 * A wrapper for ROOT's TH2D class (2-d double data)
 */

/*********************************************************************************/
/**
 * A wrapper for the ROOT TF1 1d-function plotter
 */

/*********************************************************************************/
/**
 * A wrapper for the ROOT TF2 2d-function plotter
 */

/*********************************************************************************/
/**
 * A provider of free-form ROOT plots
 */

/*********************************************************************************/
/**
 * A class that outputs a ROOT input file (compare http://root.cern.ch), based
 * on the data providers stored in it.
 */
class GPlotDesigner
	: boost::noncopyable
{
public:
	/** @brief The standard constructor */
	GPlotDesigner(const std::size_t&, const std::size_t&);

	/* @brief Emits the overall plot */
	std::string plot() const;

	/** @brief Allows to add a new plotter object */
	void registerPlotter(boost::shared_ptr<GBasePlotter>);

	/** @brief Allows to assign a global title to the entire canvas */
	void setCanvasLabel(std::string);

	/** @brief Set the dimensions of the output canvas */
	void setCanvasDimensions(const boost::uint32_t&, const boost::uint32_t&);
	/** @brief Allows to retrieve the canvas dimensions */
	boost::tuple<boost::uint32_t, boost::uint32_t> getCanvasDimensions() const;

private:
	/** @brief The default constructor -- intentionally private and undefined */
	GPlotDesigner();

	/** @brief A header for static data in a ROOT file */
	std::string staticHeader() const;

	std::vector<boost::shared_ptr<GBasePlotter> > plotters_; ///< A list of plots to be added to the diagram

	std::size_t c_x_div_, c_y_div_; ///< The number of divisions in x- and y-direction
	boost::uint32_t c_x_dim_, c_y_dim_; ///< Holds the number of pixels of the canvas

	std::string canvasLabel_; ///< A label to be assigned to the entire canvas
};

/*********************************************************************************/

} /* namespace Common */
} /* namespace Gem */

#endif /* GPLOTDESIGNER_HPP_ */