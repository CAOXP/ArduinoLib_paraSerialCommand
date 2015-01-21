#include "paraSerialCommand.h"

//=======================================================================
//   Attention:
//		1.You need to include this Library to your project folder
//		2.rewrite the function: processCommand() to add your features.
//
//=======================================================================


void other_managements_test()
{
	//add your code here
	// to precess other stuff
}

void setup()
{
    Serial.begin(115200);
    Serial.println("start");
}



void loop()
{
	cmd_in_loop();			//receive serial commands.process the serial commands.


    //test,every n milliseconds
    other_managements_test();
}



//=======================================================================
//                      ProcessCommand   need change dependently
//
//			called by processSerialCommands() in paraSericalCommandLib;
//=======================================================================

void commandProcess()
{