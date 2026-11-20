#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "check.h"

#define PATH_LEN 1024
#define LINE_LEN 1024
#define MAX_FUNCTION_LINES 60

typedef int (*LineCheck)(const char *line, int *state);

static const wchar_t *root;
static const wchar_t *const code_folders[] = {L"src/core", L"src/gui", L"res", L"tests"};
static const wchar_t *const code_endings[] = {L".c", L".h", L".rc", L".manifest"};

static int has_comment(const char *line, int *state)
{
    char quote = 0;

    (void)state;
    for (const char *p = line; *p != '\0'; p++) {
        if (quote != 0) {
            if (*p == '\\' && p[1] != '\0') p++;
            else if (*p == quote) quote = 0;
        } else if (*p == '"' || *p == '\'') {
            quote = *p;
        } else if (strncmp(p, "/*", 2) == 0 || strncmp(p, "//", 2) == 0 || strncmp(p, "<!--", 4) == 0) {
            return 1;
        }
    }
    return 0;
}

static int starts_with_hash(const char *line, int *state)
{
    (void)state;
    return line[strspn(line, " \t")] == '#';
}

static int ends_long_function(const char *line, int *lines)
{
    int too_long;

    if (strcmp(line, "{\n") == 0) *lines = 1;
    if (*lines > 0 && line[strspn(line, " \t\n")] != '\0') (*lines)++;
    if (line[0] != '}' || (line[1] != '\n' && line[1] != '\0')) return 0;
    too_long = *lines > MAX_FUNCTION_LINES;
    *lines = 0;
    return too_long;
}

static int file_breaks(const wchar_t *path, LineCheck breaks)
{
    FILE *file = _wfopen(path, L"r");
    char line[LINE_LEN] = "could not be read\n";
    int state = 0;
    int found = file == NULL;

    while (!found && fgets(line, LINE_LEN, file) != NULL) found = breaks(line, &state);
    if (file != NULL) fclose(file);
    if (found) fwprintf(stderr, L"%ls: %hs", path, line);
    return found;
}

static int is_code(const wchar_t *name)
{
    const wchar_t *ending = wcsrchr(name, L'.');

    for (size_t i = 0; ending != NULL && i < sizeof code_endings / sizeof code_endings[0]; i++) {
        if (wcscmp(ending, code_endings[i]) == 0) return 1;
    }
    return 0;
}

static int folder_breaks(const wchar_t *folder, LineCheck breaks)
{
    wchar_t path[PATH_LEN];
    WIN32_FIND_DATAW entry;
    HANDLE search;
    int found = 0;

    swprintf(path, PATH_LEN, L"%ls/%ls/*", root, folder);
    search = FindFirstFileW(path, &entry);
    if (search == INVALID_HANDLE_VALUE) return 1;
    do {
        swprintf(path, PATH_LEN, L"%ls/%ls/%ls", root, folder, entry.cFileName);
        if (is_code(entry.cFileName) && file_breaks(path, breaks)) found = 1;
    } while (FindNextFileW(search, &entry));
    FindClose(search);
    return found;
}

static int code_breaks(LineCheck breaks)
{
    int found = 0;

    for (size_t i = 0; i < sizeof code_folders / sizeof code_folders[0]; i++) {
        if (folder_breaks(code_folders[i], breaks)) found = 1;
    }
    return found;
}

static int build_file_breaks(LineCheck breaks)
{
    wchar_t path[PATH_LEN];

    swprintf(path, PATH_LEN, L"%ls/CMakeLists.txt", root);
    return file_breaks(path, breaks);
}

static int code_has_no_comments(void)
{
    return !code_breaks(has_comment) && !build_file_breaks(starts_with_hash);
}

static int no_function_is_too_long(void)
{
    return !code_breaks(ends_long_function);
}

void rules_tests_start(const wchar_t *project_dir)
{
    root = project_dir;
}

const Test rules_tests[] = {
    code_has_no_comments,
    no_function_is_too_long,
};

const int rules_test_count = (int)(sizeof rules_tests / sizeof rules_tests[0]);
