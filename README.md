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

## Author ##

* [Dmitry Marakasov](https://github.com/AMDmi3) <amdmi3@amdmi3.ru>

## License ##

* 2 clause BSD. See [COPYING](COPYING) file.
