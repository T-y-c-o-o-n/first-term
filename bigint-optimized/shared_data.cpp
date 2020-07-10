//
// Created by tycoon on 10.07.2020.
//

#include "shared_data.h"
#include <iostream>

shared_data::shared_data(size_t size, digit_t digit)
	: ref_cnt(1), buffer(std::vector<digit_t>(size, digit))
{}

shared_data::shared_data(std::vector<digit_t> const &digits)
	: ref_cnt(1), buffer(std::vector<digit_t>(digits))
{}


void shared_data::resize(size_t new_sz)
{
	buffer.resize(new_sz);
}

digit_t *shared_data::data()
{
	return buffer.data();
}

std::vector<digit_t> const &shared_data::get_buffer() const
{
	return buffer;
}

digit_t const &shared_data::operator[](size_t i) const
{
	return buffer[i];
}

digit_t &shared_data::operator[](size_t i)
{
	return buffer[i];
}

void shared_data::push_back(digit_t digit)
{
	buffer.push_back(digit);
}

void shared_data::pop_back()
{
	buffer.pop_back();
}

void shared_data::dec()
{
	--ref_cnt;
	if (ref_cnt == 0) {
		delete this;
	}
}

void shared_data::inc()
{
	++ref_cnt;
}

shared_data *shared_data::unshare()
{
	if (ref_cnt == 1) {
		return this;
	}
	auto *new_data = new shared_data(buffer);
	dec();
	return new_data;
}
