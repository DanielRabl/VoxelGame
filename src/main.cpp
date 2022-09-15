#include <qpl/qpl.hpp>

namespace graphic {
	qgl::vertex_array<qgl::flag_default, qgl::primitive_quads<qgl::pos3, qgl::frgb>> va;
}
namespace world_gen {
	qpl::perlin_noise noise;
	qpl::perlin_noise noise_lightness;
	qpl::perlin_noise noise_saturation;
	qpl::perlin_noise noise_hue;

	void init() {
		noise.set_seed_random();
		noise_lightness.set_seed_random();
		noise_saturation.set_seed_random();
		noise_hue.set_seed_random();
	}

	constexpr auto cube_size = 0.03;

	constexpr qpl::size chunks_width = 8;
}

struct cube {
	qpl::bitset<6> visible_sides;

	cube(bool b = true) {
		this->visible_sides.fill(b);
	}

	constexpr bool bottom() const {
		return this->visible_sides[0];
	}
	constexpr bool left() const {
		return this->visible_sides[1];
	}
	constexpr bool back() const {
		return this->visible_sides[2];
	}
	constexpr bool right() const {
		return this->visible_sides[3];
	}
	constexpr bool front() const {
		return this->visible_sides[4];
	}
	constexpr bool top() const {
		return this->visible_sides[5];
	}

	void set_bottom(bool b) {
		this->visible_sides[0] = b;
	}
	void set_left(bool b) {
		this->visible_sides[1] = b;
	}
	void set_back(bool b) {
		this->visible_sides[2] = b;
	}
	void set_right(bool b) {
		this->visible_sides[3] = b;
	}
	void set_front(bool b) {
		this->visible_sides[4] = b;
	}
	void set_top(bool b) {
		this->visible_sides[5] = b;
	}

	void fill_va(qpl::vector3i pos) {
		
		auto hue = world_gen::noise_hue.get(qpl::vec3d(pos).data, 0.01, 3);
		auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(hue)));
		auto lightness = (world_gen::noise_lightness.get(qpl::vec3d(pos).data, 0.008, 1) - 0.5) * 2;
		auto saturation = (world_gen::noise_saturation.get(qpl::vec3d(pos).data, 0.005, 1) - 0.5) * 2;

		if (this->bottom()) {
			auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.1, 1.0))));
			color.light(lightness);
			color.saturate(saturation);
			color *= (80 / 255.0f);
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 0)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 0)) * world_gen::cube_size, color));
		}

		if (this->left()) {
			auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.9, 1.0))));
			color.light(lightness);
			color.saturate(saturation);
			color *= (200 / 255.0f);
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 0)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 0)) * world_gen::cube_size, color));
		}

		if (this->back()) {
			auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.95, 1.0))));
			color.light(lightness);
			color.saturate(saturation);
			color *= (150 / 255.0f);
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 1)) * world_gen::cube_size, color));
		}

		if (this->right()) {
			auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.05, 1.0))));
			color.light(lightness);
			color.saturate(saturation);
			color *= (50 / 255.0f);
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 0)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 0)) * world_gen::cube_size, color));
		}

		if (this->front()) {
			auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.02, 1.0))));
			color.light(lightness);
			color.saturate(saturation);
			color *= (150 / 255.0f);
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 0, 0)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 0)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 0)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 0, 0)) * world_gen::cube_size, color));
		}

		if (this->top()) {
			auto color = qpl::frgb(qpl::get_rainbow_color(qpl::clamp_0_1(std::fmod(hue + 0.98, 1.0))));
			color.light(lightness);
			color.saturate(saturation);
			color *= (250 / 255.0f);
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 0)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(1, 1, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 1)) * world_gen::cube_size, color));
			graphic::va.add(qgl::make_vertex((pos + qpl::vec(0, 1, 0)) * world_gen::cube_size, color));
		}
	}

	constexpr operator bool() {
		return qpl::bool_cast(this->visible_sides.data);
	}
};

cube empty_cube(false);

constexpr qpl::size chunk_size = 32;
constexpr qpl::size chunk_blocks = qpl::pow(chunk_size, 3);

struct chunk {
	std::array<cube, chunk_blocks> cubes;
	qpl::vector3i position;


	constexpr static qpl::vector3<qpl::u8> to_vec3(qpl::size index) {
		return qpl::vec(
			qpl::u8_cast(index & (chunk_size - 1)),
			qpl::u8_cast((index >> qpl::log2(chunk_size)) & (chunk_size - 1)),
			qpl::u8_cast((index >> (qpl::log2(chunk_size) * 2)) & (chunk_size - 1))
		);
	}
	constexpr static qpl::size from_vec3(qpl::vector3<qpl::u8> pos) {
		return (qpl::size_cast(pos.data[0]) | (qpl::size_cast(pos.data[1]) << qpl::log2(chunk_size)) | (qpl::size_cast(pos.data[2]) << (qpl::log2(chunk_size) * 2)));
	}

