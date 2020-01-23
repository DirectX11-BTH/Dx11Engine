#include "ObjParser.h"

struct float3
{
	float x, y, z = 0;
};

struct float2
{
	float x, y = 0;
};

Mesh ObjParser::readFromObj(std::string fileName)
{
	std::vector<float3> loadedVertsCoords;
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

				loadedVertsCoords.push_back(float3{ v1, v2, v3 });

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

				int vertPosIndex = -1;
				int vertTexCoordIndex = -1;
				int vertNormalIndex = -1;

				line.erase(0, 1);//Removes f

				
				for (int k = 0; k < 3; k++) // for each of our v/vt/vn
				{
					Vertex tempVert; //Build new vert for each

					std::string tempString;
					for (int i = 0; &line[i] != "/" && &line[i] != " "; i++)
					{
						tempString += line[i];

						if (k == 0) //It's v index
						{
							if (tempString != "") //Make sure there is a number
							{
								tempVert.x = loadedVertsCoords.at((std::stoi(tempString))).x;
								tempVert.y = loadedVertsCoords.at((std::stoi(tempString))).y;
								tempVert.z = loadedVertsCoords.at((std::stoi(tempString))).z;
							}
								
							//loadedVertsCoords.at((std::stoi(tempString))
						}
						if (k == 1) //It's vt index
						{
							//loadedVertTextureCoords[std::stoi(tempString)];

							if (tempString != "")
							{
								tempVert.u = loadedVertTextureCoords.at((std::stoi(tempString))).x;
								tempVert.v = loadedVertTextureCoords.at((std::stoi(tempString))).y;
							}
						}
						if (k == 2) //It's vn index
						{
							loadedVertNormals[std::stoi(tempString)];
							if (tempString != "")
							{
								tempVert.nx = loadedVertNormals.at((std::stoi(tempString))).x;
								tempVert.nx = loadedVertNormals.at((std::stoi(tempString))).y;
								tempVert.nx = loadedVertNormals.at((std::stoi(tempString))).z;
							}
						}
							
						tempString.erase(0, tempString.length()); //removes number and slash from string
					}
					readMesh.vertices.push_back(tempVert);
				}
			}
				

		}
	}


	return readMesh;
}
