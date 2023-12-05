#include "Marble.h"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/OpenGLUtils.hpp>
#include <CSCI441/objects.hpp>

#include <cstdlib>

Marble::Marble()
    : _rotation(0.0f), _location(glm::vec3(0,0,0)), _direction(glm::vec3(1,0,0)),
      RADIUS(0.5f),
      SPEED(0.1f),
      COLOR(glm::vec3(_genRandColor(), _genRandColor(), _genRandColor())) {

}

Marble::Marble(glm::vec3 location, glm::vec3 direction, GLfloat radius )
    : _rotation(0.0f), _location(location), _direction(direction),
      RADIUS(radius),
      SPEED(0.1f),
      COLOR(glm::vec3(_genRandColor(), _genRandColor(), _genRandColor())) {
    _direction = glm::normalize( _direction );
}

glm::vec3 Marble::getLocation() const { return _location; }
void Marble::setLocationX(GLfloat x) { _location.x = x; }
void Marble::setLocationZ(GLfloat z) { _location.z = z; }
glm::vec3 Marble::getDirection() const { return _direction; }
void Marble::setDirection(glm::vec3 newDirection) { _direction = glm::normalize(newDirection); }

void Marble::draw( CSCI441::ShaderProgram *shaderProgram, GLint mvpUniformLocation, GLint colorUniformLocation,
                   GLint shouldMoveUniformLocation, GLint typeBodyPart, glm::mat4 modelMtx, glm::mat4 projViewMtx ) {
    glm::vec3 rotationAxis = glm::cross( _direction, CSCI441::Y_AXIS );
    modelMtx = glm::translate( modelMtx, _location );
     glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    shaderProgram->setProgramUniform( shouldMoveUniformLocation, true );
    modelMtx = glm::translate( modelMtx, glm::vec3( 0, RADIUS, 0));
    _drawBody(shaderProgram, mvpUniformLocation, colorUniformLocation, typeBodyPart,modelMtx, projViewMtx);
    _drawFins(shaderProgram, mvpUniformLocation, colorUniformLocation, typeBodyPart,modelMtx, projViewMtx);
    shaderProgram->setProgramUniform( shouldMoveUniformLocation, false );
    shaderProgram->setProgramUniform( typeBodyPart, 0 );
    _drawEyes(shaderProgram, mvpUniformLocation, colorUniformLocation, modelMtx, projViewMtx);

}

void Marble::_drawBody(CSCI441::ShaderProgram *shaderProgram, GLint mvpUniformLocation, GLint colorUniformLocation,
                       GLint typeBodyPart,glm::mat4 modelMtx, glm::mat4 projViewMtx) const{
    modelMtx = glm::scale ( modelMtx, glm::vec3(1.5, 1, 1));
    modelMtx = glm::rotate( modelMtx, (float) M_PI_2, CSCI441::X_AXIS );
    glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );

    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(1.0,0.0,0.0) );
    // Body Part 3
    shaderProgram->setProgramUniform( typeBodyPart, 3);
    CSCI441::drawSolidCylinder(RADIUS*0.875, RADIUS, 0.2, 16, 16);

    modelMtx = glm::rotate( modelMtx, (float) -M_PI_2, CSCI441::X_AXIS);
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    // Body Part 4
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(0.0,0.0,1.0) );
    shaderProgram->setProgramUniform( typeBodyPart, 4);
    CSCI441::drawSolidDisk(0, RADIUS*0.875, 16, 16);

    modelMtx = glm::translate( modelMtx, glm::vec3(0, 0, 0.2));
    modelMtx = glm::rotate( modelMtx, (float) M_PI_2, CSCI441::X_AXIS );
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    // Body Part 5
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(1.0,0.0,0.0) );
    shaderProgram->setProgramUniform( typeBodyPart, 5);
    CSCI441::drawSolidCylinder(RADIUS, RADIUS*0.875, 0.2, 16, 16);
    modelMtx = glm::rotate( modelMtx, (float) -M_PI_2, CSCI441::X_AXIS);
    modelMtx = glm::translate( modelMtx, glm::vec3(0,0,0.2));
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    // Body Part 6
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(0.0,0.0,1.0) );
    shaderProgram->setProgramUniform( typeBodyPart, 6);
    CSCI441::drawSolidDisk(0, RADIUS*0.875, 16, 16);
}

void Marble::_drawFins(CSCI441::ShaderProgram *shaderProgram, GLint mvpUniformLocation, GLint colorUniformLocation,
                       GLint typeBodyPart,glm::mat4 modelMtx, glm::mat4 projViewMtx) const{
    modelMtx = glm::translate( modelMtx, glm::vec3(0,0,0.4));
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(0.0,1.0,0.0) );
    modelMtx = glm::translate( modelMtx, glm::vec3( -RADIUS/4, RADIUS/6, -0.2));
    modelMtx = glm::scale( modelMtx, glm::vec3( 1, 1.5, 0.2));
    modelMtx = glm::rotate( modelMtx, glm::radians(15.f), CSCI441::Z_AXIS);
    glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    shaderProgram->setProgramUniform( typeBodyPart, 1);
    CSCI441::drawSolidCylinder( RADIUS/2, RADIUS/4, RADIUS, 16, 16);

    modelMtx = glm::translate( modelMtx, glm::vec3( 0, -RADIUS/3, 0));
    modelMtx = glm::rotate( modelMtx, (float) M_PI, CSCI441::X_AXIS);
    modelMtx = glm::rotate( modelMtx, glm::radians(30.f), CSCI441::Z_AXIS);
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    shaderProgram->setProgramUniform( typeBodyPart, 2);
    CSCI441::drawSolidCylinder( RADIUS/2, RADIUS/4, RADIUS, 16, 16);
}

void Marble::_drawEyes(CSCI441::ShaderProgram *shaderProgram, GLint mvpUniformLocation, GLint colorUniformLocation,
               glm::mat4 modelMtx, glm::mat4 projViewMtx) const {
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(0,0,0) );
    modelMtx = glm::translate( modelMtx, glm::vec3( 3*RADIUS/4, 0, 0));
    glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    CSCI441::drawSolidSphere( 0.2, 16, 16 );
    modelMtx = glm::translate( modelMtx, glm::vec3(0, 0, 0.4));
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    CSCI441::drawSolidSphere( 0.2, 16, 16 );
}
void Marble::moveForward() {
    //_location += _direction * SPEED;
    //_rotation -= SPEED;
    //if( _rotation < 0.0f ) {
    //    _rotation += 6.28f;
    //}
}

void Marble::moveBackward() {
    //_location -= _direction * SPEED;
    /*
    _rotation += SPEED;
    if( _rotation > 6.28f ) {
        _rotation -= 6.28f;
    }*/
    //_rotation += SPEED;
    //if( _rotation > 6.28f ) {
    //    _rotation -= 6.28f;
    //}
}

GLfloat Marble::_genRandColor() {
    return (GLfloat)(rand() * 100 % 50) / 100.0f + 0.5f;
}