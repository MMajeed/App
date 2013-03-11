#include "FBXMeshLoader.h"
#include <iostream>
#include <fstream>


void AddToVector(void* object, int size, std::vector<char>& binaryFile)
{
	int currentSize = binaryFile.size();
	binaryFile.resize( currentSize + size );
	memcpy( ( (&binaryFile.front()) + currentSize), object, size);
}

void FBXMeshLoader::WriteToFile(std::string path)
{
	std::vector<char> binaryMesh;
		
	{
	// Add verts
	int length = this->pMesh.mVerts.size();
	AddToVector(&length, sizeof(int), binaryMesh);
	int vecSize = sizeof(this->pMesh.mVerts.front()) + this->pMesh.mVerts.size();
	AddToVector((&this->pMesh.mVerts.front()), vecSize, binaryMesh);
	}
	{
	// Add indices
	int length = this->pMesh.mIndices.size();
	AddToVector(&length, sizeof(int), binaryMesh);
	int vecSize = sizeof(this->pMesh.mIndices.front()) + this->pMesh.mIndices.size();
	AddToVector((&this->pMesh.mIndices.front()), vecSize, binaryMesh);
	}
	{
	// Add skelton
	int length = this->pMesh.mSkeleton.size();
	AddToVector(&length, sizeof(int), binaryMesh);
	int vecSize = sizeof(this->pMesh.mSkeleton.front()) + this->pMesh.mSkeleton.size();
	AddToVector((&this->pMesh.mSkeleton.front()), vecSize, binaryMesh);
	}
	{
	// Add skelton
	int length = this->pMesh.mOrigBones.size();
	AddToVector(&length, sizeof(int), binaryMesh);
	int vecSize = sizeof(this->pMesh.mOrigBones.front()) + this->pMesh.mOrigBones.size();
	AddToVector((&this->pMesh.mOrigBones.front()), vecSize, binaryMesh);
	}
	{
	// Add origGlobalPose
	int length = this->pMesh.mOrigGlobalPose.size();
	AddToVector(&length, sizeof(int), binaryMesh);
	int vecSize = sizeof(this->pMesh.mOrigGlobalPose.front()) + this->pMesh.mOrigGlobalPose.size();
	AddToVector((&this->pMesh.mOrigGlobalPose.front()), vecSize, binaryMesh);
	}
	{
	// Add mNumBones
	int length = this->pMesh.mNumBones;
	AddToVector(&length, sizeof(int), binaryMesh);
	}

	std::ofstream outfile;
	outfile.open (path, std::ios::out|std::ios::binary);

	outfile.write(&binaryMesh.front(), binaryMesh.size());
}

Mesh FBXMeshLoader::LoadMesh(std::string path)
{
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

	this->lScene = FbxScene::Create(g_FbxSdkManager,"myScene");

	if(!lImporter->Import(lScene))
	{
		std::cout << "Call to FbxImporter::Import() failed." << std::endl;
		std::cout << "Error returned:" << lImporter->GetLastErrorString() << std::endl << std::endl;
        exit(-1);
	}

	lImporter->Destroy();
    ios->Destroy();

    LoadMeshFromScene();

	lScene->Destroy();

	return ( this->pMesh );
}

void FBXMeshLoader::LoadMeshFromScene()
{
	std::cout << "=========================================\nTriangulating" << std::endl;
	FbxNode* lRootNode = this->lScene->GetRootNode();
    if(lRootNode) 
	{
		TriangulateRecursive(lRootNode);
    }

	const int lPoseCount = this->lScene->GetPoseCount();

	std::cout << "=========================================\nGetting Skeleton Data" << std::endl;
	std::size_t numBones = CountSkeletonRecursive(lRootNode);
    if(numBones > 0)
    {
		pMesh.mSkeleton.resize(numBones);
		pMesh.mOrigBones.resize(numBones);
		pMesh.mOrigGlobalPose.resize(numBones);

        pMesh.mNumBones = 0;
        LoadSkeletonRecursive(lRootNode, -1);

		LoadBindPose();

        // Note we calculate and store the inverse bind-pose transform for skinning later
        for(std::size_t i = 0; i < pMesh.mNumBones; ++i)
        {
            XMVECTOR det;
            XMMATRIX m = XMLoadFloat4x4(&pMesh.mOrigGlobalPose[i]);
            m = XMMatrixInverse(&det, m);
            XMStoreFloat4x4(&pMesh.mSkeleton[i].invBindPose, m);
        }
    }
	std::cout << "=========================================\nGetting Mesh Data" << std::endl;
    LoadMeshRecursive(lRootNode);
}

