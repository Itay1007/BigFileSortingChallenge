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


#include "new_test.h"




/*---------------------------------------------------------------------------------
 * Class for file utilities that uses win32 API for reading and writing
 *---------------------------------------------------------------------------------*/

// Constructor of file that uses win32 API for reading and writing from files
FileUtilsWin32API::FileUtilsWin32API(const std :: string & filePath, const std :: string & inFilePath, const std :: string & outputFilePath, char mode)
{
    if(mode == 'r') // mode 'r' for "read"
    {
        this->hfile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if(this->hfile == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("Could not open to read the file: " + this->filePath + "\nError Code: " + std::to_string(GetLastError()));
        } 
    }
    else if(mode == 'w') // mode 'w' for "write"
    {
        this->hfile = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        if(this->hfile == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("Could not create to write the file: " + this->filePath + "\nError Code: " + std::to_string(GetLastError()));
        }
    }
    else if(mode == 'b') // mode 'b' for "both" read and write
    {
        this->hfile = CreateFile(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        if(this->hfile == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("Could not create to read and wirte the file: " + this->filePath + "\nError Code: " + std::to_string(GetLastError()));
        } 
    }

    this->filePath = filePath;
    this->inFilePath = inFilePath;
    this->outputFilePath = outputFilePath;
}

// load to line dwBytesInLine characters from file in hfileRead
void FileUtilsWin32API::loadFromFile(char* line, DWORD dwBytesInLine)
{
    DWORD dwBytesRead = 0;
    
    if(not ReadFile(this->hfile, line, dwBytesInLine, &dwBytesRead, NULL))
    {
        throw std::runtime_error("Could not write to file: " + this->filePath + "\nError Code: " + std::to_string(GetLastError()));
    }
}

// store dwBytesToWrite from buffer in hfileWrite
void FileUtilsWin32API::storeInFile(char* buffer, DWORD dwBytesToWrite)
{
    DWORD dwBytesWritten = 0;

     if(not WriteFile(this->hfile, buffer, dwBytesToWrite, &dwBytesWritten, NULL))
    {
        throw std::runtime_error("Could not write to file: " + this->filePath + "\nError Code: " + std::to_string(GetLastError()));
    }
}

// rewind the file pointer to the start of the file
void FileUtilsWin32API::rewind()
{
    bool value = SetFilePointer(this->hfile, 0, NULL, 0);

    if(value == INVALID_SET_FILE_POINTER)
    {
        throw std::runtime_error("Could not rewind the file: " + this->filePath + "\nError Code: " + std::to_string(GetLastError()));
    }
}

// getter for the private variable that is the path to the file
std::string FileUtilsWin32API::get_inFilePath()
{
    return this->inFilePath; 
}

// destructor that binds life time of object to its resources
FileUtilsWin32API::~FileUtilsWin32API() 
{
    CloseHandle(this->hfile); // close the handler of the file resource 

    // check if it the temporary segment file
    if(this->filePath != this->outputFilePath and this->inFilePath != this->filePath)
    {
        char str[14];

        for(int i = 0; i < 14;i++)
        {
            str[i] = filePath[i];
        }
        std::remove(str); // remove segment file
    }
}



//-----------------------------------------------------------------------------------------------------------------------------------------//
// note_from_interview{here is the public on the base class in inheritance                                                              }  //
//                    {I used it here because it is necessary for me to keep the public/protected/private of member or function as is   }  //
//                    {explanation: minimize the original  public/protected/private of a member or function in sub class                }  //
//                    {that arrived from base class with the public/protected/private of the class                                      }  //
//--------------------------------------------------------------------------------------------------------------------------------  -------//

/*-------------------------------------------------------------------------------------------------------
 * Class for segment file methods and contains the utilities that uses win32 API for reading and writing
 *-------------------------------------------------------------------------------------------------------*/


// Constructor of segment file that uses win32 API for reading and writing from files
SegmentFileWin32API::SegmentFileWin32API(const std :: string & filePath, const std :: string & inFilePath, const std :: string & outputFilePath, int counter_lines_in_output_file, char mode):FileUtilsWin32API(filePath, inFilePath, outputFilePath, mode)
{
    this->counter_lines_in_output_file = 0;
    this->relevant_line = "";
}


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
SegmentFileWin32API::~SegmentFileWin32API()
{
}


// advance by one line counter of lines from this segment in output file        
void SegmentFileWin32API::advanceLineCounter()
{
    this->counter_lines_in_output_file++;
}

// getter for the private variable that counts the lines from this segment in output file
int SegmentFileWin32API::getLineCounter()
{
    return counter_lines_in_output_file;
}





//
// Class that contains utilities in files and mainly the method to sort a huge file
//



FileSort::FileSort(int maxFileSizeBytes, int numberOfLinesPerSegment, int lineSizeBytes)
{
    this->maxFileSizeBytes = maxFileSizeBytes;
    this->numberOfLinesPerSegment = numberOfLinesPerSegment;
    this->lineSizeBytes = lineSizeBytes;
}

/*
 * Algorithm Overview:
 *   [1] Take the big input file and split the file into small segments of files
 *   [2] Sort each segment separately
 *   [3] Merge the segment line by line (merge-sort like merge) and insert the lines one by one in a sorted way to the output file
 *   [4] Cleanup all the files that were created in [1] of the file sort
 */
void FileSort::Sort(const std :: string & inFilePath, const std :: string &outFilePath)
{
    // try catch to deal with exceptions
    try
    {
        std::vector<std::unique_ptr<SegmentFileWin32API>> segments; // vector of segments
        check_valid_input_file(inFilePath); // check if the input file is valid and may throw invalid argument exception

        split_to_file_segments(segments, inFilePath, outFilePath); // split file to segment and sort each may throw run time exception in winapi32 API functions

        merge_all_segments_to_out_file(segments, inFilePath, outFilePath); // merge the segments may throw run time exception in winapi32 API functions
   }
    catch(std::invalid_argument & e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch(std::runtime_error & e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Error in Sort function!" << std::endl;
    }
}

// step [1] in the sorting algorithm to seperaete the file into smaller segments
void FileSort::split_to_file_segments(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments,const std :: string & inFilePath, const std :: string & outputFilePath)
{
    char inFileMode = 'r';

    // numebr of lines is size in bytes of file to size in bytes of lines 
    int file_num_lines = get_size_in_bytes(inFilePath) / this-> lineSizeBytes;
    int number_of_segments = file_num_lines / this-> numberOfLinesPerSegment; 


    std::string textExtention = ".txt";
    
    int counterLinesInOutputFile = 0;
    
    char segmentMode = 'b';
    
    std::unique_ptr<FileUtilsWin32API> inFile = std::unique_ptr<FileUtilsWin32API>(new FileUtilsWin32API(inFilePath, inFilePath, outputFilePath, inFileMode));

    // create was each segment an object with a path for temporary segment file and fill the segment
    for(int segment_number = 0; segment_number < number_of_segments; segment_number++)
    {
        std::string segmentPath = "segment" + std::to_string(segment_number + 11) + textExtention;
        segments.push_back(std::unique_ptr<SegmentFileWin32API>(new SegmentFileWin32API(segmentPath, inFilePath, outputFilePath, counterLinesInOutputFile, segmentMode)));
        split_file_to_segment(inFile, segments[segment_number]);
    }

}

// fill the segment file with lines from the huge file
void FileSort::split_file_to_segment(std::unique_ptr<FileUtilsWin32API> & inFile, std::unique_ptr<SegmentFileWin32API> & segment)
{
    char segmentLine[this->lineSizeBytes + 1];

    DWORD length = this->lineSizeBytes;
    DWORD segmentSizeInBytes= this-> lineSizeBytes * this-> numberOfLinesPerSegment;

    std::string strSegmentLines;
    std::vector<std::string> segmentLines;

    // load lines from the huge file to vector in order to sort them
    for(int lineNum = 0; lineNum < this-> numberOfLinesPerSegment; lineNum++)
    {
        inFile->loadFromFile(segmentLine, length);
        segmentLine[length] = '\0';

        for(int i = 0; i < length - 2; i++)
        {
            if(segmentLine[i] == '\r' or segmentLine[i] == '\n') // check if the line is too short
            {
                throw std::invalid_argument("Too short line: " + std::string(segmentLine) + "\nAll lines must be at length " + 
                      std::to_string(this->lineSizeBytes) + " including \\r\\n as 2 bytes at the end");
            }
        }

        // check if the line is too long
        if(segmentLine[length - 2] != '\r' or segmentLine[length - 1] != '\n')
        {
            throw std::invalid_argument("Too long line: " + std::string(segmentLine) + "\nAll lines must be at length" +
                  std::to_string(this->lineSizeBytes) + " including \\r\\n as 2 bytes at the end");
        }
        // save all lines in a vector
        segmentLines.push_back(std::string(segmentLine));
    }               
        
    // step [2] in the sorting algorithm sort the segment lines    
    sortSegmentLines(segmentLines, strSegmentLines);
    // move from std::string type to array of chars in order to use winapi32 API in store function
    char segmentStr[segmentSizeInBytes + 1];
    for(int i = 0 ; i < segmentSizeInBytes + 1; i++)
    {
        segmentStr[i] = strSegmentLines[i];
    }
    segmentStr[segmentSizeInBytes] = '\0';

    // stores the lines of the segment in the segment file
    segment->storeInFile(segmentStr, segmentSizeInBytes);        
}

// sort the segment lines
void FileSort::sortSegmentLines(std::vector<std::string> & segmentLines, std::string & strSegmentLines)
{
    // sort the lines in the vector in lexicographic order
    std::sort(segmentLines.begin(), segmentLines.end());

    // move the sorted vector of lines to a sorted string of lines in order to load it to the file 
    strSegmentLines = "";
    for(int i = 0 ; i < segmentLines.size(); i++)
    {
        strSegmentLines += segmentLines[i];
    }
}


/*
 * returns the size in bytes of the file in path_to_file
 */
int FileSort::get_size_in_bytes(const std :: string & path_to_file)
{
    FILE *ptr_file = fopen(path_to_file.c_str(), "r");
    fseek(ptr_file, 0, SEEK_END); // move fiile pointer to the end
    // function ftell tells the byte number that the file pointer is on
    int size = ftell(ptr_file); // tells size because the file pointer in the end of the file
    fclose(ptr_file);
    return size;
}

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
void FileSort::merge_all_segments_to_out_file(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments, const std :: string &inFilePath, const std :: string &outFilePath)
{
    char line[this->lineSizeBytes + 1];
    int segment_size_in_bytes = this-> lineSizeBytes * this-> numberOfLinesPerSegment;

    DWORD length = this->lineSizeBytes;

    char outFileMode = 'w'; // writing mode to write to output file
    char segment11Mode = 'r'; // reading more to read from segment files

    std::unique_ptr<FileUtilsWin32API> outFile = std::unique_ptr<FileUtilsWin32API>(new FileUtilsWin32API(outFilePath, inFilePath, outFilePath, outFileMode));
    
    // rewind each segment file 
    for(int segment_number = 0; segment_number < segments.size(); segment_number++)
    {
        // we wrote to them and we need to read from teh start
        segments[segment_number]->rewind();
    }

    // load the relevant line from each segment that might be the minimal and therefore the 
    // next  line to write to output file
    for(int segment_number = 0; segment_number < segments.size(); segment_number++)
    {
        segments[segment_number]->loadFromFile(line, length);
        line[this->lineSizeBytes] = '\0';

        for(int i = 0; i < this->lineSizeBytes; i++)
        {
            segments[segment_number]->relevant_line += line[i];
        }
    }

    // move each line to otuput file and advance line after line while keeping the invariant
    while(isLineLeftInSegments(segments))
    {
        store_next_min_line(segments, length, outFile);
    }    
}

// check the counters of each segment to see if some lines left to be written to output file
bool FileSort::isLineLeftInSegments(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments)
{
    bool leftLine = false; 

    for(int segment_number = 0; segment_number < segments.size(); segment_number++)
    {   
        // check counter of segment
        if(segments[segment_number]->getLineCounter() < this-> numberOfLinesPerSegment)
        {
            leftLine = true;
        }
    }

    return leftLine;
}

// make the step of storing the next minimum line from the relevant lines and 
// advance in  this segment to the next line
void FileSort::store_next_min_line(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments, DWORD length, std::unique_ptr<FileUtilsWin32API> & outFile)
{
    char min_line[this->lineSizeBytes + 1];
    char * line;
    int min_segment_num;

    // find a relevant line and 
    // there is because we entered the function from the while loop that checks
    for(int segment_number = 0; segment_number < segments.size(); segment_number++)
    {
        if(segments[segment_number]->getLineCounter() < this->numberOfLinesPerSegment)
        {
            for(int i = 0; i < this->lineSizeBytes - 2; i++)
            {
                 min_line[i] = segments[segment_number]->relevant_line[i];
            }
            min_line[lineSizeBytes - 2] = '\r';
            min_line[lineSizeBytes - 1] = '\n';
            min_line[lineSizeBytes] = '\0';
            min_segment_num = segment_number;
            break;
        }
    }


    // compare other relevant lines with the minimum to find the global minimum of the relevant lines
    for(int segment_number = min_segment_num + 1; segment_number < segments.size(); segment_number++)
    {
        if(segments[segment_number]->getLineCounter() < this->numberOfLinesPerSegment and segments[segment_number]->relevant_line.compare(std::string(min_line)) < 0)
        {
            for(int i = 0; i < this->lineSizeBytes - 2; i++)
            {
                 min_line[i] =  segments[segment_number]->relevant_line[i];
            }
            min_line[lineSizeBytes - 2] = '\r';
            min_line[lineSizeBytes - 1] = '\n';
            min_line[lineSizeBytes] = '\0';
            min_segment_num = segment_number;
        }
    }
    // store the minimum line
    outFile->storeInFile(min_line, length);
    // step to the next line in the segment of the minimum line
    update_min_segment_next_line(segments, length, min_segment_num);
}

// step to the next line in the segment
void FileSort::update_min_segment_next_line(std::vector<std::unique_ptr<SegmentFileWin32API>> & segments, int length, int min_segment_num)
{        
    // advance the counter
    segments[min_segment_num]->advanceLineCounter();

    // check if there is a next line to be written to the output file
    if(segments[min_segment_num]->getLineCounter() < this->numberOfLinesPerSegment)
    {
        std::string filepath = segments[min_segment_num]->get_inFilePath(); 

        char line[this->lineSizeBytes + 1];

        segments[min_segment_num]->loadFromFile(line, length);
        line[lineSizeBytes] = '\0';

        segments[min_segment_num]->relevant_line = std::string(line);
    }
}

// check valid input file
void FileSort::check_valid_input_file(const std :: string & inFilePath)
{
    bool isValid = true;
    // check if the input file exists
    if(CreateFile(inFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) == INVALID_HANDLE_VALUE)
    {
        throw std::invalid_argument("File: " + inFilePath + "\nis not exists. Enter a valid file path next time.");
    }

    int file_size_in_bytes = get_size_in_bytes(inFilePath);
    // check if the file is too big
    if(file_size_in_bytes > this-> maxFileSizeBytes)
    {
        throw std::invalid_argument("File: " + inFilePath + "\nis too big. More than " + 
               std::to_string(this-> maxFileSizeBytes) + " bytes.");
    } 

    int segment_size_in_bytes = this-> lineSizeBytes * this-> numberOfLinesPerSegment;

    // check if the file can be divided into segments
    if(file_size_in_bytes % segment_size_in_bytes != 0)
    {
        throw std::invalid_argument("File: " + inFilePath + "\nis not dividable into the segments.");
    }
}   
 
