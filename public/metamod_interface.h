/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * ======================================================
 * Metamod:Source
 * Copyright (C) 2004-2009 AlliedModders LLC and authors.
 * All rights reserved.
 * ======================================================
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include "metamod_sharedef.h"


NAMESPACE_METAMOD_BEGIN

constexpr auto MMIFACE_SOURCEHOOK = "ISourceHook";				// ISourceHook pointer
constexpr auto MMIFACE_PLMANAGER = "IPluginManager";			// Metamod plugin functions
constexpr auto MMIFACE_SH_HOOKMANAUTOGEN = "IHookManagerAutoGen"// SourceHook::IHookManagerAutoGen pointer
constexpr auto IFACE_MAXNUM = 999;								// Maximum interface version

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);

class IServerPluginCallbacks;

// Considering the compatible of sourcepawn plugins.
// Updating this part means that you must also update sp's include.
enum MetamodBackend
{
	MMBackend_Episode1 = 0,
	MMBackend_DarkMessiah,
	MMBackend_Episode2,
	MMBackend_BloodyGoodTime,
	MMBackend_EYE,
	MMBackend_CSS,
	MMBackend_Episode2Valve_OBSOLETE,
	MMBackend_Left4Dead,
	MMBackend_Left4Dead2,
	MMBackend_AlienSwarm,
	MMBackend_Portal2,
	MMBackend_CSGO,
	MMBackend_DOTA,
	MMBackend_HL2DM,
	MMBackend_DODS,
	MMBackend_TF2,
	MMBackend_NuclearDawn,
	MMBackend_SDK2013,
	MMBackend_Blade,
	MMBackend_Insurgency,
	MMBackend_Contagion,
	MMBackend_BMS,
	MMBackend_DOI,

	MMBackend_Mock,
	MMBackend_UNKNOWN
};

constexpr auto METAMOD_PLAPI_VERSION = 16; // Version of interface...
constexpr auto METAMOD_PLAPI_NAME = "ISmmPlugin"; // Name of the plugin interface...

// Warning: Possible recursive referencing!
class ISmmAPI;
class ISmmPlugin;
class IMetamodListener;

/**
 * The core API that Metamod:Source provides to plugins.
 */
class ISmmAPI
{
public:
	/**
	 * @brief Logs a message through the HL2 log system.
	 * Note: Newlines are appended automatically.
	 *
	 * @param pl			Plugin API pointer (used for tagging message)
	 * @param msg			Formatted string.
	 */
	virtual void LogMsg(ISmmPlugin* pl, const char* msg, ...) = 0;

	/**
	 * @brief Returns an interface factory for the HL2 engine.
	 *
	 * @param syn			If syn is true, the synthetic wrapper is returned.
	 *						If syn is false, the true function is returned.
	 * @return				CreateInterfaceFn function pointer.
	 */
	virtual CreateInterfaceFn GetEngineFactory(bool syn = true) = 0;

	/**
	 * @brief Returns an interface factory for the HL2 physics engine.
	 *
	 * @param syn			If syn is true, the synthetic wrapper is returned.
	 *						If syn is false, the true function is returned.
	 * @return				CreateInterfaceFn function pointer.
	 */
	virtual CreateInterfaceFn GetPhysicsFactory(bool syn = true) = 0;

	/**
	 * @brief Returns an interface factory for the HL2 file system.
	 *
	 * @param syn			If syn is true, the synthetic wrapper is returned.
	 *						If syn is false, the true function is returned.
	 * @return				CreateInterfaceFn function pointer.
	 */
	virtual CreateInterfaceFn GetFileSystemFactory(bool syn = true) = 0;

	/**
	 * @brief Returns an interface factory for the GameDLL.
	 *
	 *						If syn is false, the true function is returned.
	 * @return				CreateInterfaceFn function pointer.
	 */
	virtual CreateInterfaceFn GetServerFactory(bool syn = true) = 0;

	/**
	 * @brief Returns a CGlobalVars pointer from the HL2 Engine.
	 *
	 * @return				CGlobalVars pointer.
	 */
	virtual CGlobalVars* GetCGlobals() = 0;

