#pragma once
#include "cocos2d.h"
class FileHandler
{
public:
	FileHandler();
	~FileHandler();

	/**reading settings values from custom .txt file**/
	static void readSettingsFromFile(int *countTarget, int *speed, int *time);

};

