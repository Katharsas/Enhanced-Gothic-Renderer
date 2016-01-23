#include "pch.h"
#include "GModelVisual.h"
#include "GMaterial.h"
#include "Engine.h"
#include "GGame.h"
#include "GMainResources.h"
#include "zCModel.h"
#include "GModelDrawable.h"
#include "GMeshSoftSkin.h"
#include "GMeshSoftSkinDrawable.h"
#include "zCVob.h"
#include "GVobObject.h"
#include "GWorld.h"

GModelVisual::GModelVisual(zCVisual* sourceObject) : GVisual(sourceObject)
{
	// This must be a zCModel.
	zCModel* model = (zCModel *)sourceObject;
	
	// Get our submeshes, iterate over meshlibs
	for (unsigned int l = 0; l < model->GetNumModelMeshLibs(); l++)
	{
		zCModelMeshLib* lib = model->GetMeshLibArray()[l]->m_MeshLib;

		// Get the SkelMeshes from the lib
		for (unsigned int s = 0; s < lib->GetMeshSoftSkinList()->GetSize(); s++)
		{
			zCMeshSoftSkin* zMesh = lib->GetMeshSoftSkinList()->Array[s];
			GMeshSoftSkin* gMesh = (GMeshSoftSkin*)GMeshSoftSkin::QueryFromSource(zMesh);

			GASSERT(gMesh, "ModelMeshLib does not seem to be initialized");

			m_SubMeshes.push_back(SubMesh());
			m_SubMeshes.back().m_MeshSoftSkin = gMesh;

			// Assign name of the first mesh to this model, so we have an idea what this is
			if(m_FileName.empty())
				m_FileName = zMesh->GetObjectName();
		}
	}

	// Recreate nodelist
	for(unsigned int i = 0; i < model->GetNumNodes(); i++)
	{
		zCModelNodeInst* node = model->GetNodeList().Array[i];
		m_NodeAttachments.push_back(NodeAttachment(node));

		// Assign name of the first mesh to this model, so we have an idea what this is
		if (m_FileName.empty() && node->m_NodeVisual)
			m_FileName = node->m_NodeVisual->GetObjectName();
	}

	// Subtract as many matrices as there are too much
	unsigned int actualCBSize = sizeof(GModelConstantBuffer) - sizeof(Matrix) * (NUM_MAX_NODES - model->GetNumNodes());

	// Create CB
	m_ModelConstantBuffer = REngine::ResourceCache->CreateResource<RBuffer>();
	m_ModelConstantBuffer->Init(nullptr,
		actualCBSize,
		actualCBSize,
		EBindFlags::B_CONSTANTBUFFER,
		EUsageFlags::U_DYNAMIC,
		ECPUAccessFlags::CA_WRITE,
		"Model CB");

	// Sanity
	if (model->GetNumNodes() > NUM_MAX_NODES)
		if(!m_SubMeshes.empty())
			LogWarn() << "zCModel '" << m_SubMeshes[0].m_MeshSoftSkin->GetSourceObject()->GetObjectName() << "' has more than " << NUM_MAX_NODES << " nodes! (" << model->GetNumNodes() << ")";
		else
			LogWarn() << "zCModel (No softskins) has more than " << NUM_MAX_NODES << " nodes! (" << model->GetNumNodes() << ")";

	m_LastFrameDrawn = 0;
	m_UpdatedAfterIdle = false;
}


GModelVisual::~GModelVisual(void)
{
	REngine::ResourceCache->DeleteResource(m_ModelConstantBuffer);

	GASSERT(GVisual::QueryFromSource((zCVisual*)m_SourceObject) == this, "Deleted zCModel had different pointer in cache!");



	/*zCModel* model = (zCModel *)m_SourceObject;
	GVobObject* vob = GVobObject::QueryFromSource(model->GetHomeVob());


	if(vob)
	{
		LogWarn() << "GModelVisual deleted but vob still active!";
	}

	if(Engine::Game->GetActiveWorld()->RemoveVob(model->GetHomeVob()))
	{
		//LogWarn() << "Vob of deleted Model-Visual was still in the world!";
	}*/
}


/** Updates the statecache of the given drawable. If the drawable wasn't registered in
the statecache before, will create a new cache entry and return a new pointer to the
drawables state-cache. Otherwise the old pointer will be returned. */
GVisual::StateCache* GModelVisual::UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage)
{
	// The zCModel itself has no states, it's only a container for other visuals
	return nullptr;
}

