#include "../../include/lighting/Material.hpp"

#include <glm/gtc/type_ptr.hpp>

Material::~Material()
{
}

Material::Material()
{
	m_ambient = glm::vec3(0.0, 0.0, 0.0);
	m_diffuse = glm::vec3(0.0, 0.0, 0.0);
	m_specular = glm::vec3(0.0, 0.0, 0.0);
	m_shininess = 0.0;
	m_alpha = 1.0;
}

Material::Material(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, const float &shininess, const float &alpha)
{
	m_ambient = ambient;
	m_diffuse = diffuse;
	m_specular = specular;
	m_shininess = shininess;
	m_alpha = alpha;
}

Material::Material(const Material& material)
{
	m_ambient = material.m_ambient;
	m_diffuse = material.m_diffuse;
	m_specular = material.m_specular;
	m_shininess = material.m_shininess;
	m_alpha = material.m_alpha;
}

const glm::vec3& Material::ambient() const
{
	return m_ambient;
}

void Material::setAmbient(const glm::vec3& ambient)
{
	m_ambient = ambient;
}

const glm::vec3& Material::diffuse() const
{
	return m_diffuse;
}

void Material::setDiffuse(const glm::vec3& diffuse)
{
	m_diffuse = diffuse;
}

const glm::vec3& Material::specular() const
{
	return m_specular;
}

void Material::setSpecular(const glm::vec3& specular)
{
	m_specular = specular;
}

void Material::setShininess(float shininess)
{
	m_shininess = shininess;
}

const float& Material::shininess() const
{
	return m_shininess;
}

void Material::setAlpha(float alpha)
{
	m_alpha = alpha;
}

const float &Material::alpha() const
{
	return m_alpha;
}

bool Material::sendToGPU(const ShaderProgramPtr& program, const MaterialPtr& material)
{
	bool success = true;
	int location = -1;

	if (program == nullptr || material == nullptr)
	{
		return false;
	}

	location = program->getUniformLocation("material.ambient");
	if (location != ShaderProgram::null_location)
	{
		glcheck(glUniform3fv(location, 1, glm::value_ptr(material->ambient())));
	}
	else
	{
		success = false;
	}

	location = program->getUniformLocation("material.diffuse");
	if (location != ShaderProgram::null_location)
	{
		glcheck(glUniform3fv(location, 1, glm::value_ptr(material->diffuse())));
	}
	else
	{
		success = false;
	}

	location = program->getUniformLocation("material.specular");
	if (location != ShaderProgram::null_location)
	{
		glcheck(glUniform3fv(location, 1, glm::value_ptr(material->specular())));
	}
	else
	{
		success = false;
	}

	location = program->getUniformLocation("material.shininess");
	if (location != ShaderProgram::null_location)
	{
		// Just a small hack for pow(0,0) = NaN on NVidia hardware
		float actual_shininess = std::max(1e-4f, material->shininess());
		glcheck(glUniform1f(location, actual_shininess));
	}
	else
	{
		success = false;
	}

	location = program->getUniformLocation("material.alpha");
	if (location != ShaderProgram::null_location)
	{
		// Just a small hack for pow(0,0) = NaN on NVidia hardware
		float actual_alpha = std::max(1e-4f, material->alpha());
		glcheck(glUniform1f(location, actual_alpha));
	}
	else
	{
		success = false;
	}

	return success;
}

MaterialPtr Material::Pearl()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.25, 0.20725, 0.20725);
	glm::vec3 diffuse(1.0, 0.829, 0.829);
	glm::vec3 specular(0.296648, 0.296648, 0.296648);
	float shininess = openGLFactor * 0.088;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::Emerald()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.0215, 0.1745, 0.0215);
	glm::vec3 diffuse(0.07568, 0.61424, 0.07568);
	glm::vec3 specular(0.633, 0.727811, 0.633);
	float shininess = openGLFactor * 0.6;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::Bronze()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.2125, 0.1275, 0.054);
	glm::vec3 diffuse(0.714, 0.4284, 0.18144);
	glm::vec3 specular(0.393548, 0.271906, 0.166721);
	float shininess = openGLFactor * 0.2;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::Water()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.0, 0.12, 0.4);
	glm::vec3 diffuse(0.0, 0.65, 1.0);
	glm::vec3 specular(0.7, 0.7, 0.7);
	float shininess = openGLFactor * 0.01;
	float alpha = 0.7f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::Bark()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.2, 0.1, 0.05);
	glm::vec3 diffuse(0.4, 0.2, 0.1);
	glm::vec3 specular(0.1, 0.1, 0.1);
	float shininess = openGLFactor * 0.05;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::Sand()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.4, 0.32, 0.25);
	glm::vec3 diffuse(1.0, 0.85, 0.65);
	glm::vec3 specular(0.7, 0.6, 0.4);
	float shininess = openGLFactor * 0.03;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::White()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.2, 0.2, 0.2);
	glm::vec3 diffuse(1.0, 1.0, 1.0);
	glm::vec3 specular(0.5, 0.5, 0.5);
	float shininess = openGLFactor * 0.01;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::NoLighting()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(2.0, 2.0, 2.0);
	glm::vec3 diffuse(0.0, 0.0, 0.0);
	glm::vec3 specular(0.0, 0.0, 0.0);
	float shininess = openGLFactor * 0.0;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::PureBlack()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.0, 0.0, 0.0);
	glm::vec3 diffuse(0.0, 0.0, 0.0);
	glm::vec3 specular(0.0, 0.0, 0.0);
	float shininess = openGLFactor * 0.0;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::Black()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.02f, 0.02f, 0.02f);
	glm::vec3 diffuse(0.01f, 0.01f, 0.01f);
	glm::vec3 specular(0.4f, 0.4f, 0.4f);
	float shininess = openGLFactor * 0.078125f;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::DarkGreen()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.0f, 0.1f, 0.0f);
	glm::vec3 diffuse(0.0f, 0.5f, 0.0f);
	glm::vec3 specular(0.4f, 0.4f, 0.4f);
	float shininess = openGLFactor * 0.078125f;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::Green()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.0f, 0.15f, 0.0f);
	glm::vec3 diffuse(0.0f, 0.7f, 0.0f);
	glm::vec3 specular(0.5f, 0.5f, 0.5f);
	float shininess = openGLFactor * 0.1f;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::BrightOrange()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.8f, 0.4f, 0.0f);
	glm::vec3 diffuse(1.0f, 0.5f, 0.0f);
	glm::vec3 specular(1.0f, 0.6f, 0.3f);
	float shininess = openGLFactor * 0.25f;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::BrightRed()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.6f, 0.0f, 0.0f);
	glm::vec3 diffuse(1.0f, 0.0f, 0.0f);
	glm::vec3 specular(1.0f, 0.5f, 0.5f);
	float shininess = openGLFactor * 0.3f;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}

MaterialPtr Material::Rock()
{
	float openGLFactor = 128.0;
	glm::vec3 ambient(0.6f, 0.6f, 0.6f);
	glm::vec3 diffuse(0.4f, 0.4f, 0.4f);
	glm::vec3 specular(0.1f, 0.1f, 0.1f);
	float shininess = openGLFactor * 0.05f;
	float alpha = 1.0f;
	return std::make_shared<Material>(ambient, diffuse, specular, shininess, alpha);
}