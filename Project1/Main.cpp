#include "pch.h"
#include "BmpHeader.h"
#include "ImagePiece.h"
#include "InterpolationFuncs.h"
#include "InterpolationType.h"
#include "PointF.h"
#include "PointI.h"
#include "Rectangle.h"
#include "RotationMatrix.h"
#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>

using std::filesystem::path;
using std::string;
using std::ifstream;
using std::ofstream;
using std::function;
using std::min;
using std::max;
using std::clamp;
using std::array;

PointF FindBCoeffs(
	const array<PointF, 4>& cornerPoints,
	const RotationMatrix& rotationMatrix)
{
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	for (PointF p : cornerPoints)
	{
		PointF rotatedP = rotationMatrix * p;
		if (minX > rotatedP.x)
		{
			minX = rotatedP.x;
		}
		if (minY > rotatedP.y)
		{
			minY = rotatedP.y;
		}
	}

	return PointF(minX, minY);
}

PointF FindRotatedImageSize(
	const array<PointF, 4>& cornerPoints,
	const RotationMatrix& rotationMatrix)
{
	float maxX = -std::numeric_limits<float>::max();
	float maxY = -std::numeric_limits<float>::max();
	for (const PointF& p : cornerPoints)
	{
		PointF rotatedP = rotationMatrix * p;
		if (maxX < rotatedP.x)
		{
			maxX = rotatedP.x;
		}
		if (maxY < rotatedP.y)
		{
			maxY = rotatedP.y;
		}
	}

	return PointF(maxX, maxY);
}

function<array<uint8_t, 3>(const PointF&, const ImagePiece&)>
SelectInterpolationFunc(InterpolationType interpolationType)
{
	switch (interpolationType)
	{
	case InterpolationType::NearestNeighbor:
		return &NearestNeighbor;

	case InterpolationType::Bilinear:
		return &BiLerp;

	case InterpolationType::BiCubic:
		return &BiCubic;

	case InterpolationType::Lanczos2:
		return &Lanczos2;

	case InterpolationType::Lanczos3:
		return &Lanczos3;

	default:
		throw std::exception("Wrong enum type");
		break;
	}
}