/** Creates a drawable for this visual */
void GModelVisual::CreateDrawables(std::vector<GBaseDrawable*>& v, int lodLevel)
{
	zCModel* model = (zCModel *)m_SourceObject;

	//float lodRange = (float)lodLevel / (float)NUM_VISUAL_LOD_LEVELS;

	v.push_back(new GModelDrawable(this));

	if(!model->GetNumModelMeshLibs() && !m_SubMeshes.empty())
	{
		LogError() << "(" << m_FileName << ")" << " ModelMeshLib empty, but m_SubMeshes is not!";
		return;
	}

	// Also create drawables for all our softskins and node attachments
	for (SubMesh& s : m_SubMeshes)
	{
		// Need to clear this, in case this was not the first time of this running
		//Toolbox::DeleteElements(s.m_Drawables);
		s.m_Drawables[lodLevel].clear();

		// TODO: Ugly hack, maybe should template CreateDrawables to avoid that ugly cast here
		// We need to store them in the visual as well, so we can set the constant-buffer!
		s.m_MeshSoftSkin->CreateDrawables(*(std::vector<GBaseDrawable*>*)&s.m_Drawables[lodLevel], lodLevel);

		for (GMeshSoftSkinDrawable* d : s.m_Drawables[lodLevel])
		{
			d->SetBoneMatricesBuffer(m_ModelConstantBuffer);
			v.push_back(d);
		}
	}


	// Create drawables for node attachments
	for(NodeAttachment& a : m_NodeAttachments)
	{
		// Memory freed by the vob
		a.m_NodeDrawables[lodLevel].clear();

		if(a.m_Visual)
			a.m_Visual->CreateDrawables(a.m_NodeDrawables[lodLevel], lodLevel);

		v.insert(v.end(), a.m_NodeDrawables[lodLevel].begin(), a.m_NodeDrawables[lodLevel].end());
	}
}

/** Called when a drawable got drawn. Only if the InformVisual-Flag is set on it, though! */
void GModelVisual::OnDrawableDrawn(GBaseDrawable* drawable)
{
	// Since each zCModel has exactly one zCVob it operates on, this means that
	// our vob just has been queued for rendering.

	zCModel* model = (zCModel *)m_SourceObject;

	if(!model->GetHomeVob())
	{
		LogWarn() << "Model without HomeVob detected!";
		return;
	}

	if(m_LastFrameDrawn != REngine::RenderingDevice->GetFrameCounter()/* && (!model->IsInIdle() || !m_UpdatedAfterIdle)*/)
	{
		// Get new model-state onto the GPU
		UpdateModel();

		// Update attachments
		RefreshAttachments();

		// TODO: Actual value
		model->SetModelDistanceToCam(0.0f);

		m_LastFrameDrawn = REngine::RenderingDevice->GetFrameCounter();
		m_UpdatedAfterIdle = model->IsInIdle();
	}
}

/** Updates the node-transforms and constant-data for this model */
void GModelVisual::UpdateModel()
{

	zCModel* model = (zCModel *)m_SourceObject;



	// Update textures and vob attachments
	model->UpdateAttachedVobs();
	model->UpdateMeshLibTexAniState();
	UpdateTextures();
	
	static std::vector<Matrix> s_tmp;
	s_tmp.resize(model->GetNumNodes());

	model->GetNodeTransforms(&s_tmp[0]);

	// TODO: There may is a faster way of doing this
	// Only update the buffer if the transforms changed
	if(memcmp(&s_tmp[0], m_NodeTransforms, sizeof(Matrix) * model->GetNumNodes()) != 0)
	{
		memcpy(m_NodeTransforms, &s_tmp[0], sizeof(Matrix) * model->GetNumNodes());

		// Get new node-states
		GModelConstantBuffer* data;
		LEB(m_ModelConstantBuffer->Map((void**)&data));

		data->M_Fatness = model->GetModelFatness();
		data->M_Scale = model->GetModelScale();

		// Copy data for access at the attachments
		memcpy(data->M_NodeTransforms, m_NodeTransforms, sizeof(Matrix) * model->GetNumNodes());

		LEB(m_ModelConstantBuffer->Unmap());
	}
}

