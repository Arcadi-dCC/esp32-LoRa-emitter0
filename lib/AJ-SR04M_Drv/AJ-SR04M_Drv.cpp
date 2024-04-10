#include <Arduino.h>

#include <AJ-SR04M_Drv.h>
#include <AJ-SR04M_Cfg.h>

RTC_DATA_ATTR bool AJ_SR04M_configured = false;

//Checks if the selected mode of operation is implemented. If so, configures the sensor accordingly.
//Configuration performed once, at the first MCU run.
//Returns 0 if successful, 1 if mode of operation is not implemented.
uint8 AJ_SR04M_Config(void)
{
    if(!AJ_SR04M_configured)
    {
        AJ_SR04M_configured = true;
        if(OP_MODE != 2U)
        {
            return 1U;
        }
        pinMode(TRIG_PIN, OUTPUT);
        pinMode(ECHO_PIN, INPUT);
        digitalWrite(TRIG_PIN, LOW);
    }
    return 0U;
}

//Asks the AJ_SR04M untrasound sensor to measure the current distance in centimeters (cm), and returns it through reference variable.
//[Blocking]. Max blocking time = 1s
//If operation is not successful, previous distance value is not modified.
//Returns 0 if successful, 1 if selected mode of operation is not implemented, 2 if sensor did not respond.
uint8 AJ_SR04M_Distance(uint16* distance)
{
    uint16 time = 0U; 
    switch(OP_MODE)
    {
        case (2U):
        {
            digitalWrite(TRIG_PIN, LOW);
            delayMicroseconds(2);
            digitalWrite(TRIG_PIN, HIGH);
            delayMicroseconds(1100);
            digitalWrite(TRIG_PIN, LOW);

            time = pulseIn(ECHO_PIN, HIGH); //time spent by ultrasound wave going and returning (us)

            if(!time)
            {
                return 2U;
            }
            
            //distance[cm] = (time[us] / (10^6 * 2))* 343[m/s] * 100 
            *distance = time * 0.01715;            
            break;
        }
        default:
        {
            return 1U;
        }
    }
    return 0U;
}
 