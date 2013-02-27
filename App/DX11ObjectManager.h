#ifndef __DX11ObjectManager_HPP__
#define __DX11ObjectManager_HPP__

#include <map>
#include <vector>
#include <D3D11.h>
#include <xnamath.h>
#include <string>

class DX11ObjectManager
{
public:
	// Singelton
	static DX11ObjectManager* getInstance();

	template<typename T>
	class D3DMapContainer
	{
	public:
		void Add(std::string name, T in, bool overwrite = false)
		{
			if( (this->mapObject.find(name) == this->mapObject.end()) )
			{
				this->mapObject[name] = in;
			}
			else if( (overwrite == true) )
			{
				this->mapObject[name]->Release();
				this->mapObject[name] = in;
			}
		}
		bool Get(std::string name, T& out)
		{
			auto iter = this->mapObject.find(name);

			out = iter->second;
			return true;
		}
		bool Exists(std::string name)
		{
			return (this->mapObject.find(name) != this->mapObject.end());
		}
		std::map<std::string, T> mapObject;
	};

	D3DMapContainer<ID3D11Buffer*> 					VertexBuffer;
	D3DMapContainer<ID3D11Buffer*> 					IndexBuffer;
	D3DMapContainer<ID3D11Buffer*> 					CBuffer;
	D3DMapContainer<ID3D11InputLayout*>				InputLayout;
	D3DMapContainer<ID3D11VertexShader*> 			VertexShader;
	D3DMapContainer<ID3D11PixelShader*>				PixelShader;
	D3DMapContainer<ID3D11RasterizerState*>			RastersizerState;
	D3DMapContainer<ID3D11ShaderResourceView*>		Textexture;
	D3DMapContainer<ID3D11SamplerState*>			Sampler;
protected:
	static DX11ObjectManager* ObjectManager;

    DX11ObjectManager();
    DX11ObjectManager(DX11ObjectManager const&);              
    void operator=(DX11ObjectManager const&); 
};


#endif // __DX11ObjectManager_HPP__