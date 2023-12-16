#include <iostream>
#include <random>
#include <chrono>

#include "stb_image/stb_image.h"
#include "CPUSampler.h"
#include "GPUSampler.h"

int main()
{
	//const std::string& path = "assets/Checkerboard.png";
	const std::string& path = "assets/container.jpg";
	//const float u = 0.125f, v = 0.125f;
	const float u = 1.32841f, v = 0.151858f;
	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	if (!data) {
		std::cerr << "Failed to load image: " << path << std::endl;
		exit(1);
	}

	std::cout << "width: " << width << std::endl;
	std::cout << "height: " << height << std::endl;
	std::cout << "channels: " << channels << std::endl;
	std::cout << "uv: " << u << " " << v << std::endl;

	CPUSampler* cpuSampler = new CPUSampler(data, width, height, channels);
	auto cpuColor = cpuSampler->Sample(u, v);
	std::cout << "CPU Sampler Result: ";
	for (auto& c : cpuColor)
	{
		std::cout << (int)c << " ";
	}
	std::cout << std::endl;

	GPUSampler* gpuSampler = new GPUSampler(data, width, height, channels);
	auto gpuColor = gpuSampler->Sample(u, v);
	std::cout << "GPU Sampler Result: ";
	for (auto& c : gpuColor)
	{
		std::cout << (int)c << " ";
	}
	std::cout << std::endl;

	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 gen(seed);
	std::uniform_real_distribution<float> dis(-1.0f, 2.0f);

	//std::cout << "randomU" << dis(gen) << std::endl;
	//std::cout << "randomV" << dis(gen) << std::endl;

	//const int totalNum = 1000;
	//int count = 0;

	//for (int i = 0; i < totalNum; i++)
	//{
	//	float randomU = dis(gen);
	//	float randomV = dis(gen);

	//	auto rCpuColor = cpuSampler->Sample(randomU, randomV);
	//	auto rGpuColor = gpuSampler->Sample(randomU, randomV);

	//	for (int k = 0; k < rCpuColor.size(); k++)
	//	{
	//		if (rCpuColor[k] != rGpuColor[k])
	//		{
	//			std::cout << "Wrong uv: " << randomU << " " << randomV << std::endl;
	//			std::cout << "CPU Sampler Result: ";
	//			for (auto& c : rCpuColor)
	//			{
	//				std::cout << (int)c << " ";
	//			}
	//			std::cout << std::endl;
	//			std::cout << "GPU Sampler Result: ";
	//			for (auto& c : rGpuColor)
	//			{
	//				std::cout << (int)c << " ";
	//			}
	//			std::cout << std::endl;
	//			//if (randomU > 0.0f && randomU < 1.0f && randomV>0.0f && randomV < 1.0f)
	//			//{
	//				count++;
	//			//}
	//		}
	//		break;
	//	}
	//}

	//std::cout << "All Same!" << count << std::endl;

	stbi_image_free(data);

}