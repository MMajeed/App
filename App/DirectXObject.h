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

	virtual void Draw()							= 0;
	virtual void SetupDrawConstantBuffer()		= 0;
	virtual void SetupDrawVertexBuffer()		= 0;
	virtual void SetupDrawInputVertexShader()	= 0;
	virtual void SetupDrawPixelShader()			= 0;
	virtual void SetupDrawRasterizeShader()		= 0;
	virtual void SetupDrawTexture()				= 0;
	virtual void DrawObject()					= 0;
	virtual void CleanupAfterDraw()				= 0;

	bool BoolDrawDepth;

	virtual void DrawDepth();

	virtual void InitDepth();

	virtual void InitDepthPixelShader(ID3D11Device* device);
	virtual void InitDepthRastersizerState(ID3D11Device* device);

	std::pair<std::string, ID3D11PixelShader*>		pDepthPixelShader;
	std::pair<std::string, ID3D11RasterizerState*>	pDepthRastersizerState;

	DirectXObject();
};


#endif //__DirectXObject__