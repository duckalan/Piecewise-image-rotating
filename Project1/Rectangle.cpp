#include "pch.h"
#include "Rectangle.h"

Rectangle::Rectangle(PointI leftTop, uint32_t width, uint32_t height) noexcept
	: leftTop_(leftTop), width_(width), height_(height)
{
}

Rectangle::Rectangle(PointI leftTop, PointI rightBottom) noexcept
{
	leftTop_ = leftTop;
	width_ = abs(rightBottom.x - leftTop.x) + 1;
	height_ = abs(rightBottom.y - leftTop.y) + 1;
}

PointI Rectangle::GetLeftTop() const noexcept
{
	return leftTop_;
}

PointI Rectangle::GetLeftBottom() const noexcept
{
	return PointI(
		leftTop_.x,
		leftTop_.y + height_ - 1
	);
}

PointI Rectangle::GetRightTop() const noexcept
{
	return PointI(
		leftTop_.x + width_ - 1,
		leftTop_.y
	);
}

uint32_t Rectangle::GetWidth() const noexcept
{
	return width_;
}

uint32_t Rectangle::GetHeight() const noexcept
{
	return height_;
}

PointI Rectangle::GetRightBottom() const noexcept
{
	return PointI(
		leftTop_.x + width_ - 1,
		leftTop_.y + height_ - 1
	);
}

Rectangle Rectangle::GetIntersectionWithImage(int32_t imageWidthPx, int32_t imageHeightPx) const noexcept
{
	PointI lt = PointI(
		std::max(leftTop_.x, 0),
		std::max(leftTop_.y, 0)
	);

	PointI rb = PointI(
		std::min(GetRightBottom().x, imageWidthPx/* - 1*/),
		std::min(GetRightBottom().y, imageHeightPx/* - 1*/)
	);

	if (lt.x >= rb.x || lt.y >= rb.y)
	{
		return Rectangle(lt, 0, 0);
	}


	return Rectangle(lt, rb);
}