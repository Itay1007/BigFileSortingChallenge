#ifndef INPUTGENERATOR_H
#define INPUTGENERATOR_H 


#include <iostream>
#include <ctime>
#include <unistd.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

/*
 * Class to generate random valid input tests to test File Sort algorithm
 */
class InputGenerator
{
    int inputesGeneretedNum;
    int numOfFilesToGenerate;
    std::string basicOutFilePathNoExtention; 
    std::string outFilePath;

    public:

        // Constructor
        InputGenerator(int numOfFilesToGenerate, std::string basicOutFilePathNoExtention);

        /*
         * genRandomLine is an already created function from StackOverFlow https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
         * returns a psuedo random string in length len
         */
        std::string genRandomLine(const int len);

        // returns a random integer in the bounds
        int genRandomInt(int lowBound, int upBound);


        // store dwBytesToWrite from buffer in hfileWrite
        void storeInFile(HANDLE & hfile, char* buffer, DWORD dwBytesToWrite);


        // Constructor of file that uses win32 API for reading and writing from files
        void createFile(HANDLE & hfile);

        // generate the output valid test files for File Sort algorithm 
        void generateFiles();
};

#endif



