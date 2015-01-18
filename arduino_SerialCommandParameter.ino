// Do not remove the include below
#include "Sprinter_UNO.h"

#define STATUS_OK 0
#define STATUS_SD 1
#define STATUS_ERROR 2

#define ERROR_CODE_NO_ERROR             0
#define ERROR_CODE_HOTEND_TEMPERATURE   1
#define ERROR_CODE_BED_TEMPERATURE      2
const char *error_code_str[] = { "No Error", "Hotend", "Bed" };
const char *status_str[] = { "Ok", "SD", "Error"};

#define COMMAND_CRC_ENABLE true

//Printer status variables
int status = STATUS_OK; //
int error_code = ERROR_CODE_NO_ERROR; //0=Nothing, 1=Heater thermistor error

long command_No, command_LastNo;


// comm variables
#define MAX_CMD_SIZE 60
#define BUFSIZE 6
char cmdbuffer[BUFSIZE][MAX_CMD_SIZE];


int bufindr = 0;
int bufindw = 0;
int buflen = 0;
int i = 0;
char serial_char;
int serial_count = 0;
boolean comment_mode = false;
char *strchr_pointer; // just a pointer to find chars in the cmd string like X, Y, Z, E, etc




//Inactivity shutdown variables
unsigned long previous_millis_cmd = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("start");
}

void get_command();
void process_commands();

void FlushSerialRequestResend();
void ClearToSend();

void loop()
{


    if(buflen < 3)
        get_command();

    if(buflen)
    {

        process_commands();

        buflen = (buflen - 1);
        bufindr = (bufindr + 1) % BUFSIZE;
    }
    //check heater every n milliseconds
    manage_heater();

}


inline void get_command()
{
    while( Serial.available() > 0  && buflen < BUFSIZE)
    {
        serial_char = Serial.read();
        if(serial_char == '\n' || serial_char == '\r' || serial_char == ':' || serial_count >= (MAX_CMD_SIZE - 1) )
        {
            if(!serial_count) return; //if empty line
            cmdbuffer[bufindw][serial_count] = 0; //terminate string
            if(!comment_mode)
            {

                if(strstr(cmdbuffer[bufindw], "N") != NULL)
                {
                    strchr_pointer = strchr(cmdbuffer[bufindw], 'N');
                    command_No = (strtol(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL, 10));
                    if(command_No != command_LastNo + 1 && (strstr(cmdbuffer[bufindw], "M110") == NULL) )
                    {
                        Serial.print("Serial Error: Line Number is not Last Line Number+1, Last Line:");
                        Serial.println(command_LastNo);
                        //Serial.println(gcode_N);
                        FlushSerialRequestResend();
                        serial_count = 0;
                        return;
                    }

                    if(strstr(cmdbuffer[bufindw], "*") != NULL)
                    {
                        byte checksum = 0;
                        byte count = 0;
                        while(cmdbuffer[bufindw][count] != '*') checksum = checksum ^ cmdbuffer[bufindw][count++];
                        strchr_pointer = strchr(cmdbuffer[bufindw], '*');

                        if( (int)(strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL)) != checksum)
                        {
                            Serial.print("Error: checksum mismatch, Last Line:");
                            Serial.println(command_LastNo);
                            FlushSerialRequestResend();
                            serial_count = 0;
                            return;
                        }
                        //if no errors, continue parsing
                    }
                    else
                    {
                        Serial.print("Error: No Checksum with line number, Last Line:");
                        Serial.println(command_LastNo);
                        FlushSerialRequestResend();
                        serial_count = 0;
                        return;
                    }

                    command_LastNo = command_No;
                    //if no errors, continue parsing
                }
                else  // if we don't receive 'N' but still see '*'
                {
                    if((strstr(cmdbuffer[bufindw], "*") != NULL))
                    {
                        Serial.print("Error: No Line Number with checksum, Last Line:");
                        Serial.println(command_LastNo);
                        serial_count = 0;
                        return;
                    }
                }
                if((strstr(cmdbuffer[bufindw], "G") != NULL))
                {
                    strchr_pointer = strchr(cmdbuffer[bufindw], 'G');
                    switch((int)((strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL))))
                    {
                    case 0:
                    case 1:
                        Serial.println("ok");
                        break;
                    default:
                        break;
                    }

                }
                bufindw = (bufindw + 1) % BUFSIZE;
                buflen += 1;

            }
            comment_mode = false; //for new command
            serial_count = 0; //clear buffer
        }
        else
        {
            if(serial_char == ';') comment_mode = true;
            if(!comment_mode) cmdbuffer[bufindw][serial_count++] = serial_char;
        }
    }


}


inline float code_value()
{
    return (strtod(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL));
}
inline long code_value_long()
{
    return (strtol(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL, 10));
}
inline bool code_seen(char code_string[])
{
    return (strstr(cmdbuffer[bufindr], code_string) != NULL);    //Return True if the string was found
}

inline bool code_seen(char code)
{
    strchr_pointer = strchr(cmdbuffer[bufindr], code);
    return (strchr_pointer != NULL);  //Return True if a character was found
}

inline void process_commands()
{
    unsigned long codenum; //throw away variable
    char *starpos = NULL;

    if(code_seen('G'))
    {
        switch((int)code_value())
        {
        case 4: // G4 dwell
            codenum = 0;
            if(code_seen('P')) codenum = code_value(); // milliseconds to wait
            if(code_seen('S')) codenum = code_value() * 1000; // seconds to wait
            break;
        }
    }

    else if(code_seen('M'))
    {

        switch( (int)code_value() )
        {
        case 4: //Ask for status
            break;
        case 5: //Reset errors
            status = STATUS_OK;
            error_code = ERROR_CODE_NO_ERROR;
            break;
        }

    }
    else
    {
        Serial.println("Unknown command:");
        Serial.println(cmdbuffer[bufindr]);
    }

    ClearToSend();

}

int byteToint(byte value)
{
    if(value >> 7)
    {
        return (255 - value + 1) * -1;
    }
    return value;
}

void FlushSerialRequestResend()
{
    //char cmdbuffer[bufindr][100]="Resend:";
    Serial.flush();
    Serial.print("Resend:");
    Serial.println(command_LastNo + 1);
    ClearToSend();
}

void ClearToSend()
{
    previous_millis_cmd = millis();
    if (status == STATUS_ERROR)
    {
        Serial.print("EC:");
        Serial.println(error_code);
        Serial.print(", ");
        Serial.print(error_code_str[error_code]);
    }

    Serial.println("ok");
}


void manage_heater()
{
}
