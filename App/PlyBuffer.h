#ifndef __PlyBuffer__
#define __PlyBuffer__

#include <string>
#include <D3D11.h>
#include <xnamath.h>
#include <vector>

class PlyBuffer
{
public:
	struct Vertex
	{
		XMFLOAT4 Pos;
		XMFLOAT4 Normal;
		XMFLOAT2 Texture0;
		XMFLOAT2 Texture1;    
	};

	std::vector<Vertex> vertices;
	std::vector<WORD> indices;

	static bool LoadFromPlyFile(std::wstring fileName, PlyBuffer& vbOut, std::wstring error);
};

#endif //__PlyBuffer__