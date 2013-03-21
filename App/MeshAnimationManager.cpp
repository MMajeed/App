#include "MeshAnimationManager.h"

void MeshAnimationManager::LoadMesh(std::string path)
{
	if(this->ExistsMesh(path))
	{ // We already have this loaded, ignore it
		return;
	}

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
	this->mapMeshObject[path].mVerts.resize(sizeOfVec);
	int totalVecSize = sizeof(this->mapMeshObject[path].mVerts.front()) * (sizeOfVec);
	memcpy(&this->mapMeshObject[path].mVerts.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mIndices
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	this->mapMeshObject[path].mIndices.resize(sizeOfVec);
	int totalVecSize = sizeof(this->mapMeshObject[path].mIndices.front()) * (sizeOfVec);
	memcpy(&this->mapMeshObject[path].mIndices.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mSkeleton
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	this->mapMeshObject[path].mSkeleton.resize(sizeOfVec);
	int totalVecSize = sizeof(this->mapMeshObject[path].mSkeleton.front()) * (sizeOfVec);
	memcpy(&this->mapMeshObject[path].mSkeleton.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mOrigBones
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	this->mapMeshObject[path].mOrigBones.resize(sizeOfVec);
	int totalVecSize = sizeof(this->mapMeshObject[path].mOrigBones.front()) * (sizeOfVec);
	memcpy(&this->mapMeshObject[path].mOrigBones.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mOrigGlobalPose
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	this->mapMeshObject[path].mOrigGlobalPose.resize(sizeOfVec);
	int totalVecSize = sizeof(this->mapMeshObject[path].mOrigGlobalPose.front()) * (sizeOfVec);
	memcpy(&this->mapMeshObject[path].mOrigGlobalPose.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mNumBones
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	this->mapMeshObject[path].mNumBones = sizeOfVec;
	}
}
bool MeshAnimationManager::GetMesh(std::string name, Mesh*& out)
{
	auto iter = this->mapMeshObject.find(name);
	if(iter == this->mapMeshObject.end()) return false;
	out = &iter->second;
	return true;
}
bool MeshAnimationManager::ExistsMesh(std::string name)
{
	return (this->mapMeshObject.find(name) != this->mapMeshObject.end());
}

void MeshAnimationManager::LoadAnimation(std::string path)
{
	if(this->ExistsAnimation(path))
	{ // We already have this loaded, ignore it
		return;
	}
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
	this->mapAnimationObject[path].mSkeleton.resize(sizeOfVec);
	int totalVecSize = sizeof(this->mapAnimationObject[path].mSkeleton.front()) * (sizeOfVec);
	memcpy(&this->mapAnimationObject[path].mSkeleton.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
	currentlyRead += totalVecSize;
	}
	{
	//Add mNumBones
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	this->mapAnimationObject[path].mNumBones = sizeOfVec;
	}
	{
	//Add mKeys
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	this->mapAnimationObject[path].mKeys.resize(sizeOfVec);

	for(std::size_t i = 0; i < this->mapAnimationObject[path].mKeys.size(); ++i)
	{
		int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
		currentlyRead += sizeof(int);
		this->mapAnimationObject[path].mKeys[i].mBones.resize(sizeOfVec);
		int totalVecSize = sizeof(this->mapAnimationObject[path].mKeys[i].mBones.front()) * (sizeOfVec);
		memcpy(&this->mapAnimationObject[path].mKeys[i].mBones.front(), ((&binaryInput.front()) + currentlyRead), totalVecSize);
		currentlyRead += totalVecSize;

		float duration = *(reinterpret_cast<float*>( (&binaryInput.front()) + currentlyRead));
		currentlyRead += sizeof(float);
		this->mapAnimationObject[path].mKeys[i].mTime = duration;
	}
	}
	{
	//Add mDuration
	float duration = *(reinterpret_cast<float*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(float);
	this->mapAnimationObject[path].mDuration = duration;
	}
	{
	//Add mName
	int sizeOfVec = *(reinterpret_cast<int*>( (&binaryInput.front()) + currentlyRead));
	currentlyRead += sizeof(int);
	this->mapAnimationObject[path].mName = ((&binaryInput.front()) + currentlyRead);
	currentlyRead += sizeOfVec;
	}
}
bool MeshAnimationManager::GetAnimation(std::string name, SkeletalAnimation*& out)
{
	auto iter = this->mapAnimationObject.find(name);
	if(iter == this->mapAnimationObject.end()) return false;
	out = &iter->second;
	return true;
}
bool MeshAnimationManager::ExistsAnimation(std::string name)
{
	return (this->mapAnimationObject.find(name) != this->mapAnimationObject.end());
}

MeshAnimationManager* MeshAnimationManager::ObjectManager = 0;

MeshAnimationManager* MeshAnimationManager::getInstance()
{
	if(MeshAnimationManager::ObjectManager == 0)
	{
		MeshAnimationManager::ObjectManager = new MeshAnimationManager;
	}

    return MeshAnimationManager::ObjectManager;
}

MeshAnimationManager::MeshAnimationManager()
{

}
