#include "DX11Helper.h"
#include "Helper.h"
#include <sstream>
#include <d3dcompiler.h>
#include <D3DX11async.h>

HRESULT DX11Helper::CompileShaderFromFile( std::wstring shaderFileName, std::wstring entryPoint, std::wstring shaderModel, ID3DBlob** ppBlobOut, std::wstring &error )
{
	HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3DCOMPILE_DEBUG;
	#endif

    ID3DBlob* pErrorBlob = NULL;
	std::string ASCIIentryPoint = Helper::WStringtoString( entryPoint);
	std::string ASCIIshaderModel = Helper::WStringtoString( shaderModel);
    hr = D3DX11CompileFromFile( shaderFileName.c_str(), NULL, NULL, 
								ASCIIentryPoint.c_str(), 
								ASCIIshaderModel.c_str(), 
								dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
		{
            //OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
			std::string ASCIIerror( (char*)pErrorBlob->GetBufferPointer() );
			error = Helper::stringToWstring( ASCIIerror );
		}
        if( pErrorBlob ) pErrorBlob->Release();
        return false;
    }
	// Release the blob if was OK
    if( pErrorBlob ) pErrorBlob->Release();

	return true;
}
HRESULT DX11Helper::LoadInputLayoutFile( std::string vsFileName, std::string vsEntryPoint, std::string vsModel, ID3D11Device* device, ID3D11InputLayout** ilOut, std::wstring &error )
{
	std::wstring wVSFileName = std::wstring(vsFileName.begin(), vsFileName.end());
	std::wstring wVSEntryPoint = std::wstring(vsEntryPoint.begin(), vsEntryPoint.end());
	std::wstring wVSModel = std::wstring(vsModel.begin(), vsModel.end());

	return DX11Helper::LoadInputLayoutFile(wVSFileName, wVSEntryPoint, wVSModel, device, ilOut, error);
}
HRESULT DX11Helper::LoadInputLayoutFile( std::wstring vsFileName, std::wstring vsEntryPoint, std::wstring vsModel, ID3D11Device* device, ID3D11InputLayout** ilOut, std::wstring &error )
{
	HRESULT hr = S_OK;

	ID3DBlob* pVSBlob;

	if ( !CompileShaderFromFile( vsFileName, vsEntryPoint, vsModel, &pVSBlob, error ) )
    {
		std::wstringstream ssError;
		ssError << L"VS ERROR: Could not compile the " << vsFileName 
			<< L" (" << vsEntryPoint << L")." << std::endl
			<< error << std::endl
			<< L"Sorry it didn't work out.";
		error = ssError.str();
        return false;
    }

	D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
	UINT numElements = ARRAYSIZE( layout );

	hr = device->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), ilOut );
	pVSBlob->Release();	// Don't need this any more
	if ( FAILED( hr ) )
	{
		error = L"ERROR: Could not set the vertex buffer layout.";
		return false;
	}

	return true;
}
HRESULT DX11Helper::LoadVertexShaderFile( std::string vsFileName, std::string vsEntryPoint, std::string vsModel, ID3D11Device* device, ID3D11VertexShader** vsOut, std::wstring &error )
{
	std::wstring wVSFileName = std::wstring(vsFileName.begin(), vsFileName.end());
	std::wstring wVSEntryPoint = std::wstring(vsEntryPoint.begin(), vsEntryPoint.end());
	std::wstring wVSModel = std::wstring(vsModel.begin(), vsModel.end());

	return DX11Helper::LoadVertexShaderFile(wVSFileName, wVSEntryPoint, wVSModel, device, vsOut, error);
}
HRESULT DX11Helper::LoadVertexShaderFile( std::wstring vsFileName, std::wstring vsEntryPoint, std::wstring vsModel, ID3D11Device* device, ID3D11VertexShader** vsOut, std::wstring &error )
{
	HRESULT hr = S_OK;

	ID3DBlob* pVSBlob;

	if ( !CompileShaderFromFile( vsFileName, vsEntryPoint, vsModel, &pVSBlob, error ) )
    {
		std::wstringstream ssError;
		ssError << L"VS ERROR: Could not compile the " << vsFileName 
			<< L" (" << vsEntryPoint << L")." << std::endl
			<< error 
			<< L"Sorry it didn't work out.";
		error = ssError.str();
        return false;
    }

	// Now pass this compiled vertex shader to the device so it can be used
	hr = device->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, vsOut );
	// NOTE: DON'T Release the VSblob yet as it's needed for the vertex layout...
	if ( FAILED( hr ) )
	{
		pVSBlob->Release();
		error = L"ERROR: Could not assign compiled vertex shader to device.";
		return false;
	}
	
	return true;
}
HRESULT DX11Helper::LoadPixelShaderFile( std::string psFileName, std::string psEntryPoint, std::string psModel, ID3D11Device* device, ID3D11PixelShader** pxOut, std::wstring &error  )
{
	std::wstring wPSFileName = std::wstring(psFileName.begin(), psFileName.end());
	std::wstring wPSEntryPoint = std::wstring(psEntryPoint.begin(), psEntryPoint.end());
	std::wstring wPSModel = std::wstring(psModel.begin(), psModel.end());

	return DX11Helper::LoadPixelShaderFile(wPSFileName, wPSEntryPoint, wPSModel, device, pxOut, error);
}
HRESULT DX11Helper::LoadPixelShaderFile( std::wstring psFileName, std::wstring psEntryPoint, std::wstring psModel, ID3D11Device* device, ID3D11PixelShader** pxOut, std::wstring &error  )
{
	ID3DBlob* pPSBlob = NULL;
	if ( !CompileShaderFromFile( psFileName, psEntryPoint, psModel, &pPSBlob, error ) )
	{
		std::wstringstream ssError;
		ssError << L"VS ERROR: Could not compile the " << psFileName 
			<< L" (" << psEntryPoint << L")." << std::endl
			<< error 
			<< L"Sorry it didn't work out.";		
		error = ssError.str();
        return false;
    }

	// Now pass this compiled vertex shader to the device so it can be used
	auto hr = device->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, pxOut );
	pPSBlob->Release();
	if ( FAILED( hr ) )
	{
		error = L"ERROR: Could not assign compiled pixel shader to device.";
		return false;
	}

	return true;
}
HRESULT DX11Helper::LoadRasterizerState( D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool bAntialiasedLine, bool bMultisampleEnable, ID3D11Device* device, ID3D11RasterizerState** rsOut, std::wstring &error )
{
	// Additional stuff...
	D3D11_RASTERIZER_DESC RSDesc;
	memset( &RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC) );
	RSDesc.FillMode = fillMode; 
	RSDesc.AntialiasedLineEnable = bAntialiasedLine;	
	RSDesc.MultisampleEnable = bMultisampleEnable;
	RSDesc.CullMode = cullMode;	


	HRESULT hr = device->CreateRasterizerState( &RSDesc, rsOut );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Can't create rasterizer state";
		return false;
	}

	return true;
}
HRESULT DX11Helper::LoadTextureFile( std::wstring txFileName, ID3D11Device* device, ID3D11ShaderResourceView** pxOut, std::wstring &error  )
{
	HRESULT hr  = D3DX11CreateShaderResourceViewFromFile( device,
					txFileName.c_str(),
					NULL,			// Returns image information about texture
					NULL,			// Used to load texture in other thread
					pxOut, 
					NULL );

	if ( FAILED(hr) )
	{
		error = L"ERROR: Can't create texture " + txFileName;
		return false;
	}

	return true;
}
HRESULT DX11Helper::LoadSamplerState( D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addresU, D3D11_TEXTURE_ADDRESS_MODE addressV, D3D11_TEXTURE_ADDRESS_MODE addressW, D3D11_COMPARISON_FUNC camparisonFunc, float minLOD, float maxLod, ID3D11Device* device, ID3D11SamplerState** ssOut, std::wstring &error  )
{
	D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = filter;
    sampDesc.AddressU = addresU;
    sampDesc.AddressV = addressV;
    sampDesc.AddressW = addressW;
    sampDesc.ComparisonFunc = camparisonFunc;
    sampDesc.MinLOD = minLOD;
    sampDesc.MaxLOD = maxLod;
    HRESULT hr = device->CreateSamplerState( &sampDesc, ssOut );
    if ( FAILED(hr) )
	{
		error = L"ERROR: Can't create Sampler State";
		return false;
	}

	return true;
}