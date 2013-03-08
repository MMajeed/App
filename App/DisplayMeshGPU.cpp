#include "DisplayMeshGPU.h"
#include "Mesh.h"
#include "SkeletalAnimation.h"
#include "HelperFuncs.h"
#include <new>
#include "Application.h"

DisplayMeshGPU::DisplayMeshGPU() : 
    Entity(),
	mMesh(NULL),
    mAnimation(NULL),
    mChannelMap(NULL),

    mCurrentBones(NULL),
    mCurrentGlobalPose(NULL),

	mBoneTransforms(NULL),

    mDrawBindPose(false),
    mFrameInterpolate(true),
    mDebugRotation(0.0f),

    mAnimTime(0.0f),
    mAnimRate(1.0f),
    mCurrentFrame(0.0f),

    mVertexShader(NULL),
    mPixelShader(NULL),
    mVertexLayout(NULL),
    mVertexBuffer(NULL),
    mIndexBuffer(NULL),
    mConstantBuffer(NULL),
	mAnimBonesBuffer(NULL)
{
}

DisplayMeshGPU::~DisplayMeshGPU()
{
	mMesh = NULL;

    if(mChannelMap) delete [] mChannelMap;
    if(mCurrentBones) delete [] mCurrentBones;
    if(mCurrentGlobalPose) delete [] mCurrentGlobalPose;
	if(mBoneTransforms) delete [] mBoneTransforms;

    if( mConstantBuffer ) mConstantBuffer->Release();
    if( mVertexBuffer ) mVertexBuffer->Release();
    if( mIndexBuffer ) mIndexBuffer->Release();
    if( mVertexLayout ) mVertexLayout->Release();
    if( mVertexShader ) mVertexShader->Release();
    if( mPixelShader ) mPixelShader->Release();
	if( mAnimBonesBuffer ) mAnimBonesBuffer->Release();
}

void DisplayMeshGPU::SetMesh(Mesh* pMesh)
{
	mMesh = pMesh;
}

HRESULT DisplayMeshGPU::Initialize()
{
    HRESULT hr = S_OK;

	if(!mMesh)
	{
		return(E_FAIL);
	}

    mCurrentGlobalPose = new (std::nothrow) _XMFLOAT4X4[mMesh->mNumBones];
	mBoneTransforms = new (std::nothrow) _XMFLOAT4X4[128];
    mCurrentBones = new (std::nothrow) JointPose[mMesh->mNumBones];
    
    if(!mCurrentGlobalPose || !mCurrentBones || !mBoneTransforms)
    {
        return(E_FAIL);
    }
    
	int b = 0;
	for(int i = 0; i < mMesh->mNumBones; ++i)
	{
		mCurrentGlobalPose[i] = mMesh->mOrigGlobalPose[i];
		mCurrentBones[i] = mMesh->mOrigBones[i];
	}

    // Compile the vertex shader
    ID3DBlob* pVSBlob = NULL;
	std::wstring h = L"ShaderFiles/7_GpuSkinShader.fx";
    hr = CompileShaderFromFile( L"ShaderFiles/7_GpuSkinShader.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return(hr);
    }

	// Create the vertex shader
	hr = ((Application*)App::getInstance())->direct3d.pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mVertexShader );
	if( FAILED( hr ) )
	{	
		pVSBlob->Release();
        return(hr);
	}

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINTINDEX", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINTWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
	hr = ((Application*)App::getInstance())->direct3d.pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &mVertexLayout );
	pVSBlob->Release();
	if( FAILED( hr ) )
        return(hr);

    // Set the input layout
	((Application*)App::getInstance())->direct3d.pImmediateContext->IASetInputLayout( mVertexLayout );

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"ShaderFiles/7_GpuSkinShader.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return(hr);
    }

	// Create the pixel shader
	hr = ((Application*)App::getInstance())->direct3d.pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &mPixelShader );
	pPSBlob->Release();
    if( FAILED( hr ) )
        return(hr);

    // Create vertex buffer
    D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( SimpleSkinnedVertex ) * mMesh->mNumVerts;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
	InitData.pSysMem = mMesh->mVerts;
	hr = ((Application*)App::getInstance())->direct3d.pd3dDevice->CreateBuffer( &bd, &InitData, &mVertexBuffer );
    if( FAILED( hr ) )
        return(hr);

    // Set vertex buffer
    UINT stride = sizeof( SimpleSkinnedVertex );
    UINT offset = 0;
	((Application*)App::getInstance())->direct3d.pImmediateContext->IASetVertexBuffers( 0, 1, &mVertexBuffer, &stride, &offset );

    // Set primitive topology
	((Application*)App::getInstance())->direct3d.pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // Create index buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( WORD ) * mMesh->mNumIndices;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = mMesh->mIndices;
	hr = ((Application*)App::getInstance())->direct3d.pd3dDevice->CreateBuffer( &bd, &InitData, &mIndexBuffer );
    if( FAILED( hr ) )
        return(hr);

    // Set index buffer
	((Application*)App::getInstance())->direct3d.pImmediateContext->IASetIndexBuffer( mIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    // Set primitive topology
	((Application*)App::getInstance())->direct3d.pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = ((Application*)App::getInstance())->direct3d.pd3dDevice->CreateBuffer( &bd, NULL, &mConstantBuffer );
    if( FAILED( hr ) )
        return(hr);

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(AnimMatrices);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = ((Application*)App::getInstance())->direct3d.pd3dDevice->CreateBuffer( &bd, NULL, &mAnimBonesBuffer );
    if( FAILED( hr ) )
        return(hr);

	// Setup the raster description which will determine how and what polygons will be drawn.
    D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;//D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = ((Application*)App::getInstance())->direct3d.pd3dDevice->CreateRasterizerState(&rasterDesc, &g_pRasterState);
	if(FAILED(hr))
	{
		throw std::exception("");
	}

    return(S_OK);
}

