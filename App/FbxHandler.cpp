#include <fbxsdk.h>

#include "FbxHandler.h"
#include "Mesh.h"
#include "SkeletalAnimation.h"
#include <sstream>
#include <fstream>

Mesh LoadMeshFromFbx(std::string path)
{
	Mesh mesh;

	std::ifstream inputFile;
	inputFile.open (path, std::ios::in|std::ios::binary);
	
	// Get the size of the file
	std::streamoff start = inputFile.tellg();
    inputFile.seekg( 0, std::ios::end );
	std::streamoff end = inputFile.tellg();
	inputFile.seekg( 0, std::ios::beg );	
	std::streamoff size = end - start;
	
	// Read all of the binrary and put it into binary input
	std::vector<char> binaryInput;
	binaryInput.resize(static_cast<unsigned int>(size));
	inputFile.read( (&binaryInput.front()), size);
	inputFile.close();
	
	int currentlyRead = 0;
	{
	//Add mVerts
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	mesh.mVerts.resize(sizeOfVec);
	int totalVecSize = sizeof(mesh.mVerts.front()) * (sizeOfVec);
	memcpy(&mesh.mVerts.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mIndices
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	mesh.mIndices.resize(sizeOfVec);
	int totalVecSize = sizeof(mesh.mIndices.front()) * (sizeOfVec);
	memcpy(&mesh.mIndices.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mSkeleton
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	mesh.mSkeleton.resize(sizeOfVec);
	int totalVecSize = sizeof(mesh.mSkeleton.front()) * (sizeOfVec);
	memcpy(&mesh.mSkeleton.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mOrigBones
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	mesh.mOrigBones.resize(sizeOfVec);
	int totalVecSize = sizeof(mesh.mOrigBones.front()) * (sizeOfVec);
	memcpy(&mesh.mOrigBones.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mOrigGlobalPose
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	mesh.mOrigGlobalPose.resize(sizeOfVec);
	int totalVecSize = sizeof(mesh.mOrigGlobalPose.front()) * (sizeOfVec);
	memcpy(&mesh.mOrigGlobalPose.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mNumBones
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	mesh.mNumBones = sizeOfVec;
	}

	return mesh;
}

SkeletalAnimation LoadBakedAnimationFromFBX(std::string path)
{
	SkeletalAnimation anim;

	std::ifstream inputFile;
	inputFile.open (path, std::ios::in|std::ios::binary);
	
	// Get the size of the file
	std::streamoff start = inputFile.tellg();
    inputFile.seekg( 0, std::ios::end );
	std::streamoff end = inputFile.tellg();
	inputFile.seekg( 0, std::ios::beg );	
	std::streamoff size = end - start;
	
	// Read all of the binrary and put it into binary input
	std::vector<char> binaryInput;
	binaryInput.resize(static_cast<unsigned int>(size));
	inputFile.read( (&binaryInput.front()), size);
	inputFile.close();

	int currentlyRead = 0;
	{
	//Add mVerts
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	anim.mSkeleton.resize(sizeOfVec);
	int totalVecSize = sizeof(anim.mSkeleton.front()) * (sizeOfVec);
	memcpy(&anim.mSkeleton.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mNumBones
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	anim.mNumBones = sizeOfVec;
	}
	{
	//Add mKeys
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	anim.mKeys.resize(sizeOfVec);

	for(std::size_t i = 0; i < anim.mKeys.size(); ++i)
	{
		int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
		currentlyRead += sizeof(int);
		anim.mKeys[i].mBones.resize(sizeOfVec);
		int totalVecSize = sizeof(anim.mKeys[i].mBones.front()) * (sizeOfVec);
		memcpy(&anim.mKeys[i].mBones.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
		currentlyRead += totalVecSize;

		float duration = *(reinterpret_cast<float*>( (&binaryInput.front()) + currentlyRead));
		currentlyRead += sizeof(float);
		anim.mKeys[i].mTime = duration;
	}
	}
	{
	//Add mDuration
	float duration = *(reinterpret_cast<float*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(float);
	anim.mDuration = duration;
	}
	{
	//Add mName
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	anim.mName = ((&binaryInput.front()) + currentlyRead);
	currentlyRead += sizeOfVec;
	}

	return anim;
}
