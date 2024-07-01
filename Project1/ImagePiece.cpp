#include "pch.h"
#include "ImagePiece.h"

void ImagePiece::ExtendRowEdges(int32_t rowOffsetBytes, int32_t intersectionWidthPx, uint32_t pxToExtendLeftCount, uint32_t pxToExtendRightCount)
{
	uint8_t* rowStart = &imageData_[rowOffsetBytes];
	uint8_t* leftEdge = rowStart + pxToExtendLeftCount * 3;
	uint8_t* rightEdge = rowStart + pxToExtendLeftCount * 3 + (intersectionWidthPx * 3) - 3;

	for (size_t x = 0; x < pxToExtendLeftCount; x++)
	{
		*(rowStart + x * 3) = *(leftEdge);
		*(rowStart + x * 3 + 1) = *(leftEdge + 1);
		*(rowStart + x * 3 + 2) = *(leftEdge + 2);
	}

	for (size_t x = 0; x < pxToExtendRightCount; x++)
	{
		*(rightEdge + x * 3 + 3) = *(rightEdge);
		*(rightEdge + x * 3 + 4) = *(rightEdge + 1);
		*(rightEdge + x * 3 + 5) = *(rightEdge + 2);
	}
}

ImagePiece::ImagePiece(const Rectangle& imagePiece, InterpolationType interpolationType, const BmpHeader& header, std::ifstream& input)
{
	int32_t extendedPxCount = 0;;
	switch (interpolationType)
	{
	case InterpolationType::BiCubic:
	case InterpolationType::Lanczos2:
		extendedPxCount = 1;
		break;
	case InterpolationType::Lanczos3:
		extendedPxCount = 2;
		break;
	default:
		extendedPxCount = 0;
		break;
	}

	Rectangle extendedRect{
		imagePiece.GetLeftTop() - extendedPxCount,
		imagePiece.GetRightBottom() + extendedPxCount
	};

	Rectangle intersection = extendedRect.GetIntersectionWithImage(
		header.imageWidthPx,
		header.imageHeightPx
	);

	if (intersection.GetWidth() != 0 || intersection.GetHeight() != 0)
	{
		int32_t pxToExtendLeft = std::min(
			extendedPxCount,
			abs(extendedRect.GetLeftTop().x - intersection.GetLeftTop().x)
		);
		extendedPxLeftCount_ = pxToExtendLeft;
		int32_t pxToExtendUp = std::min(
			extendedPxCount,
			abs(extendedRect.GetLeftTop().y - intersection.GetLeftTop().y)
		);
		extendedPxUpCount_ = pxToExtendUp;

		int32_t pxToExtendRight = std::min(
			extendedPxCount,
			abs(extendedRect.GetRightTop().x - intersection.GetRightTop().x)
		);
		int32_t pxToExtendBottom = std::min(
			extendedPxCount,
			abs(extendedRect.GetLeftBottom().y - intersection.GetLeftBottom().y)
		);

		int32_t extendedWidth = intersection.GetWidth() + pxToExtendLeft + pxToExtendRight;
		int32_t extendedHeight = intersection.GetHeight() + pxToExtendUp + pxToExtendBottom;

		imagePiece_ = Rectangle(
			intersection.GetLeftTop(),
			extendedWidth,
			extendedHeight
		);

		imageData_ = std::vector<uint8_t>(
			extendedWidth * extendedHeight * 3
		);

		for (int32_t y = intersection.GetLeftTop().y; y <= intersection.GetLeftBottom().y; y++)
		{
			input.seekg(
				header.imageOffsetBytes + y * header.GetStride() +
				intersection.GetLeftTop().x * 3
			);

			input.read(
				(char*)&imageData_[
					((y - intersection.GetLeftTop().y + pxToExtendUp)
						* extendedWidth + pxToExtendLeft) * 3
				],
				intersection.GetWidth() * 3
			);

			ExtendRowEdges(
				(y - intersection.GetLeftTop().y + pxToExtendUp) * extendedWidth * 3,
				intersection.GetWidth(),
				pxToExtendLeft,
				pxToExtendRight
			);
		}

		// Дополнение первых строк
		for (int y = 0; y < pxToExtendUp; y++)
		{
			std::copy(
				&imageData_[pxToExtendUp * extendedWidth * 3],
				&imageData_[(pxToExtendUp + 1) * extendedWidth * 3],
				&imageData_[y * extendedWidth * 3]
			);
		}

		// Дополнение последних строк
		for (int y = intersection.GetHeight() + pxToExtendUp;
			y < intersection.GetHeight() + pxToExtendUp + pxToExtendBottom;
			y++)
		{
			std::copy(
				&imageData_[(intersection.GetHeight() + pxToExtendUp - 1) * extendedWidth * 3],
				&imageData_[(intersection.GetHeight() + pxToExtendUp) * extendedWidth * 3],
				&imageData_[y * extendedWidth * 3]
			);
		}
	}
}

int ImagePiece::GetExtendedPxUpCount() const noexcept
{
	return extendedPxUpCount_;
}

int ImagePiece::GetExtendedPxLeftCount() const noexcept
{
	return extendedPxLeftCount_;
}

uint8_t ImagePiece::operator()(int32_t x, int32_t y, uint32_t colorOffset) const
{
	return imageData_[
		((y - imagePiece_.GetLeftTop().y/* + extendedPxCount_*/) * imagePiece_.GetWidth()
			+ x - imagePiece_.GetLeftTop().x/* + extendedPxCount_*/) * 3 + colorOffset
	];
}
