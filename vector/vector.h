#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <vector>

template<typename T>
struct vector
{
public:
	typedef T *iterator;
	typedef T const *const_iterator;

	vector()
		: _data(nullptr), _size(0), _capacity(0)
	{}

	vector(vector const &other)
		: vector()
	{
		if (!other.empty()) {
			vector temp;
			temp.reserve(other.size());
			for (auto it = other.begin(); it != other.end(); ++it) {
				temp.push_back(*it);
			}
			swap(temp);
		}
	}

	vector &operator=(vector rhs)
	{
		swap(rhs);
		return *this;
	}

	~vector()
	{
		clear();
		operator delete(_data);
	}

	T &operator[](size_t i)
	{
		return _data[i];
	}

	T const &operator[](size_t i) const
	{
		return _data[i];
	}

	T *data()
	{
		return _data;
	}

	T const *data() const
	{
		return _data;
	}

	size_t size() const
	{
		return _size;
	}

	T &front()
	{
		return _data[0];
	}

	T const &front() const
	{
		return _data[0];
	}

	T &back()
	{
		return _data[_size - 1];
	}

	T const &back() const
	{
		return _data[_size - 1];
	}

	void push_back(T val)
	{
		ensure_capacity();
		new(_data + _size) T(val);
		_size++;
	}

	void pop_back()
	{
		destroy(end() - 1, end());
		_size--;
	}

	bool empty() const
	{
		return _size == 0;
	}

	size_t capacity() const
	{
		return _capacity;
	}

	void reserve(size_t new_cap)
	{
		if (new_cap > _capacity) {
			T *temp_data = static_cast<T *>(operator new(new_cap * sizeof(T)));
			size_t temp_size = 0;
			try {
				for (; temp_size < _size; ++temp_size) {
					new(temp_data + temp_size) T(_data[temp_size]);
				}
			} catch (...) {
				destroy(temp_data, temp_data + temp_size);
				operator delete(temp_data);
				throw;
			}

			destroy(begin(), end());
			operator delete(_data);
			_data = temp_data;
			_capacity = new_cap;
		}
	}

	void shrink_to_fit()
	{
		if (_capacity > _size) {
			vector<T> temp(*this);
			swap(temp);
		}
	}

	void clear()
	{
		destroy(begin(), end());
		_size = 0;
	}

	void swap(vector &other)
	{
		std::swap(_data, other._data);
		std::swap(_size, other._size);
		std::swap(_capacity, other._capacity);
	}

	iterator begin()
	{
		return _data;
	}

	iterator end()
	{
		return _data + _size;
	}

	const_iterator begin() const
	{
		return _data;
	}

	const_iterator end() const
	{
		return _data + _size;
	}

	const_iterator insert(const_iterator pnt, T val)
	{
		if (pnt == end()) {
			push_back(val);
			return end() - 1;
		}
		if (!empty()) {
			size_t ind = pnt - _data;
			push_back(back());
			auto pos = _data + ind;
			for (auto it = end() - 2; it > pos; --it) {
				*it = *(it - 1);
			}
			(*pos) = val;
			return pos;
		}
		push_back(val);
		return _data;
	}

	const_iterator erase(const_iterator pos)
	{
		return erase(pos, pos + 1);
	}

	const_iterator erase(const_iterator start, const_iterator finish)
	{
		size_t delta = finish - start;
		if (delta <= 0) return start;
		for (auto it = const_cast<iterator>(start); it < end() - delta; ++it) {
			*it = *(it + delta);
		}
		destroy(end() - delta, end());
		_size -= delta;
		return start;
	}
private:
	T *_data;
	size_t _size;
	size_t _capacity;

	void ensure_capacity()
	{
		if (_size < _capacity) return;
		if (_capacity == 0) {
			reserve(4);
		}
		else {
			reserve(_capacity * 2);
		}
	}

	void destroy(const_iterator pos)
	{
		destroy(pos, pos + 1);
	}

	void destroy(const_iterator first, const_iterator last)
	{
		for (auto it = first; it != last; ++it) {
			(*it).~T();
		}
	}
};

template<typename T>
void swap(vector<T> &a, vector<T> &b)
{
	a.swap(b);
}
