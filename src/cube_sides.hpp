#pragma once

#include <qpl/qpl.hpp>
#include "data.hpp"


template<qpl::size S>
constexpr auto make_cube_side() {
	constexpr auto size = qpl::number_of_set_bits(S) * 4;
	std::vector<qgl::vertex<vertex::pos_type, vertex::color_type>> vertices(size);

	constexpr auto binary = qpl::base_array<2, 6>(S);

	qpl::size ctr = 0u;
	if constexpr (binary[0]) { //bottom
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 0), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 0), qpl::frgb::white());
	}
	if constexpr (binary[1]) { //left
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 0), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 0), qpl::frgb::white());
	}
	if constexpr (binary[2]) { //back
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 1), qpl::frgb::white());
	}
	if constexpr (binary[3]) { //right
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 0), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 0), qpl::frgb::white());
	}
	if constexpr (binary[4]) { //front
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 0), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 0), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 0), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 0), qpl::frgb::white());
	}
	if constexpr (binary[5]) { //top
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 0), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 1), qpl::frgb::white());
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 0), qpl::frgb::white());
	}
	return vertices;
}

constexpr auto cube_sides_array_size() {
	qpl::size result = 0u;
	for (qpl::size i = 0u; i < 64; ++i) {
		result += qpl::number_of_set_bits(i) * 4;
	}
	return result;
}

constexpr std::array<vertex::vertex_type, cube_sides_array_size()> get_sides() {
	std::array<vertex::vertex_type, cube_sides_array_size()> sides;

	qpl::size ctr = 0u;
	qpl::constexpr_iterate<64>([&](auto i) {
		auto side_array = make_cube_side<i>();
		for (qpl::size j = 0u; j < side_array.size(); ++j) {
			sides[ctr + j] = side_array[j];
		}
		ctr += side_array.size();
	});
	return sides;
}

constexpr auto get_ranges() {
	std::array<std::pair<qpl::size, qpl::size>, 64> ranges;

	qpl::size ctr = 0u;
	qpl::constexpr_iterate<64>([&](auto i) {
		auto side_array = make_cube_side<i>();
		ranges[i] = std::make_pair(ctr, side_array.size());
		ctr += side_array.size();
	});
	return ranges;
}

namespace table {
	constexpr auto sides = get_sides();
	constexpr auto ranges = get_ranges();

	void add_to_va(qpl::u8 configuration) {
		auto range_start = ranges[configuration].first;
		auto range_size = ranges[configuration].second;

		auto size = graphic::va.vertices.size();
		graphic::va.vertices.resize(size + range_size);
		memcpy(graphic::va.vertices.data() + size, sides.data() + range_start, range_size * sizeof(vertex::vertex_type));
	}
}
