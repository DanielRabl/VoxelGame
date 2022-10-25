#pragma once

#include <qpl/qpl.hpp>
#include "chunk.hpp"

struct world {
	std::unordered_map<qpl::vector3i, chunk> chunks;
	std::unordered_map<qpl::vector2i, std::array<qpl::i32, chunk_size* chunk_size>> light_height_values;
	qpl::size generated_chunks_since_update = 0u;

	qpl::size get_total_surface_count() const {
		qpl::size result = 0u;
		for (auto& chunk : this->chunks) {
			result += chunk.second.visible_surfaces;
		}
		return result;
	}
	bool chunk_exists(qpl::vector3i chunk_pos) const {
		return this->chunks.find(chunk_pos) != this->chunks.cend();
	}
	bool chunk_empty(qpl::vector3i chunk_pos) const {
		if (!this->chunk_exists(chunk_pos)) {
			return true;
		}
		return this->chunks.at(chunk_pos).empty();
	}

	template<typename T, qpl::size N>
	static constexpr auto world_pos_to_chunk_pos(qpl::vectorN<T, N> world_pos) {
		auto result = world_pos / chunk_size;
		return result;
	}

	auto& get_block(qpl::vector3i world_pos) {
		auto chunk_pos = this->world_pos_to_chunk_pos(world_pos);
		auto chunk_block_pos = world_pos - (chunk_pos * chunk_size);

		if (this->chunk_empty(chunk_pos)) {
			return empty_block;
		}

		return this->chunks.at(chunk_pos).blocks[chunk::from_vec3(chunk_block_pos)];
	}
	bool block_exists(qpl::vector3i world_pos) const {
		auto chunk_pos = this->world_pos_to_chunk_pos(world_pos);
		auto chunk_block_pos = world_pos - (chunk_pos * chunk_size);

		if (this->chunk_empty(chunk_pos)) {
			return false;
		}

		return this->chunks.at(chunk_pos).blocks[chunk::from_vec3(chunk_block_pos)].visible();
	}

	void update_vertices() {
		graphic::va.clear();
		for (auto& chunk : this->chunks) {
			chunk.second.add_vertices();
		}
	}

	bool load(qpl::vector3i chunk_pos) {
		if (!this->chunk_exists(chunk_pos)) {
			++this->generated_chunks_since_update;
			//qpl::println("load chunk [", chunk_pos, "]");

			auto& chunk = this->chunks[chunk_pos];

			chunk.position = chunk_pos;
			qpl::benchmark("chunk :: generate", [&]() { chunk.generate(); });
			qpl::benchmark("chunk :: surfaces", [&]() { chunk.remove_hidden_surfaces(); });

			qpl::benchmark("chunk :: chunk border", [&]() { this->remove_chunk_border_surfaces_and_neighbour(chunk_pos); });
			chunk.remove_hidden_surfaces();
			

			chunk.add_vertices();

			//this->create_light_height_values(chunk_pos);
			//this->apply_lightness(chunk_pos);

			return true;
		}
		return false;
	}

	void load(qpl::vector3i world_pos, qpl::size surround) {

		auto chunk_pos = this->world_pos_to_chunk_pos(world_pos);
		auto start = qpl::vec3i::filled(-qpl::isize_cast(surround));
		auto end = qpl::vec3i::filled(qpl::isize_cast(surround + 1));

		std::vector<qpl::vec3i> search;
		for (qpl::vec3i pos : qpl::list_possibilities(start, end)) {
			search.push_back(pos);
		}
		qpl::shuffle(search);

		qpl::size loaded_ctr = 0u;
		for (qpl::vec3i pos : search) {
			if (this->load(chunk_pos + pos)) {
				++loaded_ctr;

				if (loaded_ctr > 6u) {
					return;
				}
			}
		}
	}

	void remove_chunk_border_surfaces(qpl::vector3i chunk_pos) {
		auto& chunk = this->chunks[chunk_pos];

		for (auto& cross : cross_surfaces) {
			auto compare_chunk_pos = chunk_pos;
			compare_chunk_pos += qpl::vec(cross.dx, cross.dy, cross.dz);

			if (this->chunk_exists(compare_chunk_pos)) {
				auto& compare_chunk = this->chunks[compare_chunk_pos];

				for (qpl::size i = 0u; i < chunk_size * chunk_size; ++i) {
					auto x = qpl::u8_cast(i % chunk_size);
					auto y = qpl::u8_cast(i / chunk_size);

					qpl::vector3<qpl::u8> check_pos;
					qpl::vector3<qpl::u8> compare_pos;
					if (cross.vec3_delta.x) {
						check_pos.y = x;
						check_pos.z = y;
					}
					else if (cross.vec3_delta.y) {
						check_pos.x = x;
						check_pos.z = y;
					}
					else if (cross.vec3_delta.z) {
						check_pos.x = x;
						check_pos.y = y;
					}
					compare_pos = check_pos + (cross.vec3_delta * (chunk_size - 1));

					auto& check = chunk.get(check_pos.x, check_pos.y, check_pos.z);
					auto& compare = compare_chunk.get(compare_pos.x, compare_pos.y, compare_pos.z);

					if (compare && check.bits[cross.surface]) {
						check.bits[cross.surface] = 0;
						compare.bits[cross.dsurface] = 0;
					}
				}
			}
		}
	}

