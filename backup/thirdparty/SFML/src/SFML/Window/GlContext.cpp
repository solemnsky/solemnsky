////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/GlContext.hpp>
#include <SFML/System/ThreadLocalPtr.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/OpenGL.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include <cstdlib>
#include <cstring>
#include <cassert>

#if !defined(SFML_OPENGL_ES)

    #if defined(SFML_SYSTEM_WINDOWS)

        #include <SFML/Window/Win32/WglContext.hpp>
        typedef sf::priv::WglContext ContextType;

    #elif defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD)

        #include <SFML/Window/Unix/GlxContext.hpp>
        typedef sf::priv::GlxContext ContextType;

    #elif defined(SFML_SYSTEM_MACOS)

        #include <SFML/Window/OSX/SFContext.hpp>
        typedef sf::priv::SFContext ContextType;

    #endif

#else

    #if defined(SFML_SYSTEM_IOS)

        #include <SFML/Window/iOS/EaglContext.hpp>
        typedef sf::priv::EaglContext ContextType;

    #else

        #include <SFML/Window/EglContext.hpp>
        typedef sf::priv::EglContext ContextType;

    #endif

#endif

#if defined(SFML_SYSTEM_WINDOWS)

    typedef const GLubyte* (APIENTRY *glGetStringiFuncType)(GLenum, GLuint);

#else

    typedef const GLubyte* (*glGetStringiFuncType)(GLenum, GLuint);

#endif

#if !defined(GL_MULTISAMPLE)
    #define GL_MULTISAMPLE 0x809D
#endif

#if !defined(GL_MAJOR_VERSION)
    #define GL_MAJOR_VERSION 0x821B
#endif

#if !defined(GL_MINOR_VERSION)
    #define GL_MINOR_VERSION 0x821C
#endif

#if !defined(GL_NUM_EXTENSIONS)
    #define GL_NUM_EXTENSIONS 0x821D
#endif

#if !defined(GL_CONTEXT_FLAGS)
    #define GL_CONTEXT_FLAGS 0x821E
#endif

#if !defined(GL_CONTEXT_FLAG_DEBUG_BIT)
    #define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#endif

#if !defined(GL_CONTEXT_PROFILE_MASK)
    #define GL_CONTEXT_PROFILE_MASK 0x9126
#endif

#if !defined(GL_CONTEXT_CORE_PROFILE_BIT)
    #define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#endif

#if !defined(GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
    #define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#endif


namespace
{
    // AMD drivers have issues with internal synchronization
    // We need to make sure that no operating system context
    // or pixel format operations are performed simultaneously
    // This mutex is also used to protect the shared context
    // from being locked on multiple threads
    sf::Mutex mutex;

    // This per-thread variable holds the current context for each thread
    sf::ThreadLocalPtr<sf::priv::GlContext> currentContext(NULL);

    // The hidden, inactive context that will be shared with all other contexts
    ContextType* sharedContext = NULL;

    // This per-thread variable is set to point to the shared context
    // if we had to acquire it when a TransientContextLock was required
    sf::ThreadLocalPtr<sf::priv::GlContext> currentSharedContext(NULL);

    // Supported OpenGL extensions
    std::vector<std::string> extensions;
}


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
void GlContext::globalInit()
{
    Lock lock(mutex);

    if (sharedContext)
        return;

    // Create the shared context
    sharedContext = new ContextType(NULL);
    sharedContext->initialize();

    // Load our extensions vector
    extensions.clear();

    // Check whether a >= 3.0 context is available
    int majorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);

    if (glGetError() == GL_INVALID_ENUM)
    {
        // Try to load the < 3.0 way
        const char* extensionString = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

        do
        {
            const char* extension = extensionString;

            while(*extensionString && (*extensionString != ' '))
                extensionString++;

            extensions.push_back(std::string(extension, extensionString));
        }
        while (*extensionString++);
    }
    else
    {
        // Try to load the >= 3.0 way
        glGetStringiFuncType glGetStringiFunc = NULL;
        glGetStringiFunc = reinterpret_cast<glGetStringiFuncType>(getFunction("glGetStringi"));

        if (glGetStringiFunc)
        {
            int numExtensions = 0;
            glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

            if (numExtensions)
            {
                for (unsigned int i = 0; i < static_cast<unsigned int>(numExtensions); ++i)
                {
                    const char* extensionString = reinterpret_cast<const char*>(glGetStringiFunc(GL_EXTENSIONS, i));

                    extensions.push_back(extensionString);
                }
            }
        }
    }

    // Deactivate the shared context so that others can activate it when necessary
    sharedContext->setActive(false);
}


