#include "MapRenderer_OpenGL.h"

#include <assert.h>

#include "Logging.h"

OsmAnd::MapRenderer_OpenGL::MapRenderer_OpenGL()
    : _textureSampler_Bitmap_NoAtlas(0)
    , _textureSampler_Bitmap_Atlas(0)
    , _textureSampler_ElevationData_NoAtlas(0)
    , _textureSampler_ElevationData_Atlas(0)
    , _maxAnisotropy(-1)
{
}

OsmAnd::MapRenderer_OpenGL::~MapRenderer_OpenGL()
{
}

GLenum OsmAnd::MapRenderer_OpenGL::validateResult()
{
    auto result = glGetError();
    if(result == GL_NO_ERROR)
        return result;

    LogPrintf(LogSeverityLevel::Error, "OpenGL error 0x%08x : %s\n", result, gluErrorString(result));

    return result;
}

void OsmAnd::MapRenderer_OpenGL::initializeRendering()
{
    MapRenderer_BaseOpenGL::initializeRendering();

    glewExperimental = GL_TRUE;
    glewInit();
    // For now, silence OpenGL error here, it's inside GLEW, so it's not ours
    (void)glGetError();
    //GL_CHECK_RESULT;

    const auto glVersionString = glGetString(GL_VERSION);
    GL_CHECK_RESULT;
    GLint glVersion[2];
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    GL_CHECK_RESULT;
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "Using OpenGL version %d.%d [%s]\n", glVersion[0], glVersion[1], glVersionString);
    assert(glVersion[0] >= 3);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<GLint*>(&_maxTextureSize));
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "OpenGL maximal texture size %dx%d\n", _maxTextureSize, _maxTextureSize);

    GLint maxTextureUnitsInFragmentShader;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnitsInFragmentShader);
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "OpenGL maximal texture units in fragment shader %d\n", maxTextureUnitsInFragmentShader);
    assert(maxTextureUnitsInFragmentShader >= (IMapRenderer::TileLayerId::IdsCount - IMapRenderer::RasterMap));

    GLint maxTextureUnitsInVertexShader;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxTextureUnitsInVertexShader);
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "OpenGL maximal texture units in vertex shader %d\n", maxTextureUnitsInVertexShader);
    assert(maxTextureUnitsInVertexShader >= IMapRenderer::RasterMap);

    GLint maxUniformsPerProgram;
    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniformsPerProgram);
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "OpenGL maximal parameter variables per program %d\n", maxUniformsPerProgram);

    if(GLEW_EXT_texture_filter_anisotropic)
    {
        GLfloat maxAnisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        GL_CHECK_RESULT;
        _maxAnisotropy = static_cast<int>(maxAnisotropy);
        LogPrintf(LogSeverityLevel::Info, "OpenGL anisotropic filtering: %dx max\n", _maxAnisotropy);
    }
    else
    {
        LogPrintf(LogSeverityLevel::Info, "OpenGL anisotropic filtering: not supported\n");
        _maxAnisotropy = -1;
    }

    // Bitmap (Atlas)
    glGenSamplers(1, &_textureSampler_Bitmap_Atlas);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_Bitmap_Atlas, GL_TEXTURE_WRAP_S, GL_CLAMP);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_Bitmap_Atlas, GL_TEXTURE_WRAP_T, GL_CLAMP);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_Bitmap_Atlas, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_Bitmap_Atlas, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CHECK_RESULT;
    if(_maxAnisotropy > 0)
    {
        glSamplerParameterf(_textureSampler_Bitmap_Atlas, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLfloat>(_maxAnisotropy));
        GL_CHECK_RESULT;
    }

    // ElevationData (Atlas)
    glGenSamplers(1, &_textureSampler_ElevationData_Atlas);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_ElevationData_Atlas, GL_TEXTURE_WRAP_S, GL_CLAMP);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_ElevationData_Atlas, GL_TEXTURE_WRAP_T, GL_CLAMP);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_ElevationData_Atlas, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_ElevationData_Atlas, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_CHECK_RESULT;

    // Bitmap (No atlas)
    glGenSamplers(1, &_textureSampler_Bitmap_NoAtlas);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_Bitmap_NoAtlas, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_Bitmap_NoAtlas, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_Bitmap_NoAtlas, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_Bitmap_NoAtlas, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CHECK_RESULT;
    if(_maxAnisotropy > 0)
    {
        glSamplerParameterf(_textureSampler_Bitmap_NoAtlas, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLfloat>(_maxAnisotropy));
        GL_CHECK_RESULT;
    }

    // ElevationData (No atlas)
    glGenSamplers(1, &_textureSampler_ElevationData_NoAtlas);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_ElevationData_NoAtlas, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_ElevationData_NoAtlas, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_ElevationData_NoAtlas, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL_CHECK_RESULT;
    glSamplerParameteri(_textureSampler_ElevationData_NoAtlas, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_CHECK_RESULT;

    glShadeModel(GL_SMOOTH);
    GL_CHECK_RESULT;

    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    GL_CHECK_RESULT;
    glEnable(GL_POLYGON_SMOOTH);
    GL_CHECK_RESULT;

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    GL_CHECK_RESULT;
    glEnable(GL_LINE_SMOOTH);
    GL_CHECK_RESULT;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    GL_CHECK_RESULT;

    glClearDepth(1.0f);
    GL_CHECK_RESULT;

    glEnable(GL_DEPTH_TEST);
    GL_CHECK_RESULT;

    glDepthFunc(GL_LEQUAL);
    GL_CHECK_RESULT;
}

void OsmAnd::MapRenderer_OpenGL::releaseRendering()
{
    if(_textureSampler_Bitmap_NoAtlas != 0)
    {
        glDeleteSamplers(1, &_textureSampler_Bitmap_NoAtlas);
        GL_CHECK_RESULT;
        _textureSampler_Bitmap_NoAtlas = 0;
    }

    if(_textureSampler_Bitmap_Atlas != 0)
    {
        glDeleteSamplers(1, &_textureSampler_Bitmap_Atlas);
        GL_CHECK_RESULT;
        _textureSampler_Bitmap_Atlas = 0;
    }

    if(_textureSampler_ElevationData_NoAtlas != 0)
    {
        glDeleteSamplers(1, &_textureSampler_ElevationData_NoAtlas);
        GL_CHECK_RESULT;
        _textureSampler_ElevationData_NoAtlas = 0;
    }

    if(_textureSampler_ElevationData_Atlas != 0)
    {
        glDeleteSamplers(1, &_textureSampler_ElevationData_Atlas);
        GL_CHECK_RESULT;
        _textureSampler_ElevationData_Atlas = 0;
    }

    MapRenderer_BaseOpenGL::releaseRendering();
}
