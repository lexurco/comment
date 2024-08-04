/*
 * Copyright (c) 2024 Alexander Arkhipov <aa@manpager.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* default values */
#define BEGIN	"/*"
#define END	"*/"
#define SINGLE	"//"

/* strings initialising/ending comments and ... */
static char	*begin = BEGIN;
static char	*end = END;
static char	*single = SINGLE;
/* ... their lengths */
static int	 beginlen;
static int	 endlen;
static int	 singlelen;

static char 	*progname; /* for warnings/errors */

/* program exit status */
static int	status = 0;

/* commentfile - strip comments from the file filename (or stdin, if "-") */
static void
commentfile(char *filename)
{
	FILE	*fp;
	char	*line = NULL;
	size_t	 linesz = 0;
	ssize_t	 linelen;
	bool	 inblockc = false;

	if (strcmp(filename, "-") == 0)
		fp = stdin;
	else if (!(fp = fopen(filename, "r"))) {
		status++;
		fprintf(stderr, "%s: couldn't open %s: %s\n", progname,
		    filename, strerror(errno));
		return;
	}

	while ((linelen = getline(&line, &linesz, fp)) != -1)
		for (int i = 0; line[i] != '\0'; i++) {
			if (inblockc) {
				if (endlen > 0 &&
				    strncmp(line+i, end, endlen) == 0) {
					inblockc = false;
					i += endlen-1;
				}
				continue;
			}

			if (beginlen > 0 &&
			    strncmp(line+i, begin, beginlen) == 0) {
				inblockc = true;
				i += beginlen-1;
			} else if (singlelen > 0 &&
			    strncmp(line+i, single, singlelen) == 0) {
				putchar('\n');
				break;
			} else
				putchar(line[i]);
		}

	free(line);
	if (ferror(fp)) {
		fprintf(stderr, "an error occured reading %s\n", filename);
		status++;
	}
}

/* comment - strip comments from files or stdin */
int
main(int argc, char *argv[])
{
	int ch;

	progname = argv[0];

	while ((ch = getopt(argc, argv, "b:e:s:")) != -1)
		switch (ch) {
		case 'b':
			begin = optarg;
			break;
		case 'e':
			end = optarg;
			break;
		case 's':
			single = optarg;
			break;
		default:
			fprintf(stderr,
"usage: comment [-b begin] [-e end] [-s single] [file ...]\n");
			return 1;
		}
	argc -= optind;
	argv += optind;

	if (strchr(begin, '\n') || strchr(end, '\n') || strchr(single, '\n')) {
		fprintf(stderr, "the -bes arguments cannot contain newlines\n");
		return 1;
	}

	singlelen = strlen(single);
	beginlen = strlen(begin);
	endlen = strlen(end);

	if (argc > 0)
		for (int i = 0; i < argc; i++)
			commentfile(argv[i]);
	else
		commentfile("-");

	return status;
}
