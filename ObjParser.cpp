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

	float v1, v2, v3;

	while(!objFile.eof() && std::getline(objFile,line))
	{
		//std::cout << line << std::endl;
		if (line.size() > 0)
		{
			if (line.at(0) != '#')
			{
				//std::cout << line.at(0) << std::endl;
				if (line.find("v ") != -1)//(line.compare(0, 2, "v ") == 0) //Does not trigger correctly
				{
					line.erase(0, 2); //Removes v and spaces
					int tempLen = line.find(' ');
					std::string tempStr = line.substr(0, tempLen);
					v1 = (float)std::stof(tempStr)/2;
					
					line.erase(0, tempLen+1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v2 = (float)std::stof(tempStr)/2;

					line.erase(0, tempLen + 1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v3 = (float)std::stof(tempStr)/2;

					// this reads next line, which makes us only read half the verts
					//objFile >> ignoreChar >> v1 >> v2 >> v3; //Will read v, x, y, z ignore v 
					//v1 = v2 = v3 = 0;
					loadedVertsCoords.push_back(float3{ v1, v2, v3 });
					//std::cout << "VertexPos: " << v1 << "\t" << v2 << "\t" << v3 << std::endl;
				}
				else if (line.at(0) == 'g')
				{
					//It's a group, irrelevant to us
				}
				else if (line.compare(0, 2, "vn") == 0)
				{
					line.erase(0, 3); //Removes vn and spaces
					int tempLen = line.find(' ');
					std::string tempStr = line.substr(0, tempLen);
					v1 = std::stof(tempStr);

					line.erase(0, tempLen + 1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v2 = std::stof(tempStr);

					line.erase(0, tempLen + 1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v3 = std::stof(tempStr);

					//its a normal
					//objFile >> ignoreChar >> ignoreChar2 >> v1 >> v2 >> v3; //Will read v, x, y, z ignore v
					loadedVertNormals.push_back(float3{ v1, v2, v3 });
				}
				else if (line.find("mtllib") != -1) //Parse mtl, we only support one material
				{
					line.erase(0, 7); //Deletes usemtl + spaces
					std::ifstream mtlFile;
					//mtlFile.open(("./TestModel/cube.mtl"));
					mtlFile.open(("./TestModel/"+line));
					std::string mtlLine;

					assert(mtlFile.is_open());

					while (!mtlFile.eof() && std::getline(mtlFile, mtlLine))
					{
						if (mtlLine.find("Ka") != -1) //Ambient light color
						{
							mtlLine.erase(0, mtlLine.find("Ka")+3);
							int numToEmpty = mtlLine.find(' ');
							float r = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float g = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float b = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							readMesh.ambientMeshColor = { r, g, b };
						}

						if (mtlLine.find("Kd ") != -1 && mtlLine.find("map") == -1) //Diffuse light color
						{
							mtlLine.erase(0, mtlLine.find("Kd") + 3);
							int numToEmpty = mtlLine.find(' ');
							float r = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float g = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float b = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							readMesh.diffueMeshColor = { r, g, b };
						}

						if (mtlLine.find("Ks") != -1) //Specular light color
						{
							mtlLine.erase(0, mtlLine.find("Ks") + 3);
							int numToEmpty = mtlLine.find(' ');
							float r = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float g = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							numToEmpty = mtlLine.find(' ');
							float b = stof(mtlLine.substr(0, numToEmpty));
							mtlLine.erase(0, numToEmpty + 1);

							readMesh.specularMeshColor = { r, g, b };
						}

						if (mtlLine.find("Ns") != -1) //Shininess of material
						{
							mtlLine.erase(0, mtlLine.find("Ns")+3);
							float shininess = 0;

							shininess = stof(mtlLine);
							readMesh.shininess = shininess;
						}

						if (mtlLine.find("map_Kd") != -1) //Texture
						{
							mtlLine.erase(0, mtlLine.find("map_Kd")+7);

							readMesh.textureName = mtlLine;
						}
					}

					mtlFile.close();
				}
				else if (line.compare(0, 2, "vt") == 0)
				{
					line.erase(0, 3); //Removes vt and spaces
					int tempLen = line.find(' ');
					std::string tempStr = line.substr(0, tempLen);
					v1 = std::stof(tempStr);

					line.erase(0, tempLen + 1);
					tempLen = line.find(' ');
					tempStr = line.substr(0, tempLen);
					v2 = std::stof(tempStr);

					//texture coord
					//objFile >> ignoreChar >> ignoreChar2 >> v1 >> v2;
					loadedVertTextureCoords.push_back(float2{ v1, v2 });
				}
				else if (line.at(0) == 'f')
				{
					//assemble vertices

					line.erase(0, 2);//Removes f and two first spaces


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
								tempLen = line.find(" ");

							}
							tempString = line.substr(0, tempLen); //Want to go up to the slash, not including it
							
							if (i == 0) //It's v index
							{
								if (tempLen != 0) //Make sure there is a number
								{
									//std::cout << "Reading v index: " << (std::stof(tempString)) - 1 << std::endl;
									tempVert.x = loadedVertsCoords.at((std::stoi(tempString))-1).x; //-1 to convert from index starting with 1 to 0
									tempVert.y = loadedVertsCoords.at((std::stoi(tempString))-1).y;
									tempVert.z = loadedVertsCoords.at((std::stoi(tempString))-1).z;
								}

								//loadedVertsCoords.at((std::stoi(tempString))
							}
							if (i == 1) //It's vt index
							{
								//loadedVertTextureCoords[std::stoi(tempString)];

								if (tempString.length() != 0)//(tempLen != 0)
								{
									//std::cout << "Reading vt index: " << (std::stoi(tempString)) - 1 << std::endl;
									tempVert.u = loadedVertTextureCoords.at((std::stoi(tempString))-1).x;
									tempVert.v = loadedVertTextureCoords.at((std::stoi(tempString))-1).y;
								}
								else
								{
									tempVert.u = 0;
									tempVert.v = 0;
								}
							}
							if (i == 2) //It's vn index
							{
								//loadedVertNormals[std::stoi(tempString)];
								if (tempString.length() != 0)//(tempLen != 0)
								{
									//std::cout << "Reading vn index: " << (std::stoi(tempString)) - 1 << std::endl;
									tempVert.nx = loadedVertNormals.at((std::stoi(tempString))-1).x;
									tempVert.ny = loadedVertNormals.at((std::stoi(tempString))-1).y;
									tempVert.nz = loadedVertNormals.at((std::stoi(tempString))-1).z;
								}
							}

							std::cout << "Before " << line << std::endl;
							line.erase(0, tempLen+1); //removes number and slash from string
							if(line.at(0) == ' ')
								line.erase(0,1); //Remove the last space in between groups of v/vt/vn
							std::cout << "After " << line << std::endl;
						}
						tempVert.r = 0.5;
						tempVert.g = 1;
						tempVert.b = 1;

						readMesh.vertices.push_back(tempVert);
					}
				}
			}
		}
		
	}

	objFile.close();
	readMesh.nrOfVertices = readMesh.vertices.size();

	return readMesh;
}
