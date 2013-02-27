#ifndef __DX11Helper__
#define __DX11Helper__

#include <string>
#include <D3D11.h>
#include <xnamath.h>
#include <vector>

class DX11Helper 
{
public:
	static HRESULT CompileShaderFromFile( std::wstring shaderFileName, std::wstring entryPoint, std::wstring shaderModel, ID3DBlob** ppBlobOut, std::wstring &error );
	static HRESULT LoadVertexShaderFile( std::string vsFileName, std::string vsEntryPoint, std::string vsModel, ID3D11Device* device, ID3D11VertexShader** vsOut, std::wstring &error );
	static HRESULT LoadVertexShaderFile( std::wstring vsFileName, std::wstring vsEntryPoint, std::wstring vsModel, ID3D11Device* device, ID3D11VertexShader** vsOut, std::wstring &error );
	static HRESULT LoadInputLayoutFile( std::string vsFileName, std::string vsEntryPoint, std::string vsModel, ID3D11Device* device, ID3D11InputLayout** ilOut, std::wstring &error );
	static HRESULT LoadInputLayoutFile( std::wstring vsFileName, std::wstring vsEntryPoint, std::wstring vsModel, ID3D11Device* device, ID3D11InputLayout** ilOut, std::wstring &error );
	static HRESULT LoadPixelShaderFile( std::string psFileName, std::string psEntryPoint, std::string psModel, ID3D11Device* device, ID3D11PixelShader** pxOut, std::wstring &error  );
	static HRESULT LoadPixelShaderFile( std::wstring psFileName, std::wstring psEntryPoint, std::wstring psModel, ID3D11Device* device, ID3D11PixelShader** pxOut, std::wstring &error  );
	static HRESULT LoadRasterizerState( D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool bAntialiasedLine, bool bMultisampleEnable, ID3D11Device* device, ID3D11RasterizerState** rsOut, std::wstring &error );
	static HRESULT LoadTextureFile( std::wstring txFileName, ID3D11Device* device, ID3D11ShaderResourceView** pxOut, std::wstring &error  );	
	static HRESULT LoadSamplerState( D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addresU,D3D11_TEXTURE_ADDRESS_MODE addressV,  D3D11_TEXTURE_ADDRESS_MODE addressw, D3D11_COMPARISON_FUNC camparisonFunc, float minLOD, float maxLod, ID3D11Device* device, ID3D11SamplerState** ssOut, std::wstring &error  );	
};

#endif //__DX11Helper__