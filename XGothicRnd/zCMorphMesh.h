#pragma once
#include "zCVisual.h"
#include "zCModel.h"

class zCProgMeshProto;
class zCMorphMesh : public zCVisualAnimate
{
public:
	/** Returns the underlaying static mesh */
	zCProgMeshProto* GetStaticMesh()
	{
		return m_MorphMesh;
	}

	/** Returns the texture animation state of this morph-mesh.
		This holds the specific texture for faces, blinking, etc */
	zCModelTexAniState* GetTexAniState()
	{
		return &m_TexAniState;
	}

	/** Updates the vertexpositions of this morphmesh */
	void CalcVertexPositions()
	{
		XCALL(MemoryLocations::Gothic::zCMorphMesh__CalcVertPositions_void);
	}

	/** Advances the animations */
	void AdvanceAnis()
	{
		XCALL(MemoryLocations::Gothic::zCMorphMesh__AdvanceAnis_void);
	}

private:
	class zCMorphMeshProto* m_MorphProto;
	zCProgMeshProto* m_MorphMesh;

	class zCMorphMeshAni* m_RefShapeAni;
	zCModelTexAniState m_TexAniState;
};