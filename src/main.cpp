// Démo : compte les nombres premiers inférieurs à N, en séquentiel puis avec le pool.
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "thread_pool.hpp"

static bool isPrime(long n) {
    if (n < 2) return false;
    for (long d = 2; d * d <= n; ++d)
        if (n % d == 0) return false;
    return true;
}

static long countPrimes(long from, long to) {
    long count = 0;
    for (long n = from; n < to; ++n) count += isPrime(n);
    return count;
}

template <typename F>
static double timeMs(F f) {
    auto start = std::chrono::steady_clock::now();
    f();
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
}

int main(int argc, char** argv) {
    const long limit = argc > 1 ? std::atol(argv[1]) : 5'000'000;
    const long chunk = 50'000;
    long sequential = 0, parallel = 0;

    double seqMs = timeMs([&] { sequential = countPrimes(0, limit); });

    ThreadPool pool;
    double parMs = timeMs([&] {
        std::vector<std::future<long>> parts;
        for (long from = 0; from < limit; from += chunk)
            parts.push_back(pool.submit(countPrimes, from, std::min(from + chunk, limit)));
        for (auto& p : parts) parallel += p.get();
    });

    std::cout << "Nombres premiers < " << limit << " : " << sequential << "\n"
              << "Séquentiel : " << seqMs << " ms\n"
              << "Pool de " << pool.size() << " threads : " << parMs << " ms (x" << seqMs / parMs << ")\n";
    return sequential == parallel ? 0 : 1;
}
