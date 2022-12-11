#pragma once

#ifndef DITHERING_H
#define DITHERING_H

#include <vector>
#include <algorithm>


#include "olcPixelGameEngine.h"

/*

Header file with all the dithering algorithms

*/

using namespace std;

// Jarvis, Judice and Ninke
void JJN_Dithering(const olc::Sprite* src, olc::Sprite* dest, std::function<olc::Pixel(const olc::Pixel)> funcQuantize) {
	std::copy(src->pColData.begin(), src->pColData.end(), dest->pColData.begin());

	olc::vi2d vPixel;
	for (vPixel.y = 0; vPixel.y < src->height; vPixel.y++) {
		for (vPixel.x = 0; vPixel.x < src->width; vPixel.x++) {
			olc::Pixel op = dest->GetPixel(vPixel);
			olc::Pixel qp = funcQuantize(op);
			int32_t qError[3] = {
				op.r - qp.r,
				op.g - qp.g,
				op.b - qp.b
			};

			dest->SetPixel(vPixel, qp);

			auto UpdatePixel = [&vPixel, &qError, &dest](const olc::vi2d& vOffset, const float fErrorBias) {
				olc::Pixel p = dest->GetPixel(vPixel + vOffset);
				int32_t k[3] = {
					p.r + int32_t(float(qError[0] * fErrorBias)),
					p.g + int32_t(float(qError[1] * fErrorBias)),
					p.b + int32_t(float(qError[2] * fErrorBias))
				};

				dest->SetPixel(vPixel + vOffset, olc::Pixel(std::clamp(k[0], 0, 255), std::clamp(k[1], 0, 255), std::clamp(k[2], 0, 255)));
			};

			UpdatePixel({ 0,2 }, 5.0f / 48.0f);
			UpdatePixel({ 2,0 }, 5.0f / 48.0f);
			UpdatePixel({ 1,0 }, 7.0f / 48.0f);
			UpdatePixel({ 0,1 }, 7.0f / 48.0f);
			UpdatePixel({ -1,1 }, 5.0f / 48.0f);
			UpdatePixel({ 1,1 }, 5.0f / 48.0f);
			UpdatePixel({ 2,1 }, 3.0f / 48.0f);
			UpdatePixel({ -2,1 }, 3.0f / 48.0f);
			UpdatePixel({ -2,2 }, 1.0f / 48.0f);
			UpdatePixel({ 2,2 }, 1.0f / 48.0f);
			UpdatePixel({ 1,2 }, 3.0f / 48.0f);
			UpdatePixel({ -1,2 }, 3.0f / 48.0f);
		}
	}

}




void Floyd_Steinberg_Dithering(const olc::Sprite* src, olc::Sprite* dest, std::function<olc::Pixel(const olc::Pixel)> funcQuantize) {
	std::copy(src->pColData.begin(), src->pColData.end(), dest->pColData.begin());

	olc::vi2d vPixel;

	for (vPixel.y = 0; vPixel.y < src->height; vPixel.y++) {
		for (vPixel.x = 0; vPixel.x < src->width; vPixel.x++) {
			olc::Pixel op = dest->GetPixel(vPixel);
			olc::Pixel qp = funcQuantize(op);
			int32_t qError[3] = {
				op.r - qp.r,
				op.g - qp.g,
				op.b - qp.b
			};

			dest->SetPixel(vPixel, qp);

			auto UpdatePixel = [&vPixel, &qError, &dest](const olc::vi2d& vOffset, const float fErrorBias) {
				olc::Pixel p = dest->GetPixel(vPixel + vOffset);
				int32_t k[3] = { p.r + int32_t(float(qError[0] * fErrorBias)),
					p.g + int32_t(float(qError[1] * fErrorBias)),
					p.b + int32_t(float(qError[2] * fErrorBias))
				};

				dest->SetPixel(vPixel + vOffset, olc::Pixel(std::clamp(k[0], 0, 255), std::clamp(k[1], 0, 255), std::clamp(k[2], 0, 255)));
			};

			UpdatePixel({ 1,0 }, 7.0f / 16.0f);
			UpdatePixel({ -1,1 }, 3.0f / 16.0f);
			UpdatePixel({ 0,1 }, 5.0f / 16.0f);
			UpdatePixel({ 1,1 }, 1.0f / 16.0f);
		}
	}
}