	/**
	 * @brief Registers a ConCommandBase.
	 *
	 * @param plugin		Parent plugin API pointer.
	 * @param pCommand		ConCommandBase to register.
	 * @return				True if successful, false otherwise.
	 */
	virtual bool RegisterConCommandBase(ISmmPlugin* plugin, ConCommandBase* pCommand) = 0;

	/**
	 * @brief Unregisters a ConCommandBase.
	 *
	 * @param plugin		Parent plugin API pointer.
	 * @param pCommand		ConCommandBase to unlink.
	 */
	virtual void UnregisterConCommandBase(ISmmPlugin* plugin, ConCommandBase* pCommand) = 0;

	/**
	 * @brief Prints an unformatted string to the remote server console.
	 *
	 * Note: Newlines are not added automatically.
	 *
	 * @param str			Message string.
	 */
	virtual void ConPrint(const char* str) = 0;

	/**
	 * @brief Prints a formatted message to the remote server console.
	 *
	 * Note: Newlines are not added automatically.
	 *
	 * @param fmt			Formatted message.
	 */
	virtual void ConPrintf(const char* fmt, ...) = 0;

	/**
	 * @brief Returns the Metamod Version numbers as major version and
	 * minor (API) version.  Changes to minor version are guaranteed to be
	 * backwards compatible.  Changes to major version are not.
	 *
	 * @param major			Filled with the major API version number.
	 * @param minor			Filled with the minor API version number.
	 * @param plvers		Filled with the current plugin API version number.
	 * @param plmin			Filled with the minimum plugin API version number
	 * 						supported.
	 */
	virtual void GetApiVersions(int& major, int& minor, int& plvers, int& plmin) = 0;

	/**
	 * @brief Returns sourcehook API version and implementation version.
	 *
	 * @param shvers		Filled with the SourceHook API version number.
	 * @param shimpl		Filled with the SourceHook implementation number.
	 */
	virtual void GetShVersions(int& shvers, int& shimpl) = 0;

	/**
	 * @brief Adds a Metamod listener.
	 *
	 * @param plugin		Plugin interface pointer.
	 * @param pListener		Listener interface pointer to add.
	 */
	virtual void AddListener(ISmmPlugin* plugin, IMetamodListener* pListener) = 0;

	/**
	  * @brief Queries the metamod factory
	  *
	  * @param iface		String containing interface name
	  * @param ret			Optional pointer to store return status
	  * @param id			Optional pointer to store id of plugin that
	  * 					overrode interface, 0 if none
	  * @return				Returned pointer
	  */
	virtual void* MetaFactory(const char* iface, int* ret, PluginId* id) = 0;

	/**
	 * @brief Given a base interface name, such as ServerGameDLL or
	 * ServerGameDLL003, reformats the string to increase the number, then
	 * returns the new number. This is the base function to InterfaceSearch()
	 * and VInterfaceMatch().
	 *
	 * @param iface			Input/output interface name.  Must be writable.
	 * @param maxlength		Maximum length of iface buffer.  Must be at least
	 * 						strlen(iface)+4 chars.
	 * @return				The newly incremented iface version number.
	 * @deprecated			Use InterfaceSearch() or VInterfaceMatch instead.
	 */
	virtual int FormatIface(char iface[], size_t maxlength) = 0;

	/**
	 * @brief Searches for an interface, eliminating the need to loop
	 * through FormatIface().
	 *
	 * @param fn			InterfaceFactory function.
	 * @param iface			Interface string name.
	 * @param max			Maximum version to look up.
	 * @param ret			Last return code from interface factory function.
	 * @return				Interface pointer, or NULL if not found.
	 */
	virtual void* InterfaceSearch(CreateInterfaceFn fn,
		const char* iface,
		int max,
		int* ret) = 0;

	/**
	 * @brief Returns the base directory of the game/server, equivalent to
	 * IVEngineServer::GetGameDir(), except the path is absolute.
	 *
	 * @return				Static pointer to game's absolute basedir.
	 */
	virtual const char* GetBaseDir() = 0;

