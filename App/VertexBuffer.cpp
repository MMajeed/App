#include "VertexBuffer.h"
#include "PlyLib\rply.h"
#include "Helper.h"

HRESULT VertexBuffer::CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer** bOut,  std::wstring& error)
{
	HRESULT hr = S_OK ;

	// Create vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( SimpleVertex ) * this->vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
	InitData.pSysMem = &this->vertices.front();
	hr = device->CreateBuffer( &bd, &InitData, bOut );
	if( FAILED( hr ) )
	{
		error = L"Failed at creating vertex buffer";
		return false;
	}
	return true;
}
HRESULT VertexBuffer::CreateIndexBuffer(ID3D11Device* device, ID3D11Buffer** bOut,  std::wstring& error)
{	
	HRESULT hr = S_OK ;

	// Create index buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( WORD ) * this->indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
	InitData.pSysMem = &(*this->indices.begin());
	hr = device->CreateBuffer( &bd, &InitData, bOut );
	if( FAILED( hr ) )
	{
		error = L"Failed at creating vertex buffer";
		return false;
	}

	return true;
}

VertexBuffer buffer;

static int vertexX(p_ply_argument argument) {
	VertexBuffer::SimpleVertex vertex;
	memset(&vertex, 0, sizeof(VertexBuffer::SimpleVertex));

	buffer.vertices.push_back(  vertex );
	float value = static_cast<float>(ply_get_argument_value(argument));
	buffer.vertices.back().Pos.x = value;
    return 1;
}
static int vertexY(p_ply_argument argument) {
	float value = static_cast<float>(ply_get_argument_value(argument));
	buffer.vertices.back().Pos.y = value;
    return 1;
}
static int vertexZ(p_ply_argument argument) {

	float value = static_cast<float>(ply_get_argument_value(argument));	
	buffer.vertices.back().Pos.z = value;	
	buffer.vertices.back().Pos.w = 1.0f;
    return 1;
}

static int vertexNX(p_ply_argument argument) {
	float value = static_cast<float>(ply_get_argument_value(argument));
	buffer.vertices.back().Normal.x = value;	
    return 1;
}
static int vertexNY(p_ply_argument argument) {
	float value = static_cast<float>(ply_get_argument_value(argument));
	buffer.vertices.back().Normal.y = value;
    return 1;
}
static int vertexNZ(p_ply_argument argument) {
	float value = static_cast<float>(ply_get_argument_value(argument));
	buffer.vertices.back().Normal.z = value;	
	buffer.vertices.back().Normal.w = 1.0f;	
    return 1;
}

static int vertexU(p_ply_argument argument) {
	float value = static_cast<float>(ply_get_argument_value(argument));
	buffer.vertices.back().Texture0.x = value;
	buffer.vertices.back().Texture1.x = value;
    return 1;
}
static int vertexV(p_ply_argument argument) {
	float value = static_cast<float>(ply_get_argument_value(argument));
	buffer.vertices.back().Texture0.y = value;
	buffer.vertices.back().Texture1.y = value;
    return 1;
}

static int face_cb(p_ply_argument argument) {
	long length, value_index;
    ply_get_argument_property(argument, NULL, &length, &value_index);

	if(value_index >= 0 && value_index <= 2)
	{
		WORD value = static_cast<WORD>(ply_get_argument_value(argument));
		buffer.indices.push_back(value);
	}
    return 1;
}


bool VertexBuffer::LoadFromPlyFile(std::wstring fileName, VertexBuffer& vbOut, std::wstring error)
{
	long nvertices, ntriangles;
	// Open the file
	p_ply ply = ply_open(Helper::WStringtoString(fileName).c_str(), NULL, 0, NULL);

	// Check that file is okay
    if (!ply) 
	{
		error = L"Failed at loading file " + fileName;
		return false;
	}
    if (!ply_read_header(ply)) 
		return false;

	// Load info
    nvertices = ply_set_read_cb(ply, "vertex", "x", vertexX, NULL, 0);
    ply_set_read_cb(ply, "vertex", "y", vertexY, NULL, 0);
    ply_set_read_cb(ply, "vertex", "z", vertexZ, NULL, 0);
	ply_set_read_cb(ply, "vertex", "nx", vertexNX, NULL, 0);
	ply_set_read_cb(ply, "vertex", "ny", vertexNY, NULL, 0);
	ply_set_read_cb(ply, "vertex", "nz", vertexNZ, NULL, 0);
	ply_set_read_cb(ply, "vertex", "u", vertexU, NULL, 0);
	ply_set_read_cb(ply, "vertex", "v", vertexV, NULL, 1);

    ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, NULL, 0);

	// Set space for vertices and indices for speed
	buffer.vertices.reserve(nvertices);
	buffer.indices.reserve(ntriangles);

	// Read file
    if (!ply_read(ply))
		return false;

	// Close file
    ply_close(ply);

	//Copy it to the output
	vbOut = buffer;
	
	// Empty the vertex so that don't take up space
	buffer.vertices = std::vector<SimpleVertex>();
	buffer.indices = std::vector<WORD>();

	return true;
}