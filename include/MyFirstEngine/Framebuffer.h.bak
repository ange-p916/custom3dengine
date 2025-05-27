#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glad/glad.h" // For GLuint

class Framebuffer {
public:
    // Constructor: width and height of the framebuffer
    Framebuffer(int width, int height);
    ~Framebuffer();

    // Bind this FBO for rendering
    void bind();
    // Unbind FBO, reverting to default framebuffer (0)
    void unbind();

    // Resize the framebuffer and its attachments
    void resize(int width, int height);

    GLuint getColorTexture() const { return colorTextureID; }
    int getWidth() const { return fboWidth; }
    int getHeight() const { return fboHeight; }

private:
    void createAttachments(); // Helper to create/recreate texture and depth buffer
    void deleteAttachments(); // Helper to delete texture and depth buffer

    GLuint fboID;             // Framebuffer object ID
    GLuint colorTextureID;    // ID of the texture used as color attachment
    GLuint depthRenderbufferID; // ID of the renderbuffer used as depth/stencil attachment

    int fboWidth;
    int fboHeight;
};

#endif // FRAMEBUFFER_H