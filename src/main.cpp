
#include <qpl/qpl.hpp>
#include "world.hpp"


struct voxel_state : qsf::base_state {

	void create() {
		qpl::clock clock;
		graphic::va.clear();
		this->world.chunks.clear();

		++this->creation_count;
		qpl::println('#', this->creation_count);
		//qpl::reset_benchmark();

		for (auto [x, y, z] : qpl::list_possibilities(world_gen::generate_chunks)) {

			auto pos = qpl::vec(x, y, z);

			std::hash<qpl::vec3i> hash;
			auto hash_value = hash(pos);

			this->world.chunks[pos].position = pos;

			qpl::benchmark("chunk :: generate()", [&]() { this->world.chunks[pos].generate(); });
			qpl::benchmark("chunk :: surfaces()", [&]() { this->world.chunks[pos].remove_hidden_surfaces(); });
		}


		qpl::benchmark("chunk :: borders()", [&]() { this->world.remove_chunk_border_surfaces(); });

		qpl::benchmark("light :: heights()", [&]() { this->world.create_light_height_values(); });
		qpl::benchmark("light :: apply()", [&]() { this->world.apply_lightness(); });

		auto total_sides = this->world.get_total_surface_count() * 4;
		qpl::println("surfaces = ", this->world.get_total_surface_count());

		graphic::va.vertices.reserve(total_sides);

		for (auto& chunk : this->world.chunks) {
			qpl::benchmark("chunk :: add_vertices()", [&]() { chunk.second.add_vertices(); });
		}

		qpl::benchmark("va.update()", [&]() { graphic::va.update(); });
		graphic::va.update();

		qpl::println();
		qpl::print_benchmark();
		qpl::println("elapsed = ", clock.elapsed_str());

		qpl::println();
		qpl::println_repeat("- ", 50);
		qpl::println();

		qpl::clear_benchmark();
	}

	void init() override {
		this->clear_color = qpl::rgb(136, 205, 220);

		auto reserve = 100 * 1024 * 1024;
		graphic::va.vertices.reserve(reserve);

		world_gen::noise.set_seed(1234);
		//world_gen::seed_random();
		//this->set_active(true);
		this->create();
		//this->set_active(false);

		qgl::gl::print_errors();

	}
	void updating() override {
		this->camera_lock.update(this->camera, *this);
		this->update(this->camera);

		if (this->generating) {
			this->world.load(this->camera.position / world_gen::block_size, 2);
		}

		if (this->world.generated_chunks_since_update && this->update_clock.has_elapsed_reset(0.1)) {
			//qpl::benchmark("world :: update_vertices", [&]() { this->world.update_vertices(); });
			//this->set_active(true);
			qpl::benchmark("graphic :: update", [&]() { 
				//graphic::va.update_sub(this->va_size_before, graphic::va.size());
				graphic::va.update();
				//graphic::va.update_sub(0, graphic::va.draw_count());
				});
			//this->set_active(false);

			qgl::gl::print_errors();
			qpl::print_benchmark();
			qpl::println("generated chunks : ", this->world.generated_chunks_since_update);
			qpl::println("generated : ", this->va_size_before, " -- ", graphic::va.size(), " ---- ", graphic::va.vertices.capacity(), " - ", graphic::va.vertices.data());
			this->world.generated_chunks_since_update = 0u;
			this->va_size_before = graphic::va.draw_count();
		}

		if (this->event().key_single_pressed(sf::Keyboard::Escape)) {
			this->pop_this_state();
		}
		if (this->event().key_single_pressed(sf::Keyboard::X)) {
			this->create();
		}
		if (this->event().key_single_pressed(sf::Keyboard::G)) {
			this->generating = !this->generating;
		}
	}

	void drawing() override {
		this->draw(graphic::va, this->camera);
	}

	qsf::camera_cursor_lock camera_lock;
	qpl::camera camera;
	qpl::size creation_count = 0u;
	qpl::clock update_clock;
	qpl::size va_size_before = 0u;
	bool generating = false;

	world world;
};

void perlin_test() {

	qpl::f64 sum = 0.0;
	qpl::perlin_noise noise;

	qpl::clock clock;
	constexpr auto range = qpl::vec(256, 256, 256);
	for (qpl::vec3i pos : qpl::list_possibilities(range)) {

		auto value = noise.get(pos, 0.01, 2);
		//auto value = qpl::random(0.0, 1.0);
		if (value > 0.5) {
			sum += 1e-3;
		}
	}
	qpl::println("sum  = ", sum);
	qpl::println("time = ", clock.elapsed_str());
}

void test() {

	std::set<std::pair<qpl::isize, qpl::isize>> seen;
	for (qpl::size i = 0u;; ++i) {
		auto pair = qpl::spiral_pairing(i);

		if (seen.find(pair) != seen.find(pair)) {
			qpl::println("duplicate! ", pair);
		}
		seen.insert(pair);
		if (qpl::get_time_signal(0.1)) {
			qpl::println("#", i, " = ", pair.first, " ", pair.second);
		}
	}
}

int main() try {

	//test();

	qsf::framework framework;
	framework.enable_gl();
	framework.set_title("QPL");
	framework.set_dimension({ 1900u, 1030u });

	framework.add_state<voxel_state>();
	framework.game_loop();
}
catch (std::exception& any) {
	qpl::println("caught exception:\n", any.what());
	qpl::system_pause();
}