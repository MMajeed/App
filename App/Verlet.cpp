#include "Verlet.h"
#include "DX11ObjectManager.h"
#include "Helper.h"
#include "MathHelper.h"
#include "Application.h"
#include <process.h>

template <class T>
T getRandom(T lowRange, T highRange)
{
	if ( lowRange > highRange )
	{
		T temp = lowRange;
		lowRange = highRange;
		highRange = temp;
	}
	T delta = highRange - lowRange;
	delta = static_cast<T>( ( static_cast<double>( rand() ) / static_cast<double>(RAND_MAX) ) * delta );
	return delta + lowRange;
}

Verlet::Verlet()
{
	this->pRastersizerState.first = "Wire";
}
Verlet::Verlet(PlyFile pf)
	: PlyFile(pf)
{
	this->pRastersizerState.first = "Wire";
}
	
void Verlet::Init()
{
	PlyFile::Init();

	this->constraints.reserve(this->vertexBuffer.vertices.size() * 2);
	// Link each node to the first and the last. But don't link the first and last to anything
	
	for(std::size_t i = 0; i < this->vertexBuffer.indices.size(); i += 3)
	{
		WORD VID_1 = this->vertexBuffer.indices[i];
		WORD VID_2 = this->vertexBuffer.indices[i + 1];
		WORD VID_3 = this->vertexBuffer.indices[i + 2];

		this->constraints.push_back(Verlet::Constrain(VID_1, VID_2));
		this->constraints.push_back(Verlet::Constrain(VID_1, VID_3));
		this->constraints.push_back(Verlet::Constrain(VID_2, VID_3));
	}	

	for(std::size_t i = 0; i < this->constraints.size(); ++i)
	{
		XMFLOAT4 length = this->vertexBuffer.vertices[this->constraints[i].Vertex2].Pos - this->vertexBuffer.vertices[this->constraints[i].Vertex1].Pos;
		this->constraints[i].RestingLength = MathHelper::Length(length);
	}

	float LargestX = 0.0f; float LargestY = 0.0f; float LargestZ = 0.0f;
	float SmallestX = 0.0f; float SmallestY = 0.0f; float SmallestZ = 0.0f;

	for(std::size_t i = 0; i < this->vertexBuffer.vertices.size(); ++i)
	{
		if(this->vertexBuffer.vertices[i].Pos.x > LargestX) LargestX = this->vertexBuffer.vertices[i].Pos.x;
		if(this->vertexBuffer.vertices[i].Pos.y > LargestY) LargestY = this->vertexBuffer.vertices[i].Pos.y;
		if(this->vertexBuffer.vertices[i].Pos.z > LargestZ) LargestZ = this->vertexBuffer.vertices[i].Pos.z;

		if(this->vertexBuffer.vertices[i].Pos.x < SmallestX) SmallestX = this->vertexBuffer.vertices[i].Pos.x;
		if(this->vertexBuffer.vertices[i].Pos.y < SmallestY) SmallestY = this->vertexBuffer.vertices[i].Pos.y;
		if(this->vertexBuffer.vertices[i].Pos.z < SmallestZ) SmallestZ = this->vertexBuffer.vertices[i].Pos.z;
	}

	float tooClose = ((LargestX + -SmallestX) + (LargestY + -SmallestY) + (LargestZ + -SmallestZ)) / 3;
	tooClose /= 2.0f;
	int numberOfConstraints = this->vertexBuffer.vertices.size() ;
	for ( int constraintCount = 0; constraintCount != numberOfConstraints; constraintCount++ )
	{
		Constrain tempConstraint;
		tempConstraint.Vertex1 = getRandom< int >( 0,  this->vertexBuffer.vertices.size() );
		XMFLOAT4 vert1 = this->vertexBuffer.vertices[tempConstraint.Vertex1].Pos;
		bool bKeepLooking = true;
		while ( bKeepLooking )
		{
			int randomNumber = getRandom< int >( 0,  this->vertexBuffer.vertices.size() );
			XMFLOAT4 vert2 = this->vertexBuffer.vertices[randomNumber].Pos;

			float howFar = MathHelper::Length(vert2, vert1);
			if ( howFar > tooClose )
			{
				tempConstraint.Vertex2 = randomNumber;
				tempConstraint.RestingLength = howFar;
				bKeepLooking = false;
			}
		}

		this->constraints.push_back(tempConstraint);
	}
	

}
void Verlet::InitRastersizerState(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->RastersizerState.Exists(this->pRastersizerState.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadRasterizerState(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, true, true, device, &(this->pRastersizerState.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->RastersizerState.Add(this->pRastersizerState.first, this->pRastersizerState.second);
	}
}

struct ThreadInfo 
{
	std::vector<Verlet::Constrain>*	constraints;
	VertexBuffer*					vertexBuffer;
	std::size_t						start;
	std::size_t						end;
};

unsigned __stdcall VerletFunction( void* pArguments )
{
	ThreadInfo* pInfo = static_cast<ThreadInfo*>(pArguments);

	for(std::size_t i = pInfo->start; i < pInfo->end; ++i)
	{
		XMFLOAT4 x1 = pInfo->vertexBuffer->vertices.at(pInfo->constraints->at(i).Vertex1).Pos;
		XMFLOAT4 x2 = pInfo->vertexBuffer->vertices.at(pInfo->constraints->at(i).Vertex2).Pos;

		XMFLOAT4 deltaDiffernce = x2 - x1;

		float deltalength = MathHelper::Length(deltaDiffernce);
		float diff = ( deltalength - pInfo->constraints->at(i).RestingLength ) / deltalength;
		if(diff > 0.001 || diff < -0.001)
		{
			XMFLOAT4 difference = deltaDiffernce * 0.5f * diff ;

			XMFLOAT4 newX1 = x1 + difference;
			XMFLOAT4 newX2 = x2 - difference;

			pInfo->vertexBuffer->vertices.at(pInfo->constraints->at(i).Vertex1).Pos = newX1;
			pInfo->vertexBuffer->vertices.at(pInfo->constraints->at(i).Vertex2).Pos = newX2;
		}
	}

	return 0;
}
std::vector<HANDLE>		vecThread;	
std::vector<ThreadInfo> vecParamters;

void Verlet::UpdateDrawing(float delta)
{
	PlyFile::UpdateDrawing(delta);

	for ( std::size_t iLoop =  0; iLoop < vecThread.size(); iLoop++)
	{
		CloseHandle( vecThread[iLoop] );
	}
	
	
	std::size_t numberOfThreads = 4;

	vecThread.resize(numberOfThreads);
	vecParamters.resize(numberOfThreads);

	for(std::size_t i = 0; i < numberOfThreads; ++i)
	{
		vecParamters[i].start = (this->constraints.size() / numberOfThreads - 1) * i;
		vecParamters[i].end = (((this->constraints.size() / numberOfThreads - 1) * (i+1)) - 1);
		vecParamters[i].constraints		= &this->constraints;
		vecParamters[i].vertexBuffer	= &this->vertexBuffer;

		unsigned int dw;
		vecThread[i] = (HANDLE)_beginthreadex(NULL, 0, &VerletFunction, &vecParamters[i], 0, &dw);
	}

	for(std::size_t i = 0; i < this->vertexBuffer.vertices.size(); ++i)
	{
		this->vertexBuffer.vertices[i].Pos.y -= 0.01f * delta ;

		if(this->vertexBuffer.vertices[i].Pos.y < -0.19f)
		{
			this->vertexBuffer.vertices[i].Pos.y = -0.19f;
		}
	}

	
	
}
void Verlet::SetupDraw()
{
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
	pImmediateContext->UpdateSubresource( this->pVertexBuffer.second, 0, NULL, &this->vertexBuffer.vertices.front(), 0, 0 );

	PlyFile::SetupDraw();
}

void Verlet::Randomize(float delta)
{
	float min = -delta;
	float max = delta;

	for(std::size_t i = 0; i < this->vertexBuffer.vertices.size(); ++i)
	{
		this->vertexBuffer.vertices[i].Pos.x = getRandom(min, max);
		this->vertexBuffer.vertices[i].Pos.y = getRandom(min, max);
		this->vertexBuffer.vertices[i].Pos.z = getRandom(min, max);

	}
}

Verlet* Verlet::Spawn(std::map<std::string, std::string> info)
{
	PlyFile* newPly = PlyFile::Spawn(info);
	Verlet* newVerlet = new Verlet(*newPly);
	delete newPly;
	newVerlet->pVertexBuffer.first = newVerlet->pVertexBuffer.first + "Verlet";
	newVerlet->pIndexBuffer.first = newVerlet->pVertexBuffer.first + "Verlet";
	return newVerlet;
}