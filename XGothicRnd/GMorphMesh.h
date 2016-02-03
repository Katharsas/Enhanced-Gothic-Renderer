#pragma once
#include "GVisual.h"

class GMaterial;
class zCMaterial;

class GMorphMesh :
	public GVisual
{
public:
	GMorphMesh(zCVisual* sourceObject);
	~GMorphMesh();

	struct SubMesh
	{
		GMaterial* m_Material;
		zCMaterial** m_SourceMaterial;
		unsigned int m_SubMeshIndexStart;
		unsigned int m_NumIndices;
	};

	/** Updates the statecache of the given drawable. If the drawable wasn't registered in
	the statecache before, will create a new cache entry and return a new pointer to the
	drawables state-cache. Otherwise the old pointer will be returned. */
	virtual StateCache* UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage);

	/** Creates a drawable for this visual */
	virtual void CreateDrawables(std::vector<GBaseDrawable*>& v, int lodLevel);

	/** Called when a drawable got drawn. Only if the InformVisual-Flag is set on it, though! */
	virtual void OnDrawableDrawn(GBaseDrawable* drawable);

	/** Updates the textures for the created pipelinestates */
	void UpdateTextures();

	/** Caches the textures used by this visual */
	virtual void CacheTextures(bool force = false);
private:
	// Logical buffers for rendering
	RAPI::RBuffer* m_VertexBuffer;
	RAPI::RBuffer* m_IndexBuffer;

	std::vector<SubMesh> m_SubMeshes;
};

