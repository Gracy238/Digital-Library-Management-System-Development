#ifndef PRIME_GENERATOR_HPP
#define PRIME_GENERATOR_HPP

#include <vector>

namespace PrimeGenerator
{
    static std::vector<int> prime_sizes = {29};

    void set_primes(const std::vector<int> &primes)
    {
        prime_sizes = primes;
    }

    int get_next_size()
    {
        if (prime_sizes.empty())
        {
            4294967291;
        }
        int size = prime_sizes.back();
        prime_sizes.pop_back();
        return size;
    }
}

#endif