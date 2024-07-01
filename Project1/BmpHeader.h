#pragma once
#include "pch.h"

#pragma pack(push, 1)
struct BmpHeader
{
	static const int32_t SizeBytes = 54;
	static const int32_t BytePerPx = 3;

	// BmpFileHeader
	uint16_t bm;
	uint32_t fileSizeBytes;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t imageOffsetBytes;

	// DibHeader
	uint32_t dibHeaderSizeBytes;
	int32_t imageWidthPx;
	int32_t imageHeightPx;
	uint16_t colorPlanesCount;
	uint16_t bitPerPixel;
	uint32_t compressionMethod;
	uint32_t imageSizeBytes;
	int32_t xPixelPerMetre;
	int32_t yPixelPerMetre;
	uint32_t paletteColorsCount;
	uint32_t importantColorsCount;

	uint32_t GetStride() const noexcept
	{
		return (imageWidthPx * BytePerPx + 3) & ~3;
	}

	uint32_t GetPaddingByteCount() const noexcept
	{
		return GetStride() - imageWidthPx * BytePerPx;
	}

	void InitNew(
		int32_t newWidthPx,
		int32_t newHeightPx,
		BmpHeader& headerToInit
	) const noexcept {
		uint32_t newStride
			= (newWidthPx * BytePerPx + 3) & ~3;
		uint32_t newImageSizeBytes
			= newHeightPx * newStride;

		headerToInit.bm = bm;
		headerToInit.fileSizeBytes = SizeBytes + newImageSizeBytes;
		headerToInit.reserved1 = reserved1;
		headerToInit.reserved2 = reserved2;
		headerToInit.imageOffsetBytes = imageOffsetBytes;

		headerToInit.dibHeaderSizeBytes = dibHeaderSizeBytes;
		headerToInit.imageWidthPx = newWidthPx;
		headerToInit.imageHeightPx = newHeightPx;
		headerToInit.colorPlanesCount = colorPlanesCount;
		headerToInit.bitPerPixel = bitPerPixel;
		headerToInit.compressionMethod = compressionMethod;
		headerToInit.imageSizeBytes = newImageSizeBytes;
		headerToInit.xPixelPerMetre = xPixelPerMetre;
		headerToInit.yPixelPerMetre = yPixelPerMetre;
		headerToInit.paletteColorsCount = paletteColorsCount;
		headerToInit.importantColorsCount = importantColorsCount;
	}
};
#pragma pack(pop)