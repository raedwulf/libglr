/* See LICENSE file for copyright and license details. */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>

#include "glr.h"

int glrmatch(const char *s, size_t l);
int glrnparsebase(const char *filename, size_t filenamelen,
                  const char **endname, int *x, int *y);

int
glrparse(const char *filename,
         const char **endname, int *x, int *y)
{
	return glrnparsebase(filename, strlen(filename), endname, x, y);
}

int
glrnparse(const char *filename, size_t filenamelen,
          const char **endname, int *x, int *y)
{
	return glrnparsebase(filename, strnlen(filename, filenamelen),
	                     endname, x, y);
}

int
glrnparsebase(const char *filename, size_t filenamelen,
              const char **endname, int *x, int *y)
{
	const char *ld, *p, *pp, *opp;
	const char *e;
	int xdim, ydim, result;

	/* recall format is either:
	 * - format
	 * - #.format
	 * - #x#.format
	 * - name.#.format
	 * - name.#x#.format
	 * - name.format
	 *
	 * where # is the both x & y dimension and
         * #x# is x & y dimensions respectively delimited by 'x'
	 *
	 * unspecified dimensions will ret -1 in their respective parameters;
	 */
	e = filename + filenamelen;
	ld = strrchr(filename, '.');

	/* match the format string */
	if (!ld) /* no '.', only format */
		return glrmatch(filename, e - filename);
	else
		result = glrmatch(ld + 1, e - ld);

	/* parse match early so we do not write to pointed dimensions or
	 * endname on error */
	if (result < 0) return result;

	/* if we know that the filename isn't just 'format', there might
	 * be dimensions */
	if (ld) {
		/* scan backwards */
		p = ld - 1;
		for (;;) {
			if (p < filename || *p == '.') break;
			--p;
		}
		p++;
		opp = pp = p;
		/* try and parse the x dimension */
		xdim = ydim = 0;
		while (p < ld && (*p != 'x' || *p != '.') && isdigit(*p)) {
			xdim *= 10; xdim += (*p - '0');
			p++;
		}
		/* check if parsing made progress, otherwise it terminated
		 * early and x & y should not contain valid input */
		if (p == pp) {
			if (x) *x = GLR_DIM_NONE;
			if (y) *y = GLR_DIM_NONE;
			if (endname) *endname = ld;
		} else {
			/* three possible options: has 2 dimensions, or
			 * assume square, or not dimensions */
			if (*p == 'x') {
				/* try and parse the y dimension */
				pp = ++p;
				while (p < ld && (*p != '.') && isdigit(*p)) {
					ydim *= 10; ydim += (*p - '0');
					p++;
				}
				/* check if parsing made progress, otherwise
				 * it terminated early and x & y
				 * should not contain valid input or, 
				 * matched [0-9]+x[0-9]* pattern, but hit a
				 * bad char, ignore both dim */
				if (*p != '.' || p == pp) {
					if (x) *x = GLR_DIM_INVALID;
					if (y) *y = GLR_DIM_INVALID;
					if (endname) *endname = ld;
				} else {
					/* return the dimensions in passed
					 * arguments */
					if (x) *x = xdim;
					if (y) *y = ydim;
					if (endname) *endname = opp - 1;
				}
			} else if (*p == '.') {
				/* copy the x dimensions to y
				 * (assume square textures) */
				if (x) *x = xdim;
				if (y) *y = xdim;
				if (endname) *endname = opp - 1;
			} else {
				if (x) *x = GLR_DIM_INVALID;
				if (y) *y = GLR_DIM_INVALID;
				if (endname) *endname = ld;
			}
		}
	} else {
		/* for the filenames that are just 'format' */
		if (x) *x = GLR_DIM_NONE;
		if (y) *y = GLR_DIM_NONE;
		/* name just points to the start of the string */
		if (endname) *endname = filename;
	}

	return result;
}

static inline int
stdtypebits(int type)
{
	switch (type) {
	case GL_UNSIGNED_BYTE:
	case GL_BYTE:
		return 8;
	case GL_HALF_FLOAT:
	case GL_UNSIGNED_SHORT:
	case GL_SHORT:
		return 16;
	case GL_FLOAT:
	case GL_UNSIGNED_INT:
	case GL_INT:
		return 32;
	default:
		return -1;
	}
}

int
glrcomponents(int format)
{
	switch (format) {
	case GL_RGBA:
	case GL_RGBA_INTEGER:
		return 4;
	case GL_RGB:
	case GL_RGB_INTEGER:
		return 3;
	case GL_RG:
	case GL_RG_INTEGER:
	case GL_DEPTH_STENCIL:
		return 2;
	case GL_RED:
	case GL_RED_INTEGER:
	case GL_DEPTH_COMPONENT:
		return 1;
	default:
		return -1;
	}
}

ssize_t
glrpixels(int internal, int filesize)
{
	int type, format, bits, bytes, rem;

	type = glrtype(internal);
	if (type < 0) return -1;
	format = glrformat(internal);
	if (format < 0) return -1;
	bits = glrbits(type, format);
	if (bits < 0) return -1;
	bytes = (bits >> 3);
	rem = filesize % bytes;
	if (rem) return -1;

	return filesize / bytes;
}

int
glrbits(int format, int type)
{
	int typebits = -1;
	switch (type) {
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_6_5:
			typebits = 16;
			break;
		case GL_UNSIGNED_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_10F_11F_11F_REV:
		case GL_UNSIGNED_INT_5_9_9_9_REV:
		case GL_UNSIGNED_INT_24_8:
			typebits = 32;
			break;
		case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
			typebits = 64;
			break;
		default:
			typebits = stdtypebits(type);
			if (typebits < 0) return -1;
			typebits *= glrcomponents(format);
			if (typebits < 0) return -1;
	}
	return typebits;
}

#ifdef __GNUC__
#define LOG2(x) __builtin_ctz(x)
#else
/* from https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup */
static const int muldebruijnbitpos2[] = {
	0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
	31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};
#define LOG2(x) muldebruijnbitpos2[(uint32_t)(x * 0x077CB531U) >> 27]
#endif

int
glrpotdim(int pixels)
{
	int pot;

	pot = LOG2(pixels);
	if (pot * pot == pixels)
		return pot;
	return -1;
}
