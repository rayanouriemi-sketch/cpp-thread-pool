#undef NDEBUG  // les assert doivent rester actifs même en Release
#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "../src/thread_pool.hpp"

using namespace std::chrono_literals;

int main() {
    // résultats et arguments
    {
        ThreadPool pool(4);
        auto sum = pool.submit([](int a, int b) { return a + b; }, 2, 3);
        auto text = pool.submit([] { return std::string("ok"); });
        assert(sum.get() == 5 && text.get() == "ok");
    }

    // les exceptions sont transmises par le future
    {
        ThreadPool pool(2);
        auto f = pool.submit([]() -> int { throw std::runtime_error("boom"); });
        bool caught = false;
        try { f.get(); } catch (const std::runtime_error& e) { caught = std::string(e.what()) == "boom"; }
        assert(caught);
        assert(pool.submit([] { return 1; }).get() == 1);  // le pool fonctionne toujours
    }

    // beaucoup de petites tâches + le destructeur attend la fin de toutes
    std::atomic<int> counter{0};
    {
        ThreadPool pool(8);
        for (int i = 0; i < 10000; ++i) pool.submit([&counter] { counter.fetch_add(1); });
    }
    assert(counter == 10000);

    // les tâches tournent vraiment en parallèle : 4 tâches s'attendent mutuellement
    {
        ThreadPool pool(4);
        std::atomic<int> arrived{0};
        std::vector<std::future<bool>> fs;
        for (int i = 0; i < 4; ++i)
            fs.push_back(pool.submit([&arrived] {
                arrived.fetch_add(1);
                auto deadline = std::chrono::steady_clock::now() + 5s;
                while (arrived < 4)
                    if (std::chrono::steady_clock::now() > deadline) return false;
                    else std::this_thread::yield();
                return true;
            }));
        for (auto& f : fs) assert(f.get());
    }

    assert(ThreadPool(0).size() == 1);

    std::cout << "Tous les tests passent.\n";
    return 0;
}
