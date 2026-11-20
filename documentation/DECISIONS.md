# Decisions

- [Rules, and how they are read](#rules-and-how-they-are-read)
- [Requirements, and how they are read](#requirements-and-how-they-are-read)
- [Code and files](#code-and-files)
- [Tests](#tests)
- [Documents](#documents)

The decisions recorded in the development log in [DEVLOG/](DEVLOG/DEVLOG1.md), by subject (Rule 23). Each ends with the
day it was made, and the time where the log has one, as a link to its entry in the log. Where a later decision replaced
an earlier one, the earlier one is named after it, with its own link.

## Rules, and how they are read

- **The main folder may hold 9 files** (Rule 14), where the template allows 8, so that `favicon.ico` and `image.png`
  stay where they are.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)
- **`CMakeLists.txt` is code**, though Rule 26 says `*.txt` is not: it has no comments, and `check-rules.sh` measures
  its lines.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)
- **One check to a test is read strictly** (Rules 5 and 25): a test is one function that returns one condition, which
  names both numbers where the thing checked has two, such as columns and rows.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)

## Requirements, and how they are read

- **The first requirements were drafted from the app as it was**, and its README, for the chief developer to read and
  correct.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)
- **A requirement has the verb "will", concerns one thing, can be answered with yes or no, and is functional.**
  [2026-11-20 06:16:32](DEVLOG/DEVLOG1.md#061632-requirements-of-one-thing-each-and-tests-of-rules-10-and-11)
- **The tie-break on fewer empty cells stays a requirement** (Requirement 18), though no case was found where it
  changes the grid, so it has no test.
  [2026-11-20 06:16:32](DEVLOG/DEVLOG1.md#061632-requirements-of-one-thing-each-and-tests-of-rules-10-and-11)

## Code and files

- **`ub-mode` is the template's copy.** The copy with a section on committing and pushing was replaced (Rule 0).
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)
- **The skills `cc`, `hfdp`, `karpathy-guidelines`, `limits` and `p10` stay**, and are not listed in `AGENTS.md`.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)
- **The security contact is left open** until before the first release; its placeholder stays in `SECURITY.md`.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)

## Tests

- **The tests are one program that numbers them as they run.** `run_tests` runs the tables of `test_layout.c` and
  `test_atlas.c` in that order, so a test's number in `TESTS.md` is its place in the tables.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)
- **Rules 10 and 11 are kept by tests written in C**, Tests 25 and 26 in `tests/test_rules.c`, which read the code.
  [2026-11-20 06:16:32](DEVLOG/DEVLOG1.md#061632-requirements-of-one-thing-each-and-tests-of-rules-10-and-11)
- **GitHub runs the checks and the tests on Windows**, `windows-latest`, since the tests need Windows Imaging
  Component.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)

## Documents

- **Building and the layout of the code are explained in `MAINTAINER.md`**, not in the README, which keeps the
  template's chapters.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)
- **The Acknowledgements of the README name both models** that the development was accelerated with.
  [2026-11-20 05:26:36](DEVLOG/DEVLOG1.md#052636-the-project-set-up-from-the-template-and-brought-under-its-rules)
