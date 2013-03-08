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
	static HRESULT LoadTransparent( ID3D11Device* device, ID3D11BlendState** bsOut, std::wstring &error  );	

	template<typename T>
	static HRESULT LoadVertexBuffer(ID3D11Device* device, T* pVertex, std::size_t numElements, ID3D11Buffer** bOut, std::wstring& error)
	{
		HRESULT hr = S_OK ;

		// Create vertex buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( T ) * numElements;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory( &InitData, sizeof(InitData) );
		InitData.pSysMem = pVertex;
		hr = device->CreateBuffer( &bd, &InitData, bOut );
		if( FAILED( hr ) )
		{
			error = L"Failed at creating vertex buffer";
			return false;
		}
		return true;
	}

	template<typename T>
	static HRESULT LoadIndexBuffer(ID3D11Device* device, T* pIndice, std::size_t numElements, ID3D11Buffer** bOut, std::wstring& error)
	{	
		HRESULT hr = S_OK ;

		// Create index buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( T ) * numElements;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory( &InitData, sizeof(InitData) );
		InitData.pSysMem = pIndice;
		hr = device->CreateBuffer( &bd, &InitData, bOut );
		if( FAILED( hr ) )
		{
			error = L"Failed at creating vertex buffer";
			return false;
		}

		return true;
	}
};

#endif //__DX11Helper__