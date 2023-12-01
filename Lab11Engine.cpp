#include "Lab11Engine.h"

#include <CSCI441/objects.hpp>
#include <CSCI441/TextureUtils.hpp>

#include <cmath>
#include <ctime>

//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265
#endif

//*************************************************************************************
//
// Public Interface

Lab11Engine::Lab11Engine()
         : CSCI441::OpenGLEngine(4, 1,
                                 640, 480,
                                 "Lab11: Collision Detection") {

    for(auto& _key : _keys) _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
}

void Lab11Engine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS) {
        switch( key ) {
            // quit!
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
                break;

            default: break; // suppress CLion warning
        }
    }
}

void Lab11Engine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void Lab11Engine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(fabs(_mousePosition.x - MOUSE_UNINITIALIZED) <= 0.000001f) {
        _mousePosition = currMousePosition;
    }

    // active motion - if the left mouse button is being held down while the mouse is moving
    if(_leftMouseButtonState == GLFW_PRESS) {
        // if shift is held down, update our camera radius
        if( _keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT] ) {
            GLfloat totChgSq = (currMousePosition.x - _mousePosition.x) + (currMousePosition.y - _mousePosition.y);
            _arcballCam->moveForward( totChgSq * 0.01f );
        }
        // otherwise, update our camera angles theta & phi
        else {
            // rotate the camera by the distance the mouse moved
            _arcballCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                                (_mousePosition.y - currMousePosition.y) * 0.005f);
        }
    }
    // passive motion
    else {

    }

    // update the mouse position
    _mousePosition = currMousePosition;
}

void Lab11Engine::handleScrollEvent(glm::vec2 offset) {
    // update the camera radius in/out
    GLfloat totChgSq = offset.y;
    _arcballCam->moveForward( totChgSq * 0.2f );
}

//*************************************************************************************
//
// Engine Setup

void Lab11Engine::mSetupGLFW() {
    CSCI441::OpenGLEngine::mSetupGLFW();

    // set our callbacks
    glfwSetKeyCallback(mpWindow, lab11_keyboard_callback);
    glfwSetMouseButtonCallback(mpWindow, lab11_mouse_button_callback);
    glfwSetCursorPosCallback(mpWindow, lab11_cursor_callback);
    glfwSetScrollCallback(mpWindow, lab11_scroll_callback);
}

void Lab11Engine::mSetupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                    // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	            // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// clear the frame buffer to black
}

void Lab11Engine::mSetupShaders() {
    //***************************************************************************
    // Setup Gouraud Shader Program

    _textureShaderProgram = new CSCI441::ShaderProgram("shaders/textureShader.v.glsl", "shaders/textureShader.f.glsl" );
    _wavyShaderProgram = new CSCI441::ShaderProgram("shaders/wavy.v.glsl","shaders/wavy.f.glsl");
    // get uniform locations
    _textureShaderProgramUniformLocations.mvpMatrix    = _textureShaderProgram->getUniformLocation("mvpMatrix");
    _textureShaderProgramUniformLocations.diffuseMap   = _textureShaderProgram->getUniformLocation("diffuseMap");
    _textureShaderProgramUniformLocations.colorTint    = _textureShaderProgram->getUniformLocation("colorTint");
    _wavyShaderProgramUniformLocations.mvpMatrix = _wavyShaderProgram->getUniformLocation("mvpMatrix");
    _wavyShaderProgramUniformLocations.time = _wavyShaderProgram->getUniformLocation("time");
    _wavyShaderProgramUniformLocations.color = _wavyShaderProgram->getUniformLocation("color");
    // get attribute locations
    _textureShaderProgramAttributeLocations.vPos       = _textureShaderProgram->getAttributeLocation("vPos");
    _textureShaderProgramAttributeLocations.vTexCoord  = _textureShaderProgram->getAttributeLocation("vTexCoord");
    _wavyShaderProgramAttributeLocations.vPos = _wavyShaderProgram->getAttributeLocation("vPos");

    // set static uniforms
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.diffuseMap, 0 );

    // hook up the CSCI441 object library to our shader program - MUST be done after the shader is used and before the objects are drawn
    // if we have multiple shaders the flow would be:
    //      1) shader->useProgram()
    //      2) CSCI441::setVertexAttributeLocations()
    //      3) CSCI441::draw*()
    // OR the alternative is to ensure that all of your shader programs use the
    // same attribute locations for the vertex position, normal, and texture coordinate
    // as this lab is doing
    CSCI441::setVertexAttributeLocations(_textureShaderProgramAttributeLocations.vPos,
                                         -1,
                                         _textureShaderProgramAttributeLocations.vTexCoord);
    CSCI441::setVertexAttributeLocations(_wavyShaderProgramAttributeLocations.vPos, -1, -1);
}

