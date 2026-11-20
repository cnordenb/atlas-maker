#include <stdio.h>
#include <wchar.h>

#include "check.h"

static int run(const Test *tests, int count, int first_number)
{
    int failures = 0;

    for (int i = 0; i < count; i++) {
        if (!tests[i]()) {
            fprintf(stderr, "Test %d failed\n", first_number + i);
            failures++;
        }
    }
    return failures;
}

int wmain(int argc, wchar_t **argv)
{
    int total = layout_test_count + atlas_test_count + rules_test_count;
    int failures;

    if (argc != 2) {
        fprintf(stderr, "usage: run_tests <project directory>\n");
        return 2;
    }
    failures = run(layout_tests, layout_test_count, 1);
    if (atlas_tests_start(argv[1]) != 0) {
        fprintf(stderr, "could not start Windows Imaging Component\n");
        return 2;
    }
    failures += run(atlas_tests, atlas_test_count, 1 + layout_test_count);
    atlas_tests_stop();
    rules_tests_start(argv[1]);
    failures += run(rules_tests, rules_test_count, 1 + layout_test_count + atlas_test_count);
    printf("%d of %d tests passed\n", total - failures, total);
    return failures;
}
