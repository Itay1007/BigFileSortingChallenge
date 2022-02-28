#include "InputGenerator.h"

/*
 * Class to generate random valid input tests to test File Sort algorithm
 */


InputGenerator::InputGenerator(int numOfFilesToGenerate, std::string basicOutFilePathNoExtention)
{
    this-> inputesGeneretedNum = 0;
    this->numOfFilesToGenerate = numOfFilesToGenerate;
    this->basicOutFilePathNoExtention = basicOutFilePathNoExtention;
    this->outFilePath = "";
}


/*
 * genRandomLine is an already created function from StackOverFlow https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
 * returns a psuedo random string in length len
 */
std::string InputGenerator::genRandomLine(const int len) {
    
    std::string tmp_s;
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    // srand( (unsigned) time(NULL) * getpid());

    // tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    
    
    return tmp_s;
    
}

// returns a random integer in the bounds
int InputGenerator::genRandomInt(int lowBound, int upBound)
{ 
    if (lowBound >= upBound)
    {
        throw std::invalid_argument("upper Bound should be greater than lower bound");
    }
    
    return rand() % (upBound - lowBound) + lowBound;
}


// store dwBytesToWrite from buffer in hfileWrite
void InputGenerator::storeInFile(HANDLE & hfile, char* buffer, DWORD dwBytesToWrite)
{
    DWORD dwBytesWritten = 0;

     if(not WriteFile(hfile, buffer, dwBytesToWrite, &dwBytesWritten, NULL))
    {
        throw std::runtime_error("Could not write to file: " + this->outFilePath + "\nError Code: " + std::to_string(GetLastError()));
    }
}

// Constructor of file that uses win32 API for reading and writing from files
void InputGenerator::createFile(HANDLE & hfile)
{

    hfile = CreateFile(this->outFilePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if(hfile == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Could not create to write the file: " + this->outFilePath + "\nError Code: " + std::to_string(GetLastError()));
    }
}

// generate the output valid test files for File Sort algorithm 
void InputGenerator::generateFiles()
{
    const std::string testExtension = "txt";
    std::string outFilePath;
    std::string line;
    HANDLE hfile;
    int fileNumLines, lineSizeBytes, numberOfLinesPerSegment, numberOfSegments;
    DWORD dwBytesWritten = 0;
    DWORD length;

    for(int i = 0; i <this->numOfFilesToGenerate; i++)
    {
        this->outFilePath =  this-> basicOutFilePathNoExtention + std::to_string(i + 1) + "." + testExtension;

        createFile(hfile);

        lineSizeBytes = genRandomInt(3, 50);
        numberOfLinesPerSegment = genRandomInt(1, 20);
        numberOfSegments = genRandomInt(1, 8);

        fileNumLines = numberOfLinesPerSegment * numberOfSegments; 
        length = lineSizeBytes;

        char strLine[lineSizeBytes + 1];

        for(int lineNum = 0; lineNum < fileNumLines; lineNum++)
        {
            line = genRandomLine(lineSizeBytes - 2);

            for(int i = 0 ; i < lineSizeBytes - 2; i++)
            {
                strLine[i] = line[i];
            }
            strLine[lineSizeBytes - 2] = '\r';
            strLine[lineSizeBytes - 1] = '\n';
            strLine[lineSizeBytes] = '\0';

            storeInFile(hfile, strLine, length);
        }
    }

    CloseHandle(hfile);
}
