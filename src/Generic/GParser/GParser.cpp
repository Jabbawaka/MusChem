// Self include files
#include "GParser/GParser.h"

// Project include files
#include "GLogger/GLogger.h"

// System include files
#include <iostream>
#include <fstream>
#include <string>


GParser::GParser(const char *pathToFile)
{
    // Open file and check
    _file.open(pathToFile);
    if( !_file.is_open() )
    {
        GLOG_ERR("Could not open file %s to parse", pathToFile);
        throw "Could not open file";
    }

    // Initialise line number
    _lineNum = 0;
}

GParser::~GParser()
{
    _file.close();
}

bool GParser::readNextTitle(GParser_Line &lineToFill)
{
    // Line type
    int lineType;

    // Get next title
    do
    {
        lineType = readNextLine();

        if(lineType == LINE_TYPE_END_OF_SECTION)
        {
            //GLOG_WRN("Found closed section at line %d: %s",
                //_lineNum, _lastSection.c_str());
        }
        else if(lineType == LINE_TYPE_DATA)
        {
            //GLOG_WRN("Found data while looking for title at line %d: %s",
                //_lineNum, _currLine.c_str());
        }
        else if(lineType == LINE_TYPE_END_OF_FILE)
        {
            //GLOG_MSG("Reached end of file at line %d", _lineNum);
            return false;
        }
    } while (lineType != LINE_TYPE_TITLE);

    // Parse next title
    size_t delimPos = _currLine.find(":");

    lineToFill.key = _currLine.substr(1, delimPos - 1);
    lineToFill.value =
        _currLine.substr(delimPos + 1, _currLine.length() - delimPos);

    removeSurrWhitespace(lineToFill.key);
    removeSurrWhitespace(lineToFill.value);

    // Copy section
    _lastSection = _currLine.substr(1, delimPos - 1);

    // Log opening section
    //GLOG_MSG("Beggining of section %s at line %d",
        //_lastSection.c_str(), _lineNum);

    return true;
}

bool GParser::readNextData(GParser_Line &lineToFill)
{
    // Line type
    int lineType;

    // Get next title
    do
    {
        lineType = readNextLine();

        if(lineType == LINE_TYPE_END_OF_SECTION)
        {
            //GLOG_MSG("End of section %s at line %d",
            //    _lastSection.c_str(), _lineNum);
            return false;
        }
        else if(lineType == LINE_TYPE_TITLE)
        {
            //GLOG_WRN("Found title while looking for data at line %d: %s",
                //_lineNum, _currLine.c_str());
        }
        else if(lineType == LINE_TYPE_END_OF_FILE)
        {
            //GLOG_WRN("Reached end of file at line %d", _lineNum);
            return false;
        }
    } while (lineType != LINE_TYPE_DATA);

    // Parse next title
    size_t delimPos = _currLine.find(":");

    lineToFill.key = _currLine.substr(0, delimPos);
    lineToFill.value =
        _currLine.substr(delimPos + 1, _currLine.length() - delimPos);

    removeSurrWhitespace(lineToFill.key);
    removeSurrWhitespace(lineToFill.value);

    return true;
}

int GParser::readNextLine()
{
    // Read next line
    if( !std::getline(_file, _currLine) )
    {
        return LINE_TYPE_END_OF_FILE;
    }
    _lineNum++;

    // Read while the lines are full of whitespace
    while( !isLineSignificant(_currLine) )
    {
        if( !std::getline(_file, _currLine) )
        {
            return LINE_TYPE_END_OF_FILE;
        }
        _lineNum++;
    }

    // Check type of line
    switch(_currLine[0])
    {
        case '[':
            return LINE_TYPE_TITLE;
        case ']':
            return LINE_TYPE_END_OF_SECTION;
        default:
            return LINE_TYPE_DATA;
    }
}

bool GParser::isLineSignificant(std::string &string)
{
    size_t firstNonWhite = string.find_first_not_of(" \t\n");

    // Check if it's all whitespace or comment
    if(firstNonWhite == std::string::npos || string[firstNonWhite] == '#')
    {
        return false;
    }

    // Remove leading and trailing whitespace
    removeSurrWhitespace(string);

    return true;
}

void GParser::removeSurrWhitespace(std::string &string)
{
    size_t firstNonWhite = string.find_first_not_of(" \t\n");
    size_t lastNonWhite = string.find_last_not_of(" \t\n");

    std::string tempString =
        string.substr(firstNonWhite, lastNonWhite - firstNonWhite + 1);

    string = tempString;
}
