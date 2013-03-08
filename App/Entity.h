#ifndef ENTITY_H
#define ENTITY_H

#include <d3d11.h>
#include <xnamath.h>

class Entity
{
public:
    Entity();
    virtual ~Entity();

    virtual HRESULT Initialize();
    virtual void Update(float delta);
    virtual void Draw();

    virtual XMMATRIX GetTransform() const;
    virtual XMFLOAT3 GetPosition() const;
    virtual void SetPosition(XMFLOAT3 p);
    virtual XMFLOAT4 GetRotation() const; //quat
    virtual void SetRotation(XMFLOAT4 r);

protected:
    XMFLOAT3 mPosition;
    XMFLOAT4 mRotation;
};

#endif
