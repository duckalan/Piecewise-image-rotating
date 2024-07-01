#include "pch.h"
#include "RotationMatrix.h"

RotationMatrix::RotationMatrix(float angleDeg, float b1, float b2) noexcept
{
	float angleRad = std::numbers::pi_v<float> * angleDeg / 180.f;
	a11_ = abs(cos(angleRad)) < std::numeric_limits<float>::epsilon() ? 0 : cos(angleRad);
	a21_ = abs(sin(angleRad)) < std::numeric_limits<float>::epsilon() ? 0 : sin(angleRad);
	a12_ = -a21_;
	a22_ = a11_;

	b1_ = b1;
	b2_ = b2;
}

void RotationMatrix::SetB1(float value) noexcept
{
	b1_ = value;
}

void RotationMatrix::SetB2(float value) noexcept
{
	b2_ = value;
}

PointF RotationMatrix::ReverseTransformation(const PointF& point) const noexcept
{
	return PointF(
		a11_ * (point.x - b1_) - a12_ * (point.y - b2_), // x1
		-a21_ * (point.x - b1_) + a22_ * (point.y - b2_) // y1
	);
}

PointF RotationMatrix::ReverseTransformation(float x, float y) const noexcept
{
	return PointF(
		a11_ * (x - b1_) - a12_ * (y - b2_), // x1
		-a21_ * (x - b1_) + a22_ * (y - b2_) // y1
	);
}

Rectangle RotationMatrix::ReverseTransformation(const Rectangle& rect) const noexcept
{
	PointF p11{ ReverseTransformation(rect.GetLeftTop()) };
	PointF p12{ ReverseTransformation(rect.GetLeftBottom()) };
	PointF p21{ ReverseTransformation(rect.GetRightTop()) };
	PointF p22{ ReverseTransformation(rect.GetRightBottom()) };

	PointF transformedLeftTop
	{
		std::min({p11.x, p12.x, p21.x, p22.x}),
		std::min({p11.y, p12.y, p21.y, p22.y})
	};
	PointF transformedRightBottom
	{
		std::max({p11.x, p12.x, p21.x, p22.x}),
		std::max({p11.y, p12.y, p21.y, p22.y})
	};

	return Rectangle(
		transformedLeftTop.Floor(),
		transformedRightBottom.Ceil()
	);
}

PointF RotationMatrix::operator*(const PointF& point) const noexcept
{
	return PointF(
		a11_ * point.x + a12_ * point.y + b1_, // x2
		a21_ * point.x + a22_ * point.y + b2_  // y2
	);
}