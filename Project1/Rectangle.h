#pragma once

#include "pch.h"
#include "PointF.h"
#include "PointI.h"

class Rectangle
{
	PointI leftTop_;
	uint32_t width_;
	uint32_t height_;

public:
	Rectangle() noexcept = default;

	Rectangle(
		PointI leftTop, 
		uint32_t width, 
		uint32_t height
	) noexcept;

	Rectangle(
		PointI leftTop, 
		PointI rightBottom
	) noexcept;

	uint32_t GetWidth() const noexcept;
	uint32_t GetHeight() const noexcept;

	PointI GetLeftTop() const noexcept;
	PointI GetLeftBottom() const noexcept;
	PointI GetRightTop() const noexcept;
	PointI GetRightBottom() const noexcept;

	Rectangle GetIntersectionWithImage(
		int32_t imageWidthPx,
		int32_t imageHeightPx
	) const noexcept;
};