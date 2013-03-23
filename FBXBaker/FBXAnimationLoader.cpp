#include "FBXAnimationLoader.h"
#include <iostream>
#include <fstream>

void AddToVec(void* object, int size, std::vector<char>& binaryFile)
{
	int currentSize = binaryFile.size();
	binaryFile.resize( currentSize + size );
	memcpy( ( (&binaryFile.front()) + currentSize), object, size);
}

void FBXAnimationLoader::WriteToFile(std::string path)
{
	std::vector<char> binaryAnimation;
	{
	// Add mSkeleton
	int length = this->pAnim.mSkeleton.size();
	AddToVec(&length, sizeof(int), binaryAnimation);
	int vecSize = sizeof(this->pAnim.mSkeleton.front()) * this->pAnim.mSkeleton.size();
	AddToVec((&this->pAnim.mSkeleton.front()), vecSize, binaryAnimation);
	}
	{
	// Add mNumBones
	int length = this->pAnim.mNumBones;
	AddToVec(&length, sizeof(int), binaryAnimation);
	}
	{
	// Add mKeys
	int length = this->pAnim.mKeys.size();
	AddToVec(&length, sizeof(int), binaryAnimation);

	for(std::size_t i = 0; i < this->pAnim.mKeys.size(); ++i)
	{
		int length = this->pAnim.mKeys[i].mBones.size();
		AddToVec(&length, sizeof(int), binaryAnimation);
		int vecSize = sizeof(this->pAnim.mKeys[i].mBones.front()) * this->pAnim.mKeys[i].mBones.size();
		AddToVec((&this->pAnim.mKeys[i].mBones.front()), vecSize, binaryAnimation);
		
		float duration = this->pAnim.mKeys[i].mTime;
		AddToVec(&duration, sizeof(float), binaryAnimation);
	}
	}
	{
	// Add mDuration
	float duration = this->pAnim.mDuration;
	AddToVec(&duration, sizeof(float), binaryAnimation);
	}
	{
	// Add mName
	int length = this->pAnim.mName.size();
	AddToVec(&length, sizeof(int), binaryAnimation);
	int vecSize = sizeof(this->pAnim.mName.front()) * this->pAnim.mName.size();
	AddToVec((&this->pAnim.mName.front()), vecSize, binaryAnimation);
	}

	std::ofstream outfile;
	outfile.open (path, std::ios::out|std::ios::binary);

	outfile.write(&binaryAnimation.front(), binaryAnimation.size());
}

XMFLOAT3 GetRotationEuler(cFBXBuffer::JointPose pos)
{
    XMFLOAT3 euler;

	float t = (pos.rotation.x * pos.rotation.y) + (pos.rotation.z * pos.rotation.w);

    euler.x = asin(2*pos.rotation.x*pos.rotation.y + 2*pos.rotation.z*pos.rotation.w);

    if(t >= 0.499999 && t <= 0.500001)
    {
        euler.y = 2 * atan2(pos.rotation.x, pos.rotation.w);
        euler.z = 0.0f;
    }
    else if(t <= -0.499999 && t >= -0.500001)
    {
		euler.y = -2 * atan2(pos.rotation.x, pos.rotation.w);
        euler.z = 0.0f;
    }
    else
    {
        euler.y = atan2(2*pos.rotation.y*pos.rotation.w - 2*pos.rotation.x*pos.rotation.z, 1 - 2*pos.rotation.y*pos.rotation.y - 2*pos.rotation.z*pos.rotation.z);
        euler.z = atan2(2*pos.rotation.x*pos.rotation.w - 2*pos.rotation.y*pos.rotation.z, 1 - 2*pos.rotation.x*pos.rotation.x - 2*pos.rotation.z*pos.rotation.z);
    }

    return(euler);
}


