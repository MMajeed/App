#ifndef __cBuffer__
#define __cBuffer__

#include <string>
#include <D3D11.h>
#include <xnamath.h>
#include <vector>

class cBuffer
{
public:
	static const int NUMBEROFLIGHTS = 10;

	struct MaterialInfo
	{
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT4 spec;
	};

	struct CLightDesc
	{
		XMFLOAT4 pos;		
		XMFLOAT4 dir;		
		MaterialInfo material;
		XMFLOAT4 attenuation;				
		XMFLOAT4 lightPowerRangeType;	// // x = power, y = range, z = light type, w = not used
	};

	struct cbNeverChanges
	{
		XMMATRIX mCameraView;			//matrix View;
		XMMATRIX mLightView;			//matrix View;
		XMMATRIX mSpecialMatrix;			//matrix View;
		XMFLOAT4 eye;					//float4 eye;
		XMFLOAT4 target;				//float4 target;
		CLightDesc lights[NUMBEROFLIGHTS];
	};

	struct cbChangeOnResize
	{
		XMMATRIX mProjection;
		XMMATRIX mLightProjection;
	};

	struct cbChangeEveryFrame
	{
		XMMATRIX mWorld;
		MaterialInfo colour;   
	};
		
	template<typename T>
	static HRESULT LoadBuffer( ID3D11Device* device, ID3D11Buffer** cbOut, std::wstring &error )
	{
		HRESULT hr = S_OK ;

		// Create vertex buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(T);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		hr = device->CreateBuffer( &bd, NULL, cbOut );
		if( FAILED( hr ) )
		{
			error = L"Failed at creating constant buffer buffer";
			return false;
		}
		return true;
	}
};
#endif //__cBuffer__