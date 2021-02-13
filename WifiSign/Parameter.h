/*
 * Parameter.h
 *
 *  Created on: Feb 8, 2021
 *      Author: tsasala
 */
#pragma once

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <Arduino.h>

class Parameter
{
public:
	Parameter();
	~Parameter();

	String key;
	String value;
};

#endif /* PARAMETER_H_ */
