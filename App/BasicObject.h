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
	virtual void Update(float delta);
	virtual void Draw();

	VertexBuffer vertexBuffer;	
	
	Object object;

	std::string					VertexBufferID;
	std::string					IndexBufferID;
	std::string					CBChangesEveryFrameID;
	std::string					InputLayoutID;
	std::string					VertexShaderID;
	std::string					PixelShaderID;
	std::string					RastersizerStateID;
	std::vector<std::string>	TextureIDs;
	std::string					CubeMapIDs;
	
	virtual void DrawTexture(ID3D11DeviceContext* pImmediateContext);

	virtual void InitVertexBuffer(ID3D11Device* device);
	virtual void InitIndexBuffer(ID3D11Device* device);
	virtual void InitInputLayout(ID3D11Device* device);
	virtual void InitVertexShader(ID3D11Device* device);
	virtual void InitPixelShader(ID3D11Device* device);
	virtual void InitRastersizerState(ID3D11Device* device);
	virtual void InitCBChangesEveryFrameBuffer(ID3D11Device* device);
	virtual void InitTextureAndCube(ID3D11Device* device);

	void LoadD3DStuff(	ID3D11Buffer*& vertexBuffer,	
						ID3D11Buffer*& indexBuffer,
						ID3D11InputLayout*& inputLayout,
						ID3D11VertexShader*& vertexShader,
						ID3D11PixelShader*& pixelShader,
						ID3D11RasterizerState*& rastersizerState,
						ID3D11Buffer*& cbChangeEveryFrame) const;
	
	BasicObject();
};


#endif //__BasicObject__