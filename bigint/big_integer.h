#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

typedef unsigned __int128 uint128_t;

struct big_integer
{
	using digit_t = uint32_t;
	static const digit_t MIN_DIGIT = static_cast<uint64_t>(0);
	static const digit_t MAX_DIGIT = UINT32_MAX;
	static const uint64_t BASE = static_cast<uint64_t>(MAX_DIGIT) + 1;
	static const uint128_t BASE128 = static_cast<uint128_t>(BASE);

	big_integer();
	big_integer(big_integer const &other);
	big_integer(int a);
	big_integer(digit_t a);
	big_integer(uint64_t a);
	big_integer(uint128_t a);
	explicit big_integer(std::string const &str);
	~big_integer();

	big_integer &operator=(big_integer const &rhs);
	big_integer &operator+=(big_integer const &rhs);
	big_integer &operator-=(big_integer const &rhs);
	big_integer naive_mul(big_integer const &rhs);
	big_integer &operator*=(big_integer const &rhs);
	big_integer div_by_short(digit_t d);
	big_integer div128(big_integer const &rhs);
	big_integer limb_div(big_integer const &rhs);
	big_integer &operator/=(big_integer const &rhs);

	big_integer &operator%=(big_integer const &rhs);
	big_integer &operator&=(big_integer const &rhs);
	big_integer &operator|=(big_integer const &rhs);
	big_integer &operator^=(big_integer const &rhs);
	big_integer &operator<<=(int rhs);
	big_integer &operator>>=(int rhs);

	big_integer operator+() const;
	big_integer operator-() const;
	big_integer operator~() const;
	big_integer &operator++();
	big_integer operator++(int);
	big_integer &operator--();
	big_integer operator--(int);

	size_t size() const;
	int compare_to(big_integer const &other) const;

	friend std::string to_string(big_integer const &bi);

private:
	std::vector<digit_t> value;
	bool is_inf_1;  // a.is_inf_1 == true <=> a < 0;

	digit_t get(size_t i) const;
	digit_t get_inf_digit() const;
	void shrink_to_fit();
	void block_shl(size_t cnt);
	void block_shr(size_t cnt);
};

big_integer operator+(big_integer a, big_integer const &b);
big_integer operator-(big_integer a, big_integer const &b);
big_integer operator*(big_integer a, big_integer const &b);
big_integer operator&(big_integer a, big_integer const &b);
big_integer operator|(big_integer a, big_integer const &b);
big_integer operator^(big_integer a, big_integer const &b);
big_integer operator/(big_integer a, big_integer const &b);
big_integer operator%(big_integer a, big_integer const &b);
big_integer operator<<(big_integer, int);
big_integer operator>>(big_integer, int);

bool operator==(big_integer const &a, big_integer const &b);
bool operator!=(big_integer const &a, big_integer const &b);
bool operator<(big_integer const &a, big_integer const &b);
bool operator>(big_integer const &a, big_integer const &b);
bool operator<=(big_integer const &a, big_integer const &b);
bool operator>=(big_integer const &a, big_integer const &b);

std::string to_string(big_integer const &a);
std::ostream &operator<<(std::ostream &s, big_integer const &a);

#endif // BIG_INTEGER_H
