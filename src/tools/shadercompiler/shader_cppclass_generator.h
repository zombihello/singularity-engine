/**
 * @file
 * @addtogroup shadercompiler shadercompiler
 */

#ifndef SHADER_CPPCLASS_GENERATOR_H
#define SHADER_CPPCLASS_GENERATOR_H

#include "tools/shadercompiler/shadercompiler.h"

/**
 * @ingroup shadercompiler
 * @brief Generator of a helper C++ class for shader
 */
class CShaderCppClassGenerator
{
public:
	/**
	 * @brief Generate a helper C++ class for shader
	 * @param shader	shader_t
	 */
	void Generate( const shader_t& shader );

	/**
	 * @brief Reset generator
	 */
	FORCEINLINE void Reset()
	{
		buffer.clear();
	}

	/**
	 * @brief Get generated C++ class
	 * @return Return generated C++ class. Returns an empty string if Generate() was not called or Reset() was called
	 */
	FORCEINLINE const std::string& GetBuffer() const
	{
		return buffer;
	}

private:
	/**
	 * @brief Generate var and set function of the one for a shader flag
	 * @param shaderFlag	shader_t flag
	 */
	void GenerateVar( const shaderFlag_t& shaderFlag );

	/**
	 * @brief Generate class constructor
	 * 
	 * @param pClassName	Class name
	 * @param shader		shader_t
	 */
	void GenerateConstructor( const achar* pClassName, const shader_t& shader );

	/**
	 * @brief Generate function for calculate a shader index for the flags combination
	 * @param shader	shader_t
	 */
	void GenerateGetIndexFunc( const shader_t& shader );

	std::string		buffer;		/**< Buffer of generated C++ class */
};

#endif // !SHADER_CPPCLASS_GENERATOR_H