	void remove_chunk_border_surfaces_both(qpl::vector3i chunk_pos, cross_surface_search cross, bool neighbour = true) {
		auto& chunk = this->chunks[chunk_pos];
		auto compare_chunk_pos = chunk_pos;
		compare_chunk_pos += qpl::vec(cross.dx, cross.dy, cross.dz);

		if (this->chunk_exists(compare_chunk_pos)) {
			auto& compare_chunk = this->chunks[compare_chunk_pos];

			for (qpl::size i = 0u; i < chunk_size * chunk_size; ++i) {
				auto x = qpl::u8_cast(i % chunk_size);
				auto y = qpl::u8_cast(i / chunk_size);

				qpl::vector3<qpl::u8> check_pos;
				qpl::vector3<qpl::u8> compare_pos;
				if (cross.vec3_delta.x) {
					check_pos.y = x;
					check_pos.z = y;
				}
				else if (cross.vec3_delta.y) {
					check_pos.x = x;
					check_pos.z = y;
				}
				else if (cross.vec3_delta.z) {
					check_pos.x = x;
					check_pos.y = y;
				}
				compare_pos = check_pos + (cross.vec3_delta * (chunk_size - 1));

				auto& check = chunk.get(check_pos.x, check_pos.y, check_pos.z);
				auto& compare = compare_chunk.get(compare_pos.x, compare_pos.y, compare_pos.z);

				if (compare && check.bits[cross.surface]) {
					check.bits[cross.surface] = 0;
					compare.bits[cross.dsurface] = 0;
				}
			}
		}

		if (neighbour) {
			auto opposite_chunk_pos = chunk_pos;
			opposite_chunk_pos -= qpl::vec(cross.dx, cross.dy, cross.dz);
			if (this->chunk_exists(opposite_chunk_pos)) {
				this->remove_chunk_border_surfaces_both(opposite_chunk_pos, cross, false);
				this->chunks[opposite_chunk_pos].update_used_indices();
			}
		}


	}

	void remove_chunk_border_surfaces_and_neighbour(qpl::vector3i chunk_pos) {
		auto& chunk = this->chunks[chunk_pos];

		for (auto& cross : cross_surfaces) {
			this->remove_chunk_border_surfaces_both(chunk_pos, cross);
		}
	}

	void remove_chunk_border_surfaces() {
		for (auto& chunk : chunks) {
			this->remove_chunk_border_surfaces(chunk.first);
		}

		for (auto& chunk : chunks) {
			chunk.second.update_used_indices();
		}
	}

	void create_light_height_values(qpl::vector3i chunk_pos) {

		auto chunk_xz = qpl::vector2i(chunk_pos.x, chunk_pos.z);

		if (this->light_height_values.find(chunk_xz) == this->light_height_values.cend()) {
			for (qpl::i32 i = 0u; i < chunk_size * chunk_size; ++i) {
				qpl::i32 x = (i % chunk_size) + (chunk_xz.x * chunk_size);
				qpl::i32 z = (i / chunk_size) + (chunk_xz.y * chunk_size);

				this->light_height_values[chunk_xz][i] = 0;
				for (qpl::i32 y = qpl::i32_cast(world_gen::world_size.y); y >= 0; --y) {
					auto pos = qpl::vec(x, y, z);


					if (this->block_exists(pos)) {
						this->light_height_values[chunk_xz][i] = y;
						break;
					}
				}
			}
		}
	}

	void create_light_height_values() {
		this->light_height_values.clear();
		for (auto& chunk : this->chunks) {
			this->create_light_height_values(chunk.first);
		}
	}

	qpl::i32 get_light_height(qpl::vector2i world_pos) const {
		auto chunk_pos = this->world_pos_to_chunk_pos(world_pos);
		if (this->light_height_values.find(chunk_pos) == this->light_height_values.cend()) {
			return 0;
		}
		else {
			auto& chunk = this->light_height_values.at(chunk_pos);
			auto block_pos = (world_pos - (chunk_pos * chunk_size));
			auto index = block_pos.y * chunk_size + block_pos.x;
			return chunk[index];
		}
	}

	void apply_lightness(qpl::vector3i chunk_pos) {
		auto& chunk = this->chunks[chunk_pos];
		for (auto& index : chunk.indices) {
			auto world_pos = chunk.world_position(index);
			auto& block = chunk.blocks[index];



			qpl::vector2i search = qpl::vec(world_pos.x, world_pos.z);
			auto height = this->get_light_height(search);
			if (world_pos.y < height) {


				auto min_length = qpl::f32_max;

				for (auto& i : world_gen::light_search_spiral) {
				
					qpl::vector2i search = qpl::vec(world_pos.x + i.x, world_pos.z + i.y);
					auto height = this->get_light_height(search);
					auto y_diff = qpl::max(0, height - world_pos.y);
				
					auto length = qpl::vec3(i.x, y_diff, i.y).length();
					if (min_length > length) {
						min_length = length;
					}
					if (y_diff == 0.0f) {
						break;
					}
				}

				auto distance = 1 - qpl::clamp_0_1(min_length / world_gen::light_distance);

				block.lightness = qpl::u8_cast(200 * distance + 55);
			}
			else {
				block.lightness = 255;
			}


		}
	}

	void apply_lightness() {
		for (auto& chunk : this->chunks) {
			this->apply_lightness(chunk.first);
		}
	}

};