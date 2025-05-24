#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_file(FILE *fp, int number_lines, int mark_ends, int squeeze_blank, 
				int show_tabs, int show_nonprinting, int show_ends, int show_blank);

int main(int argc, char *argv[]) {
	int opt;
	int number_lines = 0;    // -n
	int mark_ends = 0;       // -e
	int squeeze_blank = 0;   // -s
	int show_tabs = 0;      // -t
	int show_nonprinting = 0; // -v
	int show_ends = 0;      // -E
	int show_blank = 0;     // -b

	while ((opt = getopt(argc, argv, "belnstuv")) != -1) {
		switch (opt) {
			case 'b': show_blank = 1; number_lines = 1; break;
			case 'e': mark_ends = 1; show_nonprinting = 1; break;
			case 'l': break; // -l is ignored in GNU cat
			case 'n': number_lines = 1; break;
			case 's': squeeze_blank = 1; break;
			case 't': show_tabs = 1; show_nonprinting = 1; break;
			case 'u': break; // -u is ignored in GNU cat
			case 'v': show_nonprinting = 1; break;
			default:
				fprintf(stderr, "Usage: %s [-belnstuv] [file...]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (optind >= argc) {
		// No files specified, read from stdin
		print_file(stdin, number_lines, mark_ends, squeeze_blank, 
				  show_tabs, show_nonprinting, show_ends, show_blank);
	} else {
		// Process each file
		for (int i = optind; i < argc; i++) {
			FILE *fp = fopen(argv[i], "r");
			if (fp == NULL) {
				perror(argv[i]);
				continue;
			}
			print_file(fp, number_lines, mark_ends, squeeze_blank, 
					  show_tabs, show_nonprinting, show_ends, show_blank);
			fclose(fp);
		}
	}

	return 0;
}

void print_file(FILE *fp, int number_lines, int mark_ends, int squeeze_blank, 
				int show_tabs, int show_nonprinting, int show_ends, int show_blank) {
	char buf[4096];
	int line_number = 1;
	int prev_blank = 0;
	int curr_blank;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		curr_blank = (buf[0] == '\n');
		
		if (squeeze_blank && curr_blank && prev_blank) {
			continue;
		}

		if (number_lines && (!show_blank || !curr_blank)) {
			printf("%6d\t", line_number++);
		}

		for (size_t i = 0; buf[i] != '\0'; i++) {
			if (show_nonprinting && !isprint(buf[i]) && buf[i] != '\n' && buf[i] != '\t') {
				printf("^%c", buf[i] + 64);
			} else if (show_tabs && buf[i] == '\t') {
				printf("^I");
			} else if (mark_ends && buf[i] == '\n') {
				printf("$\n");
			} else {
				putchar(buf[i]);
			}
		}

		prev_blank = curr_blank;
	}
}