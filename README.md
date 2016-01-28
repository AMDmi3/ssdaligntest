# SSD align test

Simple program to test read/write speed of your SSD partition to
check if it's aligned correctly.

## Methodology

Since SSDs have write and erase block sizes correspondingly) much
larger than a sector size (around 4K and 256-512K correspondingly),
incorrectly aligned partitions will result in so-called [write
amplification](https://en.wikipedia.org/wiki/Write_amplification),
which means degraded performance and increased wear rate.

It is therefore important to align partition boundaries correctly,
and this utility helps to confirm that, or determine correct
alignment.

The utility performs a set of tests. Each tests reads and/or writes
the same number of blocks of specified size at specified intervals,
and each other test incorporates additional offset (usually sector
size). Throughput of each test is then calculated.

Currently, the program only prints resulting throughput rates to
be evaluated by the user.

## Usage
```
./ssdaligntest [-rw] [-b block-size] [-g gap-size] [-s offset-step] [-c count] file
```

* ```-r``` - read blocks during the test
* ```-w``` - write blocks during the test

At least one of above options is required. If only ```-w``` is
specified, zeroes are written to the target file (which is
destructive!), if both ```-r``` and ```-w``` are specified, the
same blocks which are read are written back.

* ```-b``` - read/write block size in bytes
* ```-i``` - interval size in bytes
* ```-s``` - offset step for each test in bytes
* ```-c``` - number of blocks to read/write
* ```-k``` - number of blocks to skip (useful to counter caching)
* ```file``` - usually device node for your disk or partition

## Example

Here's an example of runnig read test on some SSD partition. You
can see offsets which are multiple of 4096 bytes giving better read
performance, and since the partition begins with a "fast" offset
it's correctly aligned.

```
./ssdaligntest -r -b 4096 -i 8192 -s 512 -c 10000 /dev/ada0p1
test 16/16 (offset 7680)...
OFFSET     DURATION      THROUGHPUT
     0       1.51 s      25.93 MB/s <--
   512       1.95 s      20.04 MB/s
  1024       1.95 s      20.04 MB/s
  1536       1.95 s      19.99 MB/s
  2048       1.95 s      20.03 MB/s
  2560       1.95 s      20.02 MB/s
  3072       1.95 s      20.01 MB/s
  3584       1.96 s      19.95 MB/s
  4096       1.50 s      25.99 MB/s <--
  4608       1.73 s      22.54 MB/s
  5120       1.73 s      22.55 MB/s
  5632       1.73 s      22.54 MB/s
  6144       1.73 s      22.54 MB/s
  6656       1.74 s      22.42 MB/s
  7168       1.74 s      22.39 MB/s
  7680       1.76 s      22.21 MB/s
The partition looks to be aligned
```

## Author ##

* [Dmitry Marakasov](https://github.com/AMDmi3) <amdmi3@amdmi3.ru>

## License ##

* 2 clause BSD. See [COPYING](COPYING) file.