SkeletalAnimation FBXAnimationLoader::LoadAnimation(std::string path, int orig, int start, int end)
{
	// Create the io settings object.
    FbxIOSettings *ios = FbxIOSettings::Create(g_FbxSdkManager, IOSROOT);
    g_FbxSdkManager->SetIOSettings(ios);

    // Create an importer using our sdk manager.
    FbxImporter* lImporter = FbxImporter::Create(g_FbxSdkManager,"");
    
    // Use the first argument as the filename for the importer.
	if(!lImporter->Initialize(path.c_str(), -1, g_FbxSdkManager->GetIOSettings())) {
		std::cout << "Call to FbxImporter::Initialize() failed." << std::endl;
		std::cout << "Error returned:" << lImporter->GetLastErrorString() << std::endl << std::endl;
        exit(-1);
    }
    
    // Create a new scene so it can be populated by the imported file.
    FbxScene* lScene = FbxScene::Create(g_FbxSdkManager,"myScene");

    // Import the contents of the file into the scene.
	if(lImporter->Import(lScene))
	{
        FbxNode* lRootNode = lScene->GetRootNode();
        int numBones = CountSkeletonRecursive(lRootNode);
        if(numBones > 0)
        {
			pAnim.mSkeleton.resize(numBones);
            // First load the skeletal hierarchy
            LoadSkeletonRecursive(lRootNode, &(pAnim.mSkeleton.front()), pAnim.mNumBones, -1);

            // Then go through the stack of takes and load each one
            for(int i = 0; i < lImporter->GetAnimStackCount(); ++i)
            {
                LoadAnimationTake(lImporter->GetTakeInfo(i), lScene);
            }

			if(start > 0 && end > 0)
			{
				
				float duration = pAnim.mKeys.at(start).mTime;

				for(int i = start; i <= end; ++i)
				{
					pAnim.mKeys.at(i).mTime = pAnim.mKeys.at(i).mTime - duration;
				}

				pAnim.mDuration = pAnim.mKeys.at(end).mTime;

				pAnim.mKeys.erase(pAnim.mKeys.begin() + (end + 1), pAnim.mKeys.end());
				pAnim.mKeys.erase(pAnim.mKeys.begin(), pAnim.mKeys.begin() + (start));
			}

			if(orig >= 0 )
            {
                XMFLOAT3 zeroPos(0.0f, 0.0f, 0.0f);
                XMFLOAT4 zeroRot;
                XMStoreFloat4(&zeroRot, XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f));
                        
                auto o = pAnim.mKeys[0].mBones[1];
                o.translation.y = 0.0f;

                XMFLOAT3 euler = GetRotationEuler(o);
                euler.x = 0.0f;
                euler.z = 0.0f;
                XMStoreFloat4(&o.rotation, XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z));

                XMMATRIX newOrigin = o.GetTransform();
                XMVECTOR det;
                XMMATRIX invOrigin = XMMatrixInverse(&det, newOrigin);

				for(int k = 0; k < pAnim.mKeys.size(); ++k)
                {
                    for(int b = 0; b < numBones; ++b)
                    {
                        if(b < orig)
                        {
                            pAnim.mKeys[k].mBones[b].rotation = zeroRot;
                            pAnim.mKeys[k].mBones[b].translation = zeroPos;
                        }
                        else if(b == orig)
                        {
                            XMMATRIX m = pAnim.mKeys[k].mBones[b].GetTransform();

                            m = m * invOrigin;

                            XMVECTOR scale;
                            XMVECTOR rotQuat;
                            XMVECTOR trans;
                            if(XMMatrixDecompose(&scale, &rotQuat, &trans, m))
                            {
                                XMStoreFloat3(&pAnim.mKeys[k].mBones[b].translation, trans);
                                XMStoreFloat4(&pAnim.mKeys[k].mBones[b].rotation, rotQuat);
                                XMStoreFloat3(&pAnim.mKeys[k].mBones[b].scale, scale);
                            }
                        }
                    }
                }
            }
        }
    }

    // The file has been imported; we can get rid of the importer.
    lImporter->Destroy();
    ios->Destroy();

    lScene->Destroy();

	return this->pAnim;
}

