/*
 * Copyright (c) 2016 Dmitry Marakasov <amdmi3@amdmi3.ru>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

static struct option longopts[] = {
	{ "read",          no_argument,       NULL, 'r' },
	{ "write",         no_argument,       NULL, 'w' },
	{ "block-size",    required_argument, NULL, 'b' },
	{ "interval-size", required_argument, NULL, 'i' },
	{ "offset-step",   required_argument, NULL, 's' },
	{ "count",         required_argument, NULL, 'c' },
	{ "skip-count",    required_argument, NULL, 'k' },
	{ "help",          no_argument,       NULL, 'h' },
	{ NULL,            0,                 NULL, 0 }
};

void usage(const char* progname) {
	fprintf(stderr, "Usage: %s [-rw] [-b block-size] [-i interval-size] [-s offset-step] [-c count] [-k skip-count] file\n", progname);
}

int main(int argc, char **argv) {
	int ch;
	const char* progname = argv[0];

	/* options */
	int do_read = 0, do_write = 0;
	unsigned long long block_size = 512;
	unsigned long long interval_size = 0;
	unsigned long long offset_step = 512;
	unsigned long long skip_count = 0;
	unsigned long long count = 0;

	/* process arguments */
	while ((ch = getopt_long(argc, argv, "rwhb:i:s:c:", longopts, NULL)) != -1) {
		switch (ch) {
		case 'r':
			do_read = 1;
			break;
		case 'w':
			do_write = 1;
			break;
		case 'b':
			block_size = strtoull(optarg, NULL, 10);
			break;
		case 'i':
			interval_size = strtoull(optarg, NULL, 10);
			break;
		case 's':
			offset_step = strtoull(optarg, NULL, 10);
			break;
		case 'c':
			count = strtoull(optarg, NULL, 10);
			break;
		case 'k':
			skip_count = strtoull(optarg, NULL, 10);
			break;
		case 'h':
			usage(progname);
			return 0;
		default:
			usage(progname);
			return 1;
		}
	}
	argc -= optind;
	argv += optind;

	if (interval_size < block_size)
		interval_size = block_size;

	if (argc != 1 || (do_read == 0 && do_write == 0) || offset_step == 0) {
		usage(progname);
		return 1;
	}

	/* prepare */
	char* buffer = malloc(block_size);
	if (buffer == NULL) {
		perror("Cannot allocate buffer");
		return 1;
	}

	memset(buffer, 0, block_size);

	/* open */
	int f;
	if (do_write && do_read) {
		f = open(argv[0], O_RDWR, 0666);
	} else if (do_write) {
		f = open(argv[0], O_WRONLY, 0666);
	} else {
		f = open(argv[0], O_RDONLY);
	}

	if (f == -1) {
		perror("Cannot open file");
		free(buffer);
		return 1;
	}

	/* run test */
	for (off_t base_offset = 0; base_offset < interval_size; base_offset += offset_step) {
		fprintf(stderr, "offset %5lu", (unsigned long)base_offset);

		lseek(f, base_offset, SEEK_SET);

		struct timeval start, end;
		gettimeofday(&start, NULL);

		for (off_t i = skip_count; i < skip_count + count; i++) {
			off_t offset = base_offset + interval_size * i;
			if (do_read) {
				if (lseek(f, offset, SEEK_SET) != offset) {
					perror("Seek error");
					goto error;
				}

				if (read(f, buffer, block_size) != block_size) {
					perror("Cannot read block");
					goto error;
				}
			}

			if (do_write) {
				if (lseek(f, offset, SEEK_SET) != offset) {
					perror("Seek error");
					goto error;
				}

				if (write(f, buffer, block_size) != block_size) {
					perror("Cannot write block");
					goto error;
				}
			}
		}

		gettimeofday(&end, NULL);

		float duration = (float)(end.tv_sec - start.tv_sec) + (float)(end.tv_usec - start.tv_usec) / 1000000.0f;
		float throughput = (float)(block_size * count) / duration / 1024.0f / 1024.0f;

		fprintf(stderr, ", duration %f (%f MB/s)\n", duration, throughput);
	}

	/* finalize */
	free(buffer);
	close(f);
	return 0;

error:
	free(buffer);
	close(f);
	return 1;
}
