#pragma once

#include <qpl/qpl.hpp>
#include "cube_sides.hpp"

struct cube {
	qpl::bitset<6> bits;
	cube() {

	}
	cube(bool b) {
		if (b) {
			this->set_fully_visible();
		}
	}

	void set_fully_visible() {
		this->bits.data = (1 << 6) - 1;
	}

	constexpr bool bottom() const {
		return this->bits[0];
	}
	constexpr bool left() const {
		return this->bits[1];
	}
	constexpr bool back() const {
		return this->bits[2];
	}
	constexpr bool right() const {
		return this->bits[3];
	}
	constexpr bool front() const {
		return this->bits[4];
	}
	constexpr bool top() const {
		return this->bits[5];
	}

	void set_bottom(bool b) {
		this->bits[0] = b;
	}
	void set_left(bool b) {
		this->bits[1] = b;
	}
	void set_back(bool b) {
		this->bits[2] = b;
	}
	void set_right(bool b) {
		this->bits[3] = b;
	}
	void set_front(bool b) {
		this->bits[4] = b;
	}
	void set_top(bool b) {
		this->bits[5] = b;
	}

	void fill_va(qpl::vector3i pos) {


		qpl::f32 hue;
		//qpl::benchmark("getN()", [&]() {
		//	hue = world_gen::noise_hue.get(qpl::vec3(pos).data, 0.01f, 3);
		//});
		qpl::benchmark("get3d()", [&]() {
			hue = world_gen::noise_hue.get3d(qpl::vec3(pos).data, 0.01f, 3);
		});

		//auto hue = world_gen::noise_hue.get3d(qpl::vec3(pos).data, 0.01f, 3);
		auto color = qpl::frgb(qpl::get_rainbow_color(hue));
		//auto lightness = (world_gen::noise_lightness.get(qpl::vec3(pos).data, 0.008f, 1) - 0.5f) * 2;
		//auto saturation = (world_gen::noise_saturation.get(qpl::vec3(pos).data, 0.005f, 1) - 0.5f) * 2;

		if constexpr (config::use_cube_sides_table) {
			auto index = this->bits.data;

			auto size = graphic::va.size();
			table::add_to_va(this->bits.data);

			for (qpl::size i = size; i < graphic::va.vertices.size(); ++i) {
				graphic::va.vertices[i].position = (pos + graphic::va.vertices[i].position) * world_gen::cube_size;
				graphic::va.vertices[i].color = color;
			}

		}
		else {
			if (this->bottom()) {
				//auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.1, 1.0))));
				//color.light(lightness);
				//color.saturate(saturation);
				//color *= (80 / 255.0f);
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 0)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 0)) * world_gen::cube_size, color));
			}
		
			if (this->left()) {
				//auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.9, 1.0))));
				//color.light(lightness);
				//color.saturate(saturation);
				//color *= (200 / 255.0f);
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 0)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 0)) * world_gen::cube_size, color));
			}
		
			if (this->back()) {
				//auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.95, 1.0))));
				//color.light(lightness);
				//color.saturate(saturation);
				//color *= (150 / 255.0f);
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 1)) * world_gen::cube_size, color));
			}
		
			if (this->right()) {
				//auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.05, 1.0))));
				//color.light(lightness);
				//color.saturate(saturation);
				//color *= (50 / 255.0f);
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 0)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 0)) * world_gen::cube_size, color));
			}
		
			if (this->front()) {
				//auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.02, 1.0))));
				//color.light(lightness);
				//color.saturate(saturation);
				//color *= (150 / 255.0f);
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 0)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 0)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 0)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 0)) * world_gen::cube_size, color));
			}
		
			if (this->top()) {
				//auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.98, 1.0))));
				//color.light(lightness);
				//color.saturate(saturation);
				//color *= (250 / 255.0f);
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 0)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 1)) * world_gen::cube_size, color));
				graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 0)) * world_gen::cube_size, color));
			}
		}

		
	}

	constexpr operator bool() {
		return qpl::bool_cast(this->bits.data);
	}
};

cube empty_cube(false);