void Lab11Engine::mSetupBuffers() {
    // --------------------------------------------------------------------------
    // generate all of our VAO/VBO/IBO descriptors
    glGenVertexArrays( NUM_VAOS, _vaos );
    glGenBuffers( NUM_VAOS, _vbos );
    glGenBuffers( NUM_VAOS, _ibos );

    //***************************************************************************
    // Ground Plane generation
    _groundSize = 10.0f;
    _marbleRadius = 1.0f;

    GLfloat platformSize = _groundSize + _marbleRadius;

    VertexTextured platformVertices[4] = {
            { -platformSize, 0.0f, -platformSize,   0.0f,  0.0f }, // 0 - BL
            {  platformSize, 0.0f, -platformSize,   1.0f,  0.0f }, // 1 - BR
            { -platformSize, 0.0f,  platformSize,   0.0f,  1.0f }, // 2 - TL
            {  platformSize, 0.0f,  platformSize,   1.0f,  1.0f }  // 3 - TR
    };

    unsigned short quadIndices[4] = {0, 1, 2, 3};
    _numVAOPoints[VAO_ID::PLATFORM] = 4;

    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );

    glBindBuffer( GL_ARRAY_BUFFER, _vbos[VAO_ID::PLATFORM] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( platformVertices ), platformVertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( _textureShaderProgramAttributeLocations.vPos );
    glVertexAttribPointer( _textureShaderProgramAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0 );

    glEnableVertexAttribArray( _textureShaderProgramAttributeLocations.vTexCoord );
    glVertexAttribPointer( _textureShaderProgramAttributeLocations.vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(GLfloat) * 3) );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibos[VAO_ID::PLATFORM]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof( quadIndices ), quadIndices, GL_STATIC_DRAW );

    //***************************************************************************
    // Marbles generation

    srand( time(0) );

    fprintf( stdout, "How many marbles do you wish to create? " );
    fflush(stdout);
    fscanf( stdin, "%d", &_numMarbles );

    const GLfloat RANGE_X = _groundSize * 2.0f;
    const GLfloat RANGE_Z = _groundSize * 2.0f;
    _marbles = (Marble**)malloc(sizeof(Marble*) * _numMarbles);
    for(unsigned int i = 0; i < _numMarbles; i++) {
        glm::vec3 randomPosition(rand()/(GLfloat)RAND_MAX * RANGE_X - RANGE_X/2.0f,
                                 0.0f,
                                 (i/(GLfloat)_numMarbles) * RANGE_Z - RANGE_Z/2.0f);
        glm::vec3 randomDirection(rand()/(GLfloat)RAND_MAX,
                                  0.0,
                                  rand()/(GLfloat)RAND_MAX);
        GLfloat randomRadius = _marbleRadius * (rand()/(GLfloat)RAND_MAX + 0.25f);
        _marbles[i] = new Marble(randomPosition, randomDirection, randomRadius);
    }
}

void Lab11Engine::mSetupTextures() {
    // unused in this lab

    _textureHandles[TEXTURES::PLATFORM_TEX] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/metal.jpg");
    _textureHandles[TEXTURES::MARBLE_TEX]   = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/brick.ppm");
}

void Lab11Engine::mSetupScene() {
    // set up camera
    _arcballCam = new CSCI441::ArcballCam();
    _arcballCam->setLookAtPoint(glm::vec3(0.0f, 0.0f, 0.0f) );
    _arcballCam->setTheta(3.52f );
    _arcballCam->setPhi(1.9f );
    _arcballCam->setRadius( 15.0f );
    _arcballCam->recomputeOrientation();
}

//*************************************************************************************
//
// Engine Cleanup

void Lab11Engine::mCleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _textureShaderProgram;
}

void Lab11Engine::mCleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( NUM_VAOS, _vaos );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();
    glDeleteBuffers( NUM_VAOS, _vbos );

    fprintf( stdout, "[INFO]: ...deleting IBOs....\n" );
    glDeleteBuffers( NUM_VAOS, _ibos );

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
    for(unsigned int i = 0; i < _numMarbles; i++)
        delete _marbles[i];
    free( _marbles );
}

void Lab11Engine::mCleanupTextures() {
    fprintf( stdout, "[INFO]: ...deleting Textures\n" );
    glDeleteTextures(NUM_TEXTURES, _textureHandles);
}

void Lab11Engine::mCleanupScene() {
    fprintf( stdout, "[INFO]: ...deleting scene...\n" );
    delete _arcballCam;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void Lab11Engine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // if either shader program is null, do not continue any further to prevent run time errors
    if(!_textureShaderProgram) {
        return;
    }

    //***************************************************************************
    // Draw Ground Plane

    _textureShaderProgram->useProgram();

    glm::mat4 modelMatrix = glm::mat4( 1.0f );
    glm::mat4 projectionViewMatrix = projMtx * viewMtx;
    glm::mat4 mvpMatrix = projectionViewMatrix * modelMatrix;
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.mvpMatrix, mvpMatrix );

    glm::vec3 white(1,1,1);
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.colorTint, white );

    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::PLATFORM_TEX] );
    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );
    glDrawElements( GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM], GL_UNSIGNED_SHORT, (void*)0 );

    //***************************************************************************
    // Draw Marbles

    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::MARBLE_TEX] );
    _wavyShaderProgram->useProgram();
    glProgramUniform3f(_wavyShaderProgram->getShaderProgramHandle(),
                       _wavyShaderProgramUniformLocations.color, 0.0,0.0,1.0);
    glProgramUniform1f(_wavyShaderProgram->getShaderProgramHandle(),
                       _wavyShaderProgramUniformLocations.time,glfwGetTime());
    for(unsigned int i = 0; i < _numMarbles; i++) {
        _marbles[i]->draw( _wavyShaderProgram,
                           _wavyShaderProgramUniformLocations.mvpMatrix, _wavyShaderProgramUniformLocations.color,
                           modelMatrix, projectionViewMatrix );
    }
}

