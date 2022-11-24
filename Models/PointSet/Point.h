/**
 * @file Point.h
 * Definition of class Point
 */
#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#include <array>
#include <string>

/**
 * Point of data from the dataset.
 *
 * A single point of data from the dataset, including the features data from
 * the point, the decision value and the dimension, which should be equal in
 * each point of the dataset.
 *
 * @note This is meant for points that are constants after being constructed.
 */
class Point {
	private:
		/// Dimension of the features array.
		size_t dimension;

		/**
		 * Features data of the point.
		 *
		 * @note This is owned by the object.
		 */
		float* features;

		/// Decision value of the point
		bool value;
	
	public:
		/**
		 * Main constructor of Point.
		 *
		 * Construct a point by providing each of its attributes.
		 *
		 * @param dimension Dimension of the features array.
		 * @param features Features of the point.
		 * 	Memory is copy, hence this does not take ownership of the parameter
		 * @param value The decision value of the point
		 */
		Point(size_t dimension, const float* features, const bool value);

		/**
		 * Point class assignment operator.
		 *
		 * Copy the data of the source object.
		 *
		 * @param source The object to copy.
		 */
		Point& operator=(const Point& source);

		/**
		 * Compare Points.
		 *
		 * Compare Points by defining the smaller as the one of which the first
		 * feature that is not equal to the corresponding feature in the other
		 * point is smaller. If all features are equals, the smaller Point is
		 * the one of value false.
		 *
		 * @param other Point to compare to current point.
		 * @returns true if this is smaller than other.
		 * @note This is meant for making multisets of Point enable.
		 */
		bool operator<(const Point& other) const;

		/**
		 * Test Points equality.
		 *
		 * @param other Point to compare to current point.
		 * @returns true iff all features and value are equals.
		 */
		bool operator==(const Point& other) const;

		/**
		 * Copy constructor of Point.
		 *
		 * Create a Point by copying all features and value of source
		 *
		 * @param source Point from which data will be copied
		 */
		Point(const Point& source);

		/**
		 * Destructor of Point.
		 *
		 * Free memory of features.
		 */
		~Point();

		/// Dimension of features array
		size_t get_dimension();

		/// Decision value of the point
		bool get_value();

		/**
		 * Feature of the Point at given position
		 *
		 * @param position Position of the feature to get in the features array
		 */
		float get_feature(std::size_t position);

		/// Array of features of the Point.
		const float* get_features();

		/// Bracket operator equivalent to get_feature(size_t)
		float operator[](std::size_t position);
		
};

#endif // POINT_H_INCLUDED
