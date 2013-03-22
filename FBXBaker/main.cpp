#include "FBXMeshLoader.h"
#include "FBXAnimationLoader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
int main(int argc, char **argv)
{
	if(argc > 4)
	{
		std::cout << "Use Mesh [FileName] [FileOutput] " << std::endl
				  << "OR Animation [FileName] [FileOutput] [T|F] [Start] [End]" << std::endl;
				;
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
		bool normalize = false;
		int start = -1;
		int end = -1;
		if(argc <= 5)
		{
			normalize = argv[4] == "T" ? true : false;;
		}
		if(argc == 7)
		{ 
			std::stringstream ss;
			ss << argv[5];
			ss >> start;

			ss.clear();
			ss << argv[6];
			ss >> end;
		}
		FBXAnimationLoader animLoader;
		animLoader.LoadAnimation(input);		
		animLoader.WriteToFile(output);
	}
}