void DisplayMeshGPU::Update(float delta)
{
    if(mAnimation)
    {
        float oldTime = mAnimTime;
        mAnimTime = oldTime;
        //find the appropriate frame
        mAnimTime += delta * mAnimRate;
        while(mAnimTime >= mAnimation->mDuration)
        {
            mAnimTime -= mAnimation->mDuration;
        }
        int frame = 0;
        while(mAnimTime > mAnimation->mKeys[frame].mTime && frame < mAnimation->mNumKeys)
        {
            ++frame;
        }

        if(frame >= mAnimation->mNumKeys)
        {
            frame = 0;
        }
        if(!mFrameInterpolate)
        {
            mCurrentFrame = static_cast<float>(frame);
            // we've got the frame, so update the animation   
            for(int i = 0; i < mMesh->mNumBones; ++i)
            {
                if(mChannelMap[i] != -1)
                {
                    JointPose* jnt = &(mAnimation->mKeys[frame].mBones[mChannelMap[i]]);

                    mCurrentBones[i].translation = jnt->translation;
                    mCurrentBones[i].rotation = jnt->rotation;
                    mCurrentBones[i].scale = jnt->scale;
                }
            }
        }
        else
        {
            //frame interpolation
            int prevFrame = 0;
			float t = 0;
			if(frame > 0)
			{
				prevFrame = frame - 1;
				t = (mAnimTime - mAnimation->mKeys[prevFrame].mTime) / (mAnimation->mKeys[frame].mTime - mAnimation->mKeys[prevFrame].mTime);
			}

			mCurrentFrame = static_cast<float>(frame) + t; //this is just for show in the title-bar

            // we've got the frame, so update the animation   
            for(int i = 0; i < mMesh->mNumBones; ++i)
            {
                if(mChannelMap[i] != -1)
                {
                    JointPose* jntA = &(mAnimation->mKeys[prevFrame].mBones[mChannelMap[i]]);
					JointPose* jntB = &(mAnimation->mKeys[frame].mBones[mChannelMap[i]]);

					XMStoreFloat3(&mCurrentBones[i].translation, XMVectorLerp(XMLoadFloat3(&jntA->translation), XMLoadFloat3(&jntB->translation), t));
                    XMStoreFloat4(&mCurrentBones[i].rotation, XMQuaternionSlerp(XMLoadFloat4(&jntA->rotation), XMLoadFloat4(&jntB->rotation), t));
                    XMStoreFloat3(&mCurrentBones[i].scale, XMVectorLerp(XMLoadFloat3(&jntA->scale), XMLoadFloat3(&jntB->scale), t));
                }
            }
        }
    }
    else
    {
        //some test rotation of bones to see the skinning work
        XMVECTOR r = XMQuaternionRotationRollPitchYaw(mDebugRotation, 0.0f, 0.0f);
        for(int i = 1; i < mMesh->mNumBones; ++i)
        {
            XMVECTOR q = XMLoadFloat4(&mMesh->mOrigBones[i].rotation);

            q = XMQuaternionMultiply(q, r);

            XMStoreFloat4(&mCurrentBones[i].rotation, q);
        }
    }

    //update the current pose
    for(int i = 0; i < mMesh->mNumBones; ++i)
    {
        XMMATRIX m = mCurrentBones[i].GetTransform();
        if(mMesh->mSkeleton[i].parent >= 0)
        {
            XMMATRIX c = XMLoadFloat4x4(&mCurrentGlobalPose[mMesh->mSkeleton[i].parent]);
            m *= c;
        }
        
        XMStoreFloat4x4(&mCurrentGlobalPose[i], m);
    }
}