	/**
	 * @brief Formats a file path to the local OS.
	 *
	 * Does not include any base directories.  Note that all slashes and
	 * black slashes are reverted to the local OS's expectancy.
	 *
	 * @param buffer		Destination buffer to store path.
	 * @param len			Maximum length of buffer, including null
	 * 						terminator.
	 * @param fmt			Formatted string.
	 * @param ...			Arguments in the string.
	 * @return				Number of bytes written, not including the null
	 *						terminator.
	 */
	virtual size_t PathFormat(char* buffer, size_t len, const char* fmt, ...) = 0;

	/**
	 * @brief Prints text in the specified client's console. Same as
	 * IVEngineServer::ClientPrintf except that it allows for string
	 * formatting.
	 *
	 * @param client		Client edict pointer.
	 * @param fmt			Formatted string to print to the client.
	 */
	virtual void ClientConPrintf(edict_t* client, const char* fmt, ...) = 0;

	/**
	 * @brief Wrapper around InterfaceSearch().  Assumes no maximum.
	 * This is designed to replace the fact that searches only went upwards.
	 * The "V" is intended to convey that this is for Valve formatted
	 * interface strings.
	 *
	 * @param fn			Interface factory function.
	 * @param iface			Interface string.
	 * @param min			Minimum value to search from.  If zero, searching
	 *						begins from the first available version regardless
	 *						of the interface.  Note that this can return
	 *						interfaces EARLIER than the version specified.  A
	 *						value of -1 (default) specifies the string version
	 *						as the minimum.  Any other value specifices the
	 *						minimum value to search from.
	 * @return				Interface pointer, or NULL if not found.
	 */
	virtual void* VInterfaceMatch(CreateInterfaceFn fn,
		const char* iface,
		int min = -1) = 0;

	/**
	 * @brief Tells SourceMM to add VSP hooking capability to plugins.
	 *
	 * Since this  potentially uses more resources than it would otherwise,
	 * plugins have to explicitly enable the feature.  Whether requested or
	 * not, if it is enabled, all plugins will get a pointer to the VSP
	 * listener through IMetamodListener.  This will not be called more than
	 * once for a given plugin; if it is requested more than once, each
	 * successive call will only give the pointer to plugins which have not
	 * yet received it.
	 */
	virtual void EnableVSPListener() = 0;

	/**
	 * @brief Returns the interface version of the GameDLL's IServerGameDLL
	 * implementation.
	 *
	 * @return				Interface version of the loaded IServerGameDLL.
	 */
	virtual int GetGameDLLVersion() = 0;

	/**
	 * @brief Returns the number of user messages in the GameDLL.
	 *
	 * @return				Number of user messages, or -1 if SourceMM has
	 *						failed to get user message list.
	 */
	virtual int GetUserMessageCount() = 0;

	/**
	 * @brief Returns the index of the specified user message.
	 *
	 * @param name			User message name.
	 * @param size			Optional pointer to store size of user message.
	 * @return				Message index, or -1 on failure.
	 */
	virtual int FindUserMessage(const char* name, int* size = NULL) = 0;

	/**
	 * @brief Returns the name of the specified user message.
	 *
	 * @param index			User message index.
	 * @param size			Optional pointer to store size of user message.
	 * @return				Message name, or NULL on failure.
	 */
	virtual const char* GetUserMessage(int index, int* size = NULL) = 0;

	/**
	 * @brief Returns the highest interface version of IServerPluginCallbacks
	 * that the engine supports.  This is useful for games that run on older
	 * versions of the Source engine, such as The Ship.
	 *
	 * @return				Highest interface version of IServerPluginCallbacks.
	 *						Returns 0 if SourceMM's VSP listener isn't
	 *						currently enabled.
	 * @deprecated			Use GetVSPInfo() instead.
	 */
	virtual int GetVSPVersion() = 0;

	/**
	 * @brief Returns the engine interface that MM:S is using as a backend.
	 *
	 * The values will be one of the SOURCE_ENGINE_* constants from the top
	 * of this file.
	 *
	 * @return				A SOURCE_ENGINE_* constant value.
	 */
	virtual int GetSourceEngineBuild() = 0;

