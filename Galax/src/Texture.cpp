#include "Texture.h"
#include "stb/stb_image.h"
#include "core/Log.h"

Texture::Texture(const char* m_path) {
	// Load Texture
	bool flip = true;

	stbi_set_flip_vertically_on_load(flip);

	unsigned char* bytes = stbi_load(m_path, &width, &height, &numColCh, 0);

	if (bytes == nullptr) {
		GX_ERROR("Texture bytes were null for path {}", m_path);
		return;
	}

	// if (!bytes) { LX_CORE_ERROR("Texture Loading failed (bytes null): {}", m_path.string()); return; }

	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Creation crashes if pixel allignment needs to be divisible by 4

	GLenum colorMode = GL_RGB;
	switch (numColCh)
	{
	case 1:
		colorMode = GL_RED;
		break;
	case 2:
		colorMode = GL_RG;
		break;
	case 3:
		colorMode = GL_RGB;
		break;
	case 4:
		colorMode = GL_RGBA;
		break;
	}

	if (bytes) {
		glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, bytes);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Which filter type?
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		GX_ERROR("Failed to load image: {}", m_path);
		return;
	}
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	path = m_path;

	// Reset flip vertically on load since the next might not want to
	stbi_set_flip_vertically_on_load(false);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, ID);
}