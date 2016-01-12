#pragma once
#include <RResourceCache.h>
#include <REngine.h>
#include "zCObject.h"
#include "Engine.h"
#include "GGame.h"
#include "../Shared/hwbrk.h"

class GObject;

template<typename T, typename S>
class GzObjectExtension
{
public:

	GzObjectExtension(T* sourceObject)
	{
		m_SourceObject = sourceObject;
		m_Invalid = false;
	}

	virtual ~GzObjectExtension(void)
	{
		RemoveFromCaches();
	}

	/** Queue this into the safe-functions queue, since we never know when the zEngine 
		wants to delete this object. Just break all references and delete it later. */
	void DeleteSafe()
	{
		// Break the references immediately, so new objects can be created using the same source-object
		RemoveFromCaches();

		// Delete the memory to this when we are in a safe place
		Engine::Game->QueueSafeFunction([this]()
		{
			delete this;
		});
	}

	/** Removes this object from the caches holding a reference from the source-object to this. Use with caution. */
	void RemoveFromCaches()
	{
		REngine::ResourceCache->RemoveFromCache<S>(Toolbox::HashObject(m_SourceObject));
		REngine::ResourceCache->RemoveFromCache<GObject>(Toolbox::HashObject(m_SourceObject));
	}

	/** Returns the extension-object only if it was created before and won't
		create a new one if it hasn't been created yet */
	static S* QueryFromSource(T* source)
	{
		if(!source)
			return nullptr;

		// Check if we already got this in cache
		return REngine::ResourceCache->GetCachedObject<S>(Toolbox::HashObject(source));
	}

	/** Returns the GMaterial-Instance for the given source-material. Will create a new object
		if none has been found. */
	static S* GetFromSource(T* source)
	{
		if(!source)
			return nullptr;

		// Check if we already got this in cache
		S* obj = QueryFromSource(source);
	
		// Create new one if not cached
		if(!obj)
		{
			obj = new S(source);
			REngine::ResourceCache->AddToCache<S>(Toolbox::HashObject(source), obj);

			// Add to this type as well to have general access
			REngine::ResourceCache->AddToCache<GObject>(Toolbox::HashObject(source), (GObject*)obj);
		}

		return obj;
	}

	/**
	 * Returns the complete map of all generated extensions of the current type
	 */
	static const std::unordered_map<size_t, S*>& GetFullCacheMap()
	{
		// Unsafe cast here, but this is the only way to modify the pointer type and won't make problems.
		return *(std::unordered_map<size_t, S*>*)&REngine::ResourceCache->GetCacheMap<S>();
	}

	/**
	 * Returns the source-material of this wrapper
	 */
	T* GetSourceObject()const{return m_SourceObject;}
	void MarkInvalid(){m_Invalid = true;}
	bool IsInvalid(){return m_Invalid;}

protected:

	// Original object of the game
	T* m_SourceObject;

	// Name of the object, for debugging
	std::string m_ObjectName;

	// If true, this object is invalid, but maybe still bound somewhere
	// This is mostly for debugging purposes, but you never know when 
	// the zEngine deletes their objects
	bool m_Invalid;
};

// Simple extension-object for simple access to a general type
// Since this does not implement a destructor, it is save to call delete on this, no matter what
// type of G* it is
class GObject : public GzObjectExtension<zCObject, GObject>
{

};
