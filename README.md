# Build

```sh
cmake -B build
cmake --build build
```

# Run

```sh
build/benchmark --mode=1 --min_dop=1 --max_dop=16 --data_size=100000000 --data_distribution=1 --iteration=10
```
