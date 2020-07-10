//
// Created by tycoon on 10.07.2020.
//

#include "optimal_storage.h"
#include <cassert>
#include <iostream>

optimal_storage::optimal_storage()
	: size_(0), is_small(false)
{}

optimal_storage::optimal_storage(size_t size, digit_t digit)
	: size_(size), is_small(size <= MAX_STATIC_DATA_SIZE)
{
	if (small()) {
		for (size_t i = 0; i < size_; ++i) {
			static_data[i] = digit;
		}
	} else {
		dynamic_data = new shared_data(size, digit);
	}
}

optimal_storage::optimal_storage(optimal_storage const &other)
	: size_(other.size_), is_small(other.small())
{
	if (small()) {
		for (size_t i = 0; i < size_; ++i) {
			static_data[i] = other.static_data[i];
		}
//		std::copy(static_data, other.static_data, size());
	} else {
		dynamic_data = other.dynamic_data;
		dynamic_data -> inc();
	}
}

optimal_storage &optimal_storage::operator=(optimal_storage const &rhs)
{
	if (this == &rhs) return *this;

	if (!small()) {
		dynamic_data -> dec();
	}
	size_ = rhs.size_;
	is_small = rhs.is_small;

	if (small()) {
		for (size_t i = 0; i < rhs.size(); ++i) {
			static_data[i] = rhs.static_data[i];
		}
//		std::copy(static_data, other.static_data, size());
	} else {
		dynamic_data = rhs.dynamic_data;
		dynamic_data -> inc();
	}

	return *this;
}

optimal_storage::~optimal_storage()
{
	if (!small()) {
		dynamic_data -> dec();
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

void optimal_storage::resize(size_t new_sz)
{
	if (new_sz <= size_) {
		return;
	}

	if (new_sz <= MAX_STATIC_DATA_SIZE) {
		size_ = new_sz;
		return;
	}

	if (!small()) {
		dynamic_data = dynamic_data -> unshare();
		dynamic_data -> resize(new_sz);
		size_ = new_sz;
		is_small = false;
		return;
	}
	else {  // small -> big
		std::vector<digit_t> temp(static_data, static_data + size_);
		dynamic_data = new shared_data(temp);
		dynamic_data -> resize(new_sz);
		size_ = new_sz;
		is_small = false;
	}
}

void optimal_storage::resize(size_t new_sz, digit_t digit)
{
	size_t size_was = size_;

	resize(new_sz);
	for (size_t i = size_was; i < new_sz; ++i) {
		(*this)[i] = digit;
	}
}

digit_t &optimal_storage::operator[](size_t i)
{
	assert (i < size_);

	if (small()) {
		return static_data[i];
	}
	dynamic_data = dynamic_data -> unshare();
	return (*dynamic_data)[i];
}

digit_t const &optimal_storage::operator[](size_t i) const
{
	assert (i < size_);

	if (small()) {
		return static_data[i];
	}
	return (*dynamic_data)[i];
}

digit_t const &optimal_storage::back() const
{
	return (*this)[size_ - 1];
}

digit_t &optimal_storage::back()
{
	return (*this)[size_ - 1];
}

void optimal_storage::push_back(digit_t digit)
{
	if (size_ < MAX_STATIC_DATA_SIZE) {
		static_data[size_++] = digit;
	} else if (size_ == MAX_STATIC_DATA_SIZE) {
		resize(size_ + 1);
		(*this)[size_ - 1] = digit;
	} else {
		dynamic_data = dynamic_data -> unshare();
		dynamic_data -> push_back(digit);
		size_++;
	}
}

void optimal_storage::pop_back()
{
	if (!small()) {
		dynamic_data = dynamic_data -> unshare();
		dynamic_data -> pop_back();
	}
	size_--;
}

optimal_storage::const_iterator optimal_storage::begin() const
{
	if (small()) return reinterpret_cast<const_iterator>(&static_data);
	return dynamic_data -> data();
}

optimal_storage::const_iterator optimal_storage::end() const
{
	return begin() + size_;
}

optimal_storage::iterator optimal_storage::begin()
{
	if (small()) return reinterpret_cast<iterator>(&static_data);
	return dynamic_data -> data();
}

optimal_storage::iterator optimal_storage::end()
{
	return begin() + size_;
}