	/**
	 * @brief Returns the VSP listener loaded.
	 *
	 * This is useful for late-loading plugins which need to decide whether
	 * to add a listener or not (or need to get the pointer at all).
	 *
	 * @param pVersion		Optional pointer to store the VSP version.
	 * @return				IServerPluginCallbacks pointer, or NULL if an
	 * 						IMetamodListener event has yet to occur for
	 * 						EnableVSPListener().
	 */
	virtual IServerPluginCallbacks* GetVSPInfo(int* pVersion) = 0;

	/**
	 * @brief Formats a string.  This is a platform safe wrapper around
	 * snprintf/_snprintf.
	 *
	 * @param buffer		Buffer to write to.
	 * @param maxlength		Maximum length of the buffer.
	 * @param format		Format specifiers.
	 * @param ...			Format arguments.
	 * @return				Number of bytes actually written, not including
	 * 						the null terminator.
	 */
	virtual size_t Format(char* buffer,
		size_t maxlength,
		const char* format,
		...) = 0;

	/**
	 * @brief Formats a string.  This is a platform safe wrapper around
	 * vsnprintf/_vsnprintf.
	 *
	 * @param buffer		Buffer to write to.
	 * @param maxlength		Maximum length of the buffer.
	 * @param format		Format specifiers.
	 * @param ap			Format argument list.
	 * @return				Number of bytes actually written, not including the
	 *						null terminator.
	 */
	virtual size_t FormatArgs(char* buffer,
		size_t maxlength,
		const char* format,
		va_list ap) = 0;
};

/**
 * @brief Callbacks that a plugin must expose.
 */
class ISmmPlugin
{
public:
	/**
	 * @brief Called to request the plugin's API version.
	 *
	 * This is the first callback invoked, and always remains at the top
	 * of the virtual table.
	 *
	 * @return			Plugin API version.
	 */
	virtual int GetApiVersion()
	{
		return METAMOD_PLAPI_VERSION;
	}

	/**
	 * @brief Virtual destructor so GCC doesn't complain.
	 */
	virtual ~ISmmPlugin()
	{
	}

public:
	/**
	 * @brief Called on plugin load.
	 *
	 * This is called as DLLInit() executes - after the parameters are
	 * known, but before the original GameDLL function is called.
	 * Therefore, you cannot hook it, but you don't need to - Load() is
	 * basically your hook.  You can override factories before the engine
	 * and gamedll exchange them.  However, take care to note that if your
	 * plugin is unloaded, and the gamedll/engine have cached an interface
	 * you've passed, something will definitely crash.  Be careful.
	 *
	 * @param id		Internal id of plugin.  Saved globally by PLUGIN_SAVEVARS()
	 * @param ismm		External API for SourceMM.  Saved globally by PLUGIN_SAVEVARS()
	 * @param error		Error message buffer
	 * @param maxlength	Size of error message buffer
	 * @param late		Set to true if your plugin was loaded late (not at server load).
	 * @return			True if successful, return false to reject the load.
	 */
	virtual bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlength, bool late) = 0;

	/**
	 * @brief Called when all plugins have been loaded.
	 *
	 * This is called after DLLInit(), and thus the mod has been mostly initialized.
	 * It is also safe to assume that all other (automatically loaded) plugins are now
	 * ready to start interacting, because they are all loaded.
	 */
	virtual void AllPluginsLoaded()
	{
	}

	/**
	 * @brief Called when your plugin is "queried".
	 *
	 * This is useful for rejecting a loaded state.  For example, if your
	 * plugin wants to stop operating, it can simply return false and copy
	 * an error message.  This will notify other plugins or MM:S of
	 * something bad that happened.  MM:S will not cache the return state,
	 * so if you return false,  your plugin will not actually be paused or
	 * unloaded.  This callback will be called when:
	 *  - Another plugin requests it
	 *  - Someone types "meta list", it will show up as "REFUSED"
	 *  - When Metamod need to re-check the plugin's status
	 *  - If the plugin does something like overload a factory, Metamod
	 *    will make sure the Query() returns true before calling it.
	 *  Also note that this query will only override Metamod when the
	 *  plugin is running and not paused.
	 *
	 * @param error		Buffer for error message, or NULL if none.
	 * @param maxlen	Maximum length of error buffer.
	 * @return			Status code - true for okay, false for badness.
	 */
	virtual bool QueryRunning(char* error, size_t maxlen)
	{
		return true;
	}

	/**
	 * @brief Called on plugin unload.  You can return false if you know
	 * your plugin is not capable of restoring critical states it modifies.
	 *
	 * @param error		Error message buffer
	 * @param maxlen	Size of error message buffer
	 * @return			True on success, return false to request no unload.
	 */
	virtual bool Unload(char* error, size_t maxlen)
	{
		return true;
	}

	/**
	 * @brief Called on plugin pause.
	 *
	 * @param	error Error message buffer
	 * @param	maxlen Size of error message buffer
	 * @return	True on success, return false to request no pause.
	 */
	virtual bool Pause(char* error, size_t maxlen)
	{
		return true;
	}

	/**
	 * @brief Called on plugin unpause.
	 *
	 * @param	error Error message buffer
	 * @param	maxlen Size of error message buffer
	 * @return	True on success, return false to request no unpause.
	 */
	virtual bool Unpause(char* error, size_t maxlen)
	{
		return true;
	}
