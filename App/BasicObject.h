#ifndef __BasicObject__
#define __BasicObject__

#include "Object.h"
#include "iObjectDrawable.h"
#include "DX11Helper.h"
#include "VertexBuffer.h"
#include <string>

class BasicObject : public iObjectDrawable
{
public:
	virtual void Init();
	virtual void Clean();
	virtual void UpdateDrawing(float delta);
	virtual void UpdateObject(float delta);
	virtual void Draw();
	virtual void SetupDraw();
	virtual void SetupTexture();
	virtual void DrawObject();
	virtual void CleanupAfterDraw();
	virtual float GetOrder();

	VertexBuffer vertexBuffer;	

	std::pair<std::string, ID3D11Buffer*> 							pVertexBuffer;
	std::pair<std::string, ID3D11Buffer*>							pIndexBuffer;
	std::pair<std::string, ID3D11Buffer*>							pCBChangesEveryFrame;
	std::pair<std::string, ID3D11InputLayout*>						pInputLayout;
	std::pair<std::string, ID3D11VertexShader*>						pVertexShader;
	std::pair<std::string, ID3D11PixelShader*>						pPixelShader;
	std::pair<std::string, ID3D11RasterizerState*>					pRastersizerState;
	std::vector<std::pair<std::string, ID3D11ShaderResourceView*>>	pVecTexture;
	std::pair<std::string, ID3D11ShaderResourceView*>				pCubeMap;
	
	struct ShaderFile
	{
		std::string FileName;
		std::string EntryPoint;
		std::string Mode;
	};

	ShaderFile ShaderInput;
	ShaderFile ShaderVertex;
	ShaderFile ShaderPixel;

	virtual void InitVertexBuffer(ID3D11Device* device);
	virtual void InitIndexBuffer(ID3D11Device* device);
	virtual void InitInputLayout(ID3D11Device* device);
	virtual void InitVertexShader(ID3D11Device* device);
	virtual void InitPixelShader(ID3D11Device* device);
	virtual void InitRastersizerState(ID3D11Device* device);
	virtual void InitCBChangesEveryFrameBuffer(ID3D11Device* device);
	virtual void InitTextureAndCube(ID3D11Device* device);

	void LoadD3DStuff();
	
	BasicObject();
};


#endif //__BasicObject__