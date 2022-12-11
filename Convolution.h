#pragma once

#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <vector>
#include <algorithm>


#include "olcPixelGameEngine.h"

using namespace std;

void Generate_Gaussian_Kernel(vector<vector<float>>& kernel, float stdDev = 1.0f) {
	int dim = kernel.size() / 2;

	float kernelSum = 0.0f;
	float pi = 3.14159265358979323846f;

	float variance = stdDev * stdDev;
	for (int i = -dim; i <= dim; i++) {
		for (int j = -dim; j <= dim; j++) {
			kernel[i + dim][j + dim] = expf(-(float(i * i + j * j)) / (2.0f * variance)) / (2.0f * pi * variance);
			kernelSum += kernel[i + dim][j + dim];
		}
	}

	// Normalize the kernel
	for (int i = -dim; i <= dim; i++) {
		for (int j = -dim; j <= dim; j++) {
			kernel[i + dim][j + dim] /= kernelSum;
		}
	}
}

void Convolution(const olc::Sprite* src, olc::Sprite* dest, vector<vector<float>> &kernel) {

	int nDim = kernel.size();
	int dim = nDim / 2;
	std::copy(src->pColData.begin(), src->pColData.end(), dest->pColData.begin());

	olc::vi2d vPixel;
	for (vPixel.y = 0; vPixel.y < src->height; vPixel.y++) {
		for (vPixel.x = 0; vPixel.x < src->width; vPixel.x++) {
			olc::Pixel op = dest->GetPixel(vPixel);

			olc::Pixel cp = op;
			float conv_sum[3] = { 0.0f, 0.0f, 0.0f };
			for (int i = -dim; i <= dim; i++) {
				for (int j = -dim; j <= dim; j++) {
					olc::vi2d vOffset = { i,j };
					olc::Pixel tp = src->GetPixel(vPixel + vOffset);
					conv_sum[0] += float(tp.r) * kernel[i + dim][j + dim];
					conv_sum[1] += float(tp.g) * kernel[i + dim][j + dim];
					conv_sum[2] += float(tp.b) * kernel[i + dim][j + dim];
				}
			}

			dest->SetPixel(vPixel, olc::Pixel(std::clamp(int(conv_sum[0]), 0, 255), std::clamp(int(conv_sum[1]), 0, 255), std::clamp(int(conv_sum[2]), 0, 255)));
		}
	}
}


#endif // !CONVOLUTION_H