void Stucki_Dithering(const olc::Sprite* src, olc::Sprite* dest, std::function<olc::Pixel(const olc::Pixel)> funcQuantize) {
	std::copy(src->pColData.begin(), src->pColData.end(), dest->pColData.begin());

	olc::vi2d vPixel;
	for (vPixel.y = 0; vPixel.y < src->height; vPixel.y++) {
		for (vPixel.x = 0; vPixel.x < src->width; vPixel.x++) {
			olc::Pixel op = dest->GetPixel(vPixel);
			olc::Pixel qp = funcQuantize(op);
			int32_t qError[3] = {
				op.r - qp.r,
				op.g - qp.g,
				op.b - qp.b
			};

			dest->SetPixel(vPixel, qp);

			auto UpdatePixel = [&vPixel, &qError, &dest](const olc::vi2d& vOffset, const float fErrorBias) {
				olc::Pixel p = dest->GetPixel(vPixel + vOffset);
				int32_t k[3] = { p.r + int32_t(float(qError[0] * fErrorBias)),
					p.g + int32_t(float(qError[1] * fErrorBias)),
					p.b + int32_t(float(qError[2] * fErrorBias))
				};

				dest->SetPixel(vPixel + vOffset, olc::Pixel(std::clamp(k[0], 0, 255), std::clamp(k[1], 0, 255), std::clamp(k[2], 0, 255)));
			};

			UpdatePixel({ 2,0 }, 4.0f / 42.0f);
			UpdatePixel({ 1,0 }, 8.0f / 42.0f);
			UpdatePixel({ 0,2 }, 4.0f / 42.0f);
			UpdatePixel({ 0,1 }, 8.0f / 42.0f);
			UpdatePixel({ -1,1 }, 4.0f / 42.0f);
			UpdatePixel({ 1,1 }, 4.0f / 42.0f);
			UpdatePixel({ 2,1 }, 2.0f / 42.0f);
			UpdatePixel({ -2,1 }, 2.0f / 42.0f);
			UpdatePixel({ -2,2 }, 1.0f / 42.0f);
			UpdatePixel({ 2,2 }, 1.0f / 42.0f);
			UpdatePixel({ 1,2 }, 2.0f / 42.0f);
			UpdatePixel({ -1,2 }, 2.0f / 42.0f);
		}
	}

}




vector<vector<float>> Generate_Threshold_Map(int n=2) {

	vector<vector<float>> mat = { { 0, 2}, { 3, 1} };

	vector<vector<float>> fin_mat;
	for (int i = 0; i < n; i++) {
		// 4 * mat

		vector<vector<float>> new_mat;

		for (int r = 0; r < mat.size(); r++) {
			vector<float> new_vec;
			for (int c = 0; c < mat.size(); c++) {
				new_vec.push_back(4.0f * mat[r][c]);
			}
			new_mat.push_back(new_vec);
		}

		// a00 - new_mat
		for (int r = 0; r < mat.size(); r++) {
			vector<float> new_vec;
			for (int c = 0; c < mat.size(); c++) {
				new_vec.push_back(new_mat[r][c]);
			}
			fin_mat.push_back(new_vec);
		}

		// a01 - new_mat + 2
		for (int r = 0; r < mat.size(); r++) {
			for (int c = 0; c < mat.size(); c++) {
				fin_mat[r].push_back(new_mat[r][c] + 2.0f);
			}
		}

		// a10 - new_mat + 3
		for (int r = 0; r < mat.size(); r++) {
			vector<float> new_vec;
			for (int c = 0; c < mat.size(); c++) {
				new_vec.push_back(new_mat[r][c] + 3.0f);
			}
			fin_mat.push_back(new_vec);
		}

		// a11 - new_mat + 1
		for (int r = mat.size(); r < 2 * mat.size(); r++) {
			for (int c = mat.size(); c < 2 * mat.size(); c++) {
				fin_mat[r].push_back(new_mat[r - mat.size()][c - mat.size()] + 1.0f);
			}
		}

		mat = fin_mat;
		fin_mat.clear();
	}
	// scale by 2**(2n+2))

	float scaling_factor = powf(2.0f, (float)(2 * n + 2));

	for (int i = 0; i < mat.size(); i++) {
		for (int j = 0; j < mat.size(); j++) {

			//cout << mat[i][j] << " ";
			mat[i][j] = mat[i][j] / scaling_factor;
			mat[i][j] -= 0.5f;
		}
		//cout << endl;
	}

	return mat;

}

void Ordered_Dithering(const olc::Sprite* src, olc::Sprite* dest, std::function<olc::Pixel(const olc::Pixel)> funcQuantize, int n=2) {

	// threshold map
	vector<vector<float>> threshold_map = Generate_Threshold_Map(3);

	std::copy(src->pColData.begin(), src->pColData.end(), dest->pColData.begin());

	// r val
	float rVal = 255.0f / 4.0f;
	int map_size = threshold_map.size();

	olc::vi2d vPixel;
	for (vPixel.y = 0; vPixel.y < src->height; vPixel.y++) {
		for (vPixel.x = 0; vPixel.x < src->width; vPixel.x++) {
			olc::Pixel op = dest->GetPixel(vPixel);

			op.r += rVal * threshold_map[vPixel.y % map_size][vPixel.x % map_size];
			op.g += rVal * threshold_map[vPixel.y % map_size][vPixel.x % map_size];
			op.b += rVal * threshold_map[vPixel.y % map_size][vPixel.x % map_size];

			olc::Pixel qp = funcQuantize(op);


			dest->SetPixel(vPixel, qp);


		}
	}

}

#endif // !DITHERING_H