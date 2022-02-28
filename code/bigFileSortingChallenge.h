/*************************************************************
 * 
 * Creator: Itay Barok
 * 
 * Company: XM Cyber
 * 
 * Final Date: 21.10.2021
 * 
 * Project Description:
 * [+] Sort a huge file that might not be able to 
 * [+] be loaded into RAM
 * 
 * Sort Algorithm: 
 * [1] Divide the input file into samller segments
 * [2] Load the segments one at a different time and sort each
 * [3] Merge the sorted segments into a sorted output file
 * 
 *************************************************************/

/*--------------------------------------------------------------------------------------------------------------------------------------
 * Sort algorithm analysis:
 * [+] the handled data at a given time is at most 
 *     numberOfLinesPerSegment * lineSizeBytes
 * 
 * [+] Time Complexity: O(N*N*log(N)) while N is the size of the file
 *     
 *     calcualtion: T(Sort) = T(split) + T(merge) + O(1) = T(split) + O(N) + O(1) =  
 *                  = O(N / numberOfSegments) + N * T(std::sort on N / numberOfSegments) + O(N) = O(N) + N * O(N*log(N)) = O(N*N*log(N))
 *--------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef TEST_H
#define TEST_H

#include <vector>
#include <algorithm>
#include <windows.h>
#include <cstring>
#include <memory>
#include <fstream>
#include <iostream>





/*---------------------------------------------------------------------------------
 * Class for file utilities that uses win32 API for reading and writing
 *---------------------------------------------------------------------------------*/
class FileUtilsWin32API
{
    protected:
        HANDLE hfile;
        std :: string filePath;
        std :: string inFilePath;        
        std :: string outputFilePath;

    public:
        // Constructor of file that uses win32 API for reading and writing from files
        FileUtilsWin32API(const std :: string & filePath, const std :: string & inFilePath, const std :: string & outputFilePath, char mode='\0');

        // load to line dwBytesInLine characters from file in hfileRead
       void loadFromFile(char* line, DWORD dwBytesInLine);


        // store dwBytesToWrite from buffer in hfileWrite
        void storeInFile(char* buffer, DWORD dwBytesToWrite);

        // rewind the file pointer to the start of the file
        void rewind();

        // getter for the private variable that is the path to the file
        std::string get_inFilePath();

        // destructor that binds life time of object to its resources
        ~FileUtilsWin32API();
};



//-----------------------------------------------------------------------------------------------------------------------------------------//
// note_from_interview{here is the public on the base class in inheritance                                                              }  //
//                    {I used it here because it is necessary for me to keep the public/protected/private of member or function as is   }  //
//                    {explanation: minimize the original  public/protected/private of a member or function in sub class                }  //
//                    {that arrived from base class with the public/protected/private of the class                                      }  //
//--------------------------------------------------------------------------------------------------------------------------------  -------//

/*-------------------------------------------------------------------------------------------------------
 * Class for segment file methods and contains the utilities that uses win32 API for reading and writing
 *-------------------------------------------------------------------------------------------------------*/
class SegmentFileWin32API: public FileUtilsWin32API
{
    private:
        int counter_lines_in_output_file;

    public:
        std::string relevant_line;

        // Constructor of segment file that uses win32 API for reading and writing from files
        SegmentFileWin32API(const std :: string & filePath, const std :: string & inFilePath, const std :: string & outputFilePath, int counter_lines_in_output_file, char mode='\0');


       //---------------------------------------------------------------------------//
        // note_from_interview{here is a default destructor of sub class           } //
        //                    {It indeed executes its code first                   } //
        //                    {than it calls the base class destructor             } //
        //                    {theoretic  explaination: opositive of constructor   } //
        //                    {so the flow of call and execute code is oposite     } //  
        //                    {practical explanation: print to see flow of code or } //                               
        //                    {use debugger in an IDE                              } //
        //---------------------------------------------------------------------------//

        // destructor
        ~SegmentFileWin32API();


        // advance by one line counter of lines from this segment in output file        
        void advanceLineCounter();

        // getter for the private variable that counts the lines from this segment in output file
        int getLineCounter();
};






//
// Class that contains utilities in files and mainly the method to sort a huge file
//
class FileSort
{
    private:
        int maxFileSizeBytes;
        int numberOfLinesPerSegment;
        int lineSizeBytes;


        

    public: 
         
        // step [1] in the sorting algorithm to seperaete the file into smaller segments
        void split_to_file_segments(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments,const std :: string & inFilePath, const std :: string & outputFilePath);

        // fill the segment file with lines from the huge file
        void split_file_to_segment(std::unique_ptr<FileUtilsWin32API> & inFile, std::unique_ptr<SegmentFileWin32API> & segment);

        // sort the segment lines
        void sortSegmentLines(std::vector<std::string> & segmentLines, std::string & strSegmentLines);


        /*
         * returns the size in bytes of the file in path_to_file
         */
        int get_size_in_bytes(const std :: string & path_to_file);

        /*
         * step [3] of the sorting algorithm merge the segments to a huge file
         * the merge keep the invariant of the output file to be sorted in each step and advances line by line
         * to the end and therefore the output file is sorted
         * Algorithm: 
         * [+] using the classis merge algorithm to merge sorted small data structures 
         * [+] into a sorted big data structure which is in our case a file 
         * [+] start from the low indexes in our case lines keep the minimum line in the big file 
         * [+] and advance one step from the minimum to te next line and repeat until no more lines 
         */
        void merge_all_segments_to_out_file(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments, const std :: string &inFilePath, const std :: string &outFilePath);

        // check the counters of each segment to see if some lines left to be written to output file
        bool isLineLeftInSegments(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments);

        // make the step of storing the next minimum line from the relevant lines and 
        // advance in  this segment to the next line
        void store_next_min_line(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments, DWORD length, std::unique_ptr<FileUtilsWin32API> & outFile);

        // step to the next line in the segment
        void update_min_segment_next_line(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments, int length, int min_segment_num);

        // check valid input file
        void check_valid_input_file(const std :: string & inFilePath);
       
        FileSort(int maxFileSizeBytes, int numberOfLinesPerSegment, int lineSizeBytes);

        /*
         * Algorithm Overview:
         *   [1] Take the big input file and split the file into small segments of files
         *   [2] Sort each segment separately
         *   [3] Merge the segment line by line (merge-sort like merge) and insert the lines one by one in a sorted way to the output file
         *   [4] Cleanup all the files that were created in [1] of the file sort
         */
        void Sort(const std :: string & inFilePath, const std :: string &outFilePath);
};

#endif