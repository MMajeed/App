#ifndef __VertexBuffer__
#define __VertexBuffer__

#include <string>
#include <D3D11.h>
#include <xnamath.h>
#include <vector>

class VertexBuffer
{
public:
	struct SimpleVertex
	{
		XMFLOAT4 Pos;
		XMFLOAT4 Normal;
		XMFLOAT2 Texture0;
		XMFLOAT2 Texture1;    
	};

	std::vector<SimpleVertex> vertices;
	std::vector<WORD> indices;

	static bool LoadFromPlyFile(std::wstring fileName, VertexBuffer& vbOut, std::wstring error);
};

#endif //__VertexBuffer__