void DisplayMeshGPU::Draw()
{
	//update the current pose
    for(int i = 0; i < mMesh->mNumBones; ++i)
    {
		XMMATRIX invBind = XMLoadFloat4x4(&mMesh->mSkeleton[i].invBindPose);
		XMMATRIX currPose = XMLoadFloat4x4(&mCurrentGlobalPose[i]);
		XMMATRIX total = invBind * currPose;
		XMMATRIX invTotal = XMMatrixTranspose( total);
		XMFLOAT4X4 inv;
		XMStoreFloat4x4(&inv, invTotal);
	
		mBoneTransforms[i] = inv;
	}

    //
    // Update variables
    //
	ConstantBuffer cb;
	memset(&cb, 0, sizeof(cb));
	cb.mWorld = XMMatrixTranspose( GetTransform() );

	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;

	pImmediateContext->UpdateSubresource( mConstantBuffer, 0, NULL, &cb, 0, 0 );
	pImmediateContext->UpdateSubresource( mAnimBonesBuffer, 0, NULL, mBoneTransforms, 0, 0 );
    	
    pImmediateContext->VSSetConstantBuffers( 2, 1, &mConstantBuffer );
	pImmediateContext->VSSetConstantBuffers( 3, 1, &mAnimBonesBuffer );

    // Set vertex buffer

    UINT stride = sizeof( SimpleSkinnedVertex );
    UINT offset = 0;
    pImmediateContext->IASetVertexBuffers( 0, 1, &mVertexBuffer, &stride, &offset );
    pImmediateContext->IASetIndexBuffer( mIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
    pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


    // Render a triangle
    // Set the input layout
    pImmediateContext->IASetInputLayout( mVertexLayout );
    pImmediateContext->VSSetShader( mVertexShader, NULL, 0 );
    pImmediateContext->PSSetShader( mPixelShader, NULL, 0 );

	pImmediateContext->RSSetState(g_pRasterState);

    pImmediateContext->DrawIndexed( mMesh->mNumIndices, 0, 0 );

}

void DisplayMeshGPU::PlayAnimation(SkeletalAnimation* anim)
{
    mAnimation = anim;
    mAnimTime = 0.0f;
    if(anim && mMesh)
    {
        //init channel-map
        if(!mChannelMap)
        {
            mChannelMap = new (std::nothrow) unsigned char[mMesh->mNumBones];
        }
        if(mChannelMap)
        {

            for(int i = 0; i < mMesh->mNumBones; ++i)
            {
                mChannelMap[i] = -1;
                for(int j = 0; j < anim->mNumBones; ++j)
                {
                    if(strcmp(mMesh->mSkeleton[i].name, anim->mSkeleton[j].name) == 0)
                    {
                        mChannelMap[i] = j;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        if(mChannelMap) delete [] mChannelMap;
        mChannelMap = NULL;
    }
    
}

char* DisplayMeshGPU::GetPlayingAnimation() const
{
    static char none[] = "None";
    if(mAnimation)
    {
        return(mAnimation->mName);
    }
    else
    {
        return(none);
    }
}

float DisplayMeshGPU::GetCurrentAnimTime() const
{
    return(mAnimTime);
}

float DisplayMeshGPU::GetCurrentAnimFrame() const
{
    return(mCurrentFrame);
}
