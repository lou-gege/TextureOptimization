#include "ConvexOptimization.h"

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void PrintMat4(glm::mat4& m)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%10.6f ", m[j][i]);
		}
		std::cout << std::endl;
	}
}

int main()
{
	std::vector<float> vertices = {          
		0.0f, 0.5f, 0.0f,    // 0：头部顶点
		0.258819f, 0.5f, 0.0f,    // 1
		0.5f, 0.5f, 0.0f,    // 2
		0.707107f, 0.5f, 0.0f,    // 3
		// 上半球体
		0.5f, 0.353553f, 0.0f,    // 4
		0.353553f, 0.353553f, 0.353553f,    // 5
		0.0f, 0.353553f, 0.5f,    // 6
		-0.353553f, 0.353553f, 0.353553f,    // 7
		-0.5f, 0.353553f, 0.0f,    // 8
		-0.353553f, 0.353553f, -0.353553f,    // 9
		0.0f, 0.353553f, -0.5f,    // 10
		0.353553f, 0.353553f, -0.353553f,    // 11
		// 下半球体
		0.5f, -0.353553f, 0.0f,    // 12
		0.353553f, -0.353553f, 0.353553f,    // 13
		0.0f, -0.353553f, 0.5f,    // 14
		-0.353553f, -0.353553f, 0.353553f,    // 15
		-0.5f, -0.353553f, 0.0f,    // 16
		-0.353553f, -0.353553f, -0.353553f,    // 17
		0.0f, -0.353553f, -0.5f,    // 18
		0.353553f, -0.353553f, -0.353553f    // 19
	};
	glm::mat4 originModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 1.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 0, 1)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));

	std::cout << "Origin Model Matrix:" << std::endl;
	PrintMat4(originModelMatrix);

	glm::mat4 ViewProjectionMatrix = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f) * glm::mat4(1.0f);

	std::vector<glm::vec4> inputLocalCoords, inputClipCoords;
	for (int i = 0; i < vertices.size(); i += 3)
	{
		glm::vec4 localCoord = { vertices[i], vertices[i + 1], vertices[i + 2], 1.0f };
		inputLocalCoords.emplace_back(localCoord);
		inputClipCoords.emplace_back(ViewProjectionMatrix * originModelMatrix * localCoord);
	}

	auto resultModelMatrix = ConvexOptimization::GetModelMatrix(inputLocalCoords, inputClipCoords, ViewProjectionMatrix);
	std::cout << std::endl << "Result Model Matrix:" << std::endl;
	PrintMat4(resultModelMatrix);

}