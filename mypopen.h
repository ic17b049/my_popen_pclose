/**
* @file mypopen.h
* Betriebssysteme MyPopen File
* Beispiel 2
*
* @author Dominic Schebeck 			<ic17b049@technikum-wien.at>
* @author Thomas  Neugschwandtner 	<ic17b082@technikum-wien.at>
* @author Dominik Rychly 			<ic17b052@technikum-wien.at>
*
* @date 05/03/2018
*
* @version 1.0
*
*/

#ifndef MYPOPEN_H
	#define MYPOPEN_H

	#include <stdio.h>

	extern FILE *mypopen(const char *command, const char *type);
	extern int mypclose(FILE *stream);
#endif