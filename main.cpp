#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

#include "Dithering.h"

#include <iostream>



using namespace std;

class Dithering : public olc::PixelGameEngine {

	public:
		Dithering() {
			sAppName = "Dithering Algorithms";
		}

		olc::TransformedView tv;

		std::unique_ptr<olc::Sprite> m_pImage;
		std::unique_ptr<olc::Sprite> m_pQuantised;
		std::unique_ptr<olc::Sprite> m_pDithered;

	public: 

		void Convolution(const olc::Sprite* src, olc::Sprite* dest, vector<vector<float>> kernelInput = {}) {
			//vector<vector<float>> kernel = { 
			//	{0.0f, 0.125f, 0.0f }, 
			//	{0.125f, 0.5f, 0.125f}, 
			//	{0.0f, 0.125f, 0.0f}
			//};
			vector<vector<float>> kernel = {
				{0.111f, 0.111f, 0.111f },
				{0.111f, 0.111f, 0.111f},
				{0.111f, 0.111f, 0.111f}
			};
			vector<vector<float>> kernelBlur = {
				{0,    0,    0,      0,      0,      0,      0},
				{0,    2025, 6120,   8145,   6120,   2025,   0},
				{0,    6120, 18496,  24616,  18496,  6120,   0},
				{0,    8145, 24616,  32761,  24616,  8145,   0},
				{0,    6120, 18496,  24616,  18496,  6120,   0},
				{0,    2025, 6120,   8145,   6120,   2025,   0},
				{0,    0,    0,      0,      0,      0,      0}
			};
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

		bool OnUserCreate() override {

			tv.Initialise({ ScreenWidth(), ScreenHeight() });

			cout << "Image Processing!!!" << endl;
			m_pImage = std::make_unique<olc::Sprite>("image.jpg");
			m_pQuantised = std::make_unique<olc::Sprite>(m_pImage->width, m_pImage->height);
			m_pDithered = std::make_unique<olc::Sprite>(m_pImage->width, m_pImage->height);

			auto Convert_RGB_to_Grayscale = [](const olc::Pixel in) {
				uint8_t grayscale = uint8_t(0.2162f * float(in.r) + (0.7152f) * float(in.g) + 0.0722f * float(in.b));

				return olc::Pixel(grayscale, grayscale, grayscale);
			};

			// transform to grayscale
			//std::transform(m_pImage->pColData.begin(), m_pImage->pColData.end(), m_pImage->pColData.begin(), Convert_RGB_to_Grayscale);


			// quantize grayscale

			auto Quantize_Grayscale_1bit = [](const olc::Pixel in) {
				return in.r < 128 ? olc::BLACK : olc::WHITE;
			};

			auto Quantize_RGB_nbit = [](const olc::Pixel in) {
				constexpr int nBits = 2;
				constexpr float fLevels = (1 << nBits) - 1;
				uint8_t cr = uint8_t(std::clamp(std::round(float(in.r) / 255.0f * fLevels) / fLevels * 255.0f, 0.0f, 255.0f));
				uint8_t cb = uint8_t(std::clamp(std::round(float(in.g) / 255.0f * fLevels) / fLevels * 255.0f, 0.0f, 255.0f));
				uint8_t cg = uint8_t(std::clamp(std::round(float(in.b) / 255.0f * fLevels) / fLevels * 255.0f, 0.0f, 255.0f));
				return olc::Pixel(cr, cb, cg);
			};

			std::transform(m_pImage->pColData.begin(), m_pImage->pColData.end(), m_pQuantised->pColData.begin(), Quantize_RGB_nbit);

			// Error Diffusion Dithering
			//Floyd_Steinberg_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_RGB_nbit);
			//JJN_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_RGB_nbit);
			//Stucki_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_RGB_nbit);

			// Ordered Dithering
			//Ordered_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_RGB_nbit);
			Convolution(m_pImage.get(), m_pDithered.get());

			return true;
		}

		bool OnUserUpdate(float fElapsedTime) override {

			tv.HandlePanAndZoom();

			Clear(olc::BLACK);

			if (GetKey(olc::Key::Q).bHeld)
				tv.DrawSprite({ 0, 0 }, m_pQuantised.get());
			else if (GetKey(olc::Key::W).bHeld)
				tv.DrawSprite({ 0, 0 }, m_pDithered.get());
			else
				tv.DrawSprite({ 0,0 }, m_pImage.get());
			return true;
		}
};


int main() {
	Dithering demo;

	if (demo.Construct(1280, 960, 1, 1))
		demo.Start();

	return 0;
}