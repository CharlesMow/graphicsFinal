#include "FPEngine.h"

#include <CSCI441/objects.hpp>
#include <CSCI441/TextureUtils.hpp>

#include <cmath>
#include <time.h>
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

FPEngine::FPEngine()
         : CSCI441::OpenGLEngine(4, 1,
                                 640, 480,
                                 "Lab11: Collision Detection") {

    for(auto& _key : _keys) _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
}

void FPEngine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS) {
        switch( key ) {
            // quit!
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
                break;
            case GLFW_KEY_W:
                moveForward();
                break;
            case GLFW_KEY_S:
                moveBackward();
                break;
            case GLFW_KEY_A:
                moveLeft();
                break;
            case GLFW_KEY_D:
                moveRight();
                break;

            default: break; // suppress CLion warning
        }
    }
    if (_keys[GLFW_KEY_1]){
        _currentCamera = 1;
    }
    if (_keys[GLFW_KEY_2]){
        _currentCamera = 2;
    }

    // turn camera right
    if( _keys[GLFW_KEY_RIGHT] ) {
        _pFreeCam->rotate(_cameraSpeed.y, 0.0f);
    }
    // turn camera left
    if(_keys[GLFW_KEY_LEFT] ) {
        _pFreeCam->rotate(-_cameraSpeed.y, 0.0f);
    }
    // pitch camera up
    if(  _keys[GLFW_KEY_UP] ) {
        _pFreeCam->rotate(0.0f, _cameraSpeed.y);
    }
    // pitch camera down
    if( _keys[GLFW_KEY_DOWN] ) {
        _pFreeCam->rotate(0.0f, -_cameraSpeed.y);
    }
    if( _keys[GLFW_KEY_SPACE] ) {
        // go backward if shift held down
        if( _keys[GLFW_KEY_RIGHT_SHIFT] || _keys[GLFW_KEY_LEFT_SHIFT] ) {
            _pFreeCam->moveBackward(_cameraSpeed.x * 2.5);
        }
            // go forward
        else {
            _pFreeCam->moveForward(_cameraSpeed.x * 2.5);
        }
    }

}


void FPEngine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void FPEngine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
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
            if (_currentCamera == 1){            
                _arcballCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                                (_mousePosition.y - currMousePosition.y) * 0.005f);
            }
            else if(_currentCamera == 2){
                _pFreeCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                                (_mousePosition.y - currMousePosition.y) * 0.005f );
            }
        }
    }
    // passive motion
    else {

    }

    // update the mouse position
    _mousePosition = currMousePosition;
}

void FPEngine::handleScrollEvent(glm::vec2 offset) {
    // update the camera radius in/out
    GLfloat totChgSq = offset.y;
    _arcballCam->moveForward( totChgSq * 0.2f );
}

//*************************************************************************************
//
// Engine Setup

void FPEngine::mSetupGLFW() {
    CSCI441::OpenGLEngine::mSetupGLFW();

    // set our callbacks
    glfwSetKeyCallback(mpWindow, lab11_keyboard_callback);
    glfwSetMouseButtonCallback(mpWindow, lab11_mouse_button_callback);
    glfwSetCursorPosCallback(mpWindow, lab11_cursor_callback);
    glfwSetScrollCallback(mpWindow, lab11_scroll_callback);
}

void FPEngine::mSetupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                    // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	            // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// clear the frame buffer to black
}

