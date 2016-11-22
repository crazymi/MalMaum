/* Code from computer graphics SphereMappingSrc.zip */

#include <fstream>
#include "texture.h"
#include "lodepng.h"

void initPNG(GLuint *tex, const char *name, int &width, int &height) {
	unsigned char* buffer;
	unsigned char* image;
	size_t buffersize, imagesize;
	LodePNG_Decoder decoder;

	LodePNG_Decoder_init(&decoder);

	LodePNG_loadFile(&buffer, &buffersize, name); /*load the image file with given filename*/
	if (!buffer || buffersize <= 0) {
		printf("[PNG error] Couldn't open file\n");
		return;
	}
	LodePNG_Decoder_decode(&decoder, &image, &imagesize, buffer, buffersize); /*decode the png*/

	glDeleteTextures(1, tex);
	glGenTextures(1, tex);

	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, decoder.infoPng.width,
		decoder.infoPng.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		image);

		width = decoder.infoPng.width;
	height = decoder.infoPng.height;
	//printf("[PNG open] %s width: %d height: %d\n", name, width, height);
	free(buffer);
	free(image);
}