public:
	/** @brief Return author as string */
	virtual const char* GetAuthor() = 0;

	/** @brief Return plugin name as string */
	virtual const char* GetName() = 0;

	/** @brief Return a description as string */
	virtual const char* GetDescription() = 0;

	/** @brief Return a URL as string */
	virtual const char* GetURL() = 0;

	/** @brief Return quick license code as string */
	virtual const char* GetLicense() = 0;

	/** @brief Return version as string */
	virtual const char* GetVersion() = 0;

	/** @brief Return author as string */
	virtual const char* GetDate() = 0;

	/** @brief Return author as string */
	virtual const char* GetLogTag() = 0;
};

/**
 * @brief Various events that Metamod can fire.
 */
class IMetamodListener
{
public:
	/**
	 * @brief Called when a plugin is loaded.
	 *
	 * @param id		Id of the plugin.
	 */
	virtual void OnPluginLoad(PluginId id)
	{
	}

	/**
	 * @brief Called when a plugin is unloaded.
	 *
	 * @param id		Id of the plugin.
	 */
	virtual void OnPluginUnload(PluginId id)
	{
	}

	/**
	 * @brief Called when a plugin is paused.
	 *
	 * @param id		Id of the plugin.
	 */
	virtual void OnPluginPause(PluginId id)
	{
	}

	/**
	 * @brief Called when a plugin is unpaused.
	 *
	 * @param id		Id of the plugin.
	 */
	virtual void OnPluginUnpause(PluginId id)
	{
	}

	/**
	 * @brief Called when the level is loaded (after GameInit, before
	 * ServerActivate).
	 *
	 * To override this, hook IServerGameDLL::LevelInit().
	 *
	 * @param pMapName		Name of the map.
	 * @param pMapEntities	Lump string of the map entities, in KeyValues
	 * 						format.
	 * @param pOldLevel		Unknown.
	 * @param pLandmarkName	Unknown.
	 * @param loadGame		Unknown.
	 * @param background	Unknown.
	 */
	virtual void OnLevelInit(char const* pMapName,
		char const* pMapEntities,
		char const* pOldLevel,
		char const* pLandmarkName,
		bool loadGame,
		bool background)
	{
	}

	/**
	 * @brief Called when the level is shut down.  May be called more than
	 * once.
	 */
	virtual void OnLevelShutdown()
	{
	}

	/**
	 * @brief Called when engineFactory() is used through Metamod:Source's
	 * wrapper.  This can be used to provide interfaces to other plugins or
	 * the GameDLL.
	 *
	 * If ret is passed, you should fill it with META_IFACE_OK or META_IFACE_FAILED.
	 *
	 * @param iface			Interface string.
	 * @param ret			Optional pointer to store return code.
	 * @return				Generic pointer to the interface, or NULL if
	 * 						not found.
	 */
	virtual void* OnEngineQuery(const char* iface, int* ret)
	{
		if (ret)
		{
			*ret = META_IFACE_FAILED;
		}

		return NULL;
	}

