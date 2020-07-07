#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
#include <algorithm>

template<typename T>
struct vector
{
public:
	using iterator = T *;
	using const_iterator = T const *;

	vector()
		: data_(nullptr), size_(0), capacity_(0)
	{}

	vector(vector const &other)
		: vector()
	{
		if (!other.empty()) {
			vector temp;
			temp.reserve(other.size());
			for (T const &el : other) {
				temp.push_back(el);
			}
			swap(temp);
		}
	}

	vector &operator=(vector const &rhs)
	{
		if (this == &rhs) return *this;
		vector temp(rhs);
		swap(temp);
		return *this;
	}

	~vector()
	{
		clear();
		operator delete(data_);
	}

	T &operator[](size_t i)
	{
		return data_[i];
	}

	T const &operator[](size_t i) const
	{
		return data_[i];
	}

	T *data()
	{
		return data_;
	}

	T const *data() const
	{
		return data_;
	}

	size_t size() const
	{
		return size_;
	}

	T &front()
	{
		return data_[0];
	}

	T const &front() const
	{
		return data_[0];
	}

	T &back()
	{
		return data_[size_ - 1];
	}

	T const &back() const
	{
		return data_[size_ - 1];
	}

	void push_back(T const &val)
	{
		if (begin() <= &val && &val < end()) {
			ptrdiff_t ind = &val - begin();
			ensure_capacity();
			new(data_ + size_) T(data_[ind]);
		}
		else {
			ensure_capacity();
			new(data_ + size_) T(val);
		}
		size_++;
	}

	void pop_back()
	{
		destroy(end() - 1);
		size_--;
	}

	bool empty() const
	{
		return size_ == 0;
	}

	size_t capacity() const
	{
		return capacity_;
	}

	void reserve(size_t new_cap)
	{
		if (new_cap > capacity_) {
			T *temp_data = static_cast<T *>(operator new(new_cap * sizeof(T)));
			size_t temp_size = 0;
			try {
				for (; temp_size < size_; ++temp_size) {
					new(temp_data + temp_size) T(data_[temp_size]);
				}
			}
			catch (...) {
				destroy(temp_data, temp_data + temp_size);
				operator delete(temp_data);
				throw;
			}

			destroy(begin(), end());
			operator delete(data_);
			data_ = temp_data;
			capacity_ = new_cap;
		}
	}

	void shrink_to_fit()
	{
		if (capacity_ > size_) {
			vector temp(*this);
			swap(temp);
		}
	}

	void clear()
	{
		destroy(begin(), end());
		size_ = 0;
	}

	void swap(vector &other)
	{
		std::swap(data_, other.data_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}

	iterator begin()
	{
		return data_;
	}

	iterator end()
	{
		return data_ + size_;
	}

	const_iterator begin() const
	{
		return data_;
	}

	const_iterator end() const
	{
		return data_ + size_;
	}

	const_iterator insert(const_iterator ptr, T const &val)
	{
		ptrdiff_t ind = ptr - begin();
		push_back(val);
		ptr = begin() + ind;
		for (iterator it = end() - 1; it > ptr; --it) {
			std::swap(*it, *(it - 1));
		}
		return ptr;
	}

	const_iterator erase(const_iterator pos)
	{
		return erase(pos, pos + 1);
	}

	const_iterator erase(const_iterator first, const_iterator last)
	{
		ptrdiff_t delta = last - first;
		if (delta <= 0) return last;
		for (auto it = first - data_ + data_; it < end() - delta; ++it) {  // цинично, но почему нет
			std::swap(*it, *(it + delta));
		}
		destroy(end() - delta, end());
		size_ -= delta;

		return first;
	}
private:
	T *data_;
	size_t size_;
	size_t capacity_;

	void ensure_capacity()
	{
		if (size_ < capacity_) return;
		if (capacity_ == 0) {
			reserve(4);
		}
		else {
			reserve(capacity_ * 2);
		}
	}

	void destroy(const_iterator ptr)
	{
		ptr->~T();
	}

	void destroy(const_iterator first, const_iterator last)
	{
		for (auto it = last; it != first; --it) {
			(it - 1)->~T();
		}
	}
};

template<typename T>
void swap(vector<T> &a, vector<T> &b)
{
	a.swap(b);
}