	void fill() {
		for (qpl::size i = 0u; i < chunk_blocks; ++i) {

			auto pos = qpl::vec3d(this->to_vec3(i)) + this->position * chunk_size;
			//auto value = world_gen::noise.get(pos.data, 0.015, 1);
			auto value = world_gen::noise.get(pos.data, 0.015, 3);

			auto min = pos.min();
			auto max = pos.max();
			auto smallest_distance = (qpl::min(min, (world_gen::chunks_width * chunk_size - 1) - max) / (world_gen::chunks_width * chunk_size)) * 2;

			//if (smallest_distance > 0.3) {
			//	smallest_distance = 1.0;
			//}

			//auto border = 0.53 * smallest_distance;
			auto border = smallest_distance;

			if (value > border) {
				this->cubes[i].visible_sides.fill(false);
			}
		}
	}

	constexpr auto& get(qpl::i8 x, qpl::i8 y, qpl::i8 z) {
		if (x < 0 || x >= chunk_size || y < 0 || y >= chunk_size || z < 0 || z >= chunk_size) {
			return empty_cube;
		}
		else {
			return this->cubes[this->from_vec3(qpl::vec(x, y, z))];
		}
	}
	constexpr const auto& get(qpl::i8 x, qpl::i8 y, qpl::i8 z) const {
		if (x < 0 || x >= chunk_size || y < 0 || y >= chunk_size || z < 0 || z >= chunk_size) {
			return empty_cube;
		}
		else {
			return this->cubes[this->from_vec3(qpl::vec(x, y, z))];
		}
	}

	void add_to_va() {
		//qpl::println("add_to_va ", this->position);
		for (qpl::size i = 0u; i < chunk_blocks; ++i) {
			auto pos = this->to_vec3(i);
			this->cubes[i].fill_va(pos + this->position * chunk_size);
		}
	}

	void cleanup() {
		for (qpl::i8 z = 0; z < chunk_size; ++z) {
			for (qpl::i8 y = 0; y < chunk_size; ++y) {
				for (qpl::i8 x = 0; x < chunk_size; ++x) {

					auto index = this->from_vec3(qpl::vec(x, y, z));
					auto& check = this->cubes[index];
					if (!check) {
						continue;
					}

					auto& top = this->get(x, y + 1, z);
					auto& bottom = this->get(x, y - 1, z);
					auto& left = this->get(x - 1, y, z);
					auto& right = this->get(x + 1, y, z);
					auto& back = this->get(x, y, z + 1);
					auto& front = this->get(x, y, z - 1);

					if (check.left() && left) {
						check.set_left(false);
						left.set_right(false);
					}
					if (check.right() && right) {
						check.set_right(false);
						right.set_left(false);
					}
					if (check.front() && front) {
						check.set_front(false);
						front.set_back(false);
					}
					if (check.back() && back) {
						check.set_back(false);
						back.set_front(false);
					}
					if (check.top() && top) {
						check.set_top(false);
						top.set_bottom(false);
					}
					if (check.bottom() && bottom) {
						check.set_bottom(false);
						bottom.set_top(false);
					}
				}
			}
		}
	}
};

struct world {
	std::unordered_map<qpl::vector3i, chunk> chunks;


};

struct voxel_state : qsf::base_state {

	void create() {
		world_gen::noise.set_seed_random();
		graphic::va.clear();
		this->world.chunks.clear();

		constexpr qpl::size size = qpl::pow(world_gen::chunks_width, 3);
		for (qpl::size i = 0u; i < size; ++i) {
			auto pos = qpl::vec3i(qpl::base_array<3>(i, world_gen::chunks_width));

			std::hash<qpl::vec3i> hash;
			auto hash_value = hash(pos);

			this->world.chunks[pos].position = pos;
			this->world.chunks[pos].fill();
			this->world.chunks[pos].cleanup();
			this->world.chunks[pos].add_to_va();
		}
		graphic::va.update();
	}

	void init() override {
		this->clear_color = qpl::rgb(30, 30, 40);

		this->set_active(true);
		this->create();
		this->set_active(false);


	}
	void updating() override {
		this->camera_lock.update(this->camera, *this);
		this->update(this->camera);

		if (this->event().key_single_pressed(sf::Keyboard::Escape)) {
			this->pop_this_state();
		}
		if (this->event().key_single_pressed(sf::Keyboard::X)) {
			this->create();
		}
	}

	void drawing() override {
		this->draw(graphic::va, this->camera);
	}

	qsf::camera_cursor_lock camera_lock;
	qpl::camera camera;

	world world;
};

int main() try {
	qsf::framework framework;
	framework.enable_gl();
	framework.set_title("QPL");
	framework.set_dimension({ 1400u, 950u });

	framework.add_state<voxel_state>();
	framework.game_loop();
}
catch (std::exception& any) {
	qpl::println("caught exception:\n", any.what());
	qpl::system_pause();
}