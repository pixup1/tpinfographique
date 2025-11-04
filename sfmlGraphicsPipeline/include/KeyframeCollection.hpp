#ifndef KEYFRAMECOLLECTION_HPP_
#define KEYFRAMECOLLECTION_HPP_

#include <array>
#include <map>
#include <string>

#include "GeometricTransformation.hpp"

typedef enum KeyframeInterpolationMode
{
	LINEAR,
	CUBIC,
	CONSTANT
} KeyframeInterpolationMode;

/**
 * \brief An ordered collection of keyframes.
 *
 * This class store the keyframes in ascending time order. For now, the
 * key frames only define a geometric transformation at a given time.
 * You can either extend this class to add other attributes to interpolate,
 * such as colors, or to create another class similar to this one.
 */
class KeyframeCollection
{
   public:
	/**
	 * \brief Add a key frame to the collection.
	 *
	 * Add a key frame to the collection.
	 * \param transformation The geometric transformation of the keyframe.
	 * \param time The time of the keyframe.
	 */
	void add(const GeometricTransformation& transformation, float time, KeyframeInterpolationMode interpolation = CUBIC);

	/**
	 * \brief Add keyframes from a file.
	 *
	 * Add all the keyframes contained in a .animation file.
	 * \param animation_filename Name of the file containing the keyframes.
	 * \param time_shift The amount of time to shift all the keyframes by.
	 */
	void addFromFile(const std::string &animation_filename, float time_shift);

	/**
	 * \brief Interpolate a transformation at a given time.
	 *
	 * This function will interpolate a geometric transformation at a given
	 * time, using the stored keyframes. Since a geometric transformation is
	 * likely to be used as a mat4 to be send to the GPU, we return directly
	 * here the transformation in this representation.
	 *
	 * If the time is out of the range of stored keyframes, the closest
	 * keyframe is returned. In case there is no keyframe, the identity
	 * matrix is returned.
	 *
	 * \param time Interpolation time
	 * \param smooth Whether to use cubic interpolation (true) or linear interpolation (false)
	 * \return The interpolated geometric transformation.
	 */
	glm::mat4 interpolateTransformation(float time) const;

	/**
	 * @brief Check if the collection is empty.
	 * @return True if the collection is empty, false otherwise.
	 */
	bool empty() const;

   private:
	/**
	* \brief Definition of a keyframe.
	*/
	struct Keyframe
	{
		float time;
		GeometricTransformation transform;
		KeyframeInterpolationMode interpolation;
	};

	/**
	 * \brief Internal storage of the keyframes.
	 *
	 * Keyframes are stored inside a map. Such collection associate
	 * to a time a Keyyframe struct. Its specificity is to
	 * store the data in a way that it is trivial to iterate over the
	 * keyframes in a increasing time order. Internally, a map element
	 * is similar to a set of pair< float, GeometricTransformation >.
	 */
	std::map<float, Keyframe> m_keyframes;
};

#endif
