#include "FBXMeshLoader.h"
#include "FBXAnimationLoader.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char **argv)
{
	if(argc != 4)
	{
		std::cout << "Use [Mesh|Animation] [FileName] [FileOutput] " << std::endl;
		return 1;
	}

	std::string type = argv[1];
	std::string input = argv[2];
	std::string output = argv[3];
	
	if(type == "Mesh")
	{
		FBXMeshLoader meshLoader;
		meshLoader.LoadMesh(input);
		meshLoader.WriteToFile(output);
	}
	else if(type == "Animation")
	{
		FBXAnimationLoader animLoader;
		animLoader.LoadAnimation(input);		
		animLoader.WriteToFile(output);
	}
}