void FBXAnimationLoader::LoadAnimationTake(FbxTakeInfo* pTake, FbxScene* pScene)
{
    int numKeys = static_cast<int>(pTake->mLocalTimeSpan.GetDuration().GetFrameCount() + 1);

	std::string takeName = pTake->mName.Buffer();
	std::cout << "Take[" << pTake->mName.Buffer() << "] [[" << numKeys << "] frames]\n";

	pAnim.mName = pTake->mName.Buffer();

    // We flatten the cFBXBuffer::Joint hierarchy into an array of FbxNode* so it is easier to 
    // sample the cFBXBuffer::Joint poses during the animation
    FbxNode** lFlattenedNodes = new (std::nothrow) FbxNode*[pAnim.mNumBones];
    if(!lFlattenedNodes)
    {
        return;
    }
    int numNodes = 0;
    FlattenHeirarchyRecursive(pScene->GetRootNode(), lFlattenedNodes, &numNodes, -1);

	pAnim.mKeys.resize(numKeys);

    //prepare some intermediate arrays of matricies
    _XMFLOAT4X4* globalPose = new (std::nothrow) _XMFLOAT4X4[numNodes];
    if(!globalPose)
    {
        delete [] lFlattenedNodes;
        return;
    }
    _XMFLOAT4X4* invPose = new (std::nothrow) _XMFLOAT4X4[numNodes];
    if(!invPose)
    {
        delete [] lFlattenedNodes;
        delete [] globalPose;
        return;
    }

    FbxTime curTime = pTake->mLocalTimeSpan.GetStart();
    float startTime = static_cast<float>(curTime.GetSecondDouble());
    FbxTime period;
    // Set our desired sample-rate.
    // Time is specified in Hours, Minutes, Seconds, Frames, Fields
    period.SetTime(0, 0, 0, 1, 0);
    for(int i = 0; i < numKeys; ++i)
    {
		pAnim.mKeys[i].mBones.resize(pAnim.mNumBones);
        
        //store the 'time-stamp' for this frame
        pAnim.mKeys[i].mTime = static_cast<float>(curTime.GetSecondDouble()) - startTime;

        for(int b = 0; b < numNodes; ++b)
        {
            // Ask the scene to 'evaluate' the global transform of the node at the specified time
            FbxMatrix m = pScene->GetEvaluator()->GetNodeGlobalTransform(lFlattenedNodes[b], curTime);
            // note that we are only scaling the translation when we convert to our own matrix format
            _XMMATRIX mb(static_cast<float>(m[0][0] * 1.00), static_cast<float>(m[0][1] * 1.00), static_cast<float>(m[0][2] * 1.00), static_cast<float>(m[0][3] * 1.00),
                static_cast<float>(m[1][0] * 1.00), static_cast<float>(m[1][1] * 1.00), static_cast<float>(m[1][2] * 1.00), static_cast<float>(m[1][3] * 1.00),
                static_cast<float>(m[2][0] * 1.00), static_cast<float>(m[2][1] * 1.00), static_cast<float>(m[2][2] * 1.00), static_cast<float>(m[2][3] * 1.00),
                static_cast<float>(m[3][0] * 0.01), static_cast<float>(m[3][1] * 0.01), static_cast<float>(m[3][2] * 0.01), static_cast<float>(m[3][3] * 1.00));

            XMStoreFloat4x4(&globalPose[b], mb);

            XMVECTOR det;
            XMMATRIX invMb = XMMatrixInverse(&det, mb);
            XMStoreFloat4x4(&invPose[b], invMb);

            XMMATRIX lb = XMLoadFloat4x4(&globalPose[b]);
            if(pAnim.mSkeleton[b].parent >= 0)
            {
                // get the local cFBXBuffer::Joint tranform
                lb *= XMLoadFloat4x4(&invPose[pAnim.mSkeleton[b].parent]);
            }

            //store the individual components
            XMVECTOR scale;
            XMVECTOR rotQuat;
            XMVECTOR trans;
            if(XMMatrixDecompose(&scale, &rotQuat, &trans, lb))
            {
                XMStoreFloat3(&pAnim.mKeys[i].mBones[b].translation, trans);
                XMStoreFloat4(&pAnim.mKeys[i].mBones[b].rotation, rotQuat);
                XMStoreFloat3(&pAnim.mKeys[i].mBones[b].scale, scale);
            }
        }
        pAnim.mDuration = static_cast<float>(curTime.GetSecondDouble()) - startTime;
        //advance our time period
        curTime += period;
    }

    delete [] lFlattenedNodes;
    delete [] globalPose;
    delete [] invPose;
}

void FBXAnimationLoader::FlattenHeirarchyRecursive(FbxNode* pNode, FbxNode** pFlattenedNodes, int* numBones, int parent)
{
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute && 
		lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
        pFlattenedNodes[*numBones] = pNode;
        parent = *numBones;
        ++(*numBones);
    }

    const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		FlattenHeirarchyRecursive(pNode->GetChild(lChildIndex), pFlattenedNodes, numBones, parent);
	}
}

void FBXAnimationLoader::LoadSkeletonRecursive(FbxNode* pNode, cFBXBuffer::Joint* pSkeleton, std::size_t& numBones, int parent)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute && 
		lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
        const char* nodeName = pNode->GetName();
		std::cout << "Load Skeleton name='"<< nodeName << "' parent='"<< parent << "'" << std::endl;
		
        pSkeleton[numBones].parent = parent;
		strcpy_s(pSkeleton[numBones].name, nodeName);

        parent = numBones;
        ++(numBones);
    }

    const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		LoadSkeletonRecursive(pNode->GetChild(lChildIndex), pSkeleton, numBones, parent);
	}
}

int FBXAnimationLoader::CountSkeletonRecursive(FbxNode* pNode)
{
    int count = 0;
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute && 
		lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
        ++count;
    }

    const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		count += CountSkeletonRecursive(pNode->GetChild(lChildIndex));
	}

    return(count);
}

FBXAnimationLoader::FBXAnimationLoader()
	:g_FbxSdkManager(NULL)
{
	
	g_FbxSdkManager = FbxManager::Create();
}
FBXAnimationLoader::~FBXAnimationLoader()
{
	if(g_FbxSdkManager != NULL)
    {
        g_FbxSdkManager->Destroy();
        g_FbxSdkManager = NULL;
    }
}