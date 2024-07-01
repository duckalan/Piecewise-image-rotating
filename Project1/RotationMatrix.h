#pragma once
#include "pch.h"
#include "PointF.h"
#include "Rectangle.h"

class RotationMatrix
{
private:
	float a11_;
	float a12_;
	float a21_;
	float a22_;
	float b1_;
	float b2_;

public:
	RotationMatrix(
		float angleDeg, 
		float b1, float 
		b2
	) noexcept;

	void SetB1(float value) noexcept;

	void SetB2(float value) noexcept;

	PointF ReverseTransformation(
		const PointF& point
	) const noexcept;

	PointF ReverseTransformation(
		float x, float y
	) const noexcept;

	Rectangle ReverseTransformation(
		const Rectangle& rect
	) const noexcept;

	PointF operator *(const PointF& point) const noexcept;
};

