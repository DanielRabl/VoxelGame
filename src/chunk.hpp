#pragma once

#include <qpl/qpl.hpp>
#include "cube.hpp"
#include "data.hpp"

constexpr qpl::size chunk_size = 32;
constexpr qpl::size chunk_blocks = qpl::pow(chunk_size, 3);

struct chunk {
	std::array<cube, chunk_blocks> cubes;
	std::vector<qpl::u16> indices;
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
		this->indices.clear();

		for (qpl::size i = 0u; i < chunk_blocks; ++i) {

			auto pos = qpl::vec3d(this->to_vec3(i)) + this->position * chunk_size;
			auto value = world_gen::noise.get(pos.data, 0.015, 3);

			auto min = pos.min();
			auto max = pos.max();
			auto smallest_distance = (qpl::min(min, (world_gen::chunks_width * chunk_size - 1) - max) / (world_gen::chunks_width * chunk_size)) * 2;

			auto border = 0.5;

			if (value < border) {
				this->cubes[i].set_fully_visible();
				this->indices.push_back(qpl::u16_cast(i));
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

	void add_to_va() {
		for (auto& index : this->indices) {
			auto pos = this->to_vec3(index);
			this->cubes[index].fill_va(pos + this->position * chunk_size);
		}
	}

	void cleanup() {
		for (auto& index : this->indices) {

			auto pos = this->to_vec3(index);
			auto& check = this->cubes[index];
			if (!check) {
				continue;
			}

			auto& top		= this->get(pos.x,		pos.y + 1,	pos.z);
			auto& bottom	= this->get(pos.x,		pos.y - 1,	pos.z);
			auto& left		= this->get(pos.x - 1,	pos.y,		pos.z);
			auto& right		= this->get(pos.x + 1,	pos.y,		pos.z);
			auto& back		= this->get(pos.x,		pos.y,		pos.z + 1);
			auto& front		= this->get(pos.x,		pos.y,		pos.z - 1);

			if (left && check.left()) {
				check.set_left(false);
				left.set_right(false);
			}
			if (right && check.right()) {
				check.set_right(false);
				right.set_left(false);
			}
			if (front && check.front()) {
				check.set_front(false);
				front.set_back(false);
			}
			if (back && check.back()) {
				check.set_back(false);
				back.set_front(false);
			}
			if (top && check.top()) {
				check.set_top(false);
				top.set_bottom(false);
			}
			if (bottom && check.bottom()) {
				check.set_bottom(false);
				bottom.set_top(false);
			}
		}
	}
};
