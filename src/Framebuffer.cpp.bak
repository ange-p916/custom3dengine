#include "MyFirstEngine/Framebuffer.h"
#include <iostream> // For std::cerr

Framebuffer::Framebuffer(int width, int height)
    : fboID(0), colorTextureID(0), depthRenderbufferID(0), fboWidth(width), fboHeight(height) {
    if (fboWidth <= 0 || fboHeight <= 0) {
        std::cerr << "Framebuffer Warning: Initializing with non-positive dimensions ("
                  << fboWidth << "x" << fboHeight << "). Will not create attachments yet." << std::endl;
        // Don't create attachments if dimensions are invalid initially.
        // Resize will handle creation when valid dimensions are provided.
        return;
    }
    createAttachments();
}

Framebuffer::~Framebuffer() {
    deleteAttachments();
}

void Framebuffer::createAttachments() {
    // Delete existing attachments if any, to prevent leaks on resize
    deleteAttachments();

    if (fboWidth <= 0 || fboHeight <= 0) {
        // std::cerr << "Framebuffer Error: Cannot create attachments with invalid dimensions ("
        //           << fboWidth << "x" << fboHeight << ")" << std::endl;
        return; // Don't proceed if dimensions are invalid
    }

    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    // Create color texture attachment
    glGenTextures(1, &colorTextureID);
    glBindTexture(GL_TEXTURE_2D, colorTextureID);
    // Using GL_RGBA for more flexibility, though GL_RGB might be slightly more performant if alpha isn't needed
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevent edge artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureID, 0);

    // Create depth/stencil renderbuffer attachment
    glGenRenderbuffers(1, &depthRenderbufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind renderbuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRenderbufferID);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer Error: Framebuffer is not complete! Status: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        deleteAttachments(); // Clean up on failure
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO
}

void Framebuffer::deleteAttachments() {
    if (fboID != 0) {
        glDeleteFramebuffers(1, &fboID);
        fboID = 0;
    }
    if (colorTextureID != 0) {
        glDeleteTextures(1, &colorTextureID);
        colorTextureID = 0;
    }
    if (depthRenderbufferID != 0) {
        glDeleteRenderbuffers(1, &depthRenderbufferID);
        depthRenderbufferID = 0;
    }
}

void Framebuffer::bind() {
    if (fboID == 0 && (fboWidth > 0 && fboHeight > 0)) {
        // Attempt to create if not yet created (e.g., if initialized with 0x0 dimensions)
        createAttachments();
    }
    if (fboID == 0) return; // Still couldn't create

    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport(0, 0, fboWidth, fboHeight); // Set viewport to FBO's dimensions
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // The caller should reset the viewport to the main window's dimensions after this
}

void Framebuffer::resize(int width, int height) {
    if (this->fboWidth == width && this->fboHeight == height) {
        return; // No change needed
    }

    this->fboWidth = width;
    this->fboHeight = height;

    if (fboWidth <= 0 || fboHeight <= 0) {
        deleteAttachments(); // Delete if dimensions become invalid
        return;
    }
    
    createAttachments(); // Recreate with new dimensions
}