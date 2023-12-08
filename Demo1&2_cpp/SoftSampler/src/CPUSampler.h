#pragma once

#include <vector>
#include <string>

class CPUSampler
{
public:
	CPUSampler(unsigned char* data, int width, int height, int channels);
	CPUSampler(const std::string& path);
	~CPUSampler() = default;

	std::vector<uint8_t> Sample(float u, float v);
private:
	unsigned char* m_data;
	int m_width, m_height, m_channels;
};