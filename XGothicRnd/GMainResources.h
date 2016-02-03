#pragma once
#include "VertexTypes.h"
#include <RPagedBuffer.h>

/**
 * Contains the main resources needed to render the game, like basic shaders and buffer collections
 */

namespace RAPI
{
	class RTextureAtlas;
}

class GVisual;
class zCVisual;
class GMainResources
{
public:
	GMainResources(void);
	~GMainResources(void);

	/** Creates the right type of visual from the source */
	GVisual* CreateVisualFrom(zCVisual* sourceVisual);

	RAPI::RPagedBuffer<ExTVertexStruct>* GetExPagedVertexBuffer() { return &m_ExPagedVertexBuffer; }
	RAPI::RPagedBuffer<ExTSkelVertexStruct>* GetExSkelPagedVertexBuffer() { return &m_ExSkelPagedVertexBuffer; }
	RAPI::RPagedBuffer<unsigned int>* GetExPagedIndexBuffer(){return &m_ExPagedIndexBuffer;}
	RAPI::RPagedBuffer<unsigned int>* GetExSkelPagedIndexBuffer() { return &m_ExSkelPagedIndexBuffer; }
	RAPI::RBuffer* GetVobInstanceBuffer(){return m_VobInstanceBuffer;}

	// Returns the atlas for lightmaps of the given size
	RAPI::RTextureAtlas* GetLightmapAtlas(const INT2& singleTextureSize);

	// Constructs all lightmap atlases
	void ConstructLightmapAtlases();

	// Deletes all lightmap atlases
	void ClearLightmapAtlases();

private:
	// Paged buffer for ExTVertexStructs and indices
	RAPI::RPagedBuffer<ExTVertexStruct> m_ExPagedVertexBuffer;
	RAPI::RPagedBuffer<ExTSkelVertexStruct> m_ExSkelPagedVertexBuffer;
	RAPI::RPagedBuffer<unsigned int> m_ExPagedIndexBuffer;
	RAPI::RPagedBuffer<unsigned int> m_ExSkelPagedIndexBuffer;
	RAPI::RBuffer* m_VobInstanceBuffer;

	// Map of single-texture sizes and textureatlases
	std::map<std::pair<size_t,size_t>, RAPI::RTextureAtlas*> m_LightmapAtlasCache;
};

