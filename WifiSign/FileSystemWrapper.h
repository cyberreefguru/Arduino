/*
 * FileSystemWrapper.h
 *
 *  Created on: Jan 21, 2021
 *      Author: tsasala
 */

#pragma once

#include <Arduino.h>
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_ImageReader.h> // Image-reading functions
#include "Debug.h"


class FileSystemWrapper {
public:
	FileSystemWrapper();
	boolean initialize();
	FatFileSystem* getFileSystem();

private:
	boolean config = false;

};
