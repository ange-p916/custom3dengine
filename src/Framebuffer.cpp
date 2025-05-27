#include "MyFirstEngine/Framebuffer.h" // Adjust path as necessary
#include <iostream> // For std::cerr

Framebuffer::Framebuffer(int width, int height)
    : fboID(0), colorTextureID(0), depthRenderbufferID(0), fboWidth(width), fboHeight(height) {
    if (fboWidth <= 0 || fboHeight <= 0) {
        // Don't try to create if dimensions are invalid initially.
        // resize() will handle creation when valid dimensions are provided.
        // You could print a warning here if you like.
        // std::cerr << "Framebuffer Warning: Initialized with non-positive dimensions ("
        //           << fboWidth << "x" << fboHeight << "). Attachments not created." << std::endl;
        this->fboWidth = 1; // Set to minimal valid to avoid issues if bind is called before resize
        this->fboHeight = 1;
    }
    createAttachments();
}

Framebuffer::~Framebuffer() {
    deleteAttachments();
}

void Framebuffer::createAttachments() {
    // Delete existing attachments if any, to prevent leaks on resize/recreation
    deleteAttachments();

    if (fboWidth <= 0 || fboHeight <= 0) {
        // std::cerr << "Framebuffer Error: Cannot create attachments with invalid dimensions ("
        //           << fboWidth << "x" << fboHeight << ")." << std::endl;
        return; // Don't proceed if dimensions are invalid
    }

    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    // Create color texture attachment
    glGenTextures(1, &colorTextureID);
    glBindTexture(GL_TEXTURE_2D, colorTextureID);
    // Using GL_RGBA for more flexibility with ImGui, GL_RGB is also an option
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Common for FBO textures
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
    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer Error: Framebuffer is not complete! Status: 0x" << std::hex << fboStatus << std::endl;
        deleteAttachments(); // Clean up on failure
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO, reverting to default
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
    // If FBO wasn't created due to initial invalid size, try to create it now
    // (assuming fboWidth/Height might have been updated by resize)
    if (fboID == 0 && (fboWidth > 0 && fboHeight > 0)) {
        createAttachments();
    }
    
    if (fboID == 0) { // If still not created (e.g. dimensions still invalid)
        // std::cerr << "Framebuffer Warning: Attempting to bind an uninitialized or invalid FBO." << std::endl;
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport(0, 0, fboWidth, fboHeight); // Set viewport to FBO's dimensions
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // The caller is responsible for resetting the viewport to the main window's dimensions after this.
    // Typically done before rendering to the main window or before ImGui::Render().
}

void Framebuffer::resize(int width, int height) {
    // Only resize if dimensions actually changed AND are valid
    if ((this->fboWidth == width && this->fboHeight == height) && (fboID != 0) ) {
         // If dimensions are the same and FBO exists, no need to resize
        if (width > 0 && height > 0) return;
    }
    
    this->fboWidth = width;
    this->fboHeight = height;

    if (fboWidth <= 0 || fboHeight <= 0) {
        // std::cerr << "Framebuffer Warning: Resizing to invalid dimensions ("
        //           << fboWidth << "x" << fboHeight << "). Deleting attachments." << std::endl;
        deleteAttachments(); // Delete attachments if dimensions become invalid
        return;
    }
    
    createAttachments(); // Recreate with new dimensions
}