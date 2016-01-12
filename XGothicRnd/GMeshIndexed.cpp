#include "pch.h"
#include "GMeshIndexed.h"


GMeshIndexed::GMeshIndexed(RBuffer* meshVertexBuffer, 
						   RBuffer* meshIndexBuffer,
						   unsigned int numIndices,
						   unsigned int meshVertexStart,
						   unsigned int meshIndexStart)
{
	m_MeshVertexBuffer = meshVertexBuffer;
	m_MeshIndexBuffer = meshIndexBuffer;
	m_NumIndices = numIndices;
	m_MeshVertexStart = meshVertexStart;
	m_MeshIndexStart = meshIndexStart;
}


GMeshIndexed::~GMeshIndexed(void)
{
}
