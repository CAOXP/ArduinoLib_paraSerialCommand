
#include "paraSerialCommand.h"

//=======================================================================
//                      System status
//=======================================================================
#define STATUS_OK       0
#define STATUS_SD       1
#define STATUS_ERROR    2

#define ERROR_CODE_NO_ERROR     0
#define ERROR_CODE_ERROR_1      1
#define ERROR_CODE_ERROR_2      2

const char *error_code_str[] =  {   "No Error", 
                                    "Error 1", 
                                    "Error 2" };

const char *sysstatus_str[]  =  {   "Ok", 
                                    "SD", 
                                    "Error"};

unsigned int sysstatus  = STATUS_OK;
unsigned int error_code = ERROR_CODE_NO_ERROR; 


//=======================================================================
//                      Command Buffer
//=======================================================================

char cmdbuffer[CMD_BUF_SIZE][MAX_CMD_SIZE];

//extern char *strstr(char *sr,char *s);
//  搜索字串s在字串sr中的第一次出现的地址    返回为[实际]地址指针 char *
//extern char *strchr(char *sr,char c);
//  查找字串sr中首次出现字符c的位置的地址    返回为[实际]地址指针 char *
char *strchr_pointer; // just a pointer to find chars in the cmd string

//buffer threshold
// getCommand() from PC, if 'buflen' lower than this
#define CMD_BUF_THRESHOLD  (CMD_BUF_SIZE-1)  

unsigned int bufindr = 0;    //buffer index writing
unsigned int bufindw = 0;    //buffer index reading
unsigned int buflen  = 0;    //how many COMMANDS current in buffer

char serial_char;               
unsigned int i = 0;
unsigned int serial_count = 0;



#if COMMNAD_LINE_ENABLE || COMMAND_CRC_ENABLE
const char *cmd_error_code_str[] =  {   "Serial Error: LineNumber is not Last Line Number+1", 
                                        "Serial Error: LineNumber Loss!", 
                                        "Serial Error: CRC:Mismatch!" ,
                                        "Serial Error: CRC:No CRC!",};
#define CMD_ERROR_NO_ERROR                  0
#define CMD_ERROR_LINE_NUMBER_MISMATCH      (1<<0)
#define CMD_ERROR_LINE_NUMBER_LOSS          (1<<1)
#define CMD_ERROR_CRC_MISMATCH              (1<<2)
#define CMD_ERROR_CRC_LOSS                  (1<<3)
unsigned long command_No=0;
unsigned long command_LastNo=0;
byte cmd_error=CMD_ERROR_NO_ERROR;
#endif


#if CMD_PROCESS_CLEAR_SEND
inline void ClearToSend()
{
    //previous_millis_cmd = millis();
    if (sysstatus == STATUS_ERROR)
    {
        Serial.print("EC:");
        Serial.println(error_code);
        Serial.print(", ");
        Serial.print(error_code_str[error_code]);
    }

    Serial.println("ok");
}
#endif

#if COMMNAD_LINE_ENABLE || COMMAND_CRC_ENABLE || CMD_REQUEST_RESEND
inline void FlushSerialRequestResend()
{
    //char cmdbuffer[bufindr][100]="Resend:";
    Serial.flush();
    Serial.print("Resend:");

    #if COMMNAD_LINE_ENABLE
    Serial.println(command_LastNo + 1);
    #else
    Serial.println();

    #endif
    #if CMD_PROCESS_CLEAR_SEND
    ClearToSend();
    #endif
}
#endif


inline void getCommand()
{
    while( Serial.available() > 0  && buflen < CMD_BUF_SIZE)
    {
        //read the serial by BYTE
        serial_char = Serial.read();   

        //check the line ending
        // by \n \r : over-count
        if(serial_char == '\n' || serial_char == '\r' || serial_char == ':' || serial_count >= (MAX_CMD_SIZE - 1) )
        {
            if(!serial_count) return;               //if empty line
            cmdbuffer[bufindw][serial_count] = 0;   //terminate string
            serial_count = 0;                       //index starts again

            #if COMMAND_COMMENT_MODE
            if(cmdbuffer[bufindw][0] == ';')return;
            #endif

            #if COMMNAD_LINE_ENABLE || COMMAND_CRC_ENABLE
            cmd_error=CMD_ERROR_NO_ERROR;
            #endif

            #if COMMNAD_LINE_ENABLE

            //some commands do not need command line number.
            // if( strstr(cmdbuffer[bufindw], "RESET") != NULL  )
            // {}
            // else
            if(strstr(cmdbuffer[bufindw], "N") == NULL)
            {
                //tag error
                cmd_error |= CMD_ERROR_LINE_NUMBER_LOSS;
            }
            else
            {
                strchr_pointer = strchr(cmdbuffer[bufindw], 'N');

                //CHECK the command line
                command_No = (strtol(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL, 10));

                if( command_No != (command_LastNo + 1) )
                {
                    //tag error
                    cmd_error |= CMD_ERROR_LINE_NUMBER_MISMATCH;
                }
            }
            #endif

            #if COMMAND_CRC_ENABLE
            //CHECK the CRC
            if(strstr(cmdbuffer[bufindw], "*") == NULL)
            {
                //tag error
                cmd_error |= CMD_ERROR_CRC_LOSS;
            }
            else
            {
                byte checksum = 0;
                byte count    = 0;
                while(cmdbuffer[bufindw][count] != '*') checksum = checksum ^ cmdbuffer[bufindw][count++];
                strchr_pointer = strchr(cmdbuffer[bufindw], '*');

                if( (int)(strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL)) != checksum)
                {
                    //tag error
                    cmd_error |= CMD_ERROR_CRC_MISMATCH;
                }
            }

            #endif

            #if COMMNAD_LINE_ENABLE || COMMAND_CRC_ENABLE
            if( cmd_error !=  CMD_ERROR_NO_ERROR)
            {
                for(i=0; i<4; i++)
                {
                    if(cmd_error & (1<<i))
                    {
                        Serial.println(cmd_error_code_str[i]);
                    }
                }

                Serial.print("Serial Error: Last Line:");
                Serial.println(command_LastNo);
                FlushSerialRequestResend();
                serial_count = 0;
                return;
            }
            #endif

            #if COMMNAD_LINE_ENABLE
            //if no errors, continue parsing
            command_LastNo = command_No;
            #endif

            #if 0
            //additional pre-process the command
            //No inflect to running speed.
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
            #endif
            
            //add buffer record
            bufindw = (bufindw + 1) % CMD_BUF_SIZE;
            buflen += 1;
        }
        else
        {
            //continue to receive the command char into buffer
            cmdbuffer[bufindw][serial_count++] = serial_char;
        }

    }//while(...)
}



//=======================================================================
//                      ProcessCommand   need change dependently
//=======================================================================

void cmd_in_loop()
{
    if(buflen < CMD_BUF_THRESHOLD)
    {
        getCommand();   //one char per one call
    }

    //process the commands below
    if(buflen)
    {
        commandProcess();

        #if CMD_PROCESS_CLEAR_SEND
        ClearToSend();
        #endif

        //buffer pointer and buffer length
        buflen  --;
        bufindr ++;
        bufindr %= CMD_BUF_SIZE;
    }
}
