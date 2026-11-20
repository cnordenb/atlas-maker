# Tests

This project strives to have sensible testing. The goal is not to have 100% test coverage but to have tests which _sufficiently_ ensure the functional completeness, functional correctness, maintainability, and accessibility of Atlas Maker. For more guidelines on software quality and accessibility, see [ISO/IEC 25010:2011](https://cdn.standards.iteh.ai/samples/35733/2ca18b477b7845a5b8cae39d6de0c098/ISO-IEC-25010-2011.pdf), [ISO/IEC 25010:2023](https://cdn.standards.iteh.ai/samples/78176/13ff8ea97048443f99318920757df124/ISO-IEC-25010-2023.pdf) and [WCAG 2.2](https://www.w3.org/TR/WCAG22/).

This document lists each test, its purpose, and in which test file it resides. This document is an effort to mitigate redundant tests and ensure test quality. It is intended as an aid in the trouble-shooting of potential false positives.

Test descriptions must remain brief and parameters (eg. Purpose) may not exceed 120 characters. Tests are ordered in order of their execution.

The test report names each test by its number here, such as "Test 15 failed": a test's number is its place in the order
the tests run. So when a test is added, removed or moved, renumber this document to match.

<!--
Each test has an entry of this form, after the table of contents, in the order the tests run:

## Test N

### File residence:
`path/to/test_file`, lines 24-26

### Purpose:
Verify that ... (Req. 3, 4).

### Assertion expectation:
What must hold for the test to pass.
-->

## Table of contents per category

Functional completeness & correctness tests:

- [Test 1](#test-1): four images in a square grid
- [Test 2](#test-2): a square grid with an empty cell
- [Test 3](#test-3): squareness measured in pixels
- [Test 4](#test-4): more columns on a tie
- [Test 5](#test-5): the largest atlas allowed
- [Test 6](#test-6): no atlas wider than the limit
- [Test 7](#test-7): no atlas taller than the limit
- [Test 8](#test-8): resized height keeps the aspect ratio
- [Test 9](#test-9): resized height rounded to nearest
- [Test 10](#test-10): resized height at least 1
- [Test 11](#test-11): atlas order
- [Test 12](#test-12): the shared resolution
- [Test 13](#test-13): the differing image named
- [Test 14](#test-14): no atlas from an empty list
- [Test 15](#test-15): the saved atlas and its size
- [Test 16](#test-16): first image top left
- [Test 17](#test-17): second image top right
- [Test 18](#test-18): third image bottom left
- [Test 19](#test-19): fourth image bottom right
- [Test 20](#test-20): a resized atlas
- [Test 21](#test-21): a resized image fills its cell
- [Test 22](#test-22): an empty cell is transparent
- [Test 23](#test-23): no atlas from differing images
- [Test 24](#test-24): no file after a failure

Accessibility tests:

Maintainability tests:

- [Test 25](#test-25): no comments in the code
- [Test 26](#test-26): no function over 60 lines

## Test 1

### File residence:
`tests/test_layout.c`, lines 18-21

### Purpose:
Verify that four square images make a grid of 2 columns and 2 rows (Req. 17).

### Assertion expectation:
`layout_compute(4, 100, 100)` succeeds with `cols` 2 and `rows` 2.

## Test 2

### File residence:
`tests/test_layout.c`, lines 23-26

### Purpose:
Verify that the grid closest to square is used even when it leaves a cell empty (Req. 17).

### Assertion expectation:
`layout_compute(3, 100, 100)` succeeds with `cols` 2 and `rows` 2.

## Test 3

### File residence:
`tests/test_layout.c`, lines 28-31

### Purpose:
Verify that closeness to square is measured in pixels, not in cells (Req. 17).

### Assertion expectation:
`layout_compute(2, 200, 100)` succeeds with `cols` 1 and `rows` 2.

## Test 4

### File residence:
`tests/test_layout.c`, lines 33-36

### Purpose:
Verify that of two grids equally close to square, the one with more columns is used (Req. 19).

### Assertion expectation:
`layout_compute(2, 100, 100)` succeeds with `cols` 2 and `rows` 1.

## Test 5

### File residence:
`tests/test_layout.c`, lines 38-41

### Purpose:
Verify that an atlas of exactly 16383 x 16383 px can be made (Req. 26, 27).

### Assertion expectation:
`layout_compute(1, 16383, 16383)` returns 0.

## Test 6

### File residence:
`tests/test_layout.c`, lines 43-46

### Purpose:
Verify that an atlas wider than 16383 px cannot be made (Req. 26).

### Assertion expectation:
`layout_compute(1, 16384, 1)` does not return 0.

## Test 7

### File residence:
`tests/test_layout.c`, lines 48-51

### Purpose:
Verify that an atlas taller than 16383 px cannot be made (Req. 27).

### Assertion expectation:
`layout_compute(1, 1, 16384)` does not return 0.

## Test 8

### File residence:
`tests/test_layout.c`, lines 53-56

### Purpose:
Verify that the height of a resized image follows the aspect ratio (Req. 23).

### Assertion expectation:
`layout_scaled_height(200, 100, 60)` is 30.

## Test 9

### File residence:
`tests/test_layout.c`, lines 58-61

### Purpose:
Verify that the height of a resized image is rounded to the nearest pixel (Req. 24).

### Assertion expectation:
`layout_scaled_height(3, 2, 4)` is 3, from 2.67.

## Test 10

### File residence:
`tests/test_layout.c`, lines 63-66

### Purpose:
Verify that the height of a resized image is never less than 1 px (Req. 25).

### Assertion expectation:
`layout_scaled_height(1000, 1, 10)` is 1.

## Test 11

### File residence:
`tests/test_atlas.c`, lines 97-104

### Purpose:
Verify that images are put in alphabetical order by file name, ignoring case (Req. 7).

### Assertion expectation:
`atlas_sort` turns date, Banana, apple, cherry into apple, Banana, cherry, date.

## Test 12

### File residence:
`tests/test_atlas.c`, lines 106-113

### Purpose:
Verify that the resolution shared by the images is found (Req. 16).

### Assertion expectation:
`atlas_inspect` of the four 100 x 100 fixtures returns 0 with width 100 and height 100.

## Test 13

### File residence:
`tests/test_atlas.c`, lines 115-123

### Purpose:
Verify that an image of another resolution is named to the user (Req. 9).

### Assertion expectation:
The message of `atlas_inspect` for all five fixtures contains `odd_50x50.png`.

## Test 14

### File residence:
`tests/test_atlas.c`, lines 125-132

### Purpose:
Verify that an empty input list cannot make an atlas (Req. 12).

### Assertion expectation:
`atlas_inspect` of 0 images returns -1.

## Test 15

### File residence:
`tests/test_atlas.c`, lines 134-137

### Purpose:
Verify that the atlas is saved as a WebP image of the size of its grid (Req. 17, 30).

### Assertion expectation:
The file written for four 100 x 100 fixtures decodes as WebP, 200 x 200 px.

## Test 16

### File residence:
`tests/test_atlas.c`, lines 139-142

### Purpose:
Verify that the first image in atlas order is drawn in the top left cell (Req. 14, 15).

### Assertion expectation:
Pixel (50, 50) of the 200 x 200 atlas has the red of `apple.png`, within 24 a channel.

## Test 17

### File residence:
`tests/test_atlas.c`, lines 144-147

### Purpose:
Verify that the second image in atlas order is drawn to the right of the first (Req. 15).

### Assertion expectation:
Pixel (150, 50) of the 200 x 200 atlas has the green of `Banana.png`, within 24 a channel.

## Test 18

### File residence:
`tests/test_atlas.c`, lines 149-152

### Purpose:
Verify that the image after a full row is drawn first in the next row (Req. 15).

### Assertion expectation:
Pixel (50, 150) of the 200 x 200 atlas has the blue of `cherry.png`, within 24 a channel.

## Test 19

### File residence:
`tests/test_atlas.c`, lines 154-157

### Purpose:
Verify that the last image in atlas order is drawn in the last cell (Req. 15).

### Assertion expectation:
Pixel (150, 150) of the 200 x 200 atlas has the yellow of `date.png`, within 24 a channel.

## Test 20

### File residence:
`tests/test_atlas.c`, lines 159-162

### Purpose:
Verify that images resized to a width of 60 make an atlas of resized cells (Req. 13, 21).

### Assertion expectation:
The file written for four fixtures at tile size 60 x 60 decodes as 120 x 120 px.

## Test 21

### File residence:
`tests/test_atlas.c`, lines 164-167

### Purpose:
Verify that a resized image is drawn at the tile size, to the edge of its cell (Req. 21).

### Assertion expectation:
Pixel (115, 115) of the 120 x 120 atlas has the yellow of `date.png`, within 24 a channel.

## Test 22

### File residence:
`tests/test_atlas.c`, lines 169-174

### Purpose:
Verify that a cell without an image is fully transparent (Req. 20).

### Assertion expectation:
Pixel (150, 150) of the atlas of three fixtures has alpha 0.

## Test 23

### File residence:
`tests/test_atlas.c`, lines 176-179

### Purpose:
Verify that no atlas is created from images of different resolutions (Req. 8).

### Assertion expectation:
`atlas_build` of all five fixtures returns -1.

## Test 24

### File residence:
`tests/test_atlas.c`, lines 181-186

### Purpose:
Verify that no output file is written when the atlas could not be created (Req. 42).

### Assertion expectation:
After the failed `atlas_build`, `test_atlas_never_written.webp` does not exist.

## Test 25

### File residence:
`tests/test_rules.c`, lines 113-116

### Purpose:
Enforce that the code has no comments (Rule 10).

### Assertion expectation:
No code file under `src`, `res` or `tests` has a comment outside quotes; no line of `CMakeLists.txt` starts with `#`.

## Test 26

### File residence:
`tests/test_rules.c`, lines 118-121

### Purpose:
Enforce that no function is over 60 lines of code (Rule 11).

### Assertion expectation:
In no code file under `src`, `res` or `tests` do more than 60 lines that are not blank end at a `}` in column 1.
