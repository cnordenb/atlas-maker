# Development log, part 1

Part 1 of the development log. The decisions made in the log are listed in [DECISIONS.md](../DECISIONS.md).

- [2026-11-20](#2026-11-20)

## 2026-11-20

### 05:26:36: the project set up from the template and brought under its rules

#### What changed

- **Why.** Atlas Maker, a Windows app that packs PNG images of one resolution into a single atlas saved as lossy WebP
  at quality 90, existed before the repository template did. It had code, tests and a README, but none of the
  template's documents, rules, procedures or checks.
- **Files from the template.** Taken unchanged from github.com/cnordenb/repo_template at its commit `ab7abca`:
  `.gitattributes`, `CLAUDE.md`, `.claude/settings.json`, the skills `devlog`, `maintainer-guide`, `project-setup`,
  `rules-check`, `terminology` and `tests-doc`, `LICENSE-ub-mode.txt`, and in `.github/` `ACCESSIBILITY.md`,
  `CODE_OF_CONDUCT.md`, `CONTRIBUTING.md`, `FUNDING.yml`, `PULL_REQUEST_TEMPLATE.md`, `SECURITY.md` and
  `ISSUE_TEMPLATE/bug_report.md`. Taken and then filled in or changed, as the points below say: `AGENTS.md`,
  `ISSUE_TEMPLATE/config.yml`, `ISSUE_TEMPLATE/feature_request.md`, `scripts/check-rules.sh`, `workflows/checks.yml`
  and everything in `documentation/`. `LICENSE` was already the template's, with the year and the name filled in.
- **Placeholders.** Filled in: the project's name and description, the owner `cnordenb`, the repository
  `atlas-maker`, the chief developer in Rule 2 as `LICENSE` names its holder, the stack and the test command in
  `AGENTS.md`, and the maintenance task, its steps and the code overview in `MAINTAINER.md`. The README kept its
  badges, which already named the language.
- **ub-mode.** `.claude/skills/ub-mode/SKILL.md` is now the template's copy. The copy it replaced ended with a
  section "Commit and Push", which Rule 0 forbids to agents.
- **Other skills.** `cc`, `hfdp`, `karpathy-guidelines`, `limits` and `p10` stay in `.claude/skills/` as they were,
  and are not listed in the table of procedures in `AGENTS.md`.
- **`.gitignore`.** The template's lines and the project's own: `learning-materials/`, `build/` and `images/*.png`.
- **README.** It has the template's chapters: Description, Usage, Contributing, Maintaining and Acknowledgements. Its
  chapters "Building" and "Layout" moved into `MAINTAINER.md`. The Acknowledgements name both models used.
- **Requirements.** `REQUIREMENTS.md` has 20 requirements, drafted from what the app does and what the README said
  it does, by leave of the chief developer, who has yet to read them (question 1).
- **Comments.** Rule 10: all 55 comments were taken out of `src/`, `res/` and `CMakeLists.txt`, and what they
  explained is now in `MAINTAINER.md`, under "How an atlas is made", "The window" and "Building and running". No
  other line of `src/` changed.
- **Tests as one program.** `tests/run_tests.c` is new. It runs the table of tests at the end of
  `tests/test_layout.c`, then the one at the end of `tests/test_atlas.c`, numbers the tests as they run, and prints
  "Test 15 failed" for a test that fails. `tests/check.h` declares the tables; its `CHECK` macro is gone. In
  `CMakeLists.txt` the programs `test_layout` and `test_atlas` gave way to `run_tests`, which CTest runs as `tests`.
- **Tests.** 24, each making one check (Rules 5 and 25), where there were 9 test functions making 94 checks. Tests 1
  to 10 are in `tests/test_layout.c` and Tests 11 to 24 in `tests/test_atlas.c`. Left out, as covering no requirement
  or as covered by another test: invalid arguments to `layout_compute` and `layout_scaled_height`; the grids for 1,
  5, 9 and 56 square images and for four of 100 x 50 px; a second unchanged height; the words "same size" in the
  message; and in the atlases of three images and of 60 px tiles, the size and the pixels that the atlas of four
  already shows. Test 24 now deletes its file before it runs, so that a file left by an earlier failure cannot fail
  it.
- **Rule 14.** The main folder holds 9 files: `.gitattributes`, `.gitignore`, `AGENTS.md`, `CLAUDE.md`,
  `CMakeLists.txt`, `LICENSE`, `README.md`, `favicon.ico` and `image.png`.
- **Rules.** Rule 14: the main folder may hold 9 files, where the template allows 8. `check-rules.sh` and
  `MAINTAINER.md` say the same.
- **`check-rules.sh`.** `code_pattern` also matches `.c`, `.h`, `.rc`, `.manifest` and `CMakeLists.txt`.
- **The workflow.** `checks.yml` runs on `windows-latest` with Bash as its shell, since the tests need Windows. It
  has a step that shows the versions of gcc, CMake and Ninja, and its `TEST_COMMAND` is the test command, with `CC`
  set to `gcc`.
- **Checked.** The build gave no warning, and all 24 tests passed. The code was then broken on purpose in 17 ways,
  one at a time, in `layout.c`, `atlas.c` and `image.c`; each breakage failed at least one test, each of the 24 tests
  failed for at least one breakage, and each breakage was undone, after which all 24 passed again.
  `bash .github/scripts/check-rules.sh` passed, with the placeholder of the security contact under `todo`. dryer
  found no code written twice in `src` and `tests`. No function is over 60 lines, by reading.
- **Documents.** `TESTS.md`: Tests 1 to 24. `MAINTAINER.md`: written whole. `TERMINOLOGY.md`: image, resolution,
  input list, atlas order, tile size, cell, grid, atlas, output file and fixture. `DECISIONS.md`.

#### How the rules were read

- **Rule 26 and `CMakeLists.txt`.** Rule 26 says `*.txt` is not code, which would let `CMakeLists.txt` keep its
  comments and its long lines. It was read as code all the same, because it is the build's program and only its
  ending is that of a text file (question 5).
- **One check to a test.** A test is one function that returns one condition. Where the thing checked has two
  numbers, such as the columns and rows of a grid or the width and height of an atlas, the condition names both.
- **Rule 4, "every push".** The tests need Windows Imaging Component, so they run on GitHub's Windows machine.

#### Obstacles met, and how they were solved

- **mutator.** It stopped with `PermissionError: [WinError 5] Access is denied` while deleting its own folder under
  `target/mutation-workers/`, at a read-only file of the libwebp that each of its workers had downloaded. The folder
  was deleted by hand, and the code was broken on purpose by a script instead, as **Checked.** says.
- **A file left by a failing test.** The breakage that lets differing images through made Test 24's file, which then
  failed Test 24 on every later run. Test 24 was changed to delete it first.

#### Not done

- The workflow has not run on GitHub: nothing was pushed. Whether `windows-latest` has gcc, CMake and Ninja ready is
  what its step "Show the toolchain" will show.
- The app was built but not tried by hand after the comments were taken out.
- crapper was not run: gcovr is not installed. mutator did not finish, as said above.
- No item of `.github/ACCESSIBILITY.md` is checked or marked.
- No test checks Rule 10 or Rule 11 (question 6).
- Requirements without a test: 1, 2, 3, 6, 16, 17, 18 and 20, and of Requirement 15 that the WebP is lossy, at
  quality 90, and replaces a file already there (question 7).
- Placeholders left: the security contact in `.github/SECURITY.md`. The `project-setup` skill stays until it is
  filled in.
- `.github/FUNDING.yml` has comments (question 2).
- The "Question" link of the issue form needs Discussions turned on, on GitHub.

#### Questions

1. **The requirements.** `REQUIREMENTS.md` was drafted from the app and its README. Does each of the 20 say what is
   wanted? Proposal: read them once and change what is wrong; the tests cite them by number.
2. **Comments in `FUNDING.yml`.** By Rule 26 a `.yml` file is code, and the template's `FUNDING.yml` is all comments
   after its keys (Rule 10). Proposal: take the unused keys and their comments out when a funding platform is chosen,
   and until then exempt the file in Rule 26.
3. **The chief developer's name.** Rule 2 names the chief developer as `LICENSE` names its holder, where the template
   would take the name Git is set up with, which is the same as the owner's. Proposal: keep the name of `LICENSE`.
4. **The most common maintenance task.** `MAINTAINER.md` begins with "Updating libwebp", the one outside part that
   changes without the project. Is another task more common? Proposal: keep it.
5. **`CMakeLists.txt` as code.** Should Rule 26 say that `CMakeLists.txt` is code, though it ends in `.txt`?
   Proposal: yes, as the checks already treat it.
6. **Tests of Rules 10 and 11.** The `tests-doc` skill says rules that the script cannot check, no comments and no
   function over 60 lines, belong in a maintainability test written in C. Proposal: write the two tests, reading
   `src/`, `res/` and `tests/`.
7. **Tests of the window.** The requirements about the input list and what the window shows have no test, as the
   tests do not open the window. Proposal: leave them to checks by hand until a change to `src/gui` goes wrong.
8. **Requirement 11, fewer empty cells.** No case was found where it changes the grid: among tile sizes up to
   30 x 30 and up to 300 images, whenever two grids were equally close to square, the one with fewer cells also had
   the most columns. So no test can tell whether it holds. Proposal: keep the requirement and the code as they are.
9. **The security contact.** Where should a security hole be reported? It is asked before the first release.

### 06:16:32: requirements of one thing each, and tests of Rules 10 and 11

#### What changed

- **Answers.** Of the questions of 05:26:36, in [the project set up from the template and
  brought under its rules](#052636-the-project-set-up-from-the-template-and-brought-under-its-rules):
  the requirements were read and given a form, next point (1); Rules 10 and 11 get tests (6); Requirement 11 on
  fewer empty cells, now Requirement 18, stays as it is, with no test (8). Questions 2, 3, 4, 5, 7 and 9 are open.
- **Requirements.** `REQUIREMENTS.md` has 42 requirements where it had 20. Each has the verb "will", concerns one
  thing, can be answered with yes or no, and says what the app does: the one that was not about that, a single
  executable that needs only what comes with Windows, is gone. Line 0, where the requirements apply, is kept.
- **Tests.** 26, up from 24. Test 25 checks that no code file under `src`, `res` or `tests` has a comment and that
  no line of `CMakeLists.txt` starts with `#` (Rule 10). Test 26 checks that no function is over 60 lines of code
  (Rule 11). Both are in the new `tests/test_rules.c` and run last, as maintainability tests. Tests 1 to 24 are
  unchanged but for the numbers of the requirements they cite.
- **The test program.** `run_tests` is given the project's folder, not `tests/data`, since Tests 25 and 26 read the
  code; `atlas_tests_start` finds the fixtures under it. `CMakeLists.txt` and `tests/check.h` follow.
- **Checked.** The build gave no warning, and all 26 tests passed. With a file holding a comment put into
  `src/core`, Test 25 failed and named the file and the line; with a line starting with `#` added to
  `CMakeLists.txt`, it failed and named that line; with two slashes and a comment's signs only inside quotes, it
  passed. With a function of 61 lines put into `src/gui`, Test 26 failed and named the file; with one of 60 lines,
  it passed. Each was undone, and all 26 passed again. `bash .github/scripts/check-rules.sh` passed.
- **Documents.** `TESTS.md`: the requirements cited by Tests 1 to 24, and Tests 25 and 26. `MAINTAINER.md`: the
  chapter on tests. `DECISIONS.md`.

#### Not done

- Requirements without a test: 1 to 6, 10, 11, 18, 22, 28, 29, 31 to 41.
- Test 26 knows a function only by braces standing alone at the start of a line, and Test 25 does not see the
  later lines of a comment of several lines, only its first.
- What was not done at 05:26:36 is still not done, but for the tests of Rules 10 and 11.

#### Questions

1. **The form of a requirement.** The four marks of a requirement are written in no file of the project. Should
   they be a rule in `RULES.md`, so that they are kept when requirements are added? Proposal: yes, as a new rule.
