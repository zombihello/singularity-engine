/**
 * @file
 * @addtogroup shadercompiler shadercompiler
 */

#ifndef SHADERCOMPILER_OUTPUT_H
#define SHADERCOMPILER_OUTPUT_H

#include <string>
#include <vector>
#include <unordered_map>

#include "tools/shadercompiler/ishadercompiler_backend.h"
#include "shadercache/shadercache.h"

/**
 * @ingroup shadercompiler
 * @brief The output of the shader compile
 */
class CShaderCompilerOutput : public IShaderCompilerOutput
{
public:
	/**
	 * @brief Constructor
	 */
	CShaderCompilerOutput();

	/**
	 * @brief Set compiled bytecode of the shader
	 * 
	 * @param pData		Data
	 * @param size		Size
	 */
	virtual void SetBytecode( const byte* pData, uint64 size ) override;

	/**
	 * @brief Set error message
	 * @param pMsg		Error message
	 */
	virtual void SetErrorMsg( const achar* pMsg ) override;

	/**
	 * @brief Set reflection data
	 * @param pData		Data
	 * @param size		Size
	 */
	virtual void SetReflectionData( const byte* pData, uint64 size ) override;

	/**
	 * @brief Get bytecode
	 * @return Return bytecode
	 */
	FORCEINLINE const std::vector<byte>& GetBytecode() const
	{
		return bytecode;
	}

	/**
	 * @brief Get error message
	 * @return Return error message. If not exist returns empty string
	 */
	FORCEINLINE const std::string& GetErrorMsg() const
	{
		return errorMsg;
	}

	/**
	 * @brief Get the reflection data
	 * @return Return the reflection data
	 */
	FORCEINLINE const std::vector<byte>& GetReflectionData() const
	{
		return reflectionData;
	}

private:
	std::vector<byte>		bytecode;			/**< Output compiled code of shader */
	std::string				errorMsg;			/**< Error message. Compile putting to this field message when shader compiled is fail */
	std::vector<byte>		reflectionData;		/**< Reflection data */
};

#endif // !SHADERCOMPILER_OUTPUT_H