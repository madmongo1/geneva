/**
 * @file GCanvas.hpp
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

// Standard header files go here
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm> // for std::sort
#include <utility> // For std::pair
#include <tuple>

// Boost header files go here
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/optional.hpp>
#include <boost/shared_array.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>

#ifndef GCANVAS_HPP_
#define GCANVAS_HPP_

// Geneva header files go here
#include "common/GLogger.hpp"
#include "common/GHelperFunctions.hpp"
#include "common/GMathHelperFunctions.hpp"
#include "common/GMathHelperFunctionsT.hpp"

// aliases for ease of use
namespace bf = boost::filesystem;

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * A simple two-dimensional coordinate
 */
struct coord2D {
	/** @brief The default constructor */
	coord2D();

	/** @brief Construction with positions */
	coord2D(const float &, const float &);

	/** @brief Copy construction */
	coord2D(const coord2D &);

	/** @brief An assignment operator */
	const coord2D &operator=(const coord2D &);

	float x;
	float y;
};

/** @brief Convenience function for calculating the difference between two coordinate vectors */
coord2D operator-(const coord2D &, const coord2D &);

/** @brief Convenience function for calculating the dot product of two coordinate vectors */
float operator*(const coord2D &, const coord2D &);

/******************************************************************************/
/**
 * A struct holding the coordinates, colors and opacity of a single triangle, which is
 * defined via a surrounding circle
 */
typedef
struct triangle_circle_struct {
	coord2D middle;
	float radius;
	float angle1;
	float angle2;
	float angle3;
	float r;
	float g;
	float b;
	float a;

	/** @brief Assignment operator */
	const triangle_circle_struct &operator=(const triangle_circle_struct &);

	/** @brief Needed for sorting */
	float getAlphaValue() const;

	/** @brief Translate to a string */
	std::string toString() const;
} t_circle;

/** @brief Simplify debugging output */
G_API_COMMON std::ostream &operator<<(std::ostream &, const t_circle &);
/** @brief Simplify comparison of two t_circle structs */
G_API_COMMON bool operator==(const t_circle &, const t_circle &);
/** @brief Simplify comparison of two t_circle structs */
G_API_COMMON bool operator!=(const t_circle &, const t_circle &);

/******************************************************************************/
/**
 * A struct holding triangle definitions in standard coordinates
 */
typedef
struct t_spec_c {
	coord2D tr_one;
	coord2D tr_two;
	coord2D tr_three;
	float r;
	float g;
	float b;
	float a;
} t_cart;

/******************************************************************************/
/**
 * A simple class holding the rgb values of a pixel
 */
struct GRgb {
private:
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive &ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar
		& BOOST_SERIALIZATION_NVP(r)
		& BOOST_SERIALIZATION_NVP(g)
		& BOOST_SERIALIZATION_NVP(b);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	G_API_COMMON GRgb();
	/** @brief Initialization with colors */
	G_API_COMMON GRgb(float, float, float);
	/** @brief Initialization with colors held in a std::tuple */
	G_API_COMMON GRgb(std::tuple<float, float, float>);
	/** @brief Copy Construction */
	G_API_COMMON GRgb(const GRgb &);

	/** @brief Destructor */
	virtual G_API_COMMON ~GRgb();

	/** @brief Assignment operator */
	G_API_COMMON const GRgb &operator=(const GRgb &);

	/** @brief Explicit reset of colors */
	G_API_COMMON void setColor(float, float, float);
	/** @brief Explicit reset of colors, using a std::tuple */
	G_API_COMMON void setColor(std::tuple<float, float, float>);

