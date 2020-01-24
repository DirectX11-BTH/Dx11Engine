#pragma once
#include "EngineObject.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class ObjParser
{
public:
	static Mesh readFromObj(std::string fileName);
};