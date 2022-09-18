#pragma once

#include <qpl/qpl.hpp>
#include "block.hpp"
#include "data.hpp"
#include "cross_search.hpp"

struct chunk {
	std::array<block, chunk_blocks> blocks;
	std::vector<chunk_index_type> indices;
	qpl::vector3i position;
	qpl::size visible_surfaces = 0u;
	graphic::va_type va;


	bool empty() const {
		return this->indices.empty();
	}

	constexpr static qpl::vector3<qpl::u8> to_vec3(chunk_index_type index) {

		return qpl::vec(
			qpl::u8_cast(index & chunk_bit_mask),
			qpl::u8_cast((index >> chunk_log2) & chunk_bit_mask),
			qpl::u8_cast((index >> (chunk_log2 * 2)) & chunk_bit_mask)
		);
	}
	constexpr static chunk_index_type from_vec3(qpl::vector3<qpl::u8> pos) {
		return 
			(static_cast<chunk_index_type>(pos.data[0]) | 
			(static_cast<chunk_index_type>(pos.data[1]) << chunk_log2) | 
			(static_cast<chunk_index_type>(pos.data[2]) << (chunk_log2 * 2)));
	}
	qpl::vector3i world_position(chunk_index_type index) const {
		return qpl::vector3i(this->to_vec3(index)) + this->position * chunk_size;
	}

	void generate() {
		this->indices.clear();

		for (chunk_index_type i = 0u; i < chunk_blocks; ++i) {

			auto pos = this->world_position(i);

			//if (qpl::random_b(0.001)) {
			//	this->blocks[i].set_fully_visible();
			//	this->indices.push_back(static_cast<chunk_index_type>(i));
			//}
			auto value = world_gen::noise.get(pos, 0.01, 2);
			auto border = 0.5;
			
			if (value < border) {
				this->blocks[i].set_fully_visible();
				this->indices.push_back(static_cast<chunk_index_type>(i));
			}
		}
	}

	constexpr auto& get(qpl::i8 x, qpl::i8 y, qpl::i8 z) {
		if (x < 0 || x >= chunk_size || y < 0 || y >= chunk_size || z < 0 || z >= chunk_size) {
			return empty_block;
		}
		else {
			return this->blocks[this->from_vec3(qpl::vec(x, y, z))];
		}
	}

	void update_vertices() {
		this->va.vertices.reserve(this->visible_surfaces * 4);
		this->va.clear();

		for (auto& index : this->indices) {
			this->blocks[index].fill_va(this->va, this->world_position(index));
		}
	}

	void add_vertices() {
		graphic::va.add(this->va);

	}

	void update_used_indices() {
		this->visible_surfaces = 0u;
		std::vector<chunk_index_type> refined_indices;
		for (auto& index : this->indices) {
			auto value = this->blocks[index].bits;
			if (value.data) {
				refined_indices.push_back(index);
				this->visible_surfaces += value.number_of_set_bits();
			}
		}
		this->indices = refined_indices;
	}

	void remove_hidden_surfaces() {
		for (auto& index : this->indices) {
			auto pos = this->to_vec3(index);
			auto& check = this->blocks[index];
			if (!check) {
				continue;
			}

			auto get_side = [&](qpl::size i) {
				//return &this->get(pos.x + cross_surfaces[i].delta.x, pos.y + cross_surfaces[i].delta.y, pos.z + cross_surfaces[i].delta.z);
				return &this->get(pos.x + cross_surfaces[i].dx, pos.y + cross_surfaces[i].dy, pos.z + cross_surfaces[i].dz);
			};

			std::array<block*, 3> sides = {
				get_side(0), 
				get_side(1), 
				get_side(2), 
			};

			auto make = [&](qpl::size i) {
				if (*sides[i] && check.bits[cross_surfaces[i].surface]) {
					check.bits[cross_surfaces[i].surface] = 0;
					sides[i]->bits[cross_surfaces[i].dsurface] = 0;
				}
			};

			make(0);
			make(1);
			make(2);
		}
		this->update_used_indices();
	}
};
