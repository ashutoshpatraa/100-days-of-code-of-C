/*
 * Write a program to take an input array of size n. The array should contain all the integers between 0 to n except for one. Print that missing number
 */
#ifdef _WIN32
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static void portable_format(char *destination, size_t capacity, const char *source) {
    size_t output = 0;
    while (*source != '\0' && output + 1 < capacity) {
#ifdef _WIN32
        if (strncmp(source, "%lld", 4) == 0 && output + 5 < capacity) {
            memcpy(destination + output, "%I64d", 5);
            source += 4;
            output += 5;
            continue;
        }
        if (strncmp(source, "%llu", 4) == 0 && output + 5 < capacity) {
            memcpy(destination + output, "%I64u", 5);
            source += 4;
            output += 5;
            continue;
        }
#endif
        destination[output++] = *source++;
    }
    destination[output] = '\0';
}

int checked_scanf(const char *format, ...) {
    char adapted[512];
    int result;
    va_list arguments;
    portable_format(adapted, sizeof adapted, format);
    va_start(arguments, format);
    result = vscanf(adapted, arguments);
    va_end(arguments);
    return result;
}

int checked_fscanf(FILE *file, const char *format, ...) {
    char adapted[512];
    int result;
    va_list arguments;
    portable_format(adapted, sizeof adapted, format);
    va_start(arguments, format);
    result = vfscanf(file, adapted, arguments);
    va_end(arguments);
    return result;
}

#define scanf checked_scanf
#define fscanf checked_fscanf

int read_input_text(char *text, size_t capacity) {
    size_t length = fread(text, 1, capacity - 1, stdin);
    if (length == capacity - 1 && !feof(stdin)) return 0;
    text[length] = '\0';
    return 1;
}

int extract_integers(const char *text, long long *values, size_t capacity) {
    const char *position = text;
    size_t count = 0;
    while (*position != '\0') {
        int starts_number = isdigit((unsigned char)*position) ||
            (*position == '-' && isdigit((unsigned char)position[1]));
        if (starts_number &&
            (position == text ||
             (!isalnum((unsigned char)position[-1]) && position[-1] != '_'))) {
            char *end;
            long long value;
            errno = 0;
            value = strtoll(position, &end, 10);
            if (errno == ERANGE || end == position || count == capacity) return -1;
            values[count++] = value;
            position = end;
        } else {
            ++position;
        }
    }
    return (int)count;
}

int extract_quoted_string(const char *text, size_t *position,
                          char *destination, size_t capacity) {
    const char *start = strchr(text + *position, '"');
    const char *end;
    size_t length;
    if (start == NULL) return 0;
    end = strchr(start + 1, '"');
    if (end == NULL) return 0;
    length = (size_t)(end - start - 1);
    if (length >= capacity) return 0;
    memcpy(destination, start + 1, length);
    destination[length] = '\0';
    *position = (size_t)(end - text + 1);
    return 1;
}

int main(void) {
    char text[65536];
    long long numbers[100001], expected, actual = 0;
    int parsed, n, i, offset;
    if (!read_input_text(text, sizeof text) ||
        (parsed = extract_integers(text, numbers, 100001)) < 0) {
        fprintf(stderr, "Invalid or oversized input.\n"); return 1;
    }
    offset = strchr(text, '[') != NULL ? 0 : 1;
    n = offset == 0 ? parsed : (parsed > 0 ? (int)numbers[0] : 0);
    if (n < 0 || n > 100000 || parsed != n + offset) { fprintf(stderr, "Invalid array input.\n"); return 1; }
    expected = (long long)n * (n + 1LL) / 2LL;
    for (i = 0; i < n; ++i) {
        if (numbers[i + offset] < 0 || numbers[i + offset] > n) {
            fprintf(stderr, "Expected values from 0 through n.\n");
            return 1;
        }
        actual += numbers[i + offset];
    }
    printf("%lld\n", expected - actual);
    return 0;
}
