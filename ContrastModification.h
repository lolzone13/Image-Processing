#pragma once

#ifndef CONTRASTMODIFICATION_H
#define CONTRASTMODIFICATION_H

#include <vector>
#include <algorithm>


#include "olcPixelGameEngine.h"


using namespace std;

void Histogram_Equalization(const olc::Sprite* src, olc::Sprite* dest) {//, olc::Sprite* hc) {
	std::copy(src->pColData.begin(), src->pColData.end(), dest->pColData.begin());
	//std::copy(src->pColData.begin(), src->pColData.end(), hc->pColData.begin());

	// make the high contrast image
	//vector<float> maxPixel = { 0.0f, 0.0f, 0.0f };
	//vector<float> minPixel = {255.0f, 255.0f, 255.0f};
	//olc::vi2d vP;
	//for (vP.y = 0; vP.y < src->height; vP.y++) {
	//	for (vP.x = 0; vP.x < src->width; vP.x++) {
	//		olc::Pixel op = src->GetPixel(vP);

	//		maxPixel[0] = max(maxPixel[0], float(op.r));
	//		maxPixel[1] = max(maxPixel[1], float(op.g));
	//		maxPixel[2] = max(maxPixel[2], float(op.b));

	//		minPixel[0] = min(minPixel[0], float(op.r));
	//		minPixel[1] = min(minPixel[1], float(op.g));
	//		minPixel[2] = min(minPixel[2], float(op.b));
	//	}
	//}


	// first compute scaling factor alpha
	float numPixels = float(src->height) * float(src->width);
	float alpha = 255.0f / numPixels;

	// Create histogram
	vector<vector<int>> histogram(3, vector<int>(256, 0));

	olc::vi2d vPixel;
	for (vPixel.y = 0; vPixel.y < src->height; vPixel.y++) {
		for (vPixel.x = 0; vPixel.x < src->width; vPixel.x++) {

			//olc::Pixel hp = hc->GetPixel(vPixel);
			//hp.r = uint8_t(std::clamp(255.0f * ((float(hp.r) - minPixel[0]) / (maxPixel[0] - minPixel[0])), 0.0f, 255.0f));
			//hp.g = uint8_t(std::clamp(255.0f * ((float(hp.g) - minPixel[1]) / (maxPixel[1] - minPixel[1])), 0.0f, 255.0f));
			//hp.b = uint8_t(std::clamp(255.0f * ((float(hp.b) - minPixel[2]) / (maxPixel[2] - minPixel[2])), 0.0f, 255.0f));

			//hc->SetPixel(vPixel, hp);

			olc::Pixel px = src->GetPixel(vPixel);

			histogram[0][px.r]++;
			histogram[1][px.g]++;
			histogram[2][px.b]++;
		}
	}

	// Create Look-Up Table
	vector<vector<float>> lookupTable(3, vector<float>(256, 0.0f));

	lookupTable[0][0] = alpha * histogram[0][0];
	lookupTable[1][0] = alpha * histogram[1][0];
	lookupTable[2][0] = alpha * histogram[2][0];


	for (int i = 1; i < 256; i++) {
		lookupTable[0][i] = lookupTable[0][i - 1] + alpha * histogram[0][i];
		lookupTable[1][i] = lookupTable[1][i - 1] + alpha * histogram[1][i];
		lookupTable[2][i] = lookupTable[2][i - 1] + alpha * histogram[2][i];
	}

	for (vPixel.y = 0; vPixel.y < src->height; vPixel.y++) {
		for (vPixel.x = 0; vPixel.x < src->width; vPixel.x++) {
			olc::Pixel px = dest->GetPixel(vPixel);

			px.r = lookupTable[0][px.r];
			px.g = lookupTable[1][px.g];
			px.b = lookupTable[2][px.b];


			dest->SetPixel(vPixel, px);
		}
	}

}



#endif // !CONTRASTMODIFICATION