#ifndef __DirectXObject__
#define __DirectXObject__

#include "iObjectDrawable.h"
#include "DirectXObject.h"
#include "DX11Helper.h"

class DirectXObject : public iObjectDrawable
{
public:
	virtual void Init()					    = 0;
	virtual void Clean()				    = 0;
	virtual void UpdateDrawing(float delta)	= 0;
	virtual void UpdateObject(float delta)	= 0;
	virtual void Draw()					    = 0;

	bool BoolDrawDepth;

	virtual void DrawDepth();

	virtual void InitDepth();

	virtual void InitDepthInputLayout(ID3D11Device* device);
	virtual void InitDepthVertexShader(ID3D11Device* device);
	virtual void InitDepthwPixelShader(ID3D11Device* device);
	virtual void InitDepthRastersizerState(ID3D11Device* device);
	virtual void InitDepthCBChangesEveryFrameBuffer(ID3D11Device* device);

	std::pair<std::string, ID3D11InputLayout*>		pDepthInputLayout;
	std::pair<std::string, ID3D11VertexShader*>		pDepthVertexShader;
	std::pair<std::string, ID3D11PixelShader*>		pDepthPixelShader;
	std::pair<std::string, ID3D11RasterizerState*>	pDepthRastersizerState;
	std::pair<std::string, ID3D11Buffer*>			pDepthCBChangesEveryFrame;

	DirectXObject();
};


#endif //__DirectXObject__