void FPEngine::mSetupShaders() {
    //***************************************************************************
    // Setup Gouraud Shader Program

    _textureShaderProgram = new CSCI441::ShaderProgram("shaders/textureShader.v.glsl", "shaders/textureShader.f.glsl" );
    _wavyShaderProgram = new CSCI441::ShaderProgram("shaders/wavy.v.glsl","shaders/wavy.f.glsl");
    fprintf( stdout, "[INFO]: Wavy shader should have loaded.\n" );
    // get uniform locations
    _textureShaderProgramUniformLocations.mvpMatrix    = _textureShaderProgram->getUniformLocation("mvpMatrix");
    _textureShaderProgramUniformLocations.diffuseMap   = _textureShaderProgram->getUniformLocation("diffuseMap");
    _textureShaderProgramUniformLocations.colorTint    = _textureShaderProgram->getUniformLocation("colorTint");
    _wavyShaderProgramUniformLocations.mvpMatrix = _wavyShaderProgram->getUniformLocation("mvpMatrix");
    _wavyShaderProgramUniformLocations.time = _wavyShaderProgram->getUniformLocation("time");
    _wavyShaderProgramUniformLocations.color = _wavyShaderProgram->getUniformLocation("color");
    _wavyShaderProgramUniformLocations.shouldMove = _wavyShaderProgram->getUniformLocation("shouldMove");
    _wavyShaderProgramUniformLocations.typeBodyPart = _wavyShaderProgram->getUniformLocation("typeBodyPart");
    _wavyShaderProgramUniformLocations.diffuseMap   = _wavyShaderProgram->getUniformLocation("diffuseMap");
    _wavyShaderProgramUniformLocations.modelMatrix = _wavyShaderProgram->getUniformLocation("modelMatrix");
    _wavyShaderProgramUniformLocations.pLightPos = _wavyShaderProgram->getUniformLocation("pLightPos");
    _wavyShaderProgramUniformLocations.viewPos = _wavyShaderProgram->getUniformLocation("viewPos");
    // get attribute locations
    _textureShaderProgramAttributeLocations.vPos       = _textureShaderProgram->getAttributeLocation("vPos");
    _textureShaderProgramAttributeLocations.vTexCoord  = _textureShaderProgram->getAttributeLocation("vTexCoord");
    _wavyShaderProgramAttributeLocations.vPos = _wavyShaderProgram->getAttributeLocation("vPos");
    _wavyShaderProgramAttributeLocations.vTexCoord  = _wavyShaderProgram->getAttributeLocation("vTexCoord");
    _wavyShaderProgramAttributeLocations.normal = _wavyShaderProgram->getAttributeLocation("normal");
    // set static uniforms
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.diffuseMap, 0 );
    _wavyShaderProgram->setProgramUniform( _wavyShaderProgramUniformLocations.diffuseMap, 0 );
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
    CSCI441::setVertexAttributeLocations(_wavyShaderProgramAttributeLocations.vPos,
                                         _wavyShaderProgramAttributeLocations.normal,
                                         _wavyShaderProgramAttributeLocations.vTexCoord);
}

void FPEngine::mSetupBuffers() {
    // --------------------------------------------------------------------------
    // generate all of our VAO/VBO/IBO descriptors
    glGenVertexArrays( NUM_VAOS, _vaos );
    glGenBuffers( NUM_VAOS, _vbos );
    glGenBuffers( NUM_VAOS, _ibos );

    //***************************************************************************
    // Ground Plane generation
    _groundSize = 100.0f;
    _marbleRadius = 1.0f;

    GLfloat platformSize = _groundSize;

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

void FPEngine::mSetupTextures() {
    // unused in this lab

    _textureHandles[TEXTURES::PLATFORM_TEX] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/water.PNG");
    _textureHandles[TEXTURES::MARBLE_TEX]   = CSCI441::TextureUtils::loadAndRegisterTexture("assets/textures/sunfishSkin.png");

    // Skybox texture handles
    /*  POS_X   = +x    ft
     *  NEG_X   = -x    bk
     *
     *  POS_Y   = +y    up
     *  NEG_Y   = -y    dn
     *
     *  POS_Z   = +z    rt
     *  NEG_Z   = -z    lf
     */
    _textureHandles[TEXTURES::POS_X] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/skybox/uw_ft.jpg",
                                                                                     GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT,GL_FALSE);
    _textureHandles[TEXTURES::NEG_X] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/skybox/uw_bk.jpg",
                                                                                     GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT,GL_FALSE);

    _textureHandles[TEXTURES::POS_Y] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/skybox/uw_up.jpg",
                                                                                     GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT,GL_FALSE);
    _textureHandles[TEXTURES::NEG_Y] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/skybox/uw_dn.jpg",
                                                                                     GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT,GL_FALSE);

    _textureHandles[TEXTURES::POS_Z] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/skybox/uw_rt.jpg",
                                                                                     GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT,GL_FALSE);
    _textureHandles[TEXTURES::NEG_Z] = CSCI441::TextureUtils::loadAndRegisterTexture("assets/skybox/uw_lf.jpg",
                                                                                     GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT,GL_FALSE);
}

void FPEngine::mSetupScene() {
    // set up camera
    _arcballCam = new CSCI441::ArcballCam();
    _arcballCam->setLookAtPoint(glm::vec3(0.0f, 0.0f, 0.0f) );
    _arcballCam->setTheta(3.52f );
    _arcballCam->setPhi(1.9f );
    _arcballCam->setRadius( 15.0f );
    _arcballCam->recomputeOrientation();

    _pFreeCam= new CSCI441::FreeCam();
    _pFreeCam->setPosition(glm::vec3(60.0f, 40.0f, 30.0f) );
    _pFreeCam->setTheta(-M_PI / 3.0f );
    _pFreeCam->setPhi(M_PI / 2.8f );
    _pFreeCam->recomputeOrientation();
}

