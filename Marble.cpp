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
                   GLint shouldMoveUniformLocation,
                   GLint modelMatrixUniformLocation,
                   GLint typeBodyPart, glm::mat4 modelMtx, glm::mat4 projViewMtx ) {
    modelMtx = glm::translate( modelMtx, _location );
    modelMtx = glm::rotate(modelMtx, glm::atan(_direction.x,_direction.z)-(float) M_PI_2, glm::vec3(0,1,0));
     glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    shaderProgram->setProgramUniform( shouldMoveUniformLocation, true );
    modelMtx = glm::translate( modelMtx, glm::vec3( 0, RADIUS*1.8, 0));
    _drawBody(shaderProgram, mvpUniformLocation, colorUniformLocation, typeBodyPart, modelMatrixUniformLocation, modelMtx, projViewMtx);
    _drawFins(shaderProgram, mvpUniformLocation, colorUniformLocation, typeBodyPart, modelMatrixUniformLocation, modelMtx, projViewMtx);
    shaderProgram->setProgramUniform( shouldMoveUniformLocation, false );
    shaderProgram->setProgramUniform( typeBodyPart, 0 );
    _drawEyes(shaderProgram, mvpUniformLocation, colorUniformLocation, modelMatrixUniformLocation, modelMtx, projViewMtx);

}

void Marble::_drawBody(CSCI441::ShaderProgram *shaderProgram, GLint mvpUniformLocation, GLint colorUniformLocation,
                       GLint typeBodyPart, GLint modelMatrixUniformLocation,
                       glm::mat4 modelMtx, glm::mat4 projViewMtx) const{
    modelMtx = glm::scale ( modelMtx, glm::vec3(1.5, 1, 1));
    modelMtx = glm::rotate( modelMtx, (float) M_PI_2, CSCI441::X_AXIS );
    glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );

    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(1.0,0.0,0.0) );
    // Body Part 3
    shaderProgram->setProgramUniform( modelMatrixUniformLocation, modelMtx );
    shaderProgram->setProgramUniform( typeBodyPart, 3);
    CSCI441::drawSolidCylinder(RADIUS*0.875, RADIUS, 0.2, 16, 16);

    modelMtx = glm::rotate( modelMtx, (float) -M_PI_2, CSCI441::X_AXIS);
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    // Body Part 4
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(0.0,0.0,1.0) );
    shaderProgram->setProgramUniform( typeBodyPart, 4);
    shaderProgram->setProgramUniform( modelMatrixUniformLocation, modelMtx );
    CSCI441::drawSolidDisk(0, RADIUS*0.875, 16, 16);

    modelMtx = glm::translate( modelMtx, glm::vec3(0, 0, 0.2));
    modelMtx = glm::rotate( modelMtx, (float) M_PI_2, CSCI441::X_AXIS );
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    // Body Part 5
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(1.0,0.0,0.0) );
    shaderProgram->setProgramUniform( typeBodyPart, 5);
    shaderProgram->setProgramUniform( modelMatrixUniformLocation, modelMtx );
    CSCI441::drawSolidCylinder(RADIUS, RADIUS*0.875, 0.2, 16, 16);
    modelMtx = glm::rotate( modelMtx, (float) -M_PI_2, CSCI441::X_AXIS);
    modelMtx = glm::translate( modelMtx, glm::vec3(0,0,0.2));
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    // Body Part 6
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(0.0,0.0,1.0) );
    shaderProgram->setProgramUniform( typeBodyPart, 6);
    shaderProgram->setProgramUniform( modelMatrixUniformLocation, modelMtx );
    CSCI441::drawSolidDisk(0, RADIUS*0.875, 16, 16);
}

void Marble::_drawFins(CSCI441::ShaderProgram *shaderProgram, GLint mvpUniformLocation, GLint colorUniformLocation,
                       GLint typeBodyPart, GLint modelMatrixUniformLocation,
                       glm::mat4 modelMtx, glm::mat4 projViewMtx) const{
    modelMtx = glm::translate( modelMtx, glm::vec3(0,0,0.4));
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(0.0,1.0,0.0) );
    modelMtx = glm::translate( modelMtx, glm::vec3( -RADIUS/4, RADIUS/6, -0.2));
    modelMtx = glm::scale( modelMtx, glm::vec3( 1, 1.5, 0.2));
    modelMtx = glm::rotate( modelMtx, glm::radians(15.f), CSCI441::Z_AXIS);
    glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    shaderProgram->setProgramUniform( typeBodyPart, 1);
    shaderProgram->setProgramUniform( modelMatrixUniformLocation, modelMtx );
    CSCI441::drawSolidCylinder( RADIUS/2, RADIUS/4, RADIUS, 16, 16);

    modelMtx = glm::translate( modelMtx, glm::vec3( 0, -RADIUS/3, 0));
    modelMtx = glm::rotate( modelMtx, (float) M_PI, CSCI441::X_AXIS);
    modelMtx = glm::rotate( modelMtx, glm::radians(30.f), CSCI441::Z_AXIS);
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    shaderProgram->setProgramUniform( typeBodyPart, 2);
    shaderProgram->setProgramUniform( modelMatrixUniformLocation, modelMtx );
    CSCI441::drawSolidCylinder( RADIUS/2, RADIUS/4, RADIUS, 16, 16);
}

void Marble::_drawEyes(CSCI441::ShaderProgram *shaderProgram, GLint mvpUniformLocation, GLint colorUniformLocation,
                       GLint modelMatrixUniformLocation,
                       glm::mat4 modelMtx, glm::mat4 projViewMtx) const {
    shaderProgram->setProgramUniform(colorUniformLocation, glm::vec3(0,0,0) );
    modelMtx = glm::translate( modelMtx, glm::vec3( 3*RADIUS/4, 0, 0));
    glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    shaderProgram->setProgramUniform( modelMatrixUniformLocation, modelMtx );
    CSCI441::drawSolidSphere( 0.2, 16, 16 );
    modelMtx = glm::translate( modelMtx, glm::vec3(0, 0, 0.4));
    mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );
    shaderProgram->setProgramUniform( modelMatrixUniformLocation, modelMtx );
    CSCI441::drawSolidSphere( 0.2, 16, 16 );
}
void Marble::moveForward() {
    _location += _direction * SPEED;
}

void Marble::moveBackward() {
    _location -= _direction * SPEED;
}

GLfloat Marble::_genRandColor() {
    return (GLfloat)(rand() * 100 % 50) / 100.0f + 0.5f;
}