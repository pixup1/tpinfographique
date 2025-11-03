#include "../include/KeyframeCollection.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <regex>

void KeyframeCollection::add(const GeometricTransformation& transformation, float time)
{
	m_keyframes.insert(std::make_pair(time, transformation));
}

void KeyframeCollection::addFromFile(const std::string &animation_filename, float time_shift) {
	std::ifstream is(animation_filename);
	std::string str;
	while (getline(is, str))
	{
		std::regex regexz(",");
		std::vector<std::string> split(std::sregex_token_iterator(str.begin(), str.end(), regexz, -1),
									  std::sregex_token_iterator());
		// Don't forget to convert to Y-up !
		glm::vec3 loc = glm::vec3(std::stof(split[1]), std::stof(split[3]), -std::stof(split[2]));
		glm::vec3 size = glm::vec3(std::stof(split[8]), std::stof(split[10]), std::stof(split[9]));
		glm::quat rot = glm::quat(std::stof(split[7]), std::stof(split[4]), std::stof(split[6]), -std::stof(split[5]));

		this->add(GeometricTransformation(loc, glm::normalize(rot), size), std::stof(split[0]) + time_shift);
	}
}

// Stolen from https://graphicscompendium.com/opengl/22-interpolation
float cubicInterpolate(
	float x0, float x1,
	float x2, float x3,
	float t)
{
	float a = (3.0 * x1 - 3.0 * x2 + x3 - x0) / 2.0;
	float b = (2.0 * x0 - 5.0 * x1 + 4.0 * x2 - x3) / 2.0;
	float c = (x2 - x0) / 2.0;
	float d = x1;

	return a * t * t * t +
		   b * t * t +
		   c * t +
		   d;
}

glm::mat4 KeyframeCollection::interpolateTransformation(float time, bool smooth) const
{
	if (!m_keyframes.empty())
	{
		// Handle the case where the time parameter is outside the keyframes time scope.
		std::map<float, GeometricTransformation>::const_iterator itFirstFrame = m_keyframes.begin();
		std::map<float, GeometricTransformation>::const_reverse_iterator itLastFrame = m_keyframes.rbegin();
		if (time <= itFirstFrame->first) {
			return itFirstFrame->second.toMatrix();
		} else if (time >= itLastFrame->first) {
			return itLastFrame->second.toMatrix();
		}

		glm::mat4 iMatrix(1.0f);

		if (smooth)
		{
			std::map<float, GeometricTransformation>::const_iterator first = m_keyframes.begin();
			std::map<float, GeometricTransformation>::const_iterator last = m_keyframes.end();
			std::map<float, GeometricTransformation>::const_iterator up = m_keyframes.upper_bound(time);
			std::map<float, GeometricTransformation>::const_iterator low = std::prev(up);
			std::map<float, GeometricTransformation>::const_iterator lower;
			std::map<float, GeometricTransformation>::const_iterator upper = std::next(up);
			if (low == first) {
				lower = low;
			} else {
				lower = std::prev(low);
			}
			if (upper == last) {
				upper = up;
			}
			GeometricTransformation g0 = lower->second;
			GeometricTransformation g1 = low->second;
			GeometricTransformation g2 = up->second;
			GeometricTransformation g3 = upper->second;
			glm::vec3 t0 = g0.getTranslation();
			glm::vec3 t1 = g1.getTranslation();
			glm::vec3 t2 = g2.getTranslation();
			glm::vec3 t3 = g3.getTranslation();
			glm::vec3 s0 = g0.getScale();
			glm::vec3 s1 = g1.getScale();
			glm::vec3 s2 = g2.getScale();
			glm::vec3 s3 = g3.getScale();
			glm::quat r0 = glm::normalize(g0.getOrientation());
			glm::quat r1 = glm::normalize(g1.getOrientation());
			glm::quat r2 = glm::normalize(g2.getOrientation());
			glm::quat r3 = glm::normalize(g3.getOrientation());
			glm::vec3 interpTranslation;
			glm::vec3 interpScale;
			glm::quat interpOrientation;
			float factor = (time - low->first) / (up->first - low->first);
			for (int i = 0; i < 3; i++) {
				interpTranslation[i] = cubicInterpolate(
					t0[i], t1[i], t2[i], t3[i],
					factor
				);
				interpScale[i] = cubicInterpolate(
					s0[i], s1[i], s2[i], s3[i],
					factor
				);
			}
			for (int i = 0; i < 4; i++) {
				interpOrientation[i] = cubicInterpolate(
					r0[i], r1[i], r2[i], r3[i],
					factor);
			}
			interpOrientation = glm::normalize(interpOrientation);

			iMatrix = glm::translate(iMatrix, interpTranslation);
			iMatrix *= glm::toMat4(interpOrientation);
			iMatrix = glm::scale(iMatrix, interpScale);
		}
		else
		{
			// Get keyframes surrounding the time parameter
			std::array<Keyframe, 2> bounds = getBoundingKeyframes(time);
			
			float factor = (time - bounds[0].first) / (bounds[1].first - bounds[0].first);

			glm::vec3 interpTranslation = glm::lerp(bounds[0].second.getTranslation(), bounds[1].second.getTranslation(), factor);
			glm::vec3 interpScale = glm::lerp(bounds[0].second.getScale(), bounds[1].second.getScale(), factor);
			glm::quat interpOrientation = glm::slerp(glm::normalize(bounds[0].second.getOrientation()), glm::normalize(bounds[1].second.getOrientation()), factor);

			iMatrix = glm::translate(iMatrix, interpTranslation);
			iMatrix *= glm::toMat4(interpOrientation);
			iMatrix = glm::scale(iMatrix, interpScale);
		}

		return iMatrix;
	}
	else
	{
		return glm::mat4(1.0);
	}
}

bool KeyframeCollection::empty() const
{
	return m_keyframes.empty();
}

const std::map<float, GeometricTransformation>& KeyframeCollection::getKeyFrames() const
{
	return m_keyframes;
}

std::array<KeyframeCollection::Keyframe, 2> KeyframeCollection::getBoundingKeyframes(float time) const
{
	std::array<KeyframeCollection::Keyframe, 2> result{std::make_pair(0, GeometricTransformation()), std::make_pair(0, GeometricTransformation())};
	std::map<float, GeometricTransformation>::const_iterator upper = m_keyframes.upper_bound(time);
	std::map<float, GeometricTransformation>::const_iterator lower = std::prev(upper);
	std::map<float, GeometricTransformation>::const_iterator end = m_keyframes.end();
	if (upper != end && lower != end)
	{
		result[0] = *lower;
		result[1] = *upper;
	}
	return result;
}