//*************************************************************************************
//
// Engine Cleanup

void FPEngine::mCleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _textureShaderProgram;
    delete _wavyShaderProgram;
}

void FPEngine::mCleanupBuffers() {
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

void FPEngine::mCleanupTextures() {
    fprintf( stdout, "[INFO]: ...deleting Textures\n" );
    glDeleteTextures(NUM_TEXTURES, _textureHandles);
}

void FPEngine::mCleanupScene() {
    fprintf( stdout, "[INFO]: ...deleting scene...\n" );
    delete _arcballCam;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!
void FPEngine::drawSkybox(glm::mat4 projMtx) const{
    glm::vec3 white(1,1,1);
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.colorTint, white );

    // GOOD TO GO
    // Floor -Z
    glm::mat4 modelMatrix = glm::mat4( 1.0f );
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0,-_groundSize,0));
    glm::mat4 mvpMatrix = projMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.mvpMatrix, mvpMatrix );

    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::NEG_Y] );
    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );
    glDrawElements( GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM], GL_UNSIGNED_SHORT, (void*)0 );


    // TOP +Y
    modelMatrix = glm::mat4( 1.0f );
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0,_groundSize,0));
    modelMatrix = glm::rotate(modelMatrix,(float) M_PI,CSCI441::X_AXIS);
    mvpMatrix = projMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.mvpMatrix, mvpMatrix );

    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::POS_Y] );
    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );
    glDrawElements( GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM], GL_UNSIGNED_SHORT, (void*)0 );


    // GOOD TO GO
    modelMatrix = glm::mat4( 1.0f );
    modelMatrix = glm::rotate(modelMatrix,(float) M_PI_2,CSCI441::X_AXIS);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0,_groundSize,0));
    modelMatrix = glm::rotate(modelMatrix,(float) M_PI,CSCI441::Z_AXIS);
    mvpMatrix = projMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.mvpMatrix, mvpMatrix );

    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::NEG_Z] );
    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );
    glDrawElements( GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM], GL_UNSIGNED_SHORT, (void*)0 );

    // GOOD TO GO
    modelMatrix = glm::mat4( 1.0f );
    modelMatrix = glm::rotate(modelMatrix,(float) M_PI_2,CSCI441::X_AXIS);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0,-_groundSize,0));
    mvpMatrix = projMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.mvpMatrix, mvpMatrix );

    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::POS_Z] );
    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );
    glDrawElements( GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM], GL_UNSIGNED_SHORT, (void*)0 );

    // GOOD TO GO
    modelMatrix = glm::mat4( 1.0f );
    modelMatrix = glm::rotate(modelMatrix,(float) M_PI_2,CSCI441::Z_AXIS);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0,_groundSize,0));
    modelMatrix = glm::rotate(modelMatrix,-(float) M_PI_2,CSCI441::Y_AXIS);
    modelMatrix = glm::rotate(modelMatrix,(float) M_PI,CSCI441::Z_AXIS);
    mvpMatrix = projMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.mvpMatrix, mvpMatrix );

    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::NEG_X] );
    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );
    glDrawElements( GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM], GL_UNSIGNED_SHORT, (void*)0 );

    // GOOD TO GO
    modelMatrix = glm::mat4( 1.0f );
    modelMatrix = glm::rotate(modelMatrix,(float) M_PI_2,CSCI441::Z_AXIS);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0,-_groundSize,0));
    modelMatrix = glm::rotate(modelMatrix,-(float) M_PI_2,CSCI441::Y_AXIS);
    mvpMatrix = projMtx * modelMatrix;
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.mvpMatrix, mvpMatrix );

    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::POS_X] );
    glBindVertexArray( _vaos[VAO_ID::PLATFORM] );
    glDrawElements( GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM], GL_UNSIGNED_SHORT, (void*)0 );
}
void FPEngine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const {
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
    drawSkybox(projectionViewMatrix);


// Draw ball and move to current position for boid direction
    modelMatrix = glm::mat4( 1.0f );

    modelMatrix = glm::translate(modelMatrix, glm::vec3(bcp_X, 0.0, bcp_Z));
    mvpMatrix = projectionViewMatrix * modelMatrix;
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.mvpMatrix, mvpMatrix );
    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::MARBLE_TEX] );
    glm::vec3 yellow(1,1,0);
    _textureShaderProgram->setProgramUniform( _textureShaderProgramUniformLocations.colorTint, yellow );

    CSCI441::drawSolidSphere(1,16,16);

    modelMatrix = glm::mat4( 1.0f );
    mvpMatrix = projectionViewMatrix * modelMatrix;

    //***************************************************************************
    // Draw Marbles
    _wavyShaderProgram->useProgram();
    if(_currentCamera == 1){
        _wavyShaderProgram->setProgramUniform(_wavyShaderProgramUniformLocations.viewPos, _arcballCam->getPosition());
    }
    else{
        _wavyShaderProgram->setProgramUniform(_wavyShaderProgramUniformLocations.viewPos, _pFreeCam->getPosition());

    }
    _wavyShaderProgram->setProgramUniform(_wavyShaderProgramUniformLocations.pLightPos, glm::vec3(bcp_X, 0.0, bcp_Z));
    glBindTexture( GL_TEXTURE_2D, _textureHandles[TEXTURES::MARBLE_TEX] );
    glProgramUniform3f(_wavyShaderProgram->getShaderProgramHandle(),
                       _wavyShaderProgramUniformLocations.color, 0.0,0.0,1.0);
    glProgramUniform1f(_wavyShaderProgram->getShaderProgramHandle(),
                       _wavyShaderProgramUniformLocations.time,glfwGetTime());
    for(unsigned int i = 0; i < _numMarbles; i++) {
        _marbles[i]->draw( _wavyShaderProgram,
                           _wavyShaderProgramUniformLocations.mvpMatrix,
                           _wavyShaderProgramUniformLocations.color,
                           _wavyShaderProgramUniformLocations.shouldMove,
                           _wavyShaderProgramUniformLocations.modelMatrix,
                           _wavyShaderProgramUniformLocations.typeBodyPart,
                           modelMatrix, projectionViewMatrix );
    }
}

