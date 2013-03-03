#ifndef __Transparent__
#define __Transparent__

#include "PlyFile.h"

class Transparent : public PlyFile
{
public:	
	Transparent();
	Transparent(PlyFile base);
	static Transparent* Spawn(std::map<std::string, std::string> info);

	std::pair<std::string, ID3D11BlendState*> 		pTransparent;

	virtual void DrawObject();

	virtual void LoadTransparency(ID3D11Device* device);

	virtual float GetOrder();

	virtual void Init();
};


#endif //__Transparent__
