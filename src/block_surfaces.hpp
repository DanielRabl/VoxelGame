#pragma once

#include <qpl/qpl.hpp>
#include "data.hpp"


template<qpl::size S>
constexpr auto make_block_surface() {
	constexpr auto size = qpl::number_of_set_bits(S) * 4;
	std::vector<qgl::vertex<vertex::pos_type, vertex::color_type>> vertices(size);

	constexpr auto binary = qpl::base_array<2, 6>(S);

	qpl::size ctr = 0u;
	if constexpr (binary[direction::left]) {
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 0), qpl::frgb::grey_shade(0.6));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 1), qpl::frgb::grey_shade(0.6));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 1), qpl::frgb::grey_shade(0.6));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 0), qpl::frgb::grey_shade(0.6));
	}
	if constexpr (binary[direction::right]) {
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 0), qpl::frgb::grey_shade(0.6));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 1), qpl::frgb::grey_shade(0.6));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 1), qpl::frgb::grey_shade(0.6));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 0), qpl::frgb::grey_shade(0.6));
	}
	if constexpr (binary[direction::bottom]) { //y axis flipped
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 0), qpl::frgb::grey_shade(1.0));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 1), qpl::frgb::grey_shade(1.0));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 1), qpl::frgb::grey_shade(1.0));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 0), qpl::frgb::grey_shade(1.0));
	}
	if constexpr (binary[direction::top]) {  //y axis flipped
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 0), qpl::frgb::grey_shade(0.3));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 1), qpl::frgb::grey_shade(0.3));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 1), qpl::frgb::grey_shade(0.3));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 0), qpl::frgb::grey_shade(0.3));
	}
	if constexpr (binary[direction::front]) {
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 0), qpl::frgb::grey_shade(0.8));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 0), qpl::frgb::grey_shade(0.8));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 0), qpl::frgb::grey_shade(0.8));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 0), qpl::frgb::grey_shade(0.8));
	}
	if constexpr (binary[direction::back]) {
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 0, 1), qpl::frgb::grey_shade(0.4));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(0, 1, 1), qpl::frgb::grey_shade(0.4));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 1, 1), qpl::frgb::grey_shade(0.4));
		vertices[ctr++] = qgl::make_vertex(qpl::vec(1, 0, 1), qpl::frgb::grey_shade(0.4));
	}
	return vertices;

}

constexpr auto block_surfaces_array_size() {
	qpl::size result = 0u;
	for (qpl::size i = 0u; i < 64; ++i) {
		result += qpl::number_of_set_bits(i) * 4;
	}
	return result;
}

constexpr std::array<vertex::vertex_type, block_surfaces_array_size()> get_surfaces() {
	std::array<vertex::vertex_type, block_surfaces_array_size()> surfaces;

	qpl::size ctr = 0u;
	qpl::constexpr_iterate<64>([&](auto i) {
		auto surface_array = make_block_surface<i>();
		for (qpl::size j = 0u; j < surface_array.size(); ++j) {
			surfaces[ctr + j] = surface_array[j];
		}
		ctr += surface_array.size();
	});
	return surfaces;
}

constexpr auto get_ranges() {
	std::array<std::pair<qpl::size, qpl::size>, 64> ranges;

	qpl::size ctr = 0u;
	qpl::constexpr_iterate<64>([&](auto i) {
		auto surface_array = make_block_surface<i>();
		ranges[i] = std::make_pair(ctr, surface_array.size());
		ctr += surface_array.size();
	});
	return ranges;
}

namespace table {
	constexpr auto surfaces = get_surfaces();
	constexpr auto ranges = get_ranges();

	void add_to_va(graphic::va_type& va, qpl::u8 configuration) {
		auto range_start = ranges[configuration].first;
		auto range_size = ranges[configuration].second;

		auto size = va.vertices.size();
		va.vertices.resize(size + range_size);
		memcpy(va.vertices.data() + size, surfaces.data() + range_start, range_size * sizeof(vertex::vertex_type));
	}
	void add_to_va(qpl::u8 configuration) {
		auto range_start = ranges[configuration].first;
		auto range_size = ranges[configuration].second;

		auto size = graphic::va.vertices.size();
		graphic::va.vertices.resize(size + range_size);
		memcpy(graphic::va.vertices.data() + size, surfaces.data() + range_start, range_size * sizeof(vertex::vertex_type));
	}
}
