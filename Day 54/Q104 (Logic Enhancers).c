/*
 * Write a Program to take a positive integer n as input, and find the pivot integer x such that the sum of all elements between 1 and x inclusively equals the sum of all elements between x and n inclusively. Print the pivot integer x. If no such integer exists, print -1. Assume that it is guaranteed that there will be at most one pivot integer for the given input.
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
    unsigned long long n, target, low = 1ULL, high, middle;
    char text[1024];
    long long number[2];
    int count;
    if (!read_input_text(text, sizeof text) ||
        (count = extract_integers(text, number, 2)) != 1 ||
        number[0] <= 0 || (unsigned long long)number[0] > 4000000000ULL) {
        fprintf(stderr, "Enter n from 1 to 4000000000.\n");
        return 1;
    }
    n = (unsigned long long)number[0];
    target = n * (n + 1ULL);
    high = n;
    while (low <= high) {
        middle = low + (high - low) / 2ULL;
        if (2ULL * middle * middle == target) { printf("%llu\n", middle); return 0; }
        if (2ULL * middle * middle < target) low = middle + 1ULL;
        else high = middle - 1ULL;
    }
    puts("-1");
    return 0;
}