void FBXMeshLoader::LoadMeshRecursive(FbxNode* pNode)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute && 
		lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		const char* nodeName = pNode->GetName();
		char buffer[500];
		buffer[499] = '\0';
		_snprintf_s(buffer, 499, "Load Mesh name='%s'\n", nodeName);
		OutputDebugStringA(buffer);

		FbxMesh* lMesh = pNode->GetMesh();
		const int lVertexCount = lMesh->GetControlPointsCount();

		if(lVertexCount > 0)
		{
			int vertOffset = pMesh.mVerts.size();
			if( vertOffset > 0)
			{
				pMesh.mVerts.resize(vertOffset + lVertexCount);
			}
			else
			{
				pMesh.mVerts.resize(lVertexCount);
			}


			for(int i = 0; i < lVertexCount; ++i)
			{
				pMesh.mVerts[i + vertOffset].Pos.x = static_cast<float>(lMesh->GetControlPoints()[i][0] * 0.01);
				pMesh.mVerts[i + vertOffset].Pos.y = static_cast<float>(lMesh->GetControlPoints()[i][1] * 0.01);
				pMesh.mVerts[i + vertOffset].Pos.z = static_cast<float>(lMesh->GetControlPoints()[i][2] * 0.01);

				pMesh.mVerts[i + vertOffset].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}
				
			int totalIndices = 0;
			const int polyCount = lMesh->GetPolygonCount();
			for (int i = 0; i < polyCount; ++i)
			{
				const int vertCount = lMesh->GetPolygonSize(i);
				totalIndices += vertCount;
			}

			int indOffset = pMesh.mIndices.size();
			if(indOffset > 0)
			{
				pMesh.mIndices.resize(indOffset + totalIndices);
			}
			else
			{
				pMesh.mIndices.resize(totalIndices);
			}

			int ind = 0;
			for (int i = 0; i < polyCount; ++i)
			{
				const int vertCount = lMesh->GetPolygonSize(i);
				for (int j = 0; j < vertCount; ++j)
				{
					pMesh.mIndices[ind + indOffset] = static_cast<WORD>(lMesh->GetPolygonVertex(i, j) + vertOffset);
					++ind;
				}
			}

            _snprintf_s(buffer, 499, "  VertexCount ='%d' PolygonCount ='%d' IndicesCount ='%d'\n", lVertexCount, lMesh->GetPolygonCount(), totalIndices);
		    OutputDebugStringA(buffer);
			
            LoadSkinInfo(pNode, vertOffset, lVertexCount);
        }

	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		LoadMeshRecursive(pNode->GetChild(lChildIndex));
	}
}