	float r; ///< red
	float g; ///< green
	float b; ///< blue
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A column in a canvas
 */
class GColumn {
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive &ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar &BOOST_SERIALIZATION_NVP(columnData_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	G_API_COMMON GColumn();
	/** @brief Initialization with dimensions and colors */
	G_API_COMMON GColumn(
		const std::size_t &, std::tuple<float, float, float>
	);
	/** @brief Copy construction */
	G_API_COMMON GColumn(const GColumn &);

	/** @brief The destructor */
	virtual G_API_COMMON ~GColumn();

	/** @brief Assignment operator */
	G_API_COMMON const GColumn &operator=(const GColumn &);

	/** @brief Information about the size of this object */
	G_API_COMMON std::size_t size() const;

	/** @brief Unchecked access */
	G_API_COMMON GRgb &operator[](const std::size_t &);
	/** @brief Checked access */
	G_API_COMMON GRgb &at(const std::size_t &);
	/** @brief Unchecked access */
	G_API_COMMON const GRgb &operator[](const std::size_t &) const;
	/** @brief Checked access */
	G_API_COMMON const GRgb &at(const std::size_t &) const;

	/** @brief Initializes the object to a specific size */
	void init(
		const std::size_t &, std::tuple<float, float, float>
	);

private:
	std::vector<GRgb> columnData_;  ///< Holds this column's pixels
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A collection of pixels in a two-dimensional array
 */
template<std::size_t COLORDEPTH = 8>
class GCanvas {
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive &ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar
		& BOOST_SERIALIZATION_NVP(canvasData_)
		& BOOST_SERIALIZATION_NVP(xDim_)
		& BOOST_SERIALIZATION_NVP(yDim_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/***************************************************************************/
	/**
	 * The default constructor -- will result in an empty canvas
	 */
	GCanvas()
		: xDim_(0), yDim_(0), canvasData_(), NCOLORS(Gem::Common::PowSmallPosInt<2, COLORDEPTH>::result),
		  MAXCOLOR(NCOLORS - 1) { /* nothing */ }

	/***************************************************************************/
	/**
	 * Initialization with dimensions and background colors. The default
	 * background color is black.
	 */
	GCanvas(
		std::tuple<std::size_t, std::size_t> dim, std::tuple<float, float, float> color
	)
		: xDim_(std::get<0>(dim)), yDim_(std::get<1>(dim)), canvasData_(),
		  NCOLORS(Gem::Common::PowSmallPosInt<2, COLORDEPTH>::result), MAXCOLOR(NCOLORS - 1) {
		this->reset(dim, color);
	}

	/***************************************************************************/
	/**
	 * Initialization from data held in a string -- uses the PPM-P3 format
	 *
	 * @param ppmString A string holding a picture description in PPM-P3 format
	 */
	GCanvas(const std::string &ppmString)
		: xDim_(0), yDim_(0), canvasData_(), NCOLORS(Gem::Common::PowSmallPosInt<2, COLORDEPTH>::result),
		  MAXCOLOR(NCOLORS - 1) {
		this->loadFromPPM(ppmString);
	}

	/***************************************************************************/
	/**
	 * Copy construction
	 */
	GCanvas(const GCanvas<COLORDEPTH> &cp)
		: xDim_(cp.xDim_), yDim_(cp.yDim_), canvasData_(cp.canvasData_),
		  NCOLORS(Gem::Common::PowSmallPosInt<2, COLORDEPTH>::result), MAXCOLOR(NCOLORS - 1) { /* nothing */ }

	/***************************************************************************/
	/**
	 * The destructor
	 */
	virtual ~GCanvas() {
		xDim_ = 0;
		yDim_ = 0;
		canvasData_.clear();
	}

	/***************************************************************************/
	/**
	 * The assignment operator
	 *
	 * @param cp A copy of another GCanvas object
	 */
	const GCanvas<COLORDEPTH> &operator=(const GCanvas<COLORDEPTH> &cp) {
		canvasData_ = cp.canvasData_;
		xDim_ = cp.xDim_;
		yDim_ = cp.yDim_;

		return *this;
	}

	/***************************************************************************/
	/**
	 * Get information about the canvas dimensions
	 */
	std::tuple<std::size_t, std::size_t> dimensions() const {
		return std::tuple<std::size_t, std::size_t>(xDim_, yDim_);
	}

	/***************************************************************************/
	/**
	 * Retrieves the size in x-direction
	 *
	 * @return The value of the xDim_ parameter
	 */
	std::size_t getXDim() const {
		return xDim_;
	}

	/***************************************************************************/
	/**
	 * Retrieve the size in y-direction
	 *
	 * @return The value of the yDim_ parameter
	 */
	std::size_t getYDim() const {
		return yDim_;
	}

	/***************************************************************************/
	/**
	 * Retrieve the total number of pixels
	 *
	 * @return The total number of pixels in the canvas
	 */
	std::size_t getNPixels() const {
		return xDim_ * yDim_;
	}

	/***************************************************************************/
	/**
	 * Retrieve our color depth.
	 *
	 * @return The chosen color depth
	 */
	std::size_t getColorDepth() const {
		return COLORDEPTH;
	}

	/***************************************************************************/
	/**
	 * Retrieve the number of colors
	 *
	 * @return The number of representable colors
	 */
	std::size_t getNColors() const {
		return NCOLORS;
	}

	/***************************************************************************/
	/**
	 * Retrieve the maximum color value
	 *
	 * @return The maximum allowed color value
	 */
	std::size_t getMaxColor() const {
		return MAXCOLOR;
	}

	/***************************************************************************/
	/**
	 * Unchecked access
	 */
	GColumn &operator[](const std::size_t &pos) {
		return canvasData_[pos];
	}

	/***************************************************************************/
	/**
	 * Checked access
	 */
	GColumn &at(const std::size_t &pos) {
		return canvasData_.at(pos);
	}

	/***************************************************************************/
	/**
	 * Unchecked access
	 */
	const GColumn &operator[](const std::size_t &pos) const {
		return canvasData_[pos];
	}

	/***************************************************************************/
	/**
	 * Checked access
	 */
	const GColumn &at(const std::size_t &pos) const {
		return canvasData_.at(pos);
	}

	/***************************************************************************/
	/**
	 * Find out the deviation between this and another canvas
	 */
	virtual float diff(const GCanvas<COLORDEPTH> &cp) const {
		using namespace Gem::Common;

		if (cp.dimensions() != this->dimensions()) {
			glogger
			<< "In GCanvas::diff(): Error!" << std::endl
			<< "Dimensions differ: " << cp.dimensions() << " / " << this->dimensions() << std::endl
			<< GEXCEPTION;
		}

		float result = 0.f;
		for (std::size_t i_x = 0; i_x < xDim_; i_x++) {
			for (std::size_t i_y = 0; i_y < yDim_; i_y++) {
				result += gsqrt(
					gpow((canvasData_[i_x][i_y]).r - (cp[i_x][i_y]).r, 2.f)
					+ gpow((canvasData_[i_x][i_y]).g - (cp[i_x][i_y]).g, 2.f)
					+ gpow((canvasData_[i_x][i_y]).b - (cp[i_x][i_y]).b, 2.f)
				);
			}
		}

		return result;
	}

	/***************************************************************************/
	/**
	 * Converts the canvas to an image in PPM-P3 format
	 */
	std::string toPPM() const {
		std::ostringstream result;

		result
		<< "P3" << std::endl
		<< xDim_ << " " << yDim_ << std::endl
		<< MAXCOLOR << std::endl;

		for (std::size_t i_y = 0; i_y < yDim_; i_y++) {
			for (std::size_t i_x = 0; i_x < xDim_; i_x++) {
				result
				<< (std::size_t) (canvasData_[i_x][i_y].r * float(MAXCOLOR)) << " "
				<< (std::size_t) (canvasData_[i_x][i_y].g * float(MAXCOLOR)) << " "
				<< (std::size_t) (canvasData_[i_x][i_y].b * float(MAXCOLOR)) << " ";
			}
			result << std::endl;
		}

		return result.str();
	}

	/***************************************************************************/
	/**
	 * Loads the data held in a string in PPM-P3 format
	 *
	 * @param ppmString A string holding an image in PPM-P3 format
	 */
	void loadFromPPM(const std::string &ppmString) {
		using namespace std;

		// Some status flags
		bool header_found = false;
		bool dimensions_found = false;
		bool color_depth_found = false;

		// Allows to read the string line by line
		std::istringstream input(ppmString);

		// Read the setup information. Skip empty lines and comments along the way.
		std::vector<std::size_t> v;
		std::string s; // Will hold newly read lines
		while (std::getline(input, s)) {
			v.clear(); // Clear the vector
			std::istringstream iss(s); // Allows to retrieve sub-items from the line

			// Remove parts beginning with a # (i.e. comments)
			std::size_t pos = 0;
			if ((pos = s.find("#")) != string::npos) {
				s.erase(pos);
			}

			// Remove leading or trailing white spaces
			boost::trim(s);

			// Skip empty lines
			if (s.empty()) {
				s.clear();
				continue;
			}

			// The file should start with a header, which should read "P3". Complain if this isn't the case
			if (!header_found) {
				if (s != "P3") {
					glogger
					<< "Error: Header should be \"P3\", but got " << s << std::endl
					<< GEXCEPTION;
				}

				header_found = true;

				// Skip to the next line
				s.clear();
				continue;
			}

			// The next meaningful line of the input file should contain the picture dimensions
			if (!dimensions_found) {
				std::copy(istream_iterator<std::size_t>(iss), istream_iterator<std::size_t>(), back_inserter(v));

				if (v.size() != 2) { // We should have received exactly two numbers
					glogger
					<< "Error: Got invalid number of dimensions: " << v.size() << std::endl
					<< GEXCEPTION;
				}

				if (v[0] <= 0 || v[1] <= 0) {
					glogger
					<< "Error: Got invalid dimensions: " << v[0] << " / " << v[1] << std::endl
					<< GEXCEPTION;
				}

				xDim_ = v[0];
				yDim_ = v[1];

				// Re-initialize the canvas with black
				this->reset(
					std::tuple<std::size_t, std::size_t>(xDim_, yDim_), 0.f, 0.f, 0.f
				);

				dimensions_found = true;

				// Skip to the next line
				s.clear();
				continue;
			}

			// Next should be the color depth
			if (!color_depth_found) {
				copy(istream_iterator<std::size_t>(iss), istream_iterator<std::size_t>(), back_inserter(v));

				if (v.size() != 1) { // We should have received exactly one number
					glogger
					<< "Error: Did not find specification of the number of colors" << std::endl
					<< "or an invalid number of specifications: " << v.size() << std::endl
					<< GEXCEPTION;
				}

				// We only accept a single color depth for now. Except for this check, we
				// do nothing in this block
				if (v[0] != MAXCOLOR) {
					glogger
					<< "Error: Got invalid color depth " << v[0] << std::endl
					<< GEXCEPTION;
				}

				color_depth_found = true;
				s.clear();
			}

			// We are ready to read the real data and terminate the loop
			break;
		}

		// Read the per-pixel information
		v.clear();
		s.clear();
		while (std::getline(input, s)) {
			// Remove parts beginning with a # (i.e. comments)
			std::size_t pos = 0;
			if ((pos = s.find("#")) != string::npos) {
				s.erase(pos);
			}

			// Remove leading or trailing white spaces
			boost::trim(s);

			// Skip empty lines
			if (s.empty()) {
				s.clear();
				continue;
			}

			istringstream iss(s);

			// We are now getting to the color content. These are rgb integer triples
			copy(istream_iterator<std::size_t>(iss), istream_iterator<std::size_t>(), back_inserter(v));

			// Return the string to pristine condition
			s.clear();
		}

		// v should now contain all per-pixel information. Check the size - as
		// we are reading triplets, the size of the vector is known.
		if (v.size() != 3 * xDim_ * yDim_) {
			glogger
			<< "Error: got invalid number of entries in line." << std::endl
			<< "Expected " << 3 * xDim_ * yDim_ << ", but got " << v.size() << std::endl
			<< "Note: xDim_ = " << xDim_ << ", yDim_ = " << yDim_ << std::endl
			<< GEXCEPTION;
		}

		// Add all pixel data to the canvas
		std::size_t offset = 0;
		for (std::size_t line_counter = 0; line_counter < yDim_; line_counter++) {
			for (std::size_t pixel_counter = 0; pixel_counter < xDim_; pixel_counter++) {
				offset = 3 * (line_counter * xDim_ + pixel_counter);

				(canvasData_[pixel_counter][line_counter]).r = float(v[offset + std::size_t(0)]) / float(MAXCOLOR);
				(canvasData_[pixel_counter][line_counter]).g = float(v[offset + std::size_t(1)]) / float(MAXCOLOR);
				(canvasData_[pixel_counter][line_counter]).b = float(v[offset + std::size_t(2)]) / float(MAXCOLOR);
			}
		}
	}

	/***************************************************************************/
	/**
	 * Loads the data held in a file in PPM-P3 format
	 *
	 * @param p The name of a file holding an image in PPM-P3 format
	 */
	void loadFromFile(const bf::path &p) {
		// Read in the entire file
		std::string imageData = Gem::Common::loadTextDataFromFile(p);

		// Hand the string over to loadFromPPM() -- it will do the rest
#ifdef DEBUG
      if(imageData.empty()) {
         glogger
         << "GCanvas::loadFromFile(): Error!" << std::endl
         << "File data was empty" << std::endl
         << GEXCEPTION;
      }
#endif

		this->loadFromPPM(imageData);
	}

	/***************************************************************************/
	/**
	 * Saves the canvas to a file
	 */
	void toFile(const bf::path &p) {
		bf::ofstream result(p);

		if (!result) {
			glogger
			<< "In GCanvas<>::toFile(): Error!" << std::endl
			<< "Could not open output file " << p.string() << std::endl
			<< GEXCEPTION;
		}

		result << this->toPPM();
		result.close();
	}

	/***************************************************************************/
	/**
	 * Removes all data from the canvas
	 */
	void clear() {
		canvasData_.clear();

		xDim_ = std::size_t(0);
		yDim_ = std::size_t(0);
	}

	/***************************************************************************/
	/**
	 * Resets the canvas to a given color and dimension
	 *
	 * @param
	 */
	void reset(
		std::tuple<std::size_t, std::size_t> dimension, const float &red, const float &green, const float &blue
	) {
		this->clear();

		xDim_ = std::get<0>(dimension);
		yDim_ = std::get<1>(dimension);

		for (std::size_t i = 0; i < xDim_; i++) {
			canvasData_.push_back(GColumn(yDim_, std::tuple<float, float, float>(red, green, blue)));
		}
	}

	/***************************************************************************/
	/**
	 * Resets the canvas to a given color and dimension
	 */
	void reset(
		std::tuple<std::size_t, std::size_t> dimension, std::tuple<float, float, float> color
	) {
		this->reset(
			dimension, std::get<0>(color), std::get<1>(color), std::get<2>(color)
		);
	}

	/***************************************************************************/
	/**
	 * Adds a triangle to the canvas, using Gemfony's "circular" definition
	 */
	void addTriangle(const t_circle &t) {
		t_cart t_c;

#ifdef DEBUG
      // Check that angles are in consecutive order
      if(t.angle1 < 0.f || t.angle2 <= t.angle1 || t.angle3 <= t.angle2 || t.angle3 >= 1.f) {
         glogger
         << "In GCanvas<>::addTriangel(): Error!" << std::endl
         << "Angles are not in consecutive oder: " << std::endl
         << t.angle1 << " / " << t.angle2 << " / " << t.angle3 << std::endl
         << GEXCEPTION;
      }
#endif /* DEBUG */

		// and store them in the structs holding the cartesic coordinates
		t_c.tr_one.x = t.middle.x + t.radius * gcos(t.angle1 * 2.0f * boost::math::constants::pi<float>());
		t_c.tr_one.y = t.middle.y + t.radius * gsin(t.angle1 * 2.0f * boost::math::constants::pi<float>());

		t_c.tr_two.x = t.middle.x + t.radius * gcos(t.angle2 * 2.0f * boost::math::constants::pi<float>());
		t_c.tr_two.y = t.middle.y + t.radius * gsin(t.angle2 * 2.0f * boost::math::constants::pi<float>());

		t_c.tr_three.x = t.middle.x + t.radius * gcos(t.angle3 * 2.0f * boost::math::constants::pi<float>());
		t_c.tr_three.y = t.middle.y + t.radius * gsin(t.angle3 * 2.0f * boost::math::constants::pi<float>());

		t_c.r = t.r;
		t_c.g = t.g;
		t_c.b = t.b;
		t_c.a = t.a;

		this->addTriangle(t_c);
	}

	/***************************************************************************/
	/**
	 * Adds a complete set of triangles to the canvas, using Gemfony's
	 * "circular" definition
	 */
	void addTriangles(std::vector<t_circle> ts, std::size_t nTriangles) {
		for (std::size_t i = 0; i < nTriangles; i++) {
			this->addTriangle(ts[i]);
		}
	}

	/***************************************************************************/
	/**
	 * Adds a triangle to the canvas, using a struct holding cartesic coordinates
	 */
	void addTriangle(const t_cart &t) {
		using namespace Gem::Common;

		float xDim_inv = 1.f / float(xDim_);
		float yDim_inv = 1.f / float(yDim_);
		float dot11, dot12, dot22, dot1p, dot2p, denom_inv, u, v;
		coord2D diff31, diff21, diffp1, pos_f;

		for (std::size_t i_x = 0; i_x < xDim_; i_x++) {
			// Calculate the pixel x-position
			pos_f.x = float(i_x + 1) * xDim_inv;

			if (
				pos_f.x < t.tr_one.x
				&& pos_f.x < t.tr_two.x
				&& pos_f.x < t.tr_three.x
				) {
				continue;
			}

			if (
				pos_f.x > t.tr_one.x
				&& pos_f.x > t.tr_two.x
				&& pos_f.x > t.tr_three.x
				) {
				continue;
			}

			for (std::size_t i_y = 0; i_y < yDim_; i_y++) {
				// Calculate the pixel y-position
				pos_f.y = float(i_y + 1) * yDim_inv;

				if (
					pos_f.y < t.tr_one.y
					&& pos_f.y < t.tr_two.y
					&& pos_f.y < t.tr_three.y
					) {
					continue;
				}

				if (
					pos_f.y > t.tr_one.y
					&& pos_f.y > t.tr_two.y
					&& pos_f.y > t.tr_three.y
					) {
					continue;
				}

				diff31 = t.tr_three - t.tr_one;
				diff21 = t.tr_two - t.tr_one;
				diffp1 = pos_f - t.tr_one;

				dot11 = diff31 * diff31;
				dot12 = diff31 * diff21;
				dot22 = diff21 * diff21;
				dot1p = diff31 * diffp1;
				dot2p = diff21 * diffp1;

				denom_inv = 1.f / gmax(dot11 * dot22 - dot12 * dot12, 0.0000001f);

				u = (dot22 * dot1p - dot12 * dot2p) * denom_inv;
				v = (dot11 * dot2p - dot12 * dot1p) * denom_inv;

				if ((u >= 0.f) && (v >= 0.f) && (u + v < 1.f)) {
					canvasData_[i_x][i_y].r = gmix(canvasData_[i_x][i_y].r, t.r, t.a);
					canvasData_[i_x][i_y].g = gmix(canvasData_[i_x][i_y].g, t.g, t.a);
					canvasData_[i_x][i_y].b = gmix(canvasData_[i_x][i_y].b, t.b, t.a);
				}
			}
		}
	}

	/***************************************************************************/
	/**
	 * Calculates the average colors over all pixels
	 */
	std::tuple<float, float, float> getAverageColors() const {
		float averageRed = 0.f;
		float averageGreen = 0.f;
		float averageBlue = 0.f;

		for (std::size_t i_x = 0; i_x < xDim_; i_x++) {
			for (std::size_t i_y = 0; i_y < yDim_; i_y++) {
				averageRed += canvasData_[i_x][i_y].r;
				averageGreen += canvasData_[i_x][i_y].g;
				averageBlue += canvasData_[i_x][i_y].b;
			}
		}

		averageRed /= (float) (xDim_ * yDim_);
		averageGreen /= (float) (xDim_ * yDim_);
		averageBlue /= (float) (xDim_ * yDim_);

		return std::tuple<float, float, float>(averageRed, averageGreen, averageBlue);
	}

	/***************************************************************************/
	// Converts the three angles

	/***************************************************************************/

protected:
	std::size_t xDim_, yDim_; ///< The dimensions of this canvas
	std::vector<GColumn> canvasData_; ///< Holds this canvas' columns

	const std::size_t NCOLORS;
	const std::size_t MAXCOLOR;
};

/******************************************************************************/
/**
  * Convenience function for the calculation of the difference between two canvasses
  */
template<std::size_t COLORDEPTH>
G_API_COMMON
float operator-(const GCanvas<COLORDEPTH> &x, const GCanvas<COLORDEPTH> &y) {
	return x.diff(y);
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Specialization of GCanvas for a color depth of 8 bits
 */
class GCanvas8 : public GCanvas<8> {
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive &ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(GCanvas<8>);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	G_API_COMMON GCanvas8();
	/** @brief Initialization with dimensions and colors */
	G_API_COMMON GCanvas8(
		std::tuple<std::size_t, std::size_t>, std::tuple<float, float, float>
	);
	/** @brief Initialization from data held in a string -- uses the PPM-P3 format */
	G_API_COMMON GCanvas8(const std::string &);
	/** @brief Copy construction */
	G_API_COMMON GCanvas8(const GCanvas8 &);

	/** @brief The destructor */
	virtual G_API_COMMON ~GCanvas8();

	/** @brief The assignment operator */
	G_API_COMMON const GCanvas8 &operator=(const GCanvas8 &);
};

/** @brief Convenience function for the calculation of the difference between two canvasses */
G_API_COMMON float operator-(const GCanvas8 &, const GCanvas8 &);

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Specialization of GCanvas for a color depth of 16 bits
 */
class GCanvas16 : public GCanvas<16> {
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive &ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar
			&BOOST_SERIALIZATION_BASE_OBJECT_NVP(GCanvas<16>);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	G_API_COMMON GCanvas16();
	/** @brief Initialization with dimensions and colors */
	G_API_COMMON GCanvas16(
		std::tuple<std::size_t, std::size_t>, std::tuple<float, float, float>
	);
	/** @brief Initialization from data held in a string -- uses the PPM-P3 format */
	G_API_COMMON GCanvas16(const std::string &);
	/** @brief Copy construction */
	G_API_COMMON GCanvas16(const GCanvas16 &);

	/** @brief The destructor */
	virtual G_API_COMMON ~GCanvas16();

	/** @brief The assignment operator */
	G_API_COMMON const GCanvas16 &operator=(const GCanvas16 &);
};

/** @brief Convenience function for the calculation of the difference between two canvasses */
G_API_COMMON float operator-(const GCanvas16 &, const GCanvas16 &);

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Specialization of GCanvas for a color depth of 24 bits
 */
class GCanvas24 : public GCanvas<24> {
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive &ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(GCanvas<24>);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	G_API_COMMON GCanvas24();
	/** @brief Initialization with dimensions and colors */
	G_API_COMMON GCanvas24(
		std::tuple<std::size_t, std::size_t>, std::tuple<float, float, float>
	);
	/** @brief Initialization from data held in a string -- uses the PPM-P3 format */
	G_API_COMMON GCanvas24(const std::string &);
	/** @brief Copy construction */
	G_API_COMMON GCanvas24(const GCanvas24 &);

	/** @brief The destructor */
	virtual G_API_COMMON ~GCanvas24();

	/** @brief The assignment operator */
	G_API_COMMON const GCanvas24 &operator=(const GCanvas24 &);
};

/** @brief Convenience function for the calculation of the difference between two canvasses */
G_API_COMMON float operator-(const GCanvas24 &, const GCanvas24 &);

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Common::GRgb)
BOOST_CLASS_EXPORT_KEY(Gem::Common::GColumn)
BOOST_CLASS_EXPORT_KEY(Gem::Common::GCanvas8)
BOOST_CLASS_EXPORT_KEY(Gem::Common::GCanvas16)
BOOST_CLASS_EXPORT_KEY(Gem::Common::GCanvas24)

#endif /* GCANVAS_HPP_ */
