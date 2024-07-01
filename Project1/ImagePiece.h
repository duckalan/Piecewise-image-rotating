#pragma once
#include "pch.h"
#include "BmpHeader.h"
#include "InterpolationType.h"
#include "Rectangle.h"

class ImagePiece
{
	Rectangle imagePiece_;
	int32_t extendedPxUpCount_ = 0;
	int32_t extendedPxLeftCount_ = 0;
	std::vector<uint8_t> imageData_;

	void ExtendRowEdges(
		int32_t rowOffsetBytes,
		int32_t intersectionWidthPx,
		uint32_t pxToExtendLeftCount,
		uint32_t pxToExtendRightCount
	);

public:
	ImagePiece(
		const Rectangle& imagePiece, 
		InterpolationType interpolationType,
		const BmpHeader& header,
		std::ifstream& input
	);

	int GetExtendedPxUpCount() const noexcept;
	int GetExtendedPxLeftCount() const noexcept;

	uint8_t operator()(
		int32_t x, int32_t y,
		uint32_t colorOffset
		) const;
};