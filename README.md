# Arduino_SerialCommandParameter
serial command with parameters, changed from SPRINTER

##Format
<br>command format1 :   N1 GD0 X20 Y30 Z30 E23 *53
<br>command format2 :   G0 X20 Y30 Z40
<br>response format :   ok

##Functions
<br>with a #define to switch CMD-LINE on/off
<br>CRC will be checked if '*' included in the command
