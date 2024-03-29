#include "OpenGLSystem.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Core/Application.h>
#include <Core/Log.h>
#include <Core/CoreAssert.h>
#include <OpenGLDebug.h>

#include <iostream>
#include <vector>
#include <memory>

using namespace Mani;

void OpenGLSystem::destroyWindow(WindowContext& context)
{
    if (context.window != nullptr)
    {
        glfwDestroyWindow(context.window);
        context.window = nullptr;
    }
}

void OpenGLSystem::terminate()
{
    destroyWindow(context);
    glfwTerminate();
}

// glfw callbacks begin
void OpenGLSystem::glfwCallback_onWindowClosed(GLFWwindow* window)
{
    if (auto* openGlSystem = (OpenGLSystem*)glfwGetWindowUserPointer(window))
    {
        openGlSystem->onWindowClosed.broadcast(openGlSystem->context);
        Application::get().stop();
    }
}

void OpenGLSystem::glfwCallback_onWindowResized(GLFWwindow* window, int newWidth, int newHeight)
{
    if (auto* openGlSystem = (OpenGLSystem*)glfwGetWindowUserPointer(window))
    {
        openGlSystem->context.width = newWidth;
        openGlSystem->context.height = newHeight;
    }

    glViewport(0, 0, newWidth, newHeight);
}
// glfw callbacks begin

std::string_view OpenGLSystem::getName() const
{
    return "OpenGLSystem";
}

bool OpenGLSystem::shouldTick(EntityRegistry& registry) const
{
    return true;
}

const OpenGLSystem::WindowContext& OpenGLSystem::getWindowContext() const
{
    return context;
}

std::shared_ptr<OpenGLInput> Mani::OpenGLSystem::getInputGenerator() const
{
    return m_openGLInputGenerator;
}

void OpenGLSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    SystemBase::onInitialize(registry, systemContainer);

    // initialize glfw
    if (!glfwInit())
    {
        MANI_LOG_ERROR(LogOpenGL, "failed to init glfw");
        return;
    }

    // set glfw context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef MANI_MACOSX
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // create the window
    context.window = glfwCreateWindow(context.width, context.height, context.name.data(), NULL, NULL);
    if (context.window == nullptr)
    {
        MANI_LOG_ERROR(LogOpenGL, "failed to create glfwwindow");
        terminate();
        return;
    }

    glfwGetWindowSize(context.window, &context.width, &context.height);
    glfwSetInputMode(context.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // set this as the window's user pointer. This allows us to retrieve this pointer from the window pointer provided in glfw's callbacks.
    glfwSetWindowUserPointer(context.window, this);
    glfwMakeContextCurrent(context.window);

    // set glfw callbacks
    glfwSetWindowCloseCallback(context.window, &OpenGLSystem::glfwCallback_onWindowClosed);
    glfwSetFramebufferSizeCallback(context.window, &OpenGLSystem::glfwCallback_onWindowResized);

    // init glew to load the correct opengl runtime
    GLenum result = glewInit();
    if (result != GLEW_OK)
    {
        MANI_LOG_ERROR(LogOpenGL, "failed to init glew");
        terminate();
        return;
    }

    // set the view port to the window's size.
    glViewport(0, 0, context.width, context.height);
    
#if MANI_OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);
#endif

    m_openGLInputGenerator = std::make_shared<OpenGLInput>(shared_from_this());
}

void OpenGLSystem::onDeinitialize(EntityRegistry& entityRegistry)
{
    SystemBase::onDeinitialize(entityRegistry);

    m_openGLInputGenerator.reset();

    terminate();
}

void OpenGLSystem::tick(float deltaTime, EntityRegistry& entityRegistry)
{
    glfwSwapBuffers(context.window);
    glfwPollEvents();
}