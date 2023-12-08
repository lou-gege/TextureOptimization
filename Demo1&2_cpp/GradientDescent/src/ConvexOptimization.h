#pragma once

#include <glm/glm.hpp>

#include <vector>

#define EPS 0.0000001f
#define STEP 0.0005f

class ConvexOptimization
{
public:
	static glm::mat4 GetModelMatrix(std::vector<glm::vec4>& localCoords, std::vector<glm::vec4>& clipCoords, glm::mat4& VPMatrix);
};