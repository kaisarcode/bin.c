/**
 * bin.c - Summary of the functionality
 * Summary: Command line interface for the bin tool.
 *
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KC_BIN_VERSION "0.1.0"

/**
 * Reads text from standard input into a dynamically allocated buffer.
 * @param out_text Destination pointer for the allocated text.
 * @return 0 on success, or -1 on failure.
 */
static int kc_bin_read_stdin(char **out_text) {
    char *data = NULL;
    size_t length = 0;
    size_t capacity = 0;
    char chunk[4096];
    size_t n;

    if (!out_text) {
        return -1;
    }

    while ((n = fread(chunk, 1, sizeof(chunk), stdin)) > 0) {
        if (length + n + 1 > capacity) {
            size_t next_cap = capacity ? capacity * 2 : 4096;
            while (next_cap < length + n + 1) {
                next_cap *= 2;
            }
            char *next_data = (char *)realloc(data, next_cap);
            if (!next_data) {
                free(data);
                return -1;
            }
            data = next_data;
            capacity = next_cap;
        }
        memcpy(data + length, chunk, n);
        length += n;
    }

    if (ferror(stdin)) {
        free(data);
        return -1;
    }

    if (length == 0) {
        free(data);
        *out_text = NULL;
        return 0;
    }

    data[length] = '\0';
    *out_text = data;
    return 0;
}

/**
 * Print command usage information.
 * @param name Program executable name.
 * @return None.
 */
static void kc_print_help(const char *name) {
    printf("Usage: %s [run] <input> [options]\n", name);
    printf("\n");
    printf("Options:\n");
    printf("    -p, --param <val>   Set parameter value\n");
    printf("    -h, --help          Show this help message\n");
    printf("    -v, --version       Show version\n");
}

/**
 * Print command version information.
 * @return None.
 */
static void kc_print_version(void) {
    printf("bin %s\n", KC_BIN_VERSION);
}

/**
 * Execute the command line interface.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Process status code.
 */
int main(int argc, char **argv) {
    const char *verb = "run";
    const char *param = NULL;
    const char *text = NULL;
    char *stdin_text = NULL;
    int i = 1;

    if (i < argc && strcmp(argv[i], "run") == 0) {
        verb = argv[i++];
    }

    if (i < argc && argv[i][0] != '-') {
        text = argv[i++];
    }

    while (i < argc) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            kc_print_help(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            kc_print_version();
            return 0;
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--param") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "bin: missing value for %s\n", argv[i - 1]);
                return 1;
            }
            param = argv[i];
        } else {
            fprintf(stderr, "bin: unknown option '%s'\n", argv[i]);
            return 1;
        }
        i++;
    }

    if (!text) {
        if (kc_bin_read_stdin(&stdin_text) != 0) {
            return 1;
        }
        text = stdin_text;
    }

    (void)verb;
    (void)param;

    if (!text || !*text) {
        free(stdin_text);
        return 0;
    }

    free(stdin_text);
    return 0;
}
