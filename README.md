# Arduino_SerialCommandParameter
serial command with parameters, changed from SPRINTER

##Format
<br>command format :   N1 GD0 X20 Y30 Z30 E23 *53
<br>response format:   OK

##Functions
<br>with a #define to switch CMD-LINE on/off
<br>CRC will be checked if '*' included in the command
