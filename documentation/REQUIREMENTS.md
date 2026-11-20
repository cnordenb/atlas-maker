# Requirements

0. These requirements apply to Atlas Maker as a desktop app for Windows 10 and Windows 11, with one window and its
   text in English.

## The input list

1. The user will be able to add several PNG images to the input list at once.
2. The user will be able to add more images to an input list that already holds images.
3. An image that is already in the input list will not be added a second time.
4. The user will be able to remove the selected images from the input list.
5. The user will be able to clear the whole input list.
6. The input list will show the file name of each of its images.
7. The input list will be kept in atlas order: alphabetical by file name, ignoring case.
8. No atlas will be created while the images in the input list differ in resolution.
9. The app will name a file whose resolution differs from that of the others.
10. No atlas will be created while a file in the input list cannot be read as an image.
11. The app will name a file that cannot be read as an image.
12. No atlas will be created from an empty input list.

## The atlas

13. Every cell of the atlas will have the tile size.
14. Each image will be drawn in a cell of its own.
15. The images will be drawn in atlas order, left to right and row by row.
16. The tile size will be the resolution of the images, unless the user chooses to resize them.
17. The grid will be the one whose atlas is closest to square in pixels, such as 200 x 200 px for four images of
    100 x 100 px.
18. Of the grids equally close to square, the one with the fewest empty cells will be used.
19. Of the grids equally close to square that have equally few empty cells, the one with the most columns will be used.
20. A cell without an image will be fully transparent.
21. The user will be able to choose to resize each image to a width entered in whole pixels.
22. A width below 1 or above 16383 will not be accepted.
23. The height of a resized image will follow the aspect ratio of the images.
24. The height of a resized image will be rounded to the nearest pixel.
25. The height of a resized image will be at least 1 px.
26. No atlas wider than 16383 px will be created.
27. No atlas taller than 16383 px will be created.
28. The app will say so when the atlas would be too large to create.

## The output

29. The user will be able to choose the output file.
30. The atlas will be saved to the output file.
31. The atlas will be saved as lossy WebP.
32. The atlas will be saved at quality 90.
33. A file that is already at the place of the output file will be replaced.
34. Before the atlas is created, the app will show the columns and rows of the grid.
35. Before the atlas is created, the app will show the tile size.
36. Before the atlas is created, the app will show the width and height of the atlas.
37. "Create atlas" will not be usable while no atlas can be created.
38. "Create atlas" will not be usable while no output file is chosen.
39. After the atlas is created, the app will show its width and height.
40. After the atlas is created, the app will show where it was saved.
41. When the atlas could not be created, the app will say why.
42. When the atlas could not be created, no output file will be written.
