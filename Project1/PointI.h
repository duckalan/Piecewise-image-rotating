#pragma once

#include "pch.h"

struct PointI
{
	int32_t x;
	int32_t y;

	PointI operator+(int32_t value) const noexcept
	{
		return PointI{ x + value, y + value };
	}

	PointI operator-(int32_t value) const noexcept
	{
		return PointI{ x - value, y - value };
	}
};