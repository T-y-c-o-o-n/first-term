#include "big_integer.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#define to32(a) static_cast<uint32_t>(a)
#define to_digit(a) to32(a)
#define to64(a) static_cast<uint64_t>(a)
#define to128(a) static_cast<uint128_t>(a)

big_integer::big_integer()
	: big_integer(0)
{}

big_integer::big_integer(big_integer const &other) = default;

big_integer::big_integer(int a)
	: value(1, std::abs(static_cast<int64_t>(a))), inf_1_after_last_digit(false)
{
	if (a < 0) {
		*this = -(*this);
	}
}

big_integer::big_integer(uint32_t a)
	: value(1, a), inf_1_after_last_digit(false)
{}

big_integer::big_integer(uint64_t a)
	: value(1, to32(a % BASE)), inf_1_after_last_digit(false)
{
	value.push_back(to32(a / BASE));
	shrink_to_fit();
}

big_integer::big_integer(uint128_t a)
	: inf_1_after_last_digit(false)
{
	do {
		value.push_back(a % BASE128);
		a /= BASE128;
	}
	while (a > 0);
	shrink_to_fit();
}

big_integer::big_integer(std::string const &str)
	: big_integer()
{
	if (str.empty()) {
		return;
	}
	size_t i = 0;
	if (str[i] == '-') i++;
	for (; i < str.length(); ++i) {
		if (str[i] < '0' || str[i] > '9') throw;
		*this *= 10;
		*this += to_digit(str[i] - '0');
	}
	if (str[0] == '-') {
		*this = -(*this);
	}
}

big_integer::~big_integer() = default;

uint32_t big_integer::get(size_t i) const
{
	if (i < value.size()) {
		return value[i];
	}
	return get_inf_digit();
}

uint32_t big_integer::get_inf_digit() const
{
	return inf_1_after_last_digit ? MAX_DIGIT : MIN_DIGIT;
}

// удаляет лишние старшие цифры, которые состоят из одних нулей или единиц и совпадают с бесконечными условными лимбами
void big_integer::shrink_to_fit()
{
	while (value.size() > 1) {
		if ((value.back() == MIN_DIGIT && !inf_1_after_last_digit)
			|| (value.back() == MAX_DIGIT && inf_1_after_last_digit)) {
			value.pop_back();
		}
		else {
			return;
		}
	}
}

big_integer &big_integer::operator=(big_integer const &rhs)
{
	if (this == &rhs) {
		return *this;
	}
	value = rhs.value;
	inf_1_after_last_digit = rhs.inf_1_after_last_digit;
	return *this;
}

big_integer &big_integer::operator+=(big_integer const &b)
{
	size_t size_ = std::max(size(), b.size());
	value.resize(size_, get_inf_digit());
	uint64_t sum = 0, carry = 0;
	for (size_t i = 0; i < size_; ++i) {
		sum = to64(get(i)) + to64(b.get(i)) + carry;
		value[i] = to_digit(sum % BASE);
		carry = sum / BASE;
	}
	// теперь решим вопрос с бесконечными единицами, нулями и carry
	bool one_is_1 = (inf_1_after_last_digit && !b.inf_1_after_last_digit)
		|| (!inf_1_after_last_digit && b.inf_1_after_last_digit);
	if ((!inf_1_after_last_digit && !b.inf_1_after_last_digit && carry == 0) || (one_is_1 && carry > 0)) {
		inf_1_after_last_digit = false;
	}
	else if ((one_is_1 && carry == 0) || (inf_1_after_last_digit && b.inf_1_after_last_digit && carry > 0)) {
		inf_1_after_last_digit = true;
	}
	else if (inf_1_after_last_digit && b.inf_1_after_last_digit
		&& carry == 0) {  // a.inf_1_after_last_digit and b.inf_1_after_last_digit
		value.resize(size_ + 1);
		value[size_] = MAX_DIGIT - 1;
		inf_1_after_last_digit = true;
	}
	else {  // !a.inf_1_after_last_digit && !b.inf_1_after_last_digit && carry > 0
		value.resize(size_ + 1);
		value[size_] = MIN_DIGIT + 1;
		inf_1_after_last_digit = false;
	}
	shrink_to_fit();
	return *this;

}

big_integer &big_integer::operator-=(big_integer const &rhs)
{
	return *this += -rhs;
}

big_integer big_integer::naive_mul(big_integer const &b)
{
	big_integer res;
	res.value.resize(size() + b.size(), 0u);
	for (size_t i = 0; i < size(); ++i) {
		uint64_t sum = 0, carry = 0;
		for (size_t j = 0; j < b.size() || carry > 0; ++j) {
			sum = res.get(i + j) + carry +
				to64(get(i)) * to64(b.get(j));
			res.value[i + j] = to32(sum % BASE);
			carry = sum / BASE;
		}
	}
	res.shrink_to_fit();
	return *this = res;
}