/** Refreshes the attachments, if needed. If not, only updates the matrices of them */
void GModelVisual::RefreshAttachments()
{
	zCModel* model = (zCModel*)m_SourceObject;

	// Gothic switches these sometimes.
	// TODO: Find hook-point for this!
	for(unsigned int j=0;j<m_NodeAttachments.size();j++)
	{
		if(m_NodeAttachments[j].m_SourceNode != model->GetNodeList().Array[j])
		{
			m_NodeAttachments.clear();

			// Recreate nodelist
			for(unsigned int i = 0; i < model->GetNumNodes(); i++)
			{
				zCModelNodeInst* node = model->GetNodeList().Array[i];
				m_NodeAttachments.push_back(NodeAttachment(node));
			}

			break;
		}
	}

	// Check if all attachments are still the same
	bool outdated = false;
	for (NodeAttachment& a : m_NodeAttachments)
	{
		zCVisual* stored = a.m_Visual ? a.m_Visual->GetSourceObject() : nullptr;
		zCVisual* actual = a.m_SourceNode->m_NodeVisual;

		// Check for change
		if (stored != actual)
		{
			GVisual* vis = GVisual::QueryFromSource(a.m_SourceNode->m_NodeVisual);

			// Create this visual, if it hasn't been used before //TODO: Shouldn't this be done at load time?
			if(!vis && actual)
			{
				vis = (GModelVisual*)GVisual::CreateExtensionVisual(a.m_SourceNode->m_NodeVisual);
				//vis = Engine::Game->GetMainResources()->CreateVisualFrom(a.m_SourceNode->m_NodeVisual);

				// Unimplemented visual?
				if(!vis)
				{
					a.m_Visual = nullptr;
					continue;
				}
			}

			if(a.m_Visual)
				a.m_Visual->RemoveVob(model->GetHomeVob()->GetVobObject());

			// Update the visual
			a.m_Visual = a.m_SourceNode->m_NodeVisual
				? vis
				: nullptr;

			if(a.m_Visual)
				a.m_Visual->AddVob(model->GetHomeVob()->GetVobObject());

			// Mark outdated, so we can update the underlaying vob after updating all visuals
			outdated = true;
		}
	}


	GVobObject* vobj = model->GetHomeVob()->GetVobObject();

	if(!vobj)
	{
		// TODO: GetVobObject() returns even though the vob should have been set!
		vobj = GVobObject::QueryFromSource(model->GetHomeVob());
		if(!vobj)
			return; // Don't even need to continue here.
		else
			model->GetHomeVob()->SetVobObject(vobj);
	}

	// Update the underlaying vob if needed
	if (outdated)
	{
		vobj->ReaquireDrawables();
	}

	// TODO: Check if the nodes ever move (Benches/chairs for example)

	// Now update the transforms of the attachments.
	const Matrix& world = vobj->GetWorldMatrix();
	int i=0;
	for (NodeAttachment& a : m_NodeAttachments)
	{
		if (a.m_Visual)
		{
			for(int l = 0; l < NUM_VISUAL_LOD_LEVELS; l++)
			{
				for(GBaseDrawable* b : a.m_NodeDrawables[l])
				{
					Matrix nodeWorld = (world * m_NodeTransforms[i]);

					// Update transforms of the existing instance info
					VobInstanceInfo vi = vobj->GetInstanceInfo();
					vi.m_Position = nodeWorld.TranslationT();
					vi.m_Rotation = Quaternion::CreateFromRotationMatrix(nodeWorld);

					b->SetInstanceInfo(vi);
				}
			}
		}

		i++;
	}
}

/** Updates the models textures and reaquires the caches if they changed */
void GModelVisual::UpdateTextures()
{
	zCModel* model = (zCModel*)m_SourceObject;

	m_StoredAniTextures.resize(model->GetNumModelMeshLibs());

	// Update textures on the submeshes
	for(unsigned int i=0;i<model->GetNumModelMeshLibs();i++)
	{
		// Check if at least one animationframe changed
		if(memcmp(m_StoredAniTextures[i].data(), model->GetMeshLibArray()[i]->m_TexAniState.m_ActAniFrames, sizeof(int) * 8) != 0)
		{
			// Get new state
			memcpy(m_StoredAniTextures[i].data(), model->GetMeshLibArray()[i]->m_TexAniState.m_ActAniFrames, sizeof(int) * 8);

			// Update all drawables
			for(auto& s : m_SubMeshes)
			{
				for(int l = 0; l < NUM_VISUAL_LOD_LEVELS; l++)
				{
					for(auto& d : s.m_Drawables[l])
						d->ReaquireStateCache();
				}
			}

			return;
		}
	}

}