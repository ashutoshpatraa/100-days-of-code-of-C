/*
 * Write a program to take two sorted arrays of size m and n as input. Merge both the arrays such that the merged array is also sorted. Print the merged array.
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
    static long long numbers[100002], first[50000], second[50000];
    int parsed, n, m, i, j;
    const char *first_end;
    if (!read_input_text(text, sizeof text) ||
        (parsed = extract_integers(text, numbers, 100002)) < 0) {
        fprintf(stderr, "Invalid or oversized input.\n"); return 1;
    }
    if (strchr(text, '[') != NULL) {
        first_end = strchr(strchr(text, '[') + 1, ']');
        if (first_end == NULL) { fprintf(stderr, "Malformed array input.\n"); return 1; }
        {
            char first_text[65536];
            static long long first_numbers[50000];
            size_t length = (size_t)(first_end - text + 1);
            if (length >= sizeof first_text) { fprintf(stderr, "First array is too large.\n"); return 1; }
            memcpy(first_text, text, length);
            first_text[length] = '\0';
            n = extract_integers(first_text, first_numbers, 50000);
        }
        m = parsed - n;
        if (n > 50000 || m > 50000) { fprintf(stderr, "Arrays are too large.\n"); return 1; }
        for (i = 0; i < n; ++i) first[i] = numbers[i];
        for (i = 0; i < m; ++i) second[i] = numbers[n + i];
    } else {
        if (parsed < 2) { fprintf(stderr, "Invalid array input.\n"); return 1; }
        n = (int)numbers[0];
        if (n < 0 || n > 50000 || parsed < n + 2) { fprintf(stderr, "Invalid first array size.\n"); return 1; }
        m = (int)numbers[n + 1];
        if (m < 0 || m > 50000 || parsed != n + m + 2) { fprintf(stderr, "Invalid second array size.\n"); return 1; }
        for (i = 0; i < n; ++i) first[i] = numbers[i + 1];
        for (i = 0; i < m; ++i) second[i] = numbers[n + i + 2];
    }
    for (i = 1; i < n; ++i) if (first[i] < first[i - 1]) { fprintf(stderr, "First array must be sorted.\n"); return 1; }
    for (i = 1; i < m; ++i) if (second[i] < second[i - 1]) { fprintf(stderr, "Second array must be sorted.\n"); return 1; }
    i = 0; j = 0;
    while (i < n || j < m) {
        if (j == m || (i < n && first[i] <= second[j])) printf("%lld ", first[i++]);
        else printf("%lld ", second[j++]);
    }
    putchar('\n');
    return 0;
}
