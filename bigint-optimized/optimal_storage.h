//
// Created by tycoon on 10.07.2020.
//

#ifndef OPTIMAL_STORAGE_H
#define OPTIMAL_STORAGE_H

#include "shared_data.h"

struct optimal_storage {

  public:

	using iterator = uint32_t *;
	using const_iterator = uint32_t const *;

	optimal_storage();
	optimal_storage(size_t, uint32_t);
	optimal_storage(optimal_storage const &);
	optimal_storage &operator=(optimal_storage const &);
	~optimal_storage();
	size_t size() const;
	void resize(size_t);
	void resize(size_t, uint32_t);
	uint32_t &operator[](size_t i);
	uint32_t const &operator[](size_t i) const;
	uint32_t &back();
	uint32_t const &back() const;
	void push_back(uint32_t);
	void pop_back();
	const_iterator begin() const;
	const_iterator end() const;

  private:

	static size_t constexpr MAX_STATIC_DATA_SIZE = sizeof(shared_data *) / sizeof(uint32_t);

	size_t size_;
	bool is_small;
	union {
		shared_data *dynamic_data;
		uint32_t static_data[MAX_STATIC_DATA_SIZE];
	};

	bool small() const;
};

#endif //OPTIMAL_STORAGE_H
