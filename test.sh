# uniform distribution 
build/benchmark --mode=1 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10

build/benchmark --mode=2 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10

build/benchmark --mode=3 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10

build/benchmark --mode=4 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=1000 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=10000 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=100000 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=1000000 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10

# normal distribution 
build/benchmark --mode=1 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10

build/benchmark --mode=2 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10

build/benchmark --mode=3 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10

build/benchmark --mode=4 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=1000 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=10000 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=100000 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=1000000 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10

# skew distribution 
build/benchmark --mode=1 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10

build/benchmark --mode=2 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10

build/benchmark --mode=3 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10

build/benchmark --mode=4 \
    --min_dop=1 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=1000 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=10000 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=100000 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10

build/benchmark --mode=5 \
    --min_dop=1 --max_dop=16 \
    --block_size=1000000 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10

# uniform stream
## k-merge
build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=4096

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=8192

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=16384

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=32768

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=65536

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=131072

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=262144

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=524288

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=1048576

## parallel merge
build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=4096

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=8192

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=16384

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=32768

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=65536

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=131072

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=262144

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=524288

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=1048576

## merge path merge
build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=4096

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=8192

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=16384

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=32768

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=65536

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=131072

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=262144

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=524288

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=1048576

# normal stream
## k-merge
build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=4096

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=8192

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=16384

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=32768

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=65536

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=131072

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=262144

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=524288

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=1048576

## parallel merge
build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=4096

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=8192

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=16384

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=32768

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=65536

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=131072

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=262144

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=524288

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=1048576

## merge path merge
build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=4096

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=8192

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=16384

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=32768

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=65536

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=131072

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=262144

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=524288

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=2 \
    --iteration=10 \
    --chunk_size=1048576

# skew stream
## k-merge
build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=4096

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=8192

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=16384

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=32768

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=65536

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=131072

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=262144

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=524288

build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=1048576

## parallel merge
build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=4096

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=8192

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=16384

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=32768

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=65536

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=131072

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=262144

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=524288

build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=1048576

## merge path merge
build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=4096

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=8192

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=16384

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=32768

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=65536

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=131072

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=262144

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=524288

build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=3 \
    --iteration=10 \
    --chunk_size=1048576

# multi stream with data skewed
build/benchmark --mode=6 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=65536 \
    --merge_skew=true \
    --merge_skew_percent=80


build/benchmark --mode=7 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=65536 \
    --merge_skew=true \
    --merge_skew_percent=80


build/benchmark --mode=8 \
    --min_dop=2 --max_dop=16 \
    --data_size=100000000 --data_distribution=1 \
    --iteration=10 \
    --chunk_size=65536 \
    --merge_skew=true \
    --merge_skew_percent=80


