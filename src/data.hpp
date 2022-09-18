#pragma once
#include <qpl/qpl.hpp>

namespace vertex {
	using pos_type = qgl::pos3;
	using color_type = qgl::frgb;
	using vertex_type = qgl::vertex<pos_type, color_type>;
}
namespace graphic {
	using va_type = qgl::vertex_array<qgl::flag_default, qgl::primitive_quads<qgl::pos3, qgl::frgb>>;
	va_type va;
}


constexpr qpl::size chunk_size = 64;
constexpr qpl::size chunk_blocks = qpl::pow(chunk_size, 3);
constexpr qpl::size chunk_log2 = qpl::log2(chunk_size);
constexpr qpl::size chunk_bit_mask = chunk_size - 1;
constexpr qpl::size chunk_index_bits = (chunk_log2 * 3);

using chunk_index_type = qpl::ubit<(chunk_index_bits - 1) / 8 * 8 + 1>;


namespace world_gen {
	qpl::perlin_noise noise;
	qpl::perlin_noise_N<32, 256> noise_lightness;
	qpl::perlin_noise_N<32, 256> noise_saturation;
	qpl::perlin_noise_N<32, 256> noise_hue;
	qpl::perlin_noise_N<32, 256> noise_depth;

	void seed_random() {
		noise.set_seed_random();
		noise_lightness.set_seed_random();
		noise_saturation.set_seed_random();
		noise_hue.set_seed_random();
	}
	void seed(qpl::size seed) {
		noise.set_seed(seed);
		noise_lightness.set_seed(seed);
		noise_saturation.set_seed(seed);
		noise_hue.set_seed(seed);
	}

	constexpr auto block_size = 0.02;

	//constexpr qpl::vec3i generate_chunks = qpl::vec(15, 4, 15);
	//constexpr qpl::vec3i generate_chunks = qpl::vec(4, 4, 4);
	constexpr qpl::vec3i generate_chunks = qpl::vec(2, 2, 2);
	constexpr auto world_size = generate_chunks * chunk_size;

	constexpr qpl::size light_distance = 12u;

	constexpr auto light_search_spiral = []() { 
		std::array<qpl::vec2i, qpl::pow(light_distance * 2 + 1, 2)> result;

		for (qpl::size i = 0u; i < result.size(); ++i) {
			auto [x, y] = qpl::spiral_pairing(i);
			result[i] = qpl::vec(x, y);
		}
		return result;
	}();
}

namespace direction {
	constexpr qpl::u8 left = 0;
	constexpr qpl::u8 right = 1;
	constexpr qpl::u8 top = 2;
	constexpr qpl::u8 bottom = 3;
	constexpr qpl::u8 front = 4;
	constexpr qpl::u8 back = 5;
}