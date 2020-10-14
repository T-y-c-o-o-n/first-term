//
// Created by tycoon on 10.07.2020.
//

#include "shared_data.h"
#include <iostream>

shared_data::shared_data(size_t size, uint32_t digit)
	: ref_cnt(1), buffer(std::vector<uint32_t>(size, digit)) {}

shared_data::shared_data(std::vector<uint32_t> const &digits)
	: ref_cnt(1), buffer(std::vector<uint32_t>(digits)) {}

shared_data::shared_data(uint32_t *begin, uint32_t *end)
    : ref_cnt(1), buffer(begin, end) {}

void shared_data::resize(size_t new_sz) {
	buffer.resize(new_sz);
}

void shared_data::resize(size_t new_sz, uint32_t digit) {
	buffer.resize(new_sz, digit);
}

uint32_t *shared_data::data() {
	return buffer.data();
}

std::vector<uint32_t> const &shared_data::get_buffer() const {
	return buffer;
}

uint32_t const &shared_data::operator[](size_t i) const {
	return buffer[i];
}

uint32_t &shared_data::operator[](size_t i) {
	return buffer[i];
}

void shared_data::push_back(uint32_t digit) {
	buffer.push_back(digit);
}

void shared_data::pop_back() {
	buffer.pop_back();
}

void shared_data::dec() {
	--ref_cnt;
	if (ref_cnt == 0) {
		delete this;
	}
}

void shared_data::inc() {
	++ref_cnt;
}

shared_data *shared_data::unshare() {
	if (ref_cnt == 1) {
		return this;
	}
	auto *new_data = new shared_data(buffer);
	dec();
	return new_data;
}
