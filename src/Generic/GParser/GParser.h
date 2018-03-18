#ifndef H_GENERIC_GPARSER_H
#define H_GENERIC_GPARSER_H


// System include files
#include <iostream>
#include <fstream>
#include <string>


// Constant definitions


// GParser_Line struct:
// This structure defines a line already parsed. You can access the key and
// value in it.
struct GParser_Line
{
    // Key of the line
    std::string key;

    // Value of the line
    std::string value;
};


// GParser class:
// This class is associated to the parsing of a file. It advances through the
// file and notifies when it has finished so it can be destroyed. You can parse
// each line as title of section or data. Lines starting with '#' are
// considered comments.
class GParser
{
    public:
        // Constructor: opens the file given, throws an exception if it cannot
        // open it.
        GParser(const char *pathToFile);

        // Destructor: closes the file.
        ~GParser();

        // Read next title: advances until a section title line is found.
        bool readNextTitle(GParser_Line &lineToFill);

        // Read next data: advances until a data definition line is found.
        bool readNextData(GParser_Line &lineToFill);

    private:
        // ---- CONSTANTS ----
        // Return values for readNextLine function
        static const int LINE_TYPE_END_OF_FILE = 0;
        static const int LINE_TYPE_END_OF_SECTION = 1;
        static const int LINE_TYPE_TITLE = 2;
        static const int LINE_TYPE_DATA = 3;

        // ---- FIELDS ----
        // Input stream from the file.
        std::ifstream _file;

        // String with the current line read.
        std::string _currLine;

        // String with the last section opened
        std::string _lastSection;

        // Number of the current line being read
        int _lineNum;

        // ---- METHODS ----
        // Read next line: advances until a meaningful line (non whitespace,
        // non comment) is found.
        int readNextLine();

        // Is line significant: checks if the line is relevant or not. If it
        // is, it removes leading and trailing whitespace.
        bool isLineSignificant(std::string &string);

        // Remove surrounding whitespace: removes leading and trailing
        // whitespace from the string.
        void removeSurrWhitespace(std::string &string);

};


#endif // H_GENERIC_GPARSER_H
