#pragma once
#include <qpl/qpl.hpp>

namespace vertex {
	using pos_type = qgl::pos3;
	using color_type = qgl::frgb;
	using vertex_type = qgl::vertex<pos_type, color_type>;
}
namespace graphic {
	qgl::vertex_array<qgl::flag_default, qgl::primitive_quads<qgl::pos3, qgl::frgb>> va;
}
namespace world_gen {
	qpl::perlin_noise noise;
	qpl::perlin_noise_N<32, 256> noise_lightness;
	qpl::perlin_noise_N<32, 256> noise_saturation;
	qpl::perlin_noise_N<32, 256> noise_hue;

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

	constexpr auto cube_size = 0.06;

	constexpr qpl::size chunks_width = 6;
}

namespace config {
	constexpr bool use_cube_sides_table = true;
}