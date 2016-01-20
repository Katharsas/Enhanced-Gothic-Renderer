#pragma once
#include "VertexTypes.h"
#include <RPagedBuffer.h>

/**
 * Contains the main resources needed to render the game, like basic shaders and buffer collections
 */

class GVisual;
class RTextureAtlas;
class zCVisual;
class GMainResources
{
public:
	GMainResources(void);
	~GMainResources(void);

	/** Creates the right type of visual from the source */
	GVisual* CreateVisualFrom(zCVisual* sourceVisual);

	RPagedBuffer<ExTVertexStruct>* GetExPagedVertexBuffer() { return &m_ExPagedVertexBuffer; }
	RPagedBuffer<ExTSkelVertexStruct>* GetExSkelPagedVertexBuffer() { return &m_ExSkelPagedVertexBuffer; }
	RPagedBuffer<unsigned int>* GetExPagedIndexBuffer(){return &m_ExPagedIndexBuffer;}
	RPagedBuffer<unsigned int>* GetExSkelPagedIndexBuffer() { return &m_ExSkelPagedIndexBuffer; }
	RBuffer* GetVobInstanceBuffer(){return m_VobInstanceBuffer;}

	// Returns the atlas for lightmaps of the given size
	RTextureAtlas* GetLightmapAtlas(const INT2& singleTextureSize);

	// Constructs all lightmap atlases
	void ConstructLightmapAtlases();

	// Deletes all lightmap atlases
	void ClearLightmapAtlases();

private:
	// Paged buffer for ExTVertexStructs and indices
	RPagedBuffer<ExTVertexStruct> m_ExPagedVertexBuffer;
	RPagedBuffer<ExTSkelVertexStruct> m_ExSkelPagedVertexBuffer;
	RPagedBuffer<unsigned int> m_ExPagedIndexBuffer;
	RPagedBuffer<unsigned int> m_ExSkelPagedIndexBuffer;
	RBuffer* m_VobInstanceBuffer;

	// Map of single-texture sizes and textureatlases
	std::map<std::pair<size_t,size_t>, RTextureAtlas*> m_LightmapAtlasCache;
};