	/**
	 * @brief Called when the physics factory is used through
	 * Metamod:Source's wrapper. This can be used to provide interfaces to
	 * other plugins.
	 *
	 * If ret is passed, you should fill it with META_IFACE_OK or META_IFACE_FAILED.
	 *
	 * @param iface			Interface string.
	 * @param ret			Optional pointer to store return code.
	 * @return				Generic pointer to the interface, or NULL if
	 * 						not found.
	 */
	virtual void* OnPhysicsQuery(const char* iface, int* ret)
	{
		if (ret)
		{
			*ret = META_IFACE_FAILED;
		}

		return NULL;
	}

	/**
	 * @brief Called when the filesystem factory is used through
	 * Metamod:Source's wrapper.  This can be used to provide interfaces to
	 * other plugins.
	 *
	 * If ret is passed, you should fill it with META_IFACE_OK or META_IFACE_FAILED.
	 *
	 * @param iface			Interface string.
	 * @param ret			Optional pointer to store return code.
	 * @return				Generic pointer to the interface, or NULL if not
	 * 						found.
	 */
	virtual void* OnFileSystemQuery(const char* iface, int* ret)
	{
		if (ret)
		{
			*ret = META_IFACE_FAILED;
		}

		return NULL;
	}

	/**
	 * @brief Called when the server DLL's factory is used through
	 * Metamod:Source's wrapper.  This can be used to provide interfaces to
	 * other plugins.
	 *
	 * If ret is passed, you should fill it with META_IFACE_OK or META_IFACE_FAILED.
	 *
	 * @param iface			Interface string.
	 * @param ret			Optional pointer to store return code.
	 * @return				Generic pointer to the interface, or NULL if not
	 * 						found.
	 */
	virtual void* OnGameDLLQuery(const char* iface, int* ret)
	{
		if (ret)
		{
			*ret = META_IFACE_FAILED;
		}

		return NULL;
	}

	/**
	 * @brief Called when Metamod's own factory is invoked.
	 * This can be used to provide interfaces to other plugins.
	 *
	 * If ret is passed, you should fill it with META_IFACE_OK or META_IFACE_FAILED.
	 *
	 * @param iface			Interface string.
	 * @param ret			Optional pointer to store return code.
	 * @return				Generic pointer to the interface, or NULL if not
	 * 						found.
	 */
	virtual void* OnMetamodQuery(const char* iface, int* ret)
	{
		if (ret)
		{
			*ret = META_IFACE_FAILED;
		}

		return NULL;
	}

	/**
	 * @brief Called when Metamod:Source acquires a valid
	 * IServerPluginCallbacks pointer to be used for hooking by plugins.
	 *
	 * This will only be called after a call to ISmmAPI::EnableVSPListener().
	 * If called before GameInit, this callback will occur before LevelInit.
	 * Otherwise, it will be called on the first call after that.
	 *
	 * This callback is provided to all plugins regardless of which (or how
	 * many) called EnableVSPListener(), but only if at least one did in
	 * fact enable it, and only once for all plugins.  That is, a late
	 * loading plugin should use ISmmAPI::GetVSPInfo() before relying on
	 * this callback.
	 *
	 * This callback is never called if Metamod:Source is in VSP mode.
	 * If in VSP mode, a VSP instance is automatically and always available
	 * via ISmmAPI::GetVSPInfo(), which should be called anyway (to handle
	 * late loading cases).
	 *
	 * @param iface			Interface pointer.  If NULL, then the VSP
	 * 						listening construct failed to initialize and
	 * 						is not available.
	 */
	virtual void OnVSPListening(IServerPluginCallbacks* iface)
	{
	}

