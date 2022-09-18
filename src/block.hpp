#pragma once

#include <qpl/qpl.hpp>
#include "block_surfaces.hpp"

struct block {
	qpl::bitset<6> bits;
	qpl::u8 lightness = 255;
	block() {

	}
	block(bool b) {
		if (b) {
			this->set_fully_visible();
		}
	}

	void set_fully_visible() {
		this->bits.data = chunk_bit_mask;
	}
	bool visible() const {
		return qpl::bool_cast(this->bits.data);
	}
	bool empty() const {
		return !qpl::bool_cast(this->bits.data);
	}

	constexpr bool left() const {
		return this->bits[direction::left];
	}
	constexpr bool right() const {
		return this->bits[direction::right];
	}
	constexpr bool top() const {
		return this->bits[direction::top];
	}
	constexpr bool bottom() const {
		return this->bits[direction::bottom];
	}
	constexpr bool front() const {
		return this->bits[direction::front];
	}
	constexpr bool back() const {
		return this->bits[direction::back];
	}

	void set_left(bool b) {
		this->bits[direction::left] = b;
	}
	void set_right(bool b) {
		this->bits[direction::right] = b;
	}
	void set_top(bool b) {
		this->bits[direction::top] = b;
	}
	void set_bottom(bool b) {
		this->bits[direction::bottom] = b;
	}
	void set_front(bool b) {
		this->bits[direction::front] = b;
	}
	void set_back(bool b) {
		this->bits[direction::back] = b;
	}

	void fill_va(graphic::va_type& va, qpl::vector3i pos) {

		qpl::f32 hue = std::fmod(world_gen::noise_hue.get(pos, 0.003f, 3) + 0.5f, 1.0f);
		auto color = qpl::frgb(qpl::get_rainbow_color(hue)).grayified(0.5);

		auto index = this->bits.data;

		auto size = graphic::va.size();
		table::add_to_va(va, this->bits.data);

		for (qpl::size i = size; i < graphic::va.vertices.size(); ++i) {
			graphic::va.vertices[i].position = (pos + graphic::va.vertices[i].position) * world_gen::block_size;
			graphic::va.vertices[i].color = (graphic::va.vertices[i].color * color) * (this->lightness / 255.f);
		}
	}

	constexpr operator bool() const {
		return qpl::bool_cast(this->bits.data);
	}
};

block empty_block(false);
