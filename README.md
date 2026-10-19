# Thread Pool en C++17

Un pool de threads simple et complet, en un seul fichier d'en-tête, avec une démo de calcul parallèle de nombres premiers.

## Pourquoi un pool de threads ?

Créer un thread coûte cher. Un pool crée N threads une seule fois, puis leur distribue des tâches depuis une file d'attente partagée.

## Fonctionnalités

- `submit(f, args...)` accepte n'importe quelle fonction ou lambda et retourne un `std::future` typé
- Les exceptions levées dans une tâche sont transmises à l'appelant via `future.get()`
- Arrêt propre : le destructeur termine toutes les tâches en attente puis rejoint les threads
- Synchronisation avec `std::mutex` + `std::condition_variable` (aucune attente active)

## Exemple

```cpp
#include "thread_pool.hpp"

ThreadPool pool(4);
auto f = pool.submit([](int a, int b) { return a * b; }, 6, 7);
std::cout << f.get();  // 42
```

## Démo

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
./build/primes_demo 10000000
```

Le programme compte les nombres premiers en séquentiel puis avec le pool et affiche l'accélération obtenue.

## Tests

```bash
ctest --test-dir build --output-on-failure
```

## Notions abordées

`std::thread`, `std::mutex`, `std::condition_variable`, `std::packaged_task`, `std::future`, templates variadiques, perfect forwarding, RAII.
