#pragma once
#include "RBaseShader.h"
#include "RPipelineState.h"
#include <sstream>

struct RPipelineStateFull
{
	RPipelineStateFull()
	{
		PixelShader = nullptr;
		VertexShader = nullptr;
		InputLayout = nullptr;
		RasterizerState = nullptr;
		SamplerState = nullptr;
		BlendState = nullptr;
		DepthStencilState = nullptr;
		memset(VertexBuffers, 0, sizeof(VertexBuffers));
		IndexBuffer = nullptr;
		Viewport = nullptr;

		NumDrawElements = 0;
		StartVertexOffset = 0;
		StartIndexOffset = 0;
		StartInstanceOffset = 0;
		NumInstances = 0;
	}

	class RPixelShader* PixelShader;
	class RVertexShader* VertexShader;
	class RInputLayout* InputLayout;
	std::vector<class RBuffer*> StructuredBuffers[EShaderType::ST_NUM_SHADER_TYPES];
	std::vector<class RTexture*> Textures[EShaderType::ST_NUM_SHADER_TYPES];
	std::vector<class RBuffer*> ConstantBuffers[EShaderType::ST_NUM_SHADER_TYPES];
	
	// States
	class RRasterizerState* RasterizerState;
	class RSamplerState* SamplerState;
	class RBlendState* BlendState;
	class RDepthStencilState* DepthStencilState;

	// Vertex info
	class RBuffer* VertexBuffers[2];
	class RBuffer* IndexBuffer;

	class RViewport* Viewport;

	unsigned int NumDrawElements; // Vertices, indices...
	unsigned int StartVertexOffset;
	unsigned int StartIndexOffset;
	unsigned int StartInstanceOffset;
	unsigned int NumInstances;
	union
	{
		RPipelineState::IDStruct BoundIDs;
		RPipelineState::KeyStruct BoundKey;
	};
};


class RPixelShader;
class RVertexShader;
class RTexture;
class RInputLayout;
class RBuffer;
class RRasterizerState;
class RSamplerState;
class RBlendState;
class RDepthStencilState;
class RStateMachine
{
public:

	RStateMachine(void);
	~RStateMachine(void);

	struct ChangesStruct
	{
		bool RasterizerState;
		bool BlendState;
		bool DepthStencilState;
		bool SamplerState;

		bool VertexBuffers[2];
		bool IndexBuffer;

		bool PixelShader;
		bool VertexShader;
		bool GeometryShader;
		bool HullShader;
		bool DomainShader;
		bool InputLayout;

		bool MainTexture;
		bool Viewport;

		bool DrawFunctionID;

		bool ConstantBuffers[EShaderType::ST_NUM_SHADER_TYPES];
		bool StructuredBuffers[EShaderType::ST_NUM_SHADER_TYPES];
	};

	struct ChangesCountStruct
	{
		int RasterizerState;
		int BlendState;
		int DepthStencilState;
		int SamplerState;
		int VertexBuffers[2];
		int IndexBuffer;
		int PixelShader;
		int VertexShader;
		int GeometryShader;
		int HullShader;
		int DomainShader;
		int InputLayout;
		int MainTexture;
		int Viewport;
		int DrawFunctionID;
		int ConstantBuffers[EShaderType::ST_NUM_SHADER_TYPES];
		int StructuredBuffers[EShaderType::ST_NUM_SHADER_TYPES];

		std::string ProduceString()
		{
			std::stringstream ss;
			ss << "RasterizerState: " << RasterizerState << "\n";
			ss << "BlendState: " << BlendState << "\n";
			ss << "DepthStencilState: " << DepthStencilState << "\n";
			ss << "SamplerState: " << SamplerState << "\n";
			ss << "VertexBuffers[0]: " << VertexBuffers[0] << "\n";
			ss << "VertexBuffers[1]: " << VertexBuffers[1] << "\n";
			ss << "IndexBuffer: " << IndexBuffer << "\n";
			ss << "PixelShader: " << PixelShader << "\n";
			ss << "VertexShader: " << VertexShader << "\n";
			ss << "GeometryShader: " << GeometryShader << "\n";
			ss << "HullShader: " << HullShader << "\n";
			ss << "DomainShader: " << DomainShader << "\n";
			ss << "InputLayout: " << InputLayout << "\n";
			ss << "MainTexture: " << MainTexture << "\n";
			ss << "Viewport: " << Viewport << "\n";
			ss << "DrawFunctionID: " << DrawFunctionID << "\n";
			ss << "ConstantBuffers: " << ConstantBuffers[ST_PIXEL] + ConstantBuffers[ST_VERTEX] << "\n";
			//ss << ": "					<<  << "\n";

			return ss.str();
		}
	};


	/**
	 * Collects all resources from the small state and sets them to the current full state
	 */
	void SetFromPipelineState(const struct RPipelineState& state);
	void SetFromPipelineState(const struct RPipelineState& state, const ChangesStruct& changes);


	/**
	 * Sets the values from a pipeline-state-object to the current state
	 */
	// TODO: RENAME THIS OR MAKE PRIVATE. Very confusing with SetFromPipelineState.
	void AssignPipelineStateValues(RPipelineState* state);

	/**
	 * Makes a drawcall-state. Unregister in Resource-Cache when done!
	 */
	RPipelineState* MakeDrawCall(unsigned int numVertices, unsigned int startVertexOffset=0);
	RPipelineState* MakeDrawCallIndexed(unsigned int numIndices, unsigned int startIndexOffset=0, unsigned int startVertexOffset=0);
	RPipelineState* MakeDrawCallIndexedInstanced(unsigned int numIndices, 
		unsigned int numInstances,
		unsigned int startIndexOffset=0, 
		unsigned int startVertexOffset=0,
		unsigned int startInstanceOffset=0);

	/**
	 * Returns the struct of changed values after the last SetFromPipelineState
	 */
	const ChangesStruct& GetChanges(){return Changes;}

	/**
	 * Returns the current state
	 */
	const RPipelineStateFull& GetCurrentState(){return State;}

	/** Returns the count of changes of states */
	ChangesCountStruct& GetChangesCounts(){return ChangesCount;}

	/**
	 * Invalidates the current state. Useful for when a new frame starts for example
	 */
	void Invalidate();

	/**
	 * Sets all changes back to false
	 */
	void ResetChanges();

	/**
	 * Setters 
	 */
	void SetPixelShader(RPixelShader* shader);
	void SetVertexShader(RVertexShader* shader);
	void SetInputLayout(RInputLayout* layout);
	void SetRasterizerState(RRasterizerState* state);
	void SetSamplerState(RSamplerState* state);
	void SetBlendState(RBlendState* state);
	void SetDepthStencilState(RDepthStencilState* state);

	void SetTexture(unsigned int slot, RTexture* texture, EShaderType stage);

	void SetConstantBuffer(unsigned int slot, RBuffer* buffer, EShaderType stage);

	void SetVertexBuffer(unsigned int slot, RBuffer* buffer);

	void SetIndexBuffer(RBuffer* buffer);
	void SetStructuredBuffer(unsigned int slot, RBuffer* buffer, EShaderType stage);

	void SetViewport(class RViewport* viewport);

private:
	
	/**
	 * Figures out where the state belongs to in the current draw order
	 */
	void SetDrawOrderFor(RPipelineStateFull* state);

	// Current state, consisting of real objects, rather than IDs
	RPipelineStateFull State;

	// Set of changes after the last SetFromPipelineState
	ChangesStruct Changes;
	ChangesCountStruct ChangesCount;
};