void FBXMeshLoader::LoadSkinInfo(FbxNode* pNode, int vertOffset, int numVerts)
{
	std::cout << "  LoadSkinInformation for mesh["<< pNode->GetName() <<"] vertOffset ["<< vertOffset <<"] numVerts ["<< numVerts <<"]" << std::endl;
    
	FbxMesh* lMesh = pNode->GetMesh();

    //skin information
    //we need to get the number of clusters
    const int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
    int lClusterCount = 0;
    for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
    {
        lClusterCount += ((FbxSkin *)(lMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
    }

	std::cout << "  SkinCount ='" << lSkinCount << "' ClusterCount ='" << lClusterCount << "'" << std::endl;

    if(lClusterCount == 0)
    {
        // This mesh does not have any skin deformers, so check if it is attached directly to a skeleton cFBXBuffer::Joint and
        // do a rigid skinning to that cFBXBuffer::Joint
        FbxNode* lParent = pNode->GetParent();
        if(lParent)
        {
            FbxNodeAttribute* lNodeAttribute = lParent->GetNodeAttribute();
	        if(lNodeAttribute && 
		        lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
            {
                for(std::size_t b = 0; b < pMesh.mNumBones; ++b)
                {
					if(pMesh.mSkeleton[b].name == lParent->GetName())
                    {
						std::cout << "  Node [" << pNode->GetName() << "] attached to Skeleton [" << lParent->GetName() << "][" << b << "]\n" << std::endl;
						
						if( static_cast<std::size_t>(vertOffset + numVerts) <= pMesh.mVerts.size())
                        {
                            for(int i = 0; i < numVerts; ++i)
                            {
                                pMesh.mVerts[i + vertOffset].JointIndex[0] = b;
                                pMesh.mVerts[i + vertOffset].JointWeight[0] = 1.0f;
                            }
                        }
                        return;
                    }
                }
            }
        }
    }
    else
    {
        FbxSkin * lSkinDeformer = (FbxSkin *)lMesh->GetDeformer(0, FbxDeformer::eSkin);
        FbxSkin::EType lSkinningType = lSkinDeformer->GetSkinningType();
        
        for(int c = 0; c < lClusterCount; ++c)
        {
            FbxCluster* cluster = lSkinDeformer->GetCluster(c);
            
            if(!cluster->GetLink())
            {
				std::cout << "   cluster[" << c << "] has no link!" << std::endl;
                continue;
            }

			std::cout << "   cluster[" << c << "] has link[" << cluster->GetLink()->GetName() << "]!" << std::endl;

            for(std::size_t b = 0; b < pMesh.mNumBones; ++b)
            {
				if(pMesh.mSkeleton[b].name == cluster->GetLink()->GetName())
                {
                    int numIndices = cluster->GetControlPointIndicesCount();
                    if(numIndices > 0)
                    {
                        int* indices = cluster->GetControlPointIndices();
                        double* weights = cluster->GetControlPointWeights();
                        for(int i = 0; i < numIndices; ++i)
                        {
                            if(weights[i] > 0.f)
                            {
								if(static_cast<std::size_t>(vertOffset + indices[i]) < pMesh.mVerts.size())
                                {
                                    for(int w = 0; w < 4; ++w)
                                    {
                                        //find the next available weight
                                        if(weights[i] > pMesh.mVerts[vertOffset + indices[i]].JointWeight[w])
                                        {
                                            //ensure that our skinning info is ordered from most significant to least significant
                                            for(int wb = 3; wb > w; --wb)
                                            {
                                                pMesh.mVerts[vertOffset + indices[i]].JointIndex[wb] = pMesh.mVerts[vertOffset + indices[i]].JointIndex[wb-1];
                                                pMesh.mVerts[vertOffset + indices[i]].JointWeight[wb] = pMesh.mVerts[vertOffset + indices[i]].JointWeight[wb-1];
                                            }
                                            pMesh.mVerts[vertOffset + indices[i]].JointIndex[w] = b;
                                            pMesh.mVerts[vertOffset + indices[i]].JointWeight[w] = static_cast<float>(weights[i]);
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        //check that all weights are normalized
        for(int i = 0; i < numVerts; ++i)
        {
            float s = 0.0f;
            for(int w = 0; w < 4; ++w)
            {
                s += pMesh.mVerts[vertOffset + i].JointWeight[w];
            }
            if(s > 0.0f && (s < 0.999999f || s > 1.000001f))
            {
                for(int w = 0; w < 4; ++w)
                {
                    pMesh.mVerts[vertOffset + i].JointWeight[w] /= s;
                }
            }
        }

		std::cout << "done" << std::endl;
    }
}

void FBXMeshLoader::LoadBindPose()
{
	for(int i = 0; i < lScene->GetPoseCount(); ++i)
    {
		FbxPose* pose = lScene->GetPose(i);

        std::cout << "Pose='" << pose->GetName() << "' count='" << pose->GetCount() << "' isBindPose='" << pose->IsBindPose() << "' isRestPose='" << pose->IsRestPose() << "'" << std::endl;
        
        if(pose->IsBindPose())
        {
            for(int j = 0; j < pose->GetCount(); ++j)
            {
                FbxNode* node = pose->GetNode(j);
                if(node)
                {
                    const FbxMatrix& m = pose->GetMatrix(j);

                    FbxVector4 translation;
                    FbxQuaternion rotation;
                    FbxVector4 shearing;
                    FbxVector4 scaling;
                    double sign;
                    m.GetElements(translation, rotation, shearing, scaling, sign);

                    FbxVector4 rotEuler = rotation.DecomposeSphericalXYZ();

					std::cout << "node='" << pose->GetName() 
							  << "' translation='" << translation[0] << ", " << translation[1] << ", " << translation[2]
							  << "' rotation='" << rotEuler[0] << ", " << rotEuler[1] << ", " << rotEuler[2]
							  << "' scaling='" << scaling[0] << ", " << scaling[1] << ", " << scaling[2]
							  << "'" << std::endl;

                    //find the same bone in our mesh
                    for(std::size_t b = 0; b < pMesh.mNumBones; ++b)
                    {
						if(pMesh.mSkeleton[b].name == node->GetName())
                        {
                            //found a match
                            _XMMATRIX mb(static_cast<float>(m[0][0] * 1.00), static_cast<float>(m[0][1] * 1.00), static_cast<float>(m[0][2] * 1.00), static_cast<float>(m[0][3] * 1.00),
                                         static_cast<float>(m[1][0] * 1.00), static_cast<float>(m[1][1] * 1.00), static_cast<float>(m[1][2] * 1.00), static_cast<float>(m[1][3] * 1.00),
                                         static_cast<float>(m[2][0] * 1.00), static_cast<float>(m[2][1] * 1.00), static_cast<float>(m[2][2] * 1.00), static_cast<float>(m[2][3] * 1.00),
                                         static_cast<float>(m[3][0] * 0.01), static_cast<float>(m[3][1] * 0.01), static_cast<float>(m[3][2] * 0.01), static_cast<float>(m[3][3] * 1.00));

                            XMStoreFloat4x4(&pMesh.mOrigGlobalPose[b], mb);

                            XMVECTOR det;
                            XMMATRIX invMb = XMMatrixInverse(&det, mb);
                            XMStoreFloat4x4(&pMesh.mSkeleton[b].invBindPose, invMb);
                            break;
                        }
                    }
                }
            }
        }
    }

    //recalculate local transforms
    for(std::size_t i = 0; i < pMesh.mNumBones; ++i)
    {
        if(pMesh.mSkeleton[i].parent >= 0)
        {
            XMMATRIX mb = XMLoadFloat4x4(&pMesh.mOrigGlobalPose[i]) * XMLoadFloat4x4(&pMesh.mSkeleton[pMesh.mSkeleton[i].parent].invBindPose);

            XMVECTOR scale;
            XMVECTOR rotQuat;
            XMVECTOR trans;
            if(XMMatrixDecompose(&scale, &rotQuat, &trans, mb))
            {
                XMStoreFloat3(&pMesh.mOrigBones[i].translation, trans);
                XMStoreFloat4(&pMesh.mOrigBones[i].rotation, rotQuat);
                XMStoreFloat3(&pMesh.mOrigBones[i].scale, scale);
            }
        }
    }
}

void FBXMeshLoader::LoadSkeletonRecursive(FbxNode* pNode, int parent)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute && 
		lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		std::string nodeName = pNode->GetName();
		std::cout << "Load Skeleton name='" << nodeName << "' parent='" << parent << "'" << std::endl;

        this->pMesh.mSkeleton[this->pMesh.mNumBones].parent = parent;
		this->pMesh.mSkeleton[this->pMesh.mNumBones].name = nodeName;

        FbxDouble3 translation = pNode->LclTranslation.Get();
        FbxDouble3 rotation = pNode->LclRotation.Get();
        FbxDouble3 scaling = pNode->LclScaling.Get();

        this->pMesh.mOrigBones[this->pMesh.mNumBones].translation = XMFLOAT3(static_cast<float>(translation[0] * 0.01), static_cast<float>(translation[1] * 0.01), static_cast<float>(translation[2] * 0.01));
        XMVECTOR r = XMQuaternionRotationRollPitchYaw(static_cast<float>(rotation[2]), static_cast<float>(rotation[0]), static_cast<float>(rotation[1]));
        XMStoreFloat4(&(this->pMesh.mOrigBones[this->pMesh.mNumBones].rotation), r);
        pMesh.mOrigBones[this->pMesh.mNumBones].scale = XMFLOAT3(static_cast<float>(scaling[0]), static_cast<float>(scaling[1]), static_cast<float>(scaling[2]));


        XMMATRIX m = this->pMesh.mOrigBones[this->pMesh.mNumBones].GetTransform();
        if(this->pMesh.mSkeleton[this->pMesh.mNumBones].parent >= 0)
        {
            XMMATRIX c = XMLoadFloat4x4(&this->pMesh.mOrigGlobalPose[this->pMesh.mSkeleton[this->pMesh.mNumBones].parent]);
            m *= c;
        }
        
        XMStoreFloat4x4(&this->pMesh.mOrigGlobalPose[this->pMesh.mNumBones], m);
        
        parent = pMesh.mNumBones;
        ++pMesh.mNumBones;
    }

    const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		LoadSkeletonRecursive(pNode->GetChild(lChildIndex), parent);
	}
}

int FBXMeshLoader::CountSkeletonRecursive(FbxNode* pNode)
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

void FBXMeshLoader::TriangulateRecursive(FbxNode* pNode)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
		{
			std::string nodeName =  pNode->GetName();
			std::string buffer;
			buffer = "Triangulate node name='" + nodeName + "'";
			std::cout << buffer;

			FbxGeometryConverter lConverter(pNode->GetFbxManager());
			lConverter.TriangulateInPlace(pNode);
		}
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		TriangulateRecursive(pNode->GetChild(lChildIndex));
	}
}

FBXMeshLoader::FBXMeshLoader()
	: g_FbxSdkManager(NULL),
	lScene(NULL)
{
	g_FbxSdkManager = FbxManager::Create();
}

FBXMeshLoader::~FBXMeshLoader()
{
	if(g_FbxSdkManager != NULL)
    {
        g_FbxSdkManager->Destroy();
        g_FbxSdkManager = NULL;
    }
}