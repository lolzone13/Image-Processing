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
			// std::transform(m_pImage->pColData.begin(), m_pImage->pColData.end(), m_pImage->pColData.begin(), Convert_RGB_to_Grayscale);


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

			Floyd_Steinberg_Dithering(m_pImage.get(), m_pDithered.get(), Quantize_RGB_nbit);


			return true;
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