/*
 *  CSCI 441, Computer Graphics, Fall 2022
 *
 *  Project: lab11
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup for collision detection.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2022
 *
 */

#include "Lab11Engine.h"

///*****************************************************************************
//
// Our main function
int main() {

    auto labEngine = new Lab11Engine();
    labEngine->initialize();
    if (labEngine->getError() == CSCI441::OpenGLEngine::OPENGL_ENGINE_ERROR_NO_ERROR) {
        labEngine->run();
    }
    labEngine->shutdown();
    delete labEngine;

    return EXIT_SUCCESS;
}
