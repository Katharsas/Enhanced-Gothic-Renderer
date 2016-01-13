#pragma once

namespace GConstants
{
	namespace PipelineStates
	{
		static const char* BPS_WORLDMESH = "__BPS_WORLDMESH";
		static const char* BPS_INSTANCED_VOB = "__BPS_INSTANCED_VOB";
		static const char* BPS_INSTANCED_VOB_INVENTORY = "__BPS_INSTANCED_VOB_INVENTORY";
		static const char* BPS_PARTICLES = "__BPS_PARTICLES";
		static const char* BPS_SKEL_MESH = "__BPS_SKEL_MESH";
	};

	namespace ConstantBuffers
	{
		static const char* CB_WORLDPERFRAME = "__CB_WORLDPERFRAME";
	};

	namespace ShaderAliases
	{
		static const char* PS_DEFAULT_WORLD = "PS_World";
		static const char* PS_MASKED_WORLD = "PS_World_Masked";
		//static const char* PS_MASKED_WORLD = "PS_Simple";
	}

	enum ERenderStage
	{
		RS_WORLD,
		RS_SHADOW_SUN,
		RS_INVENTORY,
		RS_NUM_STAGES,
		RS_UNDEFINED // Special stage, which is never rendered, but causes all other stages to be updated for example
	};
};