////////////////////////////////////////////////////////////
void GlContext::globalCleanup()
{
    Lock lock(mutex);

    if (!sharedContext)
        return;

    // Destroy the shared context
    delete sharedContext;
    sharedContext = NULL;
}


////////////////////////////////////////////////////////////
void GlContext::acquireTransientContext()
{
    // If a capable context is already active on this thread
    // there is no need to use the shared context for the operation
    if (currentContext)
    {
        currentSharedContext = NULL;
        return;
    }

    mutex.lock();
    currentSharedContext = sharedContext;
    sharedContext->setActive(true);
}


////////////////////////////////////////////////////////////
void GlContext::releaseTransientContext()
{
    if (!currentSharedContext)
        return;

    sharedContext->setActive(false);
    mutex.unlock();
}


////////////////////////////////////////////////////////////
GlContext* GlContext::create()
{
    // Make sure that there's an active context (context creation may need extensions, and thus a valid context)
    assert(sharedContext != NULL);

    Lock lock(mutex);

    GlContext* context = NULL;

    // We don't use acquireTransientContext here since we have
    // to ensure we have exclusive access to the shared context
    // in order to make sure it is not active during context creation
    {
        sharedContext->setActive(true);

        // Create the context
        context = new ContextType(sharedContext);

        sharedContext->setActive(false);
    }

    context->initialize();

    return context;
}


////////////////////////////////////////////////////////////
GlContext* GlContext::create(const ContextSettings& settings, const WindowImpl* owner, unsigned int bitsPerPixel)
{
    // Make sure that there's an active context (context creation may need extensions, and thus a valid context)
    assert(sharedContext != NULL);

    Lock lock(mutex);

    GlContext* context = NULL;

    // We don't use acquireTransientContext here since we have
    // to ensure we have exclusive access to the shared context
    // in order to make sure it is not active during context creation
    {
        sharedContext->setActive(true);

        // Create the context
        context = new ContextType(sharedContext, settings, owner, bitsPerPixel);

        sharedContext->setActive(false);
    }

    context->initialize();
    context->checkSettings(settings);

    return context;
}


////////////////////////////////////////////////////////////
GlContext* GlContext::create(const ContextSettings& settings, unsigned int width, unsigned int height)
{
    // Make sure that there's an active context (context creation may need extensions, and thus a valid context)
    assert(sharedContext != NULL);

    Lock lock(mutex);

    GlContext* context = NULL;

    // We don't use acquireTransientContext here since we have
    // to ensure we have exclusive access to the shared context
    // in order to make sure it is not active during context creation
    {
        sharedContext->setActive(true);

        // Create the context
        context = new ContextType(sharedContext, settings, width, height);

        sharedContext->setActive(false);
    }

    context->initialize();
    context->checkSettings(settings);

    return context;
}


////////////////////////////////////////////////////////////
bool GlContext::isExtensionAvailable(const char* name)
{
    return std::find(extensions.begin(), extensions.end(), name) != extensions.end();
}


////////////////////////////////////////////////////////////
GlFunctionPointer GlContext::getFunction(const char* name)
{
#if !defined(SFML_OPENGL_ES)

    Lock lock(mutex);

    return ContextType::getFunction(name);

#else

    return 0;

#endif
}


////////////////////////////////////////////////////////////
GlContext::~GlContext()
{
    // Deactivate the context before killing it, unless we're inside Cleanup()
    if (sharedContext)
    {
        if (this == currentContext)
            currentContext = NULL;
    }
}


