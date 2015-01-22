
//=======================================================================
//   Attention:
//		1.You need to include this Library to your project folder
//		2.rewrite the function: processCommand() to add your features.
//
//=======================================================================
#include "paraSerialCommand.h"

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
extern char cmdbuffer[CMD_BUF_SIZE][MAX_CMD_SIZE];
extern char *strchr_pointer; extern unsigned int bufindr;
inline float cmd_value() {return (strtod(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL));}
inline long  cmd_value_long() {return (strtol(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL, 10));}
inline bool  cmd_seen(char code_string[]) { return (strstr(cmdbuffer[bufindr], code_string) != NULL);}    //Return True if the string was found
inline bool  cmd_seen(char code){strchr_pointer = strchr(cmdbuffer[bufindr], code); return (strchr_pointer != NULL); } //Return True if a character was found

void commandProcess()
{   
 	if(cmd_seen('G'))
    {
        switch((int)cmd_value())
        {
        case 3:           
            Serial.print("Command received:");
            if(cmd_seen('P')) 
            {
                Serial.print("P:");Serial.print(cmd_value());
            }

            if(cmd_seen('S')) 
            {
                Serial.print(" S:");Serial.print(cmd_value(),2);
            }
            Serial.println(); 
            break;
        }
    }

    else if(cmd_seen('M'))
    {
        switch( (int)cmd_value() )
        {
        case 4: //Ask for status
            break;
        case 5: //Reset errors
            break;
        }
    }
    else
    {
        Serial.println("Unknown command");
    }
}    
