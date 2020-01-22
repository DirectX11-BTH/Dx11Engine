#include "ObjParser.h"

struct float3
{
	float x, y, z = 0;
};

struct float2
{
	float x, y = 0;
};

Mesh ObjParser::readFromObj(std::string fileName, EngineObject& parentObject)
{
	std::vector<float3> loadedVerts;
	std::vector<float3> loadedVertNormals;
	std::vector<float2> loadedVertTextureCoords;

	std::ifstream objFile;
	objFile.open(fileName);
	std::string line;
	Mesh readMesh;

	char ignoreChar;
	char ignoreChar2;
	float v1, v2, v3;

	while(std::getline(objFile,line) && line != "")
	{
		if (&line[0] != "#")
		{
			if (&line[0] == "v")
			{
				objFile >> ignoreChar >> v1 >> v2 >> v3; //Will read v, x, y, z ignore v

				loadedVerts.push_back(float3{ v1, v2, v3 });

				//Vertex tempVert{ v1, v2, v3 }; //x y z
				//readMesh.vertices.push_back(tempVert);

				//loadedVerts.push_back(tempVert);
			}
			else if(&line[0] == "g")
			{
				//It's a group, irrelevant to us
			}
			else if(line.compare(0, 1,"vn") == 0)
			{
				//its a normal
				objFile >> ignoreChar >> ignoreChar2 >> v1 >> v2 >> v3; //Will read v, x, y, z ignore v
				loadedVertNormals.push_back(float3{ v1, v2, v3 });
			}
			else if (line.compare(0, 1, "vt") == 0)
			{
				//texture coord
				objFile >> ignoreChar >> ignoreChar2 >> v1 >> v2;
				loadedVertTextureCoords.push_back(float2{v1, v2});
			}
			else if (&line[0] == "f")
			{
				//assemble vertices
				
			}

		}
	}


	return readMesh;
}