////////////////////////////////////////////////////////////
const ContextSettings& GlContext::getSettings() const
{
    return m_settings;
}


////////////////////////////////////////////////////////////
bool GlContext::setActive(bool active)
{
    if (active)
    {
        if (this != currentContext)
        {
            Lock lock(mutex);

            // Activate the context
            if (makeCurrent(true))
            {
                // Set it as the new current context for this thread
                currentContext = this;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            // This context is already the active one on this thread, don't do anything
            return true;
        }
    }
    else
    {
        if (this == currentContext)
        {
            Lock lock(mutex);

            // Deactivate the context
            if (makeCurrent(false))
            {
                currentContext = NULL;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            // This context is not the active one on this thread, don't do anything
            return true;
        }
    }
}


////////////////////////////////////////////////////////////
GlContext::GlContext()
{
    // Nothing to do
}


////////////////////////////////////////////////////////////
int GlContext::evaluateFormat(unsigned int bitsPerPixel, const ContextSettings& settings, int colorBits, int depthBits, int stencilBits, int antialiasing, bool accelerated)
{
    int colorDiff        = static_cast<int>(bitsPerPixel)               - colorBits;
    int depthDiff        = static_cast<int>(settings.depthBits)         - depthBits;
    int stencilDiff      = static_cast<int>(settings.stencilBits)       - stencilBits;
    int antialiasingDiff = static_cast<int>(settings.antialiasingLevel) - antialiasing;

    // Weight sub-scores so that better settings don't score equally as bad as worse settings
    colorDiff        *= ((colorDiff        > 0) ? 100000 : 1);
    depthDiff        *= ((depthDiff        > 0) ? 100000 : 1);
    stencilDiff      *= ((stencilDiff      > 0) ? 100000 : 1);
    antialiasingDiff *= ((antialiasingDiff > 0) ? 100000 : 1);

    // Aggregate the scores
    int score = std::abs(colorDiff) + std::abs(depthDiff) + std::abs(stencilDiff) + std::abs(antialiasingDiff);

    // Make sure we prefer hardware acceleration over features
    if (!accelerated)
        score += 100000000;

    return score;
}


////////////////////////////////////////////////////////////
void GlContext::initialize()
{
    // Activate the context
    setActive(true);

    // Retrieve the context version number
    int majorVersion = 0;
    int minorVersion = 0;

    // Try the new way first
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

    if (glGetError() != GL_INVALID_ENUM)
    {
        m_settings.majorVersion = static_cast<unsigned int>(majorVersion);
        m_settings.minorVersion = static_cast<unsigned int>(minorVersion);
    }
    else
    {
        // Try the old way
        const GLubyte* version = glGetString(GL_VERSION);
        if (version)
        {
            // The beginning of the returned string is "major.minor" (this is standard)
            m_settings.majorVersion = version[0] - '0';
            m_settings.minorVersion = version[2] - '0';
        }
        else
        {
            // Can't get the version number, assume 1.1
            m_settings.majorVersion = 1;
            m_settings.minorVersion = 1;
        }
    }

    // 3.0 contexts only deprecate features, but do not remove them yet
    // 3.1 contexts remove features if ARB_compatibility is not present
    // 3.2+ contexts remove features only if a core profile is requested

    // If the context was created with wglCreateContext, it is guaranteed to be compatibility.
    // If a 3.0 context was created with wglCreateContextAttribsARB, it is guaranteed to be compatibility.
    // If a 3.1 context was created with wglCreateContextAttribsARB, the compatibility flag
    // is set only if ARB_compatibility is present
    // If a 3.2+ context was created with wglCreateContextAttribsARB, the compatibility flag
    // would have been set correctly already depending on whether ARB_create_context_profile is supported.

    // If the user requests a 3.0 context, it will be a compatibility context regardless of the requested profile.
    // If the user requests a 3.1 context and its creation was successful, the specification
    // states that it will not be a compatibility profile context regardless of the requested
    // profile unless ARB_compatibility is present.

    m_settings.attributeFlags = ContextSettings::Default;

    if (m_settings.majorVersion >= 3)
    {
        // Retrieve the context flags
        int flags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
            m_settings.attributeFlags |= ContextSettings::Debug;

        if ((m_settings.majorVersion == 3) && (m_settings.minorVersion == 1))
        {
            m_settings.attributeFlags |= ContextSettings::Core;

            glGetStringiFuncType glGetStringiFunc = reinterpret_cast<glGetStringiFuncType>(getFunction("glGetStringi"));

            if (glGetStringiFunc)
            {
                int numExtensions = 0;
                glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

                for (unsigned int i = 0; i < static_cast<unsigned int>(numExtensions); ++i)
                {
                    const char* extensionString = reinterpret_cast<const char*>(glGetStringiFunc(GL_EXTENSIONS, i));

                    if (std::strstr(extensionString, "GL_ARB_compatibility"))
                    {
                        m_settings.attributeFlags &= ~static_cast<Uint32>(ContextSettings::Core);
                        break;
                    }
                }
            }
        }
        else if ((m_settings.majorVersion > 3) || (m_settings.minorVersion >= 2))
        {
            // Retrieve the context profile
            int profile = 0;
            glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);

            if (profile & GL_CONTEXT_CORE_PROFILE_BIT)
                m_settings.attributeFlags |= ContextSettings::Core;
        }
    }

    // Enable antialiasing if needed
    if (m_settings.antialiasingLevel > 0)
        glEnable(GL_MULTISAMPLE);
}


////////////////////////////////////////////////////////////
void GlContext::checkSettings(const ContextSettings& requestedSettings)
{
    // Perform checks to inform the user if they are getting a context they might not have expected

    // Detect any known non-accelerated implementations and warn
    const char* vendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const char* rendererName = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

    if (vendorName && rendererName)
    {
        if ((std::strcmp(vendorName, "Microsoft Corporation") == 0) && (std::strcmp(rendererName, "GDI Generic") == 0))
        {
            err() << "Warning: Detected \"Microsoft Corporation GDI Generic\" OpenGL implementation" << std::endl
                  << "The current OpenGL implementation is not hardware-accelerated" << std::endl;
        }
    }

    int version = m_settings.majorVersion * 10 + m_settings.minorVersion;
    int requestedVersion = requestedSettings.majorVersion * 10 + requestedSettings.minorVersion;

    if ((m_settings.attributeFlags    != requestedSettings.attributeFlags)    ||
        (version                      <  requestedVersion)           ||
        (m_settings.stencilBits       <  requestedSettings.stencilBits)       ||
        (m_settings.antialiasingLevel <  requestedSettings.antialiasingLevel) ||
        (m_settings.depthBits         <  requestedSettings.depthBits))
    {
        err() << "Warning: The created OpenGL context does not fully meet the settings that were requested" << std::endl;
        err() << "Requested: version = " << requestedSettings.majorVersion << "." << requestedSettings.minorVersion
              << " ; depth bits = " << requestedSettings.depthBits
              << " ; stencil bits = " << requestedSettings.stencilBits
              << " ; AA level = " << requestedSettings.antialiasingLevel
              << std::boolalpha
              << " ; core = " << ((requestedSettings.attributeFlags & ContextSettings::Core) != 0)
              << " ; debug = " << ((requestedSettings.attributeFlags & ContextSettings::Debug) != 0)
              << std::noboolalpha << std::endl;
        err() << "Created: version = " << m_settings.majorVersion << "." << m_settings.minorVersion
              << " ; depth bits = " << m_settings.depthBits
              << " ; stencil bits = " << m_settings.stencilBits
              << " ; AA level = " << m_settings.antialiasingLevel
              << std::boolalpha
              << " ; core = " << ((m_settings.attributeFlags & ContextSettings::Core) != 0)
              << " ; debug = " << ((m_settings.attributeFlags & ContextSettings::Debug) != 0)
              << std::noboolalpha << std::endl;
    }
}

} // namespace priv

} // namespace sf
