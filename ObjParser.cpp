#include "ObjParser.h"
#include <algorithm>
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

	while(!objFile.eof() && std::getline(objFile,line))
	{
		//std::cout << line << std::endl;
		if (line.size() > 0)
		{
			if (line.at(0) != '#')
			{
				std::cout << line.at(0) << std::endl;
				if (line.find("v ") != -1)//(line.compare(0, 2, "v ") == 0) //Does not trigger correctly
				{
					line.erase(0, 3); //Removes v and spaces
					int tempLen = line.find(' ');
					std::string tempStr = line.substr(0, tempLen);
					v1 = std::stoi(tempStr);
					
					line.erase(0, tempLen+2);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v2 = std::stoi(tempStr);

					line.erase(0, tempLen + 2);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v3 = std::stoi(tempStr);

					// this reads next line, which makes us only read half the verts
					//objFile >> ignoreChar >> v1 >> v2 >> v3; //Will read v, x, y, z ignore v 
					//v1 = v2 = v3 = 0;
					loadedVertsCoords.push_back(float3{ v1, v2, v3 });
					std::cout << "VertexPos: " << v1 << "\t" << v2 << "\t" << v3 << std::endl;
				}
				else if (line.at(0) == 'g')
				{
					//It's a group, irrelevant to us
				}
				else if (line.compare(0, 2, "vn") == 0)
				{
					//its a normal
					objFile >> ignoreChar >> ignoreChar2 >> v1 >> v2 >> v3; //Will read v, x, y, z ignore v
					loadedVertNormals.push_back(float3{ v1, v2, v3 });
				}
				else if (line.compare(0, 2, "vt") == 0)
				{
					//texture coord
					objFile >> ignoreChar >> ignoreChar2 >> v1 >> v2;
					loadedVertTextureCoords.push_back(float2{ v1, v2 });
				}
				else if (line.at(0) == 'f')
				{
					//assemble vertices

					int vertPosIndex = -1;
					int vertTexCoordIndex = -1;
					int vertNormalIndex = -1;

					line.erase(0, 3);//Removes f and two first spaces


					//std::replace(line.begin(), line.end(), '/', ' '); / replace all / with \t 
					//std::cout << line << std::endl;

					for (int k = 0; k < 3; k++) //Loops 3 times, because 3 v / vt / vn groups
					{
						Vertex tempVert; //Build new vert for each

						std::string tempString;
						for (int i = 0; i < 3; i++) //for each of our v / vt / vn
						{
							int tempLen = line.find('/'); // ger oss v/vn/vt
							if (line.find(' ') < tempLen)
							{
								tempLen = line.find("  ");

							}
							tempString = line.substr(0, tempLen); //Want to go up to the slash, not including it
							
							if (i == 0) //It's v index
							{
								if (tempLen != 0) //Make sure there is a number
								{
									tempVert.x = loadedVertsCoords.at((std::stoi(tempString))).x;
									tempVert.y = loadedVertsCoords.at((std::stoi(tempString))).y;
									tempVert.z = loadedVertsCoords.at((std::stoi(tempString))).z;
								}

								//loadedVertsCoords.at((std::stoi(tempString))
							}
							if (i == 1) //It's vt index
							{
								//loadedVertTextureCoords[std::stoi(tempString)];

								if (tempLen != 0)
								{
									tempVert.u = loadedVertTextureCoords.at((std::stoi(tempString))).x;
									tempVert.v = loadedVertTextureCoords.at((std::stoi(tempString))).y;
								}
							}
							if (i == 2) //It's vn index
							{
								loadedVertNormals[std::stoi(tempString)];
								if (tempLen != 0)
								{
									tempVert.nx = loadedVertNormals.at((std::stoi(tempString))).x;
									tempVert.ny = loadedVertNormals.at((std::stoi(tempString))).y;
									tempVert.nz = loadedVertNormals.at((std::stoi(tempString))).z;
								}
							}

							std::cout << "Before " << line << std::endl;
							line.erase(0, tempLen+1); //removes number and slash from string
							if(line.at(0) == ' ')
								line.erase(0,1); //Remove the last space in between groups of v/vt/vn
							std::cout << "After " << line << std::endl;
						}
						readMesh.vertices.push_back(tempVert);
					}
				}
			}
		}
		
	}
	return readMesh;
}
