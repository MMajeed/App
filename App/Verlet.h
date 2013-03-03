#ifndef __Verlet__
#define __Verlet__

#include "PlyFile.h"
#include <set>

class Verlet : public PlyFile
{
public:	
	Verlet();
	Verlet(PlyFile);
	
	virtual void Init();
	virtual void InitRastersizerState(ID3D11Device* device);
	virtual void UpdateDrawing(float delta);
	virtual void SetupDraw();

	struct Constrain
	{
		std::size_t Vertex1;
		std::size_t Vertex2;
		float RestingLength;
		Constrain(std::size_t v1 = 0, std::size_t v2 = 0, float rl = 0.0f)
			: Vertex1(v1), Vertex2(v2), RestingLength(rl){}
	};
	std::vector<Constrain> constraints;

	void Randomize(float delta);

	static Verlet* Spawn(std::map<std::string, std::string> info);
};


#endif //__PlyFile__
