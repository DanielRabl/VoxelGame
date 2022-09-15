#include <qpl/qpl.hpp>

namespace graphic {
	qgl::vertex_array<qgl::flag_default, qgl::primitive_quads<qgl::pos3, qgl::frgb>> va;
}

struct cube {
	qpl::bitset<6> visible_sides;

	cube() {
		this->visible_sides.fill(true);
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
		if (this->bottom()) {
			auto color = qpl::rgb::grey_shade(80);
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 0, 0), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 0, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 0, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 0, 0), color));
		}

		if (this->left()) {
			auto color = qpl::rgb::grey_shade(250);
			color = qpl::rgb::magenta();
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 1, 0), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 1, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 0, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 0, 0), color));
		}

		if (this->back()) {
			auto color = qpl::rgb::grey_shade(210);
			color = qpl::rgb::blue();
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 0, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 1, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 1, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 0, 1), color));
		}

		if (this->right()) {
			auto color = qpl::rgb::grey_shade(120);
			color = qpl::rgb::green();
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 0, 0), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 0, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 1, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 1, 0), color));
		}

		if (this->front()) {
			auto color = qpl::rgb::grey_shade(180);
			color = qpl::rgb::red();
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 0, 0), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 1, 0), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 1, 0), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 0, 0), color));
		}

		if (this->top()) {
			auto color = qpl::rgb::grey_shade(250);
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 1, 0), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(1, 1, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 1, 1), color));
			graphic::va.add(qgl::make_vertex(pos + qpl::vec(0, 1, 0), color));
		}
	}

	constexpr operator bool() {
		return qpl::bool_cast(this->visible_sides.data);
	}
};

constexpr qpl::size chunk_size = 16;
constexpr qpl::size chunk_blocks = qpl::pow(chunk_size, 3);

struct chunk {
	std::array<cube, chunk_blocks> cubes;
	qpl::vector3i position;

	constexpr static qpl::vector3<qpl::u8> to_vec3(qpl::size index) {
		return qpl::vec(
			qpl::u8_cast(index & 0xF),
			qpl::u8_cast((index >> 4) & 0xF),
			qpl::u8_cast((index >> 8) & 0xF)
		);
	}
	constexpr static qpl::size from_vec3(qpl::vector3<qpl::u8> pos) {
		return (qpl::size_cast(pos.data[0]) | (qpl::size_cast(pos.data[1]) << 4) | (qpl::size_cast(pos.data[2]) << 8));
	}

	void fill() {
		for (qpl::size i = 0u; i < chunk_blocks; ++i) {
			if (qpl::random_b(0.95)) {
				this->cubes[i].visible_sides.fill(false);
			}
		}
	}

	constexpr auto& get(qpl::u8 x, qpl::u8 y, qpl::u8 z) {
		return this->cubes[this->from_vec3(qpl::vec(x - 1, y, z))];
	}
	constexpr const auto& get(qpl::u8 x, qpl::u8 y, qpl::u8 z) const {
		return this->cubes[this->from_vec3(qpl::vec(x - 1, y, z))];
	}

	void add_to_va() {
		for (qpl::size i = 0u; i < chunk_blocks; ++i) {
			auto pos = this->to_vec3(i);
			this->cubes[i].fill_va(pos + this->position);
		}
	}

	void check() {
		qpl::size removed = 0u;

		for (qpl::u8 z = 1; z < chunk_size - 1; ++z) {
			for (qpl::u8 y = 1; y < chunk_size - 1; ++y) {
				for (qpl::u8 x = 1; x < chunk_size - 1; ++x) {

					if (qpl::get_time_signal(0.1)) {
						qpl::println((int)x, " ", (int)y, " ", (int)z);
					}

					auto& check = this->get(x, y, z);
					if (!check) {
						continue;
					}

					auto& left   = this->get(x - 1, y, z);
					auto& right  = this->get(x + 1, y, z);
					auto& top    = this->get(x, y + 1, z);
					auto& bottom = this->get(x, y - 1, z);
					auto& back   = this->get(x, y, z + 1);
					auto& front  = this->get(x, y, z - 1);

					if (check.left() && left.right()) {
						check.set_left(false);
						left.set_right(false);
						++removed;
					}
					if (check.right() && right.left()) {
						check.set_right(false);
						right.set_left(false);
						++removed;
					}
					if (check.front() && front.back()) {
						check.set_front(false);
						front.set_back(false);
						++removed;
					}
					if (check.back() && back.front()) {
						check.set_back(false);
						back.set_front(false);
						++removed;
					}
					if (check.top() && top.bottom()) {
						check.set_top(false);
						top.set_bottom(false);
						++removed;
					}
					if (check.bottom() && bottom.top()) {
						check.set_bottom(false);
						bottom.set_top(false);
						++removed;
					}
				}
			}
		}

		qpl::println("Removed ", removed * 2, " edges!");
	}
};

struct world {
	std::unordered_map<qpl::vector3i, chunk> chunks;
};

struct voxel_state : qsf::base_state {

	void init() override {
		this->clear_color = qpl::rgb(30, 30, 40);

		auto pos = qpl::vec(1, 1, 1);

		this->world.chunks[pos].fill();
		this->world.chunks[pos].check();
		this->world.chunks[pos].add_to_va();

		qpl::println("size = ", graphic::va.size());
	}
	void updating() override {
		this->camera_lock.update(this->camera, *this);
		this->update(this->camera);

		if (this->event().key_single_pressed(sf::Keyboard::Escape)) {
			this->pop_this_state();
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