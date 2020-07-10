//
// Created by tycoon on 10.07.2020.
//

#ifndef OPTIMAL_STORAGE_H
#define OPTIMAL_STORAGE_H

#include "shared_data.h"

struct optimal_storage {

public:

	using iterator = digit_t *;
	using const_iterator = digit_t const *;

	optimal_storage();
	optimal_storage(size_t, digit_t);
	optimal_storage(optimal_storage const &);
	optimal_storage &operator=(optimal_storage const &);
	~optimal_storage();
	size_t size() const;
	void resize(size_t);
	void resize(size_t, digit_t);
	digit_t &operator[](size_t i);
	digit_t const &operator[](size_t i) const;
	digit_t &back();
	digit_t const &back() const;
	void push_back(digit_t);
	void pop_back();
	const_iterator begin() const;
	const_iterator end() const;

private:

	static size_t constexpr MAX_STATIC_DATA_SIZE = sizeof(void *) / sizeof(uint32_t);

	size_t size_;
	bool is_small;
	union {
		shared_data *dynamic_data;
		uint32_t static_data[MAX_STATIC_DATA_SIZE];
	};

	bool small() const;
};

#endif //OPTIMAL_STORAGE_H
