/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>

#include <GL/gl.h>
#include <GL/glext.h>

#include "../glr.h"

void
testglrparse(const char *input, int expected)
{
	char buffer[64];
	const char *endname = NULL;
	int result, x, y;

	result = glrparse(input, &endname, &x, &y);
	if (endname) {
		if ((endname - input) < (sizeof(buffer) - 1)) {
			memcpy(buffer, input, endname - input);
			buffer[endname - input] = '\0';
		} else {
			strcpy(buffer, "TOO LONG");
		}
	} else {
		strcpy(buffer, "NULL");
	}
	printf("glrparse;%s;0x%4x;0x%4x;%s;%d;%d\n", input, expected, result, buffer, x, y);
}

int
main() 
{
	/* simple tests for valid input */
	testglrparse("simple.rgba8",               GL_RGBA8);
	testglrparse("simple.srgb8_alpha8",        GL_SRGB8_ALPHA8);
	testglrparse("simple.rgba8_snorm",         GL_RGBA8_SNORM);
	testglrparse("simple.rgba4",               GL_RGBA4);
	testglrparse("simple.rgb10_a2",            GL_RGB10_A2);
	testglrparse("simple.rgb5_a1",             GL_RGB5_A1);
	testglrparse("simple.rgba16f",             GL_RGBA16F);
	testglrparse("simple.rgba32f",             GL_RGBA32F);
	testglrparse("simple.rgba8ui",             GL_RGBA8UI);
	testglrparse("simple.rgba8i",              GL_RGBA8I);
	testglrparse("simple.rgba16ui",            GL_RGBA16UI);
	testglrparse("simple.rgba16i",             GL_RGBA16I);
	testglrparse("simple.rgba32ui",            GL_RGBA32UI);
	testglrparse("simple.rgba32i",             GL_RGBA32I);
	testglrparse("simple.rgb10_a2ui",          GL_RGB10_A2UI);
	testglrparse("simple.rgb8",                GL_RGB8);
	testglrparse("simple.srgb8",               GL_SRGB8);
	testglrparse("simple.rgb8_snorm",          GL_RGB8_SNORM);
	testglrparse("simple.rgb565",              GL_RGB565);
	testglrparse("simple.r11f_g11f_b10f",      GL_R11F_G11F_B10F);
	testglrparse("simple.rgb9_e5",             GL_RGB9_E5);
	testglrparse("simple.rgb16f",              GL_RGB16F);
	testglrparse("simple.rgb32f",              GL_RGB32F);
	testglrparse("simple.rgb8ui",              GL_RGB8UI);
	testglrparse("simple.rgb8i",               GL_RGB8I);
	testglrparse("simple.rgb16ui",             GL_RGB16UI);
	testglrparse("simple.rgb16i",              GL_RGB16I);
	testglrparse("simple.rgb32ui",             GL_RGB32UI);
	testglrparse("simple.rgb32i",              GL_RGB32I);
	testglrparse("simple.rg8",                 GL_RG8);
	testglrparse("simple.rg8_snorm",           GL_RG8_SNORM);
	testglrparse("simple.rg16f",               GL_RG16F);
	testglrparse("simple.rg32f",               GL_RG32F);
	testglrparse("simple.rg8ui",               GL_RG8UI);
	testglrparse("simple.rg8i",                GL_RG8I);
	testglrparse("simple.rg16ui",              GL_RG16UI);
	testglrparse("simple.rg16i",               GL_RG16I);
	testglrparse("simple.rg32ui",              GL_RG32UI);
	testglrparse("simple.rg32i",               GL_RG32I);
	testglrparse("simple.r8",                  GL_R8);
	testglrparse("simple.r8_snorm",            GL_R8_SNORM);
	testglrparse("simple.r16f",                GL_R16F);
	testglrparse("simple.r32f",                GL_R32F);
	testglrparse("simple.r8ui",                GL_R8UI);
	testglrparse("simple.r8i",                 GL_R8I);
	testglrparse("simple.r16ui",               GL_R16UI);
	testglrparse("simple.r16i",                GL_R16I);
	testglrparse("simple.r32ui",               GL_R32UI);
	testglrparse("simple.r32i",                GL_R32I);
	testglrparse("simple.depth_component16",   GL_DEPTH_COMPONENT16);
	testglrparse("simple.depth_component24",   GL_DEPTH_COMPONENT24);
	testglrparse("simple.depth_component32f",  GL_DEPTH_COMPONENT32F);
	testglrparse("simple.depth24_stencil8",    GL_DEPTH24_STENCIL8);
	testglrparse("simple.depth32f_stencil8",   GL_DEPTH32F_STENCIL8);

	/* parse dimensions */
	testglrparse("pimple.32x32.r8_snorm",      GL_R8_SNORM);
	testglrparse("pimple.64.r32ui",            GL_R32UI);
	testglrparse("pimple.29.r32ui",            GL_R32UI);
	testglrparse("pim.ple.29.r32ui",           GL_R32UI);
	testglrparse("pim.ple.29.r32ui",           GL_R32UI);
	testglrparse("32x32.r8_snorm",             GL_R8_SNORM);
	testglrparse("32x32.r8_snorm",             GL_R8_SNORM);

	/* invalid input */
	testglrparse("thistringaintexisting", -1);
	testglrparse("thistringain.adss", -1);
	testglrparse("thitr.esting.8dss", -1);
	testglrparse("verylongstringshouldautomaticallybeignored", -1);
	testglrparse("verylongstringshouldautomati.allybeignored", -1);
	testglrparse("pimple.32x32aa.r8_snorm",  GL_R8_SNORM);
	testglrparse("pimple.32x.r8_snorm",      GL_R8_SNORM);
	testglrparse("pimple.32x32.x.r8_snorm",  GL_R8_SNORM);
	testglrparse("pimple.32x32.x.r8_snorm",  GL_R8_SNORM);
	testglrparse("32x32.x.r8_snorm",         GL_R8_SNORM);
	testglrparse("32x32.x.r8_snorm",         GL_R8_SNORM);

}
