#pragma once

#include "variant.h"

struct Circle { double radius_; };

struct Square { double width_; };

struct Triangle { double length_[3]; };

using Shape = Yupei::variant<Circle, Square, Triangle>;

double GetPerimeter(const Shape& s)
{
	return Yupei::match<double>(
		s,
		[](const Circle& c) {return 6.28 * c.radius_;},
		[](const Square& s) {return 4.0 * s.width_;},
		[](const Triangle& t) {return t.length_[0] + t.length_[1] + t.length_[2];});
}
