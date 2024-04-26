#ifndef AJ_SR04M_Cfg_H
#define AJ_SR04M_Cfg_H

/*  MODES OF OPERATION                  |   R19 VALUE   |   IMPLEMENTED
    ==================================  |   ==========  |   ===========
    [1] -> Normal square wave pulse     |   NOT PLACED  |   no
    [2] -> Low-power square wave pulse  |   300   kOhm  |   YES
    [3] -> Automatic Serial             |   120   kOhm  |   no
    [4] -> Serial Trigger               |   47    kOhm  |   no
    [5] -> ASCII code output            |   0      Ohm  |   no
*/
#define OP_MODE     2U      //Mode of operation

#define TRIG_PIN    25U     //Trigger pin
#define ECHO_PIN    4U      //Echo pin

#endif //AJ_SR04M_Cfg_H
