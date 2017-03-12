#include "FileHandler.h"
#include "Constants.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace std;

FileHandler::FileHandler()
{
}


FileHandler::~FileHandler()
{
}

void FileHandler::readSettingsFromFile(int *countTarget, int *speed, int *time) {
	int* settings[3] = { countTarget, speed, time };//array of pointers
	
	std::string filePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(inputFileName);
	ifstream fin(filePath);

	if (!fin.is_open()) { //if file isn't opened, setting up default values
		*settings[0] = defCountTargets;
		*settings[1] = defSpeed;
		*settings[2] = defTime;
	}
	else//if file is opened, then read file line by line
	{
        std::string str;
		int i = 0;//setting's array index
		
		for (int i=0; i<3;i++)
		{
		std::getline(fin, str);//get line from file
        int index = str.find("=");//find first occurence with "="
		std::string subStr = str.substr(index+1);//separating substring with numeric value
		int value = std::stoi(subStr);//converting string to integer
		*settings[i] = abs(value);//assign value to current pointer
		}
		
		fin.close();//closing file
	}
}
