/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "smatdoc/smat_compiled_doc.h"
#include "studiorender/studioapi/istudioapi_buffer.h"
#include "studiorender/istudio_rendercmd.h"
#include "materialsystem/imaterial.h"

// Forward declarations
class IStudioAPICmdContext;
class CMaterialVar;

/**
 * @ingroup materialsystem
 * @brief Material
 */
class CMaterial : public TRefCounted<IMaterial>
{
public:
	/**
	 * @brief Constructor
	 */
	CMaterial();

	/**
	 * @brief Constructor
	 * @param smatCompiledDoc	SMAT compiled document
	 */
	CMaterial( const CSMATCompiledMaterialDoc& smatCompiledDoc );

	/**
	 * @brief Destructor
	 */
	~CMaterial();

	/**
	 * @brief Clear the material
	 */
	void Clear();

	/**
	 * @brief Place barriers into a list
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioAPICmdList	StudioAPI command list
	 */
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) override;

	/**
	 * @brief Prepare for draw
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioAPICmdList	StudioAPI command list
	 * @param renderPassType	Render pass type
	 */
	virtual void R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType ) override;

	/**
	 * @brief Set a shader by name
	 * Find in the shader system a shader by name, set it and add to the material all shader parameters
	 * @param pShaderName	Shader name
	 */
	virtual void SetShader( const achar* pShaderName ) override;

	/**
	 * @brief Find material variable
	 * @param pName		Variable name
	 * @return Return founded material variable. If not found returns NULL
	 */
	virtual IMaterialVar* FindVar( const achar* pName ) const override;

	/**
	 * @brief Get number of material variables
	 * @return Return number of material variables
	 */
	virtual uint32 GetNumVars() const override;

	/**
	 * @brief Get material variables
	 * @return Return array of material variables
	 */
	virtual IMaterialVar** GetVars() const override;

	/**
	 * @brief Get shader name
	 * @return Return shader name. If not set return empty string
	 */
	virtual const achar* GetShaderName() const override;

	/**
	 * @brief Get the material shader
	 * @return Return the material shader. If not set returns NULL
	 */
	virtual IShader* GetShader() const override;

	/**
	 * @brief Mark dirty buffers
	 */
	FORCEINLINE void MarkDirtyBuffers()
	{
		bDirtyBuffers = true;
	}

private:
	/**
	 * @brief Calculate a hash for a string to use it in std::unordered_map
	 */
	struct insensitiveStringHash_t
	{
		/**
		 * @brief Calculate a hash for a string
		 * @param pString	String
		 * @return Return calculated hash for the string
		 */
		FORCEINLINE std::size_t operator()( const achar* pString ) const
		{
			return FastHashFromString( pString );
		}
	};

	/**
	 * @brief Comparator for std::unordered_map to insensitive compre strings
	 */
	struct insensitiveCompareString_t
	{
		/**
		 * @brief Compare two string
		 * @param pLeft		First string
		 * @param pRight	Second string
		 * @return Return TRUE if they are is same, otherwise FALSE
		 */
		bool operator()( const achar* pLeft, const achar* pRight ) const
		{
			return !S_Stricmp( pLeft, pRight );
		}
	};

	/**
	 * @brief Material variables dictionary type
	 */
	typedef std::unordered_map<const achar*, uint32, insensitiveStringHash_t, insensitiveCompareString_t >		materialVarsDict_t;

	/**
	 * @brief Initialize the material by a SMAT document
	 * @param smatCompiledDoc	SMAT compiled document
	 */
	void Init( const CSMATCompiledMaterialDoc& smatCompiledDoc );

	/**
	 * @brief Update buffers
	 * @warning This is only called by the render thread
	 * @param pCmdContext	Command context
	 */
	void R_UpdateBuffers( IStudioAPICmdContext* pCmdContext );

	/**
	 * @brief Destroy buffers
	 */
	FORCEINLINE void DestroyBuffers()
	{
		PROFILE_SCOPE();

		// Remove buffers, do they on the render thread to make sure that it will be destroyed at the render thread
		if ( !studioAPIBuffers.empty() )
		{
			UNIQUE_RENDER_COMMAND_ONEPARAMETER( CRemoveStudioAPIBuffersCmd, 
												std::vector<TRefPtr<IStudioAPIBuffer>>, studioAPIBuffers, std::move( studioAPIBuffers ),
												{
													studioAPIBuffers.clear();
												} );
		}
	}

	bool										bDirtyBuffers;				/**< Is dirty buffers */
	IShader*									pShader;					/**< Shader */
	std::vector<CMaterialVar*>					vars;						/**< Material variables */
	materialVarsDict_t 							varsDict;					/**< Material variable dictionary for find by name */
	std::vector<TRefPtr<IStudioAPIBuffer>>		studioAPIBuffers;			/**< StudioAPI buffers */
};

#endif // !MATERIAL_H