big_integer &big_integer::operator*=(big_integer const &rhs)
{
	bool sign = (inf_1_after_last_digit && !rhs.inf_1_after_last_digit)
		|| (!inf_1_after_last_digit && rhs.inf_1_after_last_digit);
	if (inf_1_after_last_digit) *this = -(*this);
	naive_mul(rhs.inf_1_after_last_digit ? -rhs : rhs);
	if (sign) {
		*this = -(*this);
	}
	shrink_to_fit();
	return *this;
}

big_integer big_integer::div_by_short(digit_t val)
{
	if (val == 0) {
		throw std::runtime_error("division by zero");
	}
	if (*this == 0) {
		return 0;
	}
	uint32_t carry = 0;
	for (size_t i = size(); i > 0; --i) {
		uint64_t tmp = (to64(carry) << 32u) + value[i - 1];
		value[i - 1] = tmp / val;
		carry = tmp % val;
	}
	shrink_to_fit();
	return *this;
}

big_integer big_integer::div128(big_integer const &rhs)
{
	uint128_t a, b;
	a = to128(get(0)) + (to128(get(1)) << 32ull)
		+ (to128(get(2)) << 64ull) + (to128(get(3)) << 96ull);
	b = to128(rhs.get(0)) + (to128(rhs.get(1)) << 32ull)
		+ (to128(rhs.get(2)) << 64ull) + (to128(rhs.get(3)) << 96ull);
	return *this = a / b;
}

using digit_t = uint32_t;
static digit_t div_3_by_2(digit_t a, digit_t b, digit_t c, digit_t x, digit_t y)
{
	uint128_t l, r;
	l = to128(c) + (to128(b) << 32ull) + (to128(a) << 64ull);
	r = to128(y) + (to128(x) << 32ull);
	return l / r;
}

static bool normalised(digit_t a)
{
	return (a & (1u << 31u)) > 0;
}

big_integer big_integer::limb_div(big_integer const &rhs)
{
	if (rhs == 0) {
		throw std::runtime_error("division by zero");
	}
	if (*this < rhs) {
		return *this = 0;
	}
	if (*this == rhs) {
		return *this = 1;
	}
	if (rhs.size() == 1) {
		return div_by_short(rhs.value[0]);
	}
	if (size() <= 4) {
		return div128(rhs);
	}

	big_integer u = *this;
	big_integer d = rhs;
	int shift = 0;
	while (!normalised(d.value.back())) {
		d <<= 1;
		shift++;
	}
	if (shift > 0) {
		return (*this <<= shift) /= d;
	}

	ssize_t m = u.size(), n = d.size();
	ssize_t k = m - n;
	big_integer q, dbk = d << (32 * static_cast<int>(k));
	q.value.resize(k + 1, 0);
	if (u >= dbk) {
		q.value[k] = 1;
		u -= dbk;
	}
	else {
		q.value[k] = 0;
	}
	while (k > 0) {
		dbk >>= 32;
		k--;
		if (u.get(n + k) == d.get(n - 1) && u.get(n + k - 1) == d.get(n - 2)) {
			q.value[k] = MAX_DIGIT;
		}
		else {
			q.value[k] = div_3_by_2(u.get(n + k), u.get(n + k - 1), u.get(n + k - 2),
									d.get(n - 1), d.get(n - 2));
		}
		u -= q.value[k] * dbk;
		if (u < 0) {
			u += d;
			q.value[k]--;
		}
	}
	q.shrink_to_fit();
	return *this = q;
}

big_integer &big_integer::operator/=(big_integer const &rhs)
{
	bool sign = (inf_1_after_last_digit && !rhs.inf_1_after_last_digit)
		|| (!inf_1_after_last_digit && rhs.inf_1_after_last_digit);
	if (inf_1_after_last_digit) *this = -(*this);

	limb_div(rhs.inf_1_after_last_digit ? -rhs : rhs);
	if (sign) {
		*this = -(*this);
	}

	shrink_to_fit();
	return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs)
{
	return *this = *this - ((*this / rhs) * rhs);
}

big_integer &big_integer::operator&=(big_integer const &rhs)
{
	size_t size_ = std::max(size(), rhs.size());
	value.resize(size_, get_inf_digit());
	for (size_t i = 0; i < size_; ++i) {
		value[i] = get(i) & rhs.get(i);
	}
	inf_1_after_last_digit = inf_1_after_last_digit && rhs.inf_1_after_last_digit;
	shrink_to_fit();
	return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs)
{
	size_t size_ = std::max(size(), rhs.size());
	value.resize(size_, get_inf_digit());
	for (size_t i = 0; i < size_; ++i) {
		value[i] = get(i) | rhs.get(i);
	}
	inf_1_after_last_digit = inf_1_after_last_digit || rhs.inf_1_after_last_digit;
	shrink_to_fit();
	return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs)
{
	size_t size_ = std::max(size(), rhs.size());
	value.resize(size_, get_inf_digit());
	for (size_t i = 0; i < size_; ++i) {
		value[i] = get(i) ^ rhs.get(i);
	}
	inf_1_after_last_digit = (!inf_1_after_last_digit && rhs.inf_1_after_last_digit)
		|| (inf_1_after_last_digit && !rhs.inf_1_after_last_digit);
	shrink_to_fit();
	return *this;
}

