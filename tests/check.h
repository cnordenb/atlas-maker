#ifndef CHECK_H
#define CHECK_H

#include <wchar.h>

typedef int (*Test)(void);

extern const Test layout_tests[];
extern const int layout_test_count;

extern const Test atlas_tests[];
extern const int atlas_test_count;

extern const Test rules_tests[];
extern const int rules_test_count;

int atlas_tests_start(const wchar_t *project_dir);
void atlas_tests_stop(void);
void rules_tests_start(const wchar_t *project_dir);

#endif
