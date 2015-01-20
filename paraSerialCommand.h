#ifndef _SERIAL_COMMAND_PARAMETER_H_
#define	_SERIAL_COMMAND_PARAMETER_H_

//=======================================================================
//   Attention:
//		1.You need to include this Library to your project folder
//		2.rewrite the function: processCommand() to add your features.
//		3.call cmd_in_loop() in your loop()
//		4.dont add delay() functions in your loop(), 
//										nor functions in loop()
//		5.
//
//											20Jan2015	CAOXP@HONGKONG
//=======================================================================



	#include "Arduino.h"
	extern void cmd_in_loop();

	//Configurations below

//=======================================================================
//                      command line defination
//=======================================================================

#define COMMNAD_LINE_ENABLE false       //N2 G0 X2
//check the CRC, if '*' included.
#define COMMAND_CRC_ENABLE  false       //N2 G0 X2 *23



//=======================================================================
//                      Command Buffer
//
//=======================================================================

#define MAX_CMD_SIZE        80      //length of each buffer
#define CMD_BUF_SIZE        5       //how many buffers

	
#endif