void RotateImagePiecewisely(
	const std::filesystem::path& inputPath,
	const std::filesystem::path& outputPath,
	float angleDeg,
	InterpolationType interpolationType,
	int squareSideSize)
{
	std::ifstream input(
		inputPath,
		std::ios_base::binary
	);
	std::ofstream output(
		outputPath,
		std::ios_base::binary
	);
	if (!input.is_open() || !output.is_open())
	{
		throw std::ios_base::failure(
			"Can't open input or output files"
		);
	}


	BmpHeader header{};
	input.read((char*)&header, BmpHeader::SizeBytes);

	if (header.imageOffsetBytes != BmpHeader::SizeBytes)
	{
		throw std::ios_base::failure(
			"Program can work only with BITMAPINFOHEADER (header size must be 54 bytes)"
		);
	}

	RotationMatrix rotationMatrix(angleDeg, 0.f, 0.f);
	array<PointF, 4> cornerPoints{
		PointF(0.f, 0.f),
		PointF(header.imageWidthPx - 1, 0.f),
		PointF(0.f, header.imageHeightPx - 1),
		PointF(header.imageWidthPx - 1, header.imageHeightPx - 1)
	};

	// Нахождение коэффициентов b1, b2
	PointF minPointF = FindBCoeffs(
		cornerPoints,
		rotationMatrix
	);
	rotationMatrix.SetB1(-minPointF.x);
	rotationMatrix.SetB2(-minPointF.y);

	// Нахождение размеров перевёрнутого изображения
	PointF maxPointF = FindRotatedImageSize(
		cornerPoints,
		rotationMatrix
	);

	int newWidthPx = ceil(maxPointF.x + 1);
	int newHeightPx = ceil(maxPointF.y + 1);

	BmpHeader newHeader{};
	header.InitNew(newWidthPx, newHeightPx, newHeader);

	output.write((char*)&newHeader, BmpHeader::SizeBytes);

	function<array<uint8_t, 3>(const PointF&, const ImagePiece&)>
		interpolationFunc = SelectInterpolationFunc(
			interpolationType
		);

	// --------------------Новое------------------------

	int32_t squareIndexX = ceil((float)newWidthPx / squareSideSize);
	int32_t squareIndexY = ceil((float)newHeightPx / squareSideSize);

	// Проход по всему новому изображению квадратами 
	for (int i = 0; i < squareIndexY; i++)
	{
		for (int j = 0; j < squareIndexX; j++)
		{
			std::cout << std::format(
				"Current progress: {:.2f}%\r",
				(static_cast<float>(i * squareIndexX + j) / (squareIndexX * squareIndexY) * 100.f)
			);

			Rectangle r2(
				PointI{
					.x = j * squareSideSize,
					.y = i * squareSideSize
				},
				squareSideSize/* - 1*/,
				squareSideSize/* - 1*/
			);

			if (j == squareIndexX - 1 || i == squareIndexY - 1)
			{
				r2 = r2.GetIntersectionWithImage(newWidthPx, newHeightPx);
			}

			vector<uint8_t> outputLine((r2.GetWidth() /*+ 1*/) * 3);

			Rectangle r1 = rotationMatrix.ReverseTransformation(r2);
			ImagePiece imagePiece(r1, interpolationType, header, input);

			for (int y2 = r2.GetLeftTop().y; y2 <= r2.GetLeftBottom().y; y2++)
			{
				for (int x2 = r2.GetLeftTop().x; x2 <= r2.GetRightTop().x; x2++)
				{
					PointF p1 = rotationMatrix.ReverseTransformation(x2, y2);

					if (p1.x >= 0 &&
						p1.x <= (header.imageWidthPx - 1) &&
						p1.y >= 0 &&
						p1.y <= (header.imageHeightPx - 1))
					{
						array<uint8_t, 3> bgr
							= interpolationFunc(p1, imagePiece);

						outputLine[(x2 - r2.GetLeftTop().x) * 3] = bgr[0];
						outputLine[(x2 - r2.GetLeftTop().x) * 3 + 1] = bgr[1];
						outputLine[(x2 - r2.GetLeftTop().x) * 3 + 2] = bgr[2];
					}
					else
					{
						outputLine[(x2 - r2.GetLeftTop().x) * 3] = 0;
						outputLine[(x2 - r2.GetLeftTop().x) * 3 + 1] = 0;
						outputLine[(x2 - r2.GetLeftTop().x) * 3 + 2] = 0;
					}
				}

				output.seekp(newHeader.imageOffsetBytes + y2 * newHeader.GetStride() + r2.GetLeftTop().x * 3);
				output.write((char*)outputLine.data(), outputLine.size());
			}

			input.clear();
		}
	}

	vector<uint8_t> paddingBytes(newHeader.GetPaddingByteCount());
	output.write((char*)paddingBytes.data(), paddingBytes.size());
}


int main()
{
	float angleDeg = 66;
	string rootPath = "C:\\Users\\Duck\\Desktop\\test images\\";
	string inputImageName = "crop";
	string outputImageName = inputImageName + '_';
	string bmp = ".bmp";
	InterpolationType interpolationType = InterpolationType::Lanczos3;

	switch (interpolationType)
	{
	case InterpolationType::NearestNeighbor:
		outputImageName += "NearestNeighbor";
		break;

	case InterpolationType::Bilinear:
		outputImageName += "Bilinear";
		break;

	case InterpolationType::BiCubic:
		outputImageName += "BiCubic";
		break;

	case InterpolationType::Lanczos2:
		outputImageName += "Lanczos2";
		break;

	case InterpolationType::Lanczos3:
		outputImageName += "Lanczos3";
		break;
	default:
		break;
	}

	outputImageName += std::format("({:.2f}deg)", angleDeg);
	try
	{
		auto now = std::chrono::high_resolution_clock::now();

		RotateImagePiecewisely(
			rootPath + inputImageName + bmp,
			rootPath + outputImageName + bmp,
			angleDeg,
			interpolationType,
			2500
		);

		auto resultTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - now);
		std::cout << "\nDone in " << resultTime.count() << " ms.\n";
	}
	catch (const std::ios_base::failure& e)
	{
		std::cerr << e.what() << '\n';
	}
}