void Lab11Engine::_updateScene() {
    _moveMarbles();
    _collideMarblesWithWall();
    _collideMarblesWithMarbles();
}

void Lab11Engine::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(mpWindow) ) {	        // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( mpWindow, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projectionMatrix = glm::perspective( 45.0f, (GLfloat) framebufferWidth / (GLfloat) framebufferHeight, 0.001f, 1000.0f );

        // set up our look at matrix to position our camera
        glm::mat4 viewMatrix = _arcballCam->getViewMatrix();

        // draw everything to the window
        _renderScene(viewMatrix, projectionMatrix);

        // animate the scene
        _updateScene();

        glfwSwapBuffers(mpWindow);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper Functions

void Lab11Engine::_moveMarbles() {
    //  TODO #1: move every ball forward along its heading
    for( int i = 0; i < _numMarbles; i++){
        _marbles[i]->moveForward();
    }

}

void Lab11Engine::_collideMarblesWithWall() {
    // TODO #2: check if each marble has hit any wall

    for( int i = 0; i < _numMarbles; i++){
        if (_marbles[i]->getLocation().x > _groundSize || _marbles[i]->getLocation().x < -_groundSize)
        {
            _marbles[i]->moveBackward();
            _marbles[i]->moveBackward();

            glm::vec3 vecOut = _marbles[i]->getDirection() - 2 * dot(_marbles[i]->getDirection(), glm::vec3(-1,0,0)) * glm::vec3(-1,0,0);

            _marbles[i]->setDirection(vecOut);
        }

        if (_marbles[i]->getLocation().z > _groundSize || _marbles[i]->getLocation().z < -_groundSize)
        {
            _marbles[i]->moveBackward();
            _marbles[i]->moveBackward();
            glm::vec3 vecOut = _marbles[i]->getDirection() - 2 * dot(_marbles[i]->getDirection(), glm::vec3(0,0,-1)) * glm::vec3(0,0,-1);

            _marbles[i]->setDirection(vecOut);
        }
    }

}

void Lab11Engine::_collideMarblesWithMarbles() {
    // TODO #3: check for interball collisions
    // warning this isn't perfect...balls can get caught and
    // continually bounce back-and-forth in place off
    // each other
    for( int i = 0; i < _numMarbles; i++){
        for(int j = 0; j < _numMarbles; j++){
            if(i == j){
                continue;
            }

            float currentDistance = sqrt(pow((_marbles[j]->getLocation().x - _marbles[i]->getLocation().x),2) + pow((_marbles[j]->getLocation().z - _marbles[i]->getLocation().z),2));
            float radDistance = _marbles[j]->RADIUS + _marbles[i]->RADIUS;

            if ( currentDistance < radDistance){
                    _marbles[i]->moveBackward();
                    _marbles[j]->moveBackward();

                // change direction of the first ball 
                glm::vec3 firstNormal = _marbles[j]->getLocation() - _marbles[i]->getLocation();
                //make the normal a unit vec
                // firstNormal = 
                glm::vec3 vecOut = _marbles[i]->getDirection() - 2 * dot(_marbles[i]->getDirection(), firstNormal) * firstNormal;

                _marbles[i]->setDirection(vecOut);

                // change direction of the second ball 
                glm::vec3 secondNormal = _marbles[i]->getLocation() - _marbles[j]->getLocation();
                glm::vec3 vecOut2 = _marbles[j]->getDirection() - 2 * dot(_marbles[j]->getDirection(), secondNormal) * secondNormal;

                _marbles[j]->setDirection(vecOut2);


                    _marbles[i]->moveForward();
                    _marbles[j]->moveForward();
            }
        }

    }
}

//*************************************************************************************
//
// Callbacks

void lab11_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (Lab11Engine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the key and action through to the engine
        engine->handleKeyEvent(key, action);
    }
}

void lab11_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (Lab11Engine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the cursor position through to the engine
        engine->handleCursorPositionEvent(glm::vec2(x, y));
    }
}

void lab11_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (Lab11Engine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the mouse button and action through to the engine
        engine->handleMouseButtonEvent(button, action);
    }
}

void lab11_scroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
    auto engine = (Lab11Engine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the scroll offset through to the engine
        engine->handleScrollEvent(glm::vec2(xOffset, yOffset));
    }
}
