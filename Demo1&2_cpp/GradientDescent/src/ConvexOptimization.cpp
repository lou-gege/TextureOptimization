#include "ConvexOptimization.h"
#include <iostream>

glm::mat4 ConvexOptimization::GetModelMatrix(std::vector<glm::vec4>& localCoords, std::vector<glm::vec4>& clipCoords, glm::mat4& VPMatrix)
{
	int vertexNum = localCoords.size();
	glm::mat4 resultModelMatrix = glm::mat4(1.0f);

	std::vector<glm::vec4> VPInverseClipCoords(vertexNum);
	glm::mat4 VPInverse = glm::inverse(VPMatrix);
	for (int i = 0; i < vertexNum; i++)
	{
		VPInverseClipCoords[i] = VPInverse * clipCoords[i];
	}

	std::vector<float> uBar(vertexNum);
	for (int i = 0; i < 4; i++)
	{
		while (1)
		{
			glm::mat4 tResultModelMatrix = glm::transpose(resultModelMatrix);
			double lossFuncVal = 0.0;
			for (int k = 0; k < vertexNum; k++)
			{
				uBar[k] = glm::dot(tResultModelMatrix[i], localCoords[k]);
				lossFuncVal += pow(uBar[k] - VPInverseClipCoords[k][i], 2);
			}
			//std::cout << lossFuncVal << std::endl;
			if (lossFuncVal < EPS) break;

			for (int j = 0; j < 4; j++) {
				double gradient = 0.0;
				for (int k = 0; k < vertexNum; k++)
				{
					gradient += localCoords[k][j] * (uBar[k] - VPInverseClipCoords[k][i]);
				}
				resultModelMatrix[j][i] -= gradient * STEP;
			}
		}
	}

	return resultModelMatrix;
}
