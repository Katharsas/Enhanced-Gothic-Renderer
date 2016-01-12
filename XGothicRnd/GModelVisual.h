#pragma once
#include "GVisual.h"
#include "GMeshIndexed.h"
#include <RPagedBuffer.h>
#include "VertexTypes.h"
#include <array>

// Max number of nodes a skeletal can have. Probably split this. Only dragons have this many nodes!
static const int NUM_MAX_NODES = 96;

struct GModelConstantBuffer
{
	float M_Fatness;
	float M_Scale;
	float2 M_Pad1;
	Matrix M_NodeTransforms[NUM_MAX_NODES];
};

class GMeshSoftSkin;
class GMaterial;
class GMeshSoftSkinDrawable;
class GModelVisual :
	public GVisual
{
public:

	GModelVisual(zCVisual* sourceObject);
	~GModelVisual();

	/** Updates the statecache of the given drawable. If the drawable wasn't registered in
	the statecache before, will create a new cache entry and return a new pointer to the
	drawables state-cache. Otherwise the old pointer will be returned. */
	virtual StateCache* UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage);

	/** Creates a drawable for this visual */
	virtual void CreateDrawables(std::vector<GBaseDrawable*>& v);

	/** Called when a drawable got drawn. Only if the InformVisual-Flag is set on it, though! */
	virtual void OnDrawableDrawn(GBaseDrawable* drawable);

protected:

	/** Updates the models textures and reaquires the caches if they changed */
	void UpdateTextures();

	/** Updates the node-transforms and constant-data for this model */
	void UpdateModel();

	/** Refreshes the attachments, if needed. If not, only updates the matrices of them */
	void RefreshAttachments();

	struct SubMesh
	{
		GMeshSoftSkin* m_MeshSoftSkin;
		std::vector<GMeshSoftSkinDrawable*> m_Drawables;
	};

	// All softskin-meshes
	std::vector<SubMesh> m_SubMeshes;

	// Matrices of the skeletal nodes
	Matrix m_NodeTransforms[NUM_MAX_NODES];

	struct NodeAttachment
	{
		NodeAttachment(struct zCModelNodeInst* source)
		{
			m_SourceNode = source;
			m_Visual = nullptr;
		}

		// Original node instance
		struct zCModelNodeInst* m_SourceNode;

		// Visual attached to this node
		GVisual* m_Visual;

		// Drawables from the visual
		std::vector<GBaseDrawable*> m_NodeDrawables;
	};

	// All attachments to this model
	std::vector<NodeAttachment> m_NodeAttachments;

	// Constantbuffer for this single model. Constains general data and node-transforms
	RBuffer* m_ModelConstantBuffer;

	// Last frame this was drawn
	unsigned int m_LastFrameDrawn;

	// We need to update the buffer at least once after going idle
	// to get all of the last changes. This bool stores if we did so.
	bool m_UpdatedAfterIdle;

	// Current state of the animated textures, since every zCModel sets their own
	// animation frame just before rendering
	std::vector<std::array<int, 8>> m_StoredAniTextures;
};

