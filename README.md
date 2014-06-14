
testmem
=======

Memory Benchmark for *nix OS

Usage
--

```sh
lovewilliam@mac ~/testmem $ ./testmem
./testmem.osx [auto]|[mem_size in 4 byte] [s]
```


Output
--

```sh
lovewilliam@mac ~/testmem $ ./testmem.osx  1024
testing mem size: 4096 Byte
w = 976.62500 MB/s r = 976.562500 MB/s
Caught SIGNAL
--------------------------------------
Peak    W       R
        976.562500MB/s  976.562500MB/s
Average W       R
        976.562281MB/s  976.562500MB/s
Test complete in 271423 cycles
with write burst inf 213281 cycles, 78.578823 Percent
with read  burst inf 68484 cycles, 25.231465 Percent
--------------------------------------
lovewilliam@mac ~/testmem $
```

License
----

GPLv2


