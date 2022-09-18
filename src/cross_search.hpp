#pragma once

#include <qpl/qpl.hpp>
#include "data.hpp"

struct cross_surface_search {
	//qpl::vector3<qpl::i8> delta;
	qpl::i8 dx;
	qpl::i8 dy;
	qpl::i8 dz;
	qpl::u8 surface;
	qpl::u8 dsurface;
	qpl::vector3<qpl::u8> vec3_start;
	qpl::vector3<qpl::u8> vec3_end;
	qpl::vector3<qpl::u8> vec3_delta;
};
constexpr auto cross_surfaces = std::array{
	cross_surface_search{-1,  0,  0,  direction::left,   direction::right,  qpl::vec(0, 0, 0), qpl::vec(0, 1, 1), qpl::vec(1, 0, 0)},
	cross_surface_search{ 0, -1,  0,  direction::top,    direction::bottom, qpl::vec(0, 0, 0), qpl::vec(1, 0, 1), qpl::vec(0, 1, 0)},
	cross_surface_search{ 0,  0, -1,  direction::front,  direction::back,   qpl::vec(0, 0, 0), qpl::vec(1, 1, 0), qpl::vec(0, 0, 1)},
};