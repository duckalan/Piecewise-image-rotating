#pragma once
#include "pch.h"
#include "PointI.h"

struct PointF
{
	float x;
	float y;

	//PointF() noexcept = default;

	PointF(float x, float y) noexcept
		: x(x), y(y) {}

	PointF(PointI pointI)
		: x(pointI.x), y(pointI.y) {}

	PointI Truncate() const noexcept
	{
		return PointI(
			static_cast<int32_t>(x),
			static_cast<int32_t>(y)
		);
	}

	PointI Ceil() const noexcept
	{
		return PointI(
			static_cast<int32_t>(ceil(x)),
			static_cast<int32_t>(ceil(y))
		);
	}

	PointI Floor() const noexcept
	{
		return PointI(
			static_cast<int32_t>(floor(x)),
			static_cast<int32_t>(floor(y))
		);
	}
};

