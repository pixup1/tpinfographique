#include "../include/KeyframeCollection.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <regex>

void KeyframeCollection::add(const GeometricTransformation &transformation, float time, KeyframeInterpolationMode interpolation)
{
	Keyframe k;
	k.time = time;
	k.transform = transformation;
	k.interpolation = interpolation;
	m_keyframes.insert(std::make_pair(time, k));
}

void KeyframeCollection::addFromFile(const std::string &animation_filename, float time_shift)
{
	std::ifstream is(animation_filename);
	std::string str;
	while (getline(is, str))
	{
		std::regex regexz(",");
		std::vector<std::string> split(std::sregex_token_iterator(str.begin(), str.end(), regexz, -1),
									   std::sregex_token_iterator());
		// Don't forget to convert to Y-up !
		glm::vec3 loc = glm::vec3(std::stof(split[2]), std::stof(split[4]), -std::stof(split[3]));
		glm::vec3 size = glm::vec3(std::stof(split[9]), std::stof(split[11]), std::stof(split[10]));
		glm::quat rot = glm::quat(std::stof(split[8]), std::stof(split[5]), std::stof(split[7]), -std::stof(split[6]));

		// Default interpolation is cubic
		KeyframeInterpolationMode interp = CUBIC;
		if (split[1] == "LINEAR")
		{
			interp = LINEAR;
		}
		else if (split[1] == "CONSTANT")
		{
			interp = CONSTANT;
		}
		else if (split[1] == "CUBIC")
		{
			interp = CUBIC;
		}

		this->add(GeometricTransformation(loc, glm::normalize(rot), size), std::stof(split[0]) + time_shift, interp);
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

glm::mat4 KeyframeCollection::interpolateTransformation(float time) const
{
	if (!m_keyframes.empty())
	{
		// Handle the case where the time parameter is outside the keyframes time scope.
		std::map<float, Keyframe>::const_iterator first = m_keyframes.begin();
		std::map<float, Keyframe>::const_iterator last = std::prev(m_keyframes.end());
		if (time <= first->first)
		{
			return first->second.transform.toMatrix();
		}
		else if (time >= last->first)
		{
			return last->second.transform.toMatrix();
		}

		glm::mat4 iMatrix(1.0f);

		std::map<float, Keyframe>::const_iterator ki2 = m_keyframes.upper_bound(time);
		std::map<float, Keyframe>::const_iterator ki1 = std::prev(ki2);

		float factor = (time - ki1->first) / (ki2->first - ki1->first);

		switch (ki1->second.interpolation)
		{
		case CUBIC:
		{
			std::map<float, Keyframe>::const_iterator first = m_keyframes.begin();
			std::map<float, Keyframe>::const_iterator last = m_keyframes.end();

			std::map<float, Keyframe>::const_iterator ki0 = std::prev(ki1);
			if (ki1 == first || ki0->second.interpolation != CUBIC) // Only take cubic keyframes into account
			{
				ki0 = ki1;
			}
			std::map<float, Keyframe>::const_iterator ki3 = std::next(ki2);
			if (ki3 == last || ki2->second.interpolation != CUBIC)
			{
				ki3 = ki2;
			}
			GeometricTransformation g0 = ki0->second.transform;
			GeometricTransformation g1 = ki1->second.transform;
			// Current instant is between these two keyframes
			GeometricTransformation g2 = ki2->second.transform;
			GeometricTransformation g3 = ki3->second.transform;
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
			for (int i = 0; i < 3; i++)
			{
				interpTranslation[i] = cubicInterpolate(
					t0[i], t1[i], t2[i], t3[i],
					factor);
				interpScale[i] = cubicInterpolate(
					s0[i], s1[i], s2[i], s3[i],
					factor);
			}
			for (int i = 0; i < 4; i++)
			{
				interpOrientation[i] = cubicInterpolate(
					r0[i], r1[i], r2[i], r3[i],
					factor);
			}
			interpOrientation = glm::normalize(interpOrientation);

			iMatrix = glm::translate(iMatrix, interpTranslation);
			iMatrix *= glm::toMat4(interpOrientation);
			iMatrix = glm::scale(iMatrix, interpScale);

			break;
		}
		case LINEAR:
		{
			Keyframe k1 = ki1->second;
			Keyframe k2 = ki2->second;

			glm::vec3 interpTranslation = glm::lerp(k1.transform.getTranslation(), k2.transform.getTranslation(), factor);
			glm::vec3 interpScale = glm::lerp(k1.transform.getScale(), k2.transform.getScale(), factor);
			glm::quat interpOrientation = glm::slerp(glm::normalize(k1.transform.getOrientation()), glm::normalize(k2.transform.getOrientation()), factor);

			iMatrix = glm::translate(iMatrix, interpTranslation);
			iMatrix *= glm::toMat4(interpOrientation);
			iMatrix = glm::scale(iMatrix, interpScale);

			break;
		}
		case CONSTANT:
		{
			// Just use the previous keyframe's transform
			iMatrix = ki1->second.transform.toMatrix();

			break;
		}
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