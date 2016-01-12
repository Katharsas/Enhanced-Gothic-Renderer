#pragma once
#include "pch.h"
#include "zDefinitions.h"
#include "zEngineFunctions.h"
#include "zSTRING.h"
#include "zCObject.h"
#include "zCMeshSoftSkin.h"

class zCVisual;
class zCMeshSoftSkin;
class zCTexture;
struct zCModelNode;
class zCModelMeshLib;

/** Handles animations like blinking eyes */
struct zCModelTexAniState
{
	void UpdateTexList()
	{
		XCALL(MemoryLocations::Gothic::zCModelTexAniState__UpdateTexList_void);
	}

	int	m_NumNodeTex;
	zCTexture** m_NodeTexList;
	int	m_ActAniFrames[8];
};

/** One joint in the models skeleton */
struct zCModelNodeInst
{
	zCModelNodeInst* m_ParentNode;
	zCModelNode* m_ProtoNode;

	// If this is not nullptr, it can hold something like a weapon
	zCVisual* m_NodeVisual;

	// Current transform of this bone
	Matrix m_Trafo;

	// Obsolete
	Matrix m_TrafoObjToCam;

	// Same as visuals bbox maybe?
	zTBBox3D m_BBox3D;

	zCModelTexAniState m_TexAniState;
};


/** One joint in the models skeleton. These are used by the prototypes? */
struct zCModelNode
{
	zCModelNode* m_ParentNode;

	// Name of the joint
	zSTRING	m_NodeName;

	// If this is not nullptr, it can hold something like a weapon
	zCVisual* m_Visual;

	// Current transform of this bone
	Matrix m_Transform;

	// Useful for ragdolls, maybe?
	float3 m_NodeRotAxis;
	float m_NodeRotAngle;

	float3 m_Translation;

	Matrix m_TransformObjToWorld;
	Matrix* m_NodeTransformList;
	zCModelNodeInst* m_LastInstNode;
};

/** Attachment of a vob to a node */
struct zTMdl_NodeVobAttachment
{
	zCVob* m_Vob;
	zCModelNodeInst* m_NodeInst;
};


struct zTMeshLibEntry
{
	zCModelTexAniState TexAniState;
	zCModelMeshLib* MeshLibPtr;
};

class zCModelMeshLib : public zCObject
{
public:
	/** This should be already in the nodes? */
	struct zTNodeMesh
	{
		zCVisual* Visual;
		int NodeIndex;
	};



	/** This returns the list of nodes which hold information about the bones and attachments later */
	zCArray<zTNodeMesh>* GetNodeList()
	{
		return &m_NodeList;
	}

	/** Returns the list of meshes which store the vertex-positions and weights */
	zCArray<zCMeshSoftSkin *>* GetMeshSoftSkinList()
	{
		return &m_SoftSkinList;
	}

	/** Returns the name of the visual. Can't use the objects name because this can have more than one meshes 
		Only getting the name of the first one for now. */
	const char* GetVisualName()
	{
		if (GetMeshSoftSkinList()->NumInArray > 0)
			return m_SoftSkinList.Array[0]->GetObjectName();

		return "";
		//return __GetVisualName().ToChar();
	}

private:
	zCArray<zTNodeMesh>	m_NodeList;
	zCArray<zCMeshSoftSkin*> m_SoftSkinList;
};