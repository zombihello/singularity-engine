/**
 * @file
 * @addtogroup gameinfo gameinfo
 */

#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <string>
#include <vector>

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/jsondoc.h"

/**
 * @ingroup gameinfo
 * @brief Game info search path
 */
struct gameInfoSearchPath_t
{
	std::string		id;		/**< Path ID */
	std::string		path;	/**< Path */
};

/**
 * @ingroup gameinfo
 * @brief Helper for work with gameinfo.txt files
 */
class CGameInfoDoc
{
public:
	/**
	 * @brief Constructor
	 */
	CGameInfoDoc();

	/**
	 * @brief Load from file
	 * @warning For use StdLib must be connected by ConnectStdLib
	 * 
	 * @param pPath		Path to file
	 * @return Return TRUE when file success loaded, otherwise returns FALSE
	 */
	bool LoadFromFile( const achar* pPath );

	/**
	 * @brief Load from buffer
	 *
	 * @param pBuffer			Buffer. The buffer must be null terminated
	 * @param pGameInfoPath		Path to directory where gameinfo.txt is. This is need for replace |gameinfo_path|
	 * @return Return TRUE if buffer successfully loaded, otherwise returns FALSE
	 */
	bool LoadFromBuffer( const achar* pBuffer, const achar* pGameInfoPath );

	/**
	 * @brief Clear game info
	 */
	void Clear();

	/**
	 * @brief Get game name
	 * @return Return game name. If isn't set return empty string
	 */
	FORCEINLINE const std::string& GetGame() const
	{
		return game;
	}

	/**
	 * @brief Get game version
	 * @return Return game version. If isn't set return empty string
	 */
	FORCEINLINE const std::string& GetVersion() const
	{
		return version;
	}

	/**
	 * @brief Get support email
	 * @return Return support email. If isn't set return empty string
	 */
	FORCEINLINE const std::string& GetSupportEmail() const
	{
		return supportEmail;
	}

	/**
	 * @brief Get support URL
	 * @return Return support URL. If isn't set return empty string
	 */
	FORCEINLINE const std::string& GetSupportURL() const
	{
		return supportURL;
	}

	/**
	 * @brief Get search paths
	 * @return Return search paths
	 */
	FORCEINLINE const std::vector<gameInfoSearchPath_t>& GetSearchPaths() const
	{
		return searchPaths;
	}

	/**
	 * @brief Is loaded game info
	 * @return Return TRUE if game info is loaded, otherwise returns FALSE
	 */
	FORCEINLINE bool IsLoaded() const
	{
		return bLoaded;
	}

private:
	/**
	 * @brief Grab data from JSON file
	 * 
	 * @param jsonDoc			JSON document
	 * @param pGameInfoPath		Path to directory where gameinfo.txt is. This is need for replace |gameinfo_path|
	 * @return Return TRUE if successfully got data from JSON file, otherwise returns FALSE
	 */
	bool GrabData( const CJsonDoc& jsonDoc, const achar* pGameInfoPath );

	/**
	 * @brief Replace macros in string
	 * 
	 * @param string			String where need replace macros
	 * @param pGameInfoPath		Path to directory where gameinfo.txt is. This is need for replace |gameinfo_path|
	 */
	void ReplaceMacros( std::string& string, const achar* pGameInfoPath );

	bool									bLoaded;		/**< Is loaded gameinfo */
	std::string								game;			/**< Game name */
	std::string								version;		/**< Game version */
	std::string								supportEmail;	/**< Support email */
	std::string								supportURL;		/**< Support URL */
	std::vector<gameInfoSearchPath_t>		searchPaths;	/**< Search paths */
};

#endif // !GAMEINFO_H