void FPEngine::_updateScene() {

    _moveMarbles();
    moveToBcp();

    _collideMarblesWithWall();
    // _collideMarblesWithMarbles();

}

void FPEngine::run() {
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

        glm::mat4 viewMatrix = glm::mat4(1.0);

        // draw everything to the window
        switch (_currentCamera) {
            case 1:
                viewMatrix = _arcballCam->getViewMatrix();
                //send camera position to get view direction
                break;
            case 2:
                viewMatrix = _pFreeCam->getViewMatrix();
                break;
        }
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


void FPEngine::moveForward(){
    bcp_X += 2.0;
}
void FPEngine::moveBackward(){
    bcp_X -= 2.0;
}
void FPEngine::moveLeft(){
    bcp_Z -= 2.0;
}
void FPEngine::moveRight(){
    bcp_Z += 2.0;
}


void FPEngine::moveToBcp(){

    for( unsigned int i = 0; i < _numMarbles; i++){
        glm::vec3 collision = glm::vec3(0.0f);

        for( unsigned int j = 0; j < _numMarbles; j++){
            if(i == j){continue;}
            
            GLfloat distance = sqrt( pow((_marbles[j]->getLocation().x - _marbles[i]->getLocation().x),2) + pow((_marbles[j]->getLocation().y - _marbles[i]->getLocation().y),2) + pow((_marbles[j]->getLocation().z - _marbles[i]->getLocation().z),2)  );
            if(distance < 1.0f){
                collision = collision - (_marbles[j]->getLocation() - _marbles[i]->getLocation());
                // _marbles[i]->setLocationX(_marbles[i]->getLocation().x + 1.0);
                // _marbles[i]->setLocationX(_marbles[i]->getLocation().z + 1.0);

            }
        }



        collision = glm::vec3(collision.x, 0.0f, collision.z);

        glm::vec3 temp = _marbles[i]->getLocation();
        glm::vec3 newDirection = temp - glm::vec3(bcp_X, 0.0, bcp_Z);

        newDirection = (_marbles[i]->getDirection() - (newDirection * 0.01f));
        _marbles[i]->setDirection(newDirection + collision);
    }
}

void FPEngine::_moveMarbles() {
    //  TODO #1: move every ball forward along its heading
    for( int i = 0; i < _numMarbles; i++){
        _marbles[i]->moveForward();
    }

}

void FPEngine::_collideMarblesWithWall() {
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

void FPEngine::_collideMarblesWithMarbles() {
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
    auto engine = (FPEngine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the key and action through to the engine
        engine->handleKeyEvent(key, action);
    }
}

void lab11_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (FPEngine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the cursor position through to the engine
        engine->handleCursorPositionEvent(glm::vec2(x, y));
    }
}

void lab11_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (FPEngine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the mouse button and action through to the engine
        engine->handleMouseButtonEvent(button, action);
    }
}

void lab11_scroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
    auto engine = (FPEngine*) glfwGetWindowUserPointer(window);

    // ensure our engine is not null
    if(engine) {
        // pass the scroll offset through to the engine
        engine->handleScrollEvent(glm::vec2(xOffset, yOffset));
    }
}
