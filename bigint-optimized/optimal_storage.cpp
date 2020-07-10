//
// Created by tycoon on 10.07.2020.
//

#include "optimal_storage.h"
#include <cassert>
#include <iostream>
#include <algorithm>

optimal_storage::optimal_storage()
	: size_(0), is_small(false)
{}

optimal_storage::optimal_storage(size_t size, uint32_t digit)
	: size_(size), is_small(size <= MAX_STATIC_DATA_SIZE)
{
	if (small()) {
//		for (size_t i = 0; i < size_; ++i) {
//			static_data[i] = digit;
//		}
		std::fill(static_data, static_data + size_, digit);
	} else {
		dynamic_data = new shared_data(size, digit);
	}
}

optimal_storage::optimal_storage(optimal_storage const &other)
	: size_(other.size_), is_small(other.small())
{
	if (small()) {
		std::copy_n(other.static_data, size(), static_data);
	} else {
		dynamic_data = other.dynamic_data;
		dynamic_data->inc();
	}
}

optimal_storage &optimal_storage::operator=(optimal_storage const &rhs)
{
	if (this == &rhs) return *this;

	this->~optimal_storage();

	size_ = rhs.size_;
	is_small = rhs.is_small;

	if (small()) {
		std::copy_n(rhs.static_data, size(), static_data);
	} else {
		dynamic_data = rhs.dynamic_data;
		dynamic_data->inc();
	}

	return *this;
}

optimal_storage::~optimal_storage()
{
	if (!small()) {
		dynamic_data->dec();
	}
}

size_t optimal_storage::size() const
{
	return size_;
}

bool optimal_storage::small() const
{
	return is_small;
}

void optimal_storage::resize(size_t new_sz, uint32_t digit)
{
	assert (new_sz >= size_);

	if (new_sz <= MAX_STATIC_DATA_SIZE) {
//		for (size_t i = size_; i < new_sz; ++i) {
//			static_data[i] = digit;
//		}
		std::fill(static_data + size_, static_data + new_sz, digit);
		size_ = new_sz;
		return;
	}

	size_t size_was = size_;
	
	if (!small()) {
		dynamic_data = dynamic_data->unshare();
		dynamic_data->resize(new_sz);
		size_ = new_sz;
		is_small = false;
		return;
	}
	else {  // small->big
		std::vector<uint32_t> temp(static_data, static_data + size_);
		dynamic_data = new shared_data(temp);
		dynamic_data->resize(new_sz);
		size_ = new_sz;
		is_small = false;
	}
	
	for (size_t i = size_was; i < new_sz; ++i) {
		(*this)[i] = digit;
	}

}

void optimal_storage::resize(size_t new_sz)
{
	resize(new_sz, 0);
}

uint32_t &optimal_storage::operator[](size_t i)
{
	assert (i < size_);

	if (small()) {
		return static_data[i];
	}
	dynamic_data = dynamic_data->unshare();
	return (*dynamic_data)[i];
}

uint32_t const &optimal_storage::operator[](size_t i) const
{
	assert (i < size_);

	if (small()) {
		return static_data[i];
	}
	return (*dynamic_data)[i];
}

uint32_t const &optimal_storage::back() const
{
	return (*this)[size_ - 1];
}

uint32_t &optimal_storage::back()
{
	return (*this)[size_ - 1];
}

void optimal_storage::push_back(uint32_t digit)
{
	if (size_ < MAX_STATIC_DATA_SIZE) {
		static_data[size_++] = digit;
	} else if (size_ == MAX_STATIC_DATA_SIZE) {
		resize(size_ + 1);
		(*this)[size_ - 1] = digit;
	} else {
		dynamic_data = dynamic_data->unshare();
		dynamic_data->push_back(digit);
		size_++;
	}
}

void optimal_storage::pop_back()
{
	if (!small()) {
		dynamic_data = dynamic_data->unshare();
		dynamic_data->pop_back();
	}
	size_--;
}

optimal_storage::const_iterator optimal_storage::begin() const
{
	if (small()) return static_data;
	return dynamic_data->data();
}

optimal_storage::const_iterator optimal_storage::end() const
{
	return begin() + size_;
}
