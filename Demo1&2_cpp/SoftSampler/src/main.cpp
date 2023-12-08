#include <iostream>
#include "stb_image/stb_image.h"
#include "CPUSampler.h"
#include "GPUSampler.h"

int main()
{
	//const std::string& path = "assets/Checkerboard.png";
	const std::string& path = "assets/container.jpg";
	const float u = 0.68f, v = 0.891f;
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

	stbi_image_free(data);

}