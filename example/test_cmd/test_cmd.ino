#include "arduino_SerialCommandParameter.h"



//=======================================================================
//                      Setup and Loop and others
//=======================================================================
void other_managements_test()
{
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