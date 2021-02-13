/*
 * FileSystemWrapper.cpp
 *
 *  Created on: Jan 21, 2021
 *      Author: tsasala
 */

#include "FileSystemWrapper.h"

Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS, PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
Adafruit_SPIFlash    flash(&flashTransport);
FatFileSystem        filesys;

FileSystemWrapper::FileSystemWrapper()
{
	config = false;
}

boolean FileSystemWrapper::initialize()
{
	boolean status = config;
	if( !config )
	{
		// SPI or QSPI flash requires two steps, one to access the bare flash
		// memory itself, then the second to access the filesystem within...
		if(flash.begin())
		{
			if(filesys.begin(&flash))
			{
//				Serial.println("File System Data:");
//				filesys.ls(&Serial, "/", 0);
//				Serial.println("Dir List Complete");
				status = true;
				config = true;
			}
			else
			{
				LOG_ERROR(F("filesys begin() failed"));
			}
		}
		else
		{
			LOG_ERROR(F("flash begin() failed"));
		}
	}
    return status;
}

FatFileSystem* FileSystemWrapper::getFileSystem()
{
	return &filesys;
}
