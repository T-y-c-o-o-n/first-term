//
// Created by tycoon on 10.07.2020.
//

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <cstdint>
#include <vector>

using digit_t = uint32_t;

struct shared_data {
public:
	size_t ref_cnt;
	std::vector<digit_t> buffer;

	shared_data(size_t, digit_t);
	explicit shared_data(std::vector<digit_t> const &);
	~shared_data() = default;
	void resize(size_t i);
	void push_back(digit_t);
	void pop_back();
	digit_t *data();
	std::vector<digit_t> const &get_buffer() const;
	digit_t const &operator[](size_t) const;
	digit_t &operator[](size_t);
	void dec();
	void inc();
	shared_data *unshare();
};

#endif //SHARED_DATA_H
