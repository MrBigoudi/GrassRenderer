#pragma once

#include <cstdlib>
#include <iostream>

#include <glad/gl.h>
#include <stdarg.h>

/**
 * @enum The level of the error
*/
enum ErrorLevel{
    FATAL,
    WARNING,
};

/**
 * @enum The different error codes
*/
enum ErrorCodes{
    NO_ERROR, 
    GLAD_ERROR,
    GLFW_ERROR,
    GL_ERROR,
    NOT_INITALIZED,
    IO_ERROR,
    OUT_OF_RANGE,
    BAD_VALUE,
};

/**
 * The class to handle errors
*/
class ErrorHandler{
    private:
        /**
         * Private constructor to make the class purely virtual
        */
        ErrorHandler(){};

    public:
        /**
         * Handle the error
         * @param error The error to handle
         * @param level The error level
        */
        static void handle(ErrorCodes error, ErrorLevel level = FATAL){
            switch(error){
                case NO_ERROR:
                    break;
                default:
                    if(level == FATAL){
                        fprintf(stderr, "Exiting the program!\n");
                        exit(EXIT_FAILURE);
                        break;
                    }
                    if(level == WARNING){
                        fprintf(stderr, "Warning, continue the program!\n");
                        break;
                    }
            }
        }
};