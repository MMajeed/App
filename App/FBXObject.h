#ifndef DISPLAY_MESH_GPU_H
#define DISPLAY_MESH_GPU_H

#include "cFBXBuffer.h"
#include "iObjectDrawable.h"
#include "ShaderFiles.h"
#include "Mesh.h"
#include "SkeletalAnimation.h"
#include "AnimationPlayer.h"
#include "AnimationController.h"
#include <map>

class FBXObject : public iObjectDrawable
{
public:
	virtual void Init();
	virtual void Clean();
	virtual void UpdateDrawing(float delta);
	virtual void UpdateObject(float delta);
	virtual void Draw();
	virtual float GetOrder(){ return 1000.0f; }

	virtual void InitVertexBuffer(ID3D11Device* device);
	virtual void InitIndexBuffer(ID3D11Device* device);
	virtual void InitInputLayout(ID3D11Device* device);
	virtual void InitVertexShader(ID3D11Device* device);
	virtual void InitPixelShader(ID3D11Device* device);
	virtual void InitRastersizerState(ID3D11Device* device);
	virtual void InitCBChangesEveryFrameBuffer(ID3D11Device* device);
	virtual void InitAnimBuffer(ID3D11Device* device);

	void LoadMesh(std::string path);

	void AddAnimation(std::string name, std::string path);

	void PlayAnimation(std::string anim, AnimationController::AnimationState);    
    std::string GetPlayingAnimation() const;
    float GetCurrentAnimTime() const;
    std::size_t GetCurrentAnimFrame() const;

	void SetAnimRate(float rate);
    float GetAnimRate() const;

	AnimationController			AnimController;

	std::string							MeshKey;
	std::map<std::string, std::string>	AnimationKey;

	std::pair<std::string, ID3D11Buffer*> 			pMeshVertexBuffer;
	std::pair<std::string, ID3D11Buffer*>			pMeshIndexBuffer;
	std::pair<std::string, ID3D11InputLayout*>		pInputLayout;
	std::pair<std::string, ID3D11VertexShader*>		pVertexShader;
	std::pair<std::string, ID3D11PixelShader*>		pPixelShader;
	std::pair<std::string, ID3D11RasterizerState*>	pRastersizerState;
	std::pair<std::string, ID3D11Buffer*>			pCBChangesEveryFrame;
	std::pair<std::string, ID3D11Buffer*>			pAnimBonesBuffer;

	void LoadD3DStuff();

	ShaderFiles Shader;

	static FBXObject* Spawn(std::map<std::string, std::string> info);

	virtual iObjectDrawable* clone() const;

    FBXObject();
    virtual ~FBXObject();
};

#endif