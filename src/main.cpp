
#include <qpl/qpl.hpp>
#include "data.hpp"
#include "cube.hpp"
#include "chunk.hpp"
#include "cube_sides.hpp"

struct world {
	std::unordered_map<qpl::vector3i, chunk> chunks;

	qpl::size get_cube_count() const {
		qpl::size result = 0u;
		for (auto& chunk : this->chunks) {
			result += chunk.second.indices.size();
		}
		return result;
	}
};

struct voxel_state : qsf::base_state {

	void create() {
		qpl::clock clock;
		world_gen::noise.set_seed(1234);
		//world_gen::seed_random();
		graphic::va.clear();
		this->world.chunks.clear();

		qpl::println("use table : ", qpl::bool_cast(config::use_cube_sides_table));
		qpl::reset_benchmark();
		constexpr qpl::size size = qpl::pow(world_gen::chunks_width, 3);
		for (qpl::size i = 0u; i < size; ++i) {
			auto pos = qpl::vec3i(qpl::base_array<3>(i, world_gen::chunks_width));

			std::hash<qpl::vec3i> hash;
			auto hash_value = hash(pos);

			this->world.chunks[pos].position = pos;

			qpl::benchmark("chunk.fill()",		[&]() { this->world.chunks[pos].fill(); });
			qpl::benchmark("chunk.cleanup()",	[&]() { this->world.chunks[pos].cleanup(); });
		}

		auto total_cubes = this->world.get_cube_count();

		graphic::va.vertices.reserve(total_cubes);

		for (auto& chunk : this->world.chunks) {
			qpl::benchmark("chunk.add_to_va()", [&]() { chunk.second.add_to_va(); });
		}

		qpl::benchmark("va.update()", [&]() { graphic::va.update(); });
		graphic::va.update();
		qpl::print_benchmark();
		qpl::println("elapsed = ", clock.elapsed_str());

		qpl::println();
		qpl::println_repeat("- ", 50);
		qpl::println();
		
	}

	void init() override {
		this->clear_color = qpl::rgb(30, 30, 40);

		//prepare_cube_sides();

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