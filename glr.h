/* See LICENSE file for copyright and license details. */

enum {
	GLR_DIM_NONE = -1,
	GLR_DIM_INVALID = -2
};

int glrparse(const char * filename, const char **endname, int *x, int *y);
int glrnparse(const char * filename, size_t filenamelen, const char **endname, int *x, int *y);
ssize_t glrpixels(int internal, int filesize);
int glrcomponents(int format);
int glrtype(int internal);
int glrformat(int internal);
int glrbits(int format, int type);
int glrpotdim(int pixels);
