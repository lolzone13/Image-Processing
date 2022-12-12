#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

#include "Dithering.h"
#include "Convolution.h"

#include <iostream>


using namespace std;

class ImageProcessing : public olc::PixelGameEngine {

	public:
		ImageProcessing() {
			sAppName = "Image Processing";
		}

		olc::TransformedView tv;

		std::unique_ptr<olc::Sprite> m_pImage;
		std::unique_ptr<olc::Sprite> m_pQuantised;
		std::unique_ptr<olc::Sprite> m_pDithered;
		std::unique_ptr<olc::Sprite> m_pBlurred;
		std::unique_ptr<olc::Sprite> m_pEdge;
		std::unique_ptr<olc::Sprite> m_pGray;
	public: 

		bool OnUserCreate() override {

			tv.Initialise({ ScreenWidth(), ScreenHeight() });

			cout << "Image Processing!!!" << endl;
			m_pImage = std::make_unique<olc::Sprite>("image.jpg");
			m_pQuantised = std::make_unique<olc::Sprite>(m_pImage->width, m_pImage->height);
			m_pDithered = std::make_unique<olc::Sprite>(m_pImage->width, m_pImage->height);
			m_pBlurred = std::make_unique<olc::Sprite>(m_pImage->width, m_pImage->height);
			m_pEdge = std::make_unique<olc::Sprite>("image.jpg");
			m_pGray = std::make_unique<olc::Sprite>(m_pImage->width, m_pImage->height);

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
			Floyd_Steinberg_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_RGB_nbit);
			//JJN_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_RGB_nbit);
			//Stucki_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_RGB_nbit);

			// Ordered Dithering
			//Ordered_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_RGB_nbit);



			// Gaussian Blur
			vector<vector<float>> kernel(7, vector<float>(7, 0.0f));
			Generate_Gaussian_Kernel(kernel, 7.0f);
			Convolution(m_pImage.get(), m_pBlurred.get(), kernel);


			// Edge Detection
			// transform to grayscale
			std::transform(m_pEdge->pColData.begin(), m_pEdge->pColData.end(), m_pGray->pColData.begin(), Convert_RGB_to_Grayscale);
			Sobel_Edge_Detection(m_pGray.get(), m_pEdge.get());

		
			return true;
		}

		bool OnUserUpdate(float fElapsedTime) override {

			tv.HandlePanAndZoom();

			Clear(olc::BLACK);

			if (GetKey(olc::Key::Q).bHeld)
				tv.DrawSprite({ 0, 0 }, m_pQuantised.get());
			else if (GetKey(olc::Key::W).bHeld)
				tv.DrawSprite({ 0, 0 }, m_pDithered.get());
			else if (GetKey(olc::Key::E).bHeld)
				tv.DrawSprite({ 0, 0 }, m_pBlurred.get());
			else if (GetKey(olc::Key::R).bHeld)
				tv.DrawSprite({ 0, 0 }, m_pEdge.get());
			else if (GetKey(olc::Key::T).bHeld)
				tv.DrawSprite({ 0, 0 }, m_pGray.get());
			else
				tv.DrawSprite({ 0,0 }, m_pImage.get());
			return true;
		}
};


int main() {
	ImageProcessing demo;

	if (demo.Construct(1280, 960, 1, 1))
		demo.Start();

	return 0;
}