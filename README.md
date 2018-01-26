# libglr (not yet released)

## Introduction

Utility library for handling OpenGL raw image files by storing the
image metadata within the filename.  This library provides a very
fast parser for such a filename.

Image-loading involves reading in a file and storing it into a buffer.
The format can be determined are as follows:

```C
const char *ep = NULL;
int xdim = -1, ydim = -1;
int internal = -1, format = -1, type = -1;

/* e.g. filename = "raw.512x512.rgba8" */
internal = glrparse(filename, &ep, &xdim, &ydim);

if (internal < 0) {
	fprintf(stderr, "error: could not parse the image name.\n");
	exit(EXIT_FAILURE);
}

if (xdim < 0 || ydim < 0) {
	fprintf(stderr, "error: could not parse the image dimensions.\n");
	exit(EXIT_FAILURE);
}

/* if internal > 0, then it should contain the internal OpenGL format -
 * that is specifically Table 2 from the (glTexImage2D reference page)[https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml]
 */

/* the format of the pixel data and the data type can be determined then */
format = glrformat(internal);
assert(format > 0);
type = glrtype(type);
assert(type > 0);
```

This library is prerelease and not fully tested as yet; the repository
will be updated when it is so.

## Prerequisites

Headers in build path are required:

- `#include <GL/gl.h>`
- `#include <GL/glew.h>`

Build tools:

- awk
- ninja or [samurai](https://github.com/michaelforney/samurai)

## TODO

- Add simple image-loading example
- Support gathering constants from other sources other than GLEW
- More testing required 
- Compressed OpenGL textures

## License

This is released under the MIT license; see [LICENSE](https://github.com/raedwulf/libglr/tree/master/LICENSE).
A list of copyright holders is in the repository in [AUTHORS](https://github.com/raedwulf/libglr/tree/master/AUTHORS).
