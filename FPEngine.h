#ifndef LAB11_LAB11_ENGINE_H
#define LAB11_LAB11_ENGINE_H

#include <CSCI441/FreeCam.hpp>
#include <CSCI441/ArcballCam.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>

#include "Sunfish.h"

class FPEngine : public CSCI441::OpenGLEngine {
public:
    //***************************************************************************
    // Engine Interface

    FPEngine();

    void run() final;

    //***************************************************************************
    // Event Handlers

    /// \desc handle any key events inside the engine
    /// \param key key as represented by GLFW_KEY_ macros
    /// \param action key event action as represented by GLFW_ macros
    void handleKeyEvent(GLint key, GLint action);

    /// \desc handle any mouse button events inside the engine
    /// \param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
    /// \param action mouse event as represented by GLFW_ macros
    void handleMouseButtonEvent(GLint button, GLint action);

    /// \desc handle any cursor movement events inside the engine
    /// \param currMousePosition the current cursor position
    void handleCursorPositionEvent(glm::vec2 currMousePosition);
    void handleCursorPositionEventFreeCam(glm::vec2 currMousePosition);

    /// \desc handle any scroll events inside the engine
    /// \param offset the current scroll offset
    void handleScrollEvent(glm::vec2 offset);

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

private:
    //***************************************************************************
    // Engine Setup

    void mSetupGLFW() final;
    void mSetupOpenGL() final;
    void mSetupShaders() final;
    void mSetupBuffers() final;
    void mSetupTextures() final;
    void mSetupScene() final;

    //***************************************************************************
    // Engine Cleanup

    void mCleanupScene() final;
    void mCleanupTextures() final;
    void mCleanupBuffers() final;
    void mCleanupShaders() final;

    //***************************************************************************
    // Engine Rendering & Updating

    /// \desc draws everything to the scene from a particular point of view
    /// \param viewMtx the current view matrix for our camera
    /// \param projMtx the current projection matrix for our camera
    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const;
    /// \desc handles moving our FreeCam as determined by keyboard input
    void _updateScene();

    //***************************************************************************
    // Input Tracking (Keyboard & Mouse)

    /// \desc tracks the number of different keys that can be present as determined by GLFW
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    /// \desc boolean array tracking each key state.  if true, then the key is in a pressed or held
    /// down state.  if false, then the key is in a released state and not being interacted with
    GLboolean _keys[NUM_KEYS];

    /// \desc last location of the mouse in window coordinates
    glm::vec2 _mousePosition;
    /// \desc current state of the left mouse button
    GLint _leftMouseButtonState;

    //***************************************************************************
    // Camera Information

    /// \desc the arcball camera in our world
    CSCI441::ArcballCam* _arcballCam;

    /// \desc the arcball camera in our world
    CSCI441::FreeCam* _pFreeCam;

    int _currentCamera = 1;
    glm::vec2 _cameraSpeed = glm::vec2(1.0,1.0);

    //***************************************************************************
    // VAO & Object Information

    /// \desc total number of VAOs in our scene
    static constexpr GLuint NUM_VAOS = 1;
    /// \desc used to index through our VAO/VBO/IBO array to give named access
    enum VAO_ID {
        /// \desc the ground plane
        PLATFORM = 0,
    };
    /// \desc VAO for our objects
    GLuint _vaos[NUM_VAOS];
    /// \desc VBO for our objects
    GLuint _vbos[NUM_VAOS];
    /// \desc IBO for our objects
    GLuint _ibos[NUM_VAOS];
    /// \desc the number of points that make up our VAO
    GLsizei _numVAOPoints[NUM_VAOS];

    /// \desc stores the particle system of fish

    Sunfish** _fish = nullptr;
    /// \desc the number of fish
    /// in the fish
    /// array
    GLuint _numFish;
    /// \desc the size of each of our fish

    GLfloat _fishRadius;
    /// \desc the size of our ground plane
    GLfloat _groundSize;

  //Boid control point
    GLfloat bcp_X = 0;
    GLfloat bcp_Z = 0; 

    void moveToBcp();
    void moveBCP(float side, float front);

    //***************************************************************************
    // Texture Information

    /// \desc total number of textures in our scene
    static constexpr GLuint NUM_TEXTURES = 9;
    /// \desc used to index through our texture handle array to give named access
    enum TEXTURES {
        /// \desc the ground plane
        PLATFORM_TEX = 0,
        /// \desc our fish
        /// spheres
        FISH_TEX = 1,
        POS_X = 2,
        NEG_X = 3,
        POS_Y = 4,
        NEG_Y = 5,
        POS_Z = 6,
        NEG_Z = 7,
        BCP_TEX = 8

    };
    /// \desc texture handles for our objects
    GLuint _textureHandles[NUM_TEXTURES];

    //***************************************************************************
    // Shader Program Information

    /// \desc shader program that performs Texture Shading
    CSCI441::ShaderProgram* _textureShaderProgram;
    CSCI441::ShaderProgram* _wavyShaderProgram;
    /// \desc stores the locations of all of our shader uniforms
    struct TextureShaderProgramUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        /// \desc diffuse texture map location
        GLint diffuseMap;
        /// \desc color tin location
        GLint colorTint;
    } _textureShaderProgramUniformLocations;
    struct WavyShaderProgramUniformLocation{
        GLint mvpMatrix;
        GLint modelMatrix;
        GLint color;
        GLint shouldMove;
        GLint typeBodyPart;
        GLint diffuseMap;
        GLint time;
        GLint viewPos;
        GLint pLightPos;
    } _wavyShaderProgramUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct TextureShaderProgramAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
        /// \desc vertex texture coordinate location
        GLint vTexCoord;
    } _textureShaderProgramAttributeLocations;
    struct WavyShaderProgramAttributeLocations{
        GLint vPos;
        GLint vTexCoord;
        GLint normal;
    } _wavyShaderProgramAttributeLocations;
    //***************************************************************************
    // Private Helper Functions

    /// \desc stores the attributes for a single texture
    struct VertexTextured {
        GLfloat x, y, z;
        GLfloat s, t;
    };

    /// \desc moves every fish
    /// one step forward
    void _moveFish();
    /// \desc checks if each fish
    /// has collided with any wall
    void _collideFishWithWall();
    /// \desc checks if any two fish
    /// have collided with each other
    void _collideFishWithFish();
    void drawSkybox(glm::mat4 projMtx) const;
};

void lab11_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );
void lab11_cursor_callback(GLFWwindow *window, double x, double y );
void lab11_mouse_button_callback(GLFWwindow *window, int button, int action, int mods );
void lab11_scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

#endif // LAB11_LAB11_ENGINE_H