big_integer &big_integer::operator<<=(int rhs)
{
	if (rhs < 0) return *this >>= -rhs;
	block_shl(static_cast<size_t>(rhs) / 32);
	uint32_t c = rhs % 32;
	if (c == 0) {
		return *this;
	}
	uint32_t d = (32 - c) % 32;
	value.resize(size() + 1, get_inf_digit());
	for (size_t i = size() - 1; i > 0; --i) {
		value[i] <<= c;
		value[i] |= (value[i - 1] >> d);
	}
	value[0] <<= c;
	shrink_to_fit();
	return *this;
}

big_integer big_integer::operator+() const
{
	return *this;
}

big_integer big_integer::operator-() const
{
	return ++(~(*this));
}

big_integer &big_integer::operator>>=(int rhs)
{
	if (rhs < 0) return *this <<= -rhs;
	block_shr(static_cast<size_t>(rhs) / 32);
	uint32_t c = rhs % 32;
	if (c == 0) return *this;
	uint32_t d = (32 - c) % 32;  // ?
	value.resize(size() + 1, get_inf_digit());
	for (size_t i = 0; i < size() - 1; ++i) {
		value[i] >>= c;
		value[i] |= (value[i + 1] << d);
	}
//	res.value[res.size() - 1] >>= c;  не требуется, так как этот блок равен всем слудующим (все нули или все единицы)
	shrink_to_fit();
	return *this;
}

big_integer big_integer::operator~() const
{
	big_integer res(*this);
	for (digit_t &digit : res.value) {
		digit = ~digit;
	}
	res.inf_1_after_last_digit = !res.inf_1_after_last_digit;
	res.shrink_to_fit();
	return res;
}

big_integer &big_integer::operator++()
{
	return *this += 1;
}

big_integer big_integer::operator++(int)
{
	big_integer r = *this;
	++(*this);
	return r;
}

big_integer &big_integer::operator--()
{
	return *this -= 1;
}

big_integer big_integer::operator--(int)
{
	big_integer r = *this;
	--(*this);
	return r;
}

big_integer operator+(big_integer a, big_integer const &b)
{
	return a += b;
}

big_integer operator-(big_integer a, big_integer const &b)
{
	return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b)
{
	return a *= b;
}

big_integer operator/(big_integer lhs, big_integer const &rhs)
{
	return lhs /= rhs;
}

big_integer operator%(big_integer a, big_integer const &b)
{
	return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b)
{
	return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b)
{
	return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b)
{
	return a ^= b;
}

void big_integer::block_shl(size_t cnt)
{
	std::vector<digit_t> new_value(cnt, 0);
	for (digit_t &digit : value) {
		new_value.push_back(digit);
	}
	swap(value, new_value);
}

void big_integer::block_shr(size_t cnt)
{
	if (cnt < value.size()) {
		value.erase(value.begin(), value.begin() + cnt);
	}
	else {
		std::vector<digit_t> new_value(1, get(size()));
		swap(value, new_value);
	}
}

big_integer operator<<(big_integer a, int b)
{
	return a <<= b;
}

big_integer operator>>(big_integer a, int b)
{
	return a >>= b;
}

size_t big_integer::size() const
{
	return value.size();
}

int big_integer::compare_to(big_integer const &other) const
{
	if (inf_1_after_last_digit != other.inf_1_after_last_digit) {
		return inf_1_after_last_digit ? -1 : 1;
	}
	int res = 0;
	if (size() != other.size()) {
		res = size() < other.size() ? -1 : 1;
		return inf_1_after_last_digit ? -res : res;
	}
	else {
		for (int64_t i = size() - 1; i >= 0; i--) {
			if (value[i] != other.value[i]) {
				res = value[i] < other.value[i] ? -1 : 1;
				break;
			}
		}
	}
	return res;
}

bool operator==(big_integer const &a, big_integer const &b)
{
	return a.compare_to(b) == 0;
}

bool operator!=(big_integer const &a, big_integer const &b)
{
	return a.compare_to(b) != 0;
}

bool operator<(big_integer const &a, big_integer const &b)
{
	return a.compare_to(b) == -1;
}

bool operator>(big_integer const &a, big_integer const &b)
{
	return a.compare_to(b) == 1;
}

bool operator<=(big_integer const &a, big_integer const &b)
{
	return a.compare_to(b) <= 0;
}

bool operator>=(big_integer const &a, big_integer const &b)
{
	return a.compare_to(b) >= 0;
}

std::string to_string(big_integer const &a)
{
	std::string res;
	big_integer tmp(a < 0 ? -a : a);
	while (tmp >= 0) {
		res += std::to_string((tmp % 10).value[0]);
		tmp /= 10;
		if (tmp == 0) {
			break;
		}
	}
	if (a < 0) {
		res += '-';
	}
	std::reverse(res.begin(), res.end());
	return res;
}

std::ostream &operator<<(std::ostream &out, big_integer const &a)
{
	return out << to_string(a);
}
