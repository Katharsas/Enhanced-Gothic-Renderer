#include "pch.h"
#include "GBaseDrawable.h"

static std::set<GBaseDrawable*> g_Test;

GBaseDrawable::GBaseDrawable(GVisual* sourceVisual, bool informVisual, bool drawAlways)
{
	m_SourceVisual = sourceVisual;
	m_InformVisual = informVisual;
	m_DrawAlways = drawAlways;
	memset(m_StateCaches, 0, sizeof(m_StateCaches));
	m_SupportInstancing = false;
	m_SourceVisual->RegisterDrawable(this);

	g_Test.insert(this);
}


GBaseDrawable::~GBaseDrawable(void)
{
	g_Test.erase(this);

	static int s_test = 0;
	s_test++;

	if(s_test == 4)
		sinf(1.0f);

	//LogInfo() << "Deleting drawable at 0x" << this;

	if(m_HomeVob)
	{
		LogWarn() << "Drawable deleted from visual!";
	}

	m_SourceVisual->UnregisterDrawable(this);
}

/** Queries the visual for new pipeline states */
void GBaseDrawable::ReaquireStateCache()
{
	// Make new states for this
	GVisual::StateCache* cache = m_SourceVisual->UpdatePipelineStatesFor(this, GConstants::ERenderStage::RS_WORLD);

	// No memleak here, the cachepointer will stay the same if this isn't the first call
	m_StateCaches[GConstants::ERenderStage::RS_WORLD] = cache;
}

/** Pushes the cached pipeline-states to the renderer */
void GBaseDrawable::PushRenderStateCache(GConstants::ERenderStage stage, RRenderQueueID queue, unsigned int instanceIndex, unsigned int numInstances)
{
	if (m_StateCaches[stage])
	{
		// Get cache for the current stage
		GVisual::StateCache& cache = *m_StateCaches[stage];

		// Modify instance index
		for (RPipelineState* s : cache.PipelineStates)
		{
			s->StartInstanceOffset = instanceIndex;
			s->NumInstances = numInstances;
		}

		// Push the states into the current renderqueue
		for (unsigned int i = 0; i < cache.PipelineStates.size(); i++)
		{
			cache.PipelineStates[i]->source = this;
			cache.PipelineStates[i]->Locked = true;
			REngine::RenderingDevice->QueuePipelineState(cache.PipelineStates[i], queue);

#ifndef PUBLIC_RELASE
			// Helps to find broken pipeline-states
			REngine::RenderingDevice->__GetRenderQueueByID(queue)->Sources.push_back(this);
#endif
		}
	}
}

/** Sets a new instance info for this drawable */
void GBaseDrawable::SetInstanceInfo(const VobInstanceInfo& instance)
{
	m_InstanceInfo = instance;
}