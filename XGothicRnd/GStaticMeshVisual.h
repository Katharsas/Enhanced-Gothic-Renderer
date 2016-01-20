#pragma once
#include "gvisual.h"
#include "GMeshIndexed.h"
#include <RPagedBuffer.h>
#include "VertexTypes.h"
#include "GConstants.h"

class GMaterial;
class zCMaterial;

class GStaticMeshVisual :
	public GVisual, public IObserver
{
public:
	GStaticMeshVisual(zCVisual* sourceObject);
	~GStaticMeshVisual(void);

	struct SubMesh
	{
		GMaterial* m_Material;
		zCMaterial** m_SourceMaterial;
		unsigned int m_SubMeshIndexStart;
		unsigned int m_NumIndices;
	};

	/**
	 * Called when the paged index buffer got rebuilt
	 */
	virtual void OnNotified(size_t id, void* userData);

	/** Updates the statecache of the given drawable. If the drawable wasn't registered in
		the statecache before, will create a new cache entry and return a new pointer to the
		drawables state-cache. Otherwise the old pointer will be returned. */
	virtual StateCache* UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage);

	/** Creates a drawable for this visual */
	virtual void CreateDrawables(std::vector<GBaseDrawable*>& v, int lodLevel);

	/** Caches the textures used by this visual */
	virtual void CacheTextures(bool force = false);
protected:
	// Logical buffers for rendering
	RLogicalBuffer<ExTVertexStruct>* m_LogicalVertexBuffer;
	RLogicalBuffer<unsigned int>* m_LogicalIndexBuffer;

	std::vector<SubMesh> m_SubMeshes;
};

