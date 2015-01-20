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
    cmd_in_loop();
    //test,every n milliseconds
    other_managements_test();
}