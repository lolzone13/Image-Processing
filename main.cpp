#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

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

		bool OnUserCreate() override {

			tv.Initialise({ ScreenWidth(), ScreenHeight() });

			cout << "Dithering!!!" << endl;
			m_pImage = std::make_unique<olc::Sprite>("image.jpg");
			m_pQuantised = std::make_unique<olc::Sprite>(m_pImage->width, m_pImage->height);
			m_pDithered = std::make_unique<olc::Sprite>(m_pImage->width, m_pImage->height);

			auto Convert_RGB_to_Grayscale = [](const olc::Pixel in) {
				uint8_t grayscale = uint8_t(0.2162f * float(in.r) + (0.7152f) * float(in.g) + 0.0722f * float(in.b));

				return olc::Pixel(grayscale, grayscale, grayscale);
			};

			// transform to grayscale
			std::transform(m_pImage->pColData.begin(), m_pImage->pColData.end(), m_pImage->pColData.begin(), Convert_RGB_to_Grayscale);


			// quantize grayscale

			auto Quantize_Grayscale_1bit = [](const olc::Pixel in) {
				return in.r < 128 ? olc::BLACK : olc::WHITE;
			};
			std::transform(m_pImage->pColData.begin(), m_pImage->pColData.end(), m_pQuantised->pColData.begin(), Quantize_Grayscale_1bit);

			Floyd_Steinberg_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_Grayscale_1bit);


			return true;
		}

		void Floyd_Steinberg_Dithering(const olc::Sprite* src, const olc::Sprite* dest, std::function<olc::Pixel(const olc::Pixel)> funcQuantize) {

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