	/**
	 * @brief Called when Metamod:Source is about to remove a concommand or
	 * convar.  This can also be called if ISmmAPI::UnregisterConCmdBase is
	 * used by a plugin.
	 *
	 * @param id			Id of the plugin that created the concommand or
	 * 						convar.
	 * @param pCommand		Pointer to concommand or convar that is being
	 * 						removed.
	 */
	virtual void OnUnlinkConCommandBase(PluginId id, ConCommandBase* pCommand)
	{
	}
};

/**
 * @brief Used to uniquely identify plugins.
 * 
 * @note Should it be std::size_t?
 */
using PluginId = int;

constexpr auto METAMOD_FAIL_API_V1 = 7; // Minimum API version to detect for V1
constexpr auto METAMOD_FAIL_API_V2 = 14; // Minimum API version to detect for V2

/**
 * Use this to instantiate a plugin that will always fail.
 * This class definition works against major API versions 1 and 2.
 */
class ISmmFailPlugin
{
public:
	/**
	 * @brief You must return METAMOD_FAIL_API_V1 or METAMOD_FAIL_API_V2 here,
	 * depending on which Metamod:Source version you detected.
	 */
	virtual int GetApiVersion() = 0;

	/**
	 * @brief Do not change.
	 */
	virtual ~ISmmFailPlugin()
	{
	}

	/**
	 * @brief Return false here -- fill in the error buffer appropriately.
	 *
	 * Do not ever return true.  If you do, MM:S will crash because the class layout is
	 * incomplete against ISmmPlugin.
	 *
	 * @param id			Ignore.
	 * @param ismm			Ignore.
	 * @param error			Error buffer (must be filled).
	 * @param maxlength		Maximum size of error buffer.
	 * @param late			Ignore.
	 * @return				Must return false.
	 */
	virtual bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlength, bool late) = 0;
};

/**
 * @brief Contains version information.
 */
struct MetamodVersionInfo
{
	int api_major;			/**< ISmmAPI major version */
	int api_minor;			/**< ISmmAPI minor version */
	int sh_iface;			/**< SourceHook interface version */
	int sh_impl;			/**< SourceHook implementation version */
	int pl_min;				/**< Plugin API minimum version */
	int pl_max;				/**< Plugin API maximum version */
	int source_engine;		/**< Source Engine version (SOURCE_* constants) */
	const char* game_dir;	/**< Game directory name */

	/**
	 * @brief Returns the game folder.
	 *
	 * @return      Game folder, or NULL if not available on this version
	 *              of Metamod:Source.
	 */
	inline const char* GetGameDir() const
	{
		if (pl_max < 15)
			return NULL;
		return game_dir;
	}
};

/**
 * @brief Contains information about loading a plugin.
 */
struct MetamodLoaderInfo
{
	const char* pl_file;	/**< File path to the plugin being loaded. */
	const char* pl_path;	/**< Folder path containing the plugin. */
};

/**
 * @brief If a function of this type is exposed as "CreateInterface_MMS", then
 * Metamod:Source will attempt to call this function before calling
 * CreateInterface.  If this function returns a valid ISmmPlugin pointer, then
 * CreateInterface will not be called.
 *
 * This is useful for implementing a mini-loader plugin for multiple versions.
 *
 * @param mvi				MetamodVersionInfo structure.
 * @param mli				MetamodLoaderInfo structure.
 * @return					ISmmAPI pointer, or NULL if none.
 */
typedef ISmmPlugin* (*METAMOD_FN_LOAD)(const MetamodVersionInfo* mvi,
	const MetamodLoaderInfo* mli);

/**
 * @brief If a function of this type is exposed as "UnloadInterface_MMS", then
 * Metamod:Source will attempt to call this function after calling
 * ISmmAPI::Unload(), and before closing the library.  This lets loader plugins
 * clean up before exiting.
 *
 * Note: This function will be ignored unless CreateInterfce_MMS was exposed.
 * It may be called even if ISmmAPI::Unload() could not be called.
 */
typedef void (*METAMOD_FN_UNLOAD)();

/**
 * @brief Original type of load function.  CreateInterfaceFn from Valve.
 *
 * Plugins will expose this as "CreateInterface".
 */
typedef void* (*METAMOD_FN_ORIG_LOAD)(const char*, int*);

NAMESPACE_METAMOD_END
