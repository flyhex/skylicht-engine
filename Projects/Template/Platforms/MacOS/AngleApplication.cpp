//
// Copyright 2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "AngleApplication.h"

#include "util/EGLWindow.h"

#include <string.h>
#include <iostream>
#include <utility>

namespace
{
const char *kUseAngleArg = "--use-angle=";

using DisplayTypeInfo = std::pair<const char *, EGLint>;

const DisplayTypeInfo kDisplayTypes[] = {
    {"d3d9", EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE},
    {"d3d11", EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE},
    {"gl", EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE},
    {"gles", EGL_PLATFORM_ANGLE_TYPE_OPENGLES_ANGLE},
    {"metal", EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE},
    {"null", EGL_PLATFORM_ANGLE_TYPE_NULL_ANGLE},
    {"swiftshader", EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE},
    {"vulkan", EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE},
};

EGLint GetDisplayTypeFromArg(const char *displayTypeArg)
{
    for (const auto &displayTypeInfo : kDisplayTypes)
    {
        if (strcmp(displayTypeInfo.first, displayTypeArg) == 0)
        {
            std::cout << "Using ANGLE back-end API: " << displayTypeInfo.first << std::endl;
            return displayTypeInfo.second;
        }
    }

    std::cout << "Unknown ANGLE back-end API: " << displayTypeArg << std::endl;
    return EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE;
}

EGLint GetDeviceTypeFromArg(const char *displayTypeArg)
{
    if (strcmp(displayTypeArg, "swiftshader") == 0)
    {
        return EGL_PLATFORM_ANGLE_DEVICE_TYPE_SWIFTSHADER_ANGLE;
    }
    else
    {
        return EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE;
    }
}

}  // anonymous namespace

AngleApplication::AngleApplication(std::string name,
                                     int argc,
                                     char **argv,
                                     EGLint glesMajorVersion,
                                     EGLint glesMinorVersion,
                                     uint32_t width,
                                     uint32_t height)
    : mName(std::move(name)),
      mWidth(width),
      mHeight(height),
      mRunning(false),
      mEGLWindow(nullptr),
      mOSWindow(nullptr)
{
    mPlatformParams.renderer = EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE;
    mPlatformParams.deviceType = EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE;
    
    if (argc > 1 && strncmp(argv[1], kUseAngleArg, strlen(kUseAngleArg)) == 0)
    {
        const char *arg            = argv[1] + strlen(kUseAngleArg);
        mPlatformParams.renderer   = GetDisplayTypeFromArg(arg);
        mPlatformParams.deviceType = GetDeviceTypeFromArg(arg);
    }

    // Load EGL library so we can initialize the display.
    mEntryPointsLib.reset(angle::OpenSharedLibrary(ANGLE_EGL_LIBRARY_NAME, angle::SearchType::SystemDir));
    
    mEGLWindow = EGLWindow::New(glesMajorVersion, glesMinorVersion);
    mOSWindow  = OSWindow::New();
}

AngleApplication::~AngleApplication()
{
    EGLWindow::Delete(&mEGLWindow);
    OSWindow::Delete(&mOSWindow);
}

bool AngleApplication::initialize()
{
    return true;
}

void AngleApplication::destroy() {}

void AngleApplication::step(float dt, double totalTime) {}

void AngleApplication::draw()
{
    // Set the viewport
    int w = getWindow()->getWidth();
    int h = getWindow()->getHeight();
    glViewport(0, 0, w, h);

    // Clear the color buffer
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void AngleApplication::swap()
{
    mEGLWindow->swap();
}

OSWindow *AngleApplication::getWindow() const
{
    return mOSWindow;
}

EGLConfig AngleApplication::getConfig() const
{
    return mEGLWindow->getConfig();
}

EGLDisplay AngleApplication::getDisplay() const
{
    return mEGLWindow->getDisplay();
}

EGLSurface AngleApplication::getSurface() const
{
    return mEGLWindow->getSurface();
}

EGLContext AngleApplication::getContext() const
{
    return mEGLWindow->getContext();
}

int AngleApplication::run()
{
    if (!mOSWindow->initialize(mName, mWidth, mHeight))
    {
        return -1;
    }

    mOSWindow->setVisible(true);

    ConfigParameters configParams;
    configParams.redBits     = 8;
    configParams.greenBits   = 8;
    configParams.blueBits    = 8;
    configParams.alphaBits   = 8;
    configParams.depthBits   = 24;
    configParams.stencilBits = 8;

    if (!mEGLWindow->initializeGL(mOSWindow, mEntryPointsLib.get(), mPlatformParams, configParams))
    {
        return -1;
    }

    // Disable vsync
    if (!mEGLWindow->setSwapInterval(0))
    {
        return -1;
    }
    
    mRunning   = true;
    int result = 0;

    if (!initialize())
    {
        mRunning = false;
        result   = -1;
    }

    mTimer.start();
    double prevTime = 0.0;

    while (mRunning)
    {
        // Clear events that the application did not process from this frame
        Event event;
        while (popEvent(&event))
        {
            // If the application did not catch a close event, close now
            switch (event.Type)
            {
                case Event::EVENT_CLOSED:
                    exit();
                    break;
                case Event::EVENT_KEY_RELEASED:
                    onKeyUp(event.Key);
                    break;
                case Event::EVENT_KEY_PRESSED:
                    onKeyDown(event.Key);
                    break;
                case Event::EVENT_MOUSE_MOVED:
                    onMouseMoved(event.MouseMove);
                    break;
                case Event::EVENT_MOUSE_BUTTON_PRESSED:
                    onMouseButtonPressed(event.MouseButton);
                    break;
                case Event::EVENT_MOUSE_BUTTON_RELEASED:
                    onMouseButtonRelease(event.MouseButton);
                    break;
                case Event::EVENT_MOUSE_WHEEL_MOVED:
                    onWheel(event.MouseWheel);
                    break;
                default:
                    break;
            }
        }

        if (!mRunning)
        {
            break;
        }

        double elapsedTime = mTimer.getElapsedTime();
        double deltaTime   = elapsedTime - prevTime;

        step(static_cast<float>(deltaTime), elapsedTime);
        
        draw();
        swap();

        mOSWindow->messageLoop();

        prevTime = elapsedTime;
    }

    destroy();
    mEGLWindow->destroyGL();
    mOSWindow->destroy();

    return result;
}

void AngleApplication::exit()
{
    mRunning = false;
}

bool AngleApplication::popEvent(Event *event)
{
    return mOSWindow->popEvent(event);
}

void AngleApplication::onKeyUp(const Event::KeyEvent &keyEvent)
{
    // Default no-op.
}

void AngleApplication::onKeyDown(const Event::KeyEvent &keyEvent)
{
    // Default no-op.
}

void AngleApplication::onMouseMoved(const Event::MouseMoveEvent &mouseEvent)
{
    // Default no-op.
}

void AngleApplication::onMouseButtonPressed(const Event::MouseButtonEvent &mouseEvent)
{
    // Default no-op.
}

void AngleApplication::onMouseButtonRelease(const Event::MouseButtonEvent &mouseEvent)
{
    // Default no-op.
}

void AngleApplication::onWheel(const Event::MouseWheelEvent &wheelEvent)
{
    // Default no-op.
}