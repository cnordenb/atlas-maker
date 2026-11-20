requirement - a numbered statement in REQUIREMENTS.md of what the project must do, e.g. "Requirement 4"
rule - a numbered statement in RULES.md of how the project must be made and kept, e.g. "Rule 12"
chief developer - the person who alone writes the requirements, named in Rule 2
test - a check of the project, run on every push and listed by its number in TESTS.md, e.g. "Test 15"
log - the development log in documentation/DEVLOG/: what changed in the project, why, and what is left
part - one file of the log, DEVLOG1.md, DEVLOG2.md and so on, each of at most 1000 lines
day - the entry of the log for one date, titled by it, e.g. "2026-11-02", never split between two parts
section - one change recorded under its day, titled by its time if it was the first written in that minute
decision - a choice made in a section and listed by subject in DECISIONS.md, with a link to that section
question - a numbered point for the chief developer at the end of a section, open until a later section answers it
success criterion - a numbered WCAG 2.2 statement at level A or AA that the interface must meet, e.g. "2.1.1 Keyboard"
human verification - a person's check that a success criterion holds, needed where no test can check it in full
image - a PNG file that the user gives the app, e.g. "apple.png"
resolution - the width and height of an image in pixels, e.g. 100 x 100
input list - the images the user has added, shown in the window by file name
atlas order - the order of the input list: alphabetical by file name, ignoring case
tile size - the width and height at which every image is drawn: its resolution, or the size it is resized to
cell - one place of the tile size, holding one image or nothing
grid - the cells laid out in columns and rows, e.g. 2 x 2 for four images
atlas - the one picture of the whole grid, with the images drawn in atlas order, left to right and row by row
output file - the WebP file that the user chooses and the atlas is saved to
fixture - an image in tests/data/ of one known colour, which a test builds an atlas from
