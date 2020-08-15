//
// Created by tycoon on 10.07.2020.
//

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <cstdint>
#include <vector>

struct shared_data {
  public:
	size_t ref_cnt;
	std::vector<uint32_t> buffer;

	shared_data(size_t, uint32_t);
	explicit shared_data(std::vector<uint32_t> const &);
	~shared_data() = default;
	void resize(size_t i);
	void resize(size_t i, uint32_t);
	void push_back(uint32_t);
	void pop_back();
	uint32_t *data();
	std::vector<uint32_t> const &get_buffer() const;
	uint32_t const &operator[](size_t) const;
	uint32_t &operator[](size_t);
	void dec();
	void inc();
	shared_data *unshare();
};

#endif //SHARED_DATA_H
