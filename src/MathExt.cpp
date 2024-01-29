//By Alexej Fedorenko, 37676
#include <cstddef>
#include <exception>
#include <map>
#include <stdexcept>

float Factorial(size_t n)
{
    if (n <= 1)
        return 1;

    static std::map<size_t, float> cache;
    return (cache.count(n) == 1) ? cache.at(n) : cache.emplace(n, (float)n * Factorial(n - 1)).first->second;
}

float Combination(size_t n, size_t k)
{
    if (k > n)
        throw std::runtime_error("Combination error, k > n");

    return Factorial(n) / (Factorial(k) * Factorial(n - k)); 
}

