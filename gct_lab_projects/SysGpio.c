//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

void CSys_GpioInit(int Port, int Pin, int Drn)
{
    switch (Port) {
    case uCPORTA: CCpu_InitPPinA(Pin, Drn);    break;
    case uCPORTB: CCpu_InitPPinB(Pin, Drn);    break;
    case uCPORTC: CCpu_InitPPinC(Pin, Drn);    break;
    case uCPORTD: CCpu_InitPPinD(Pin, Drn);    break;
    case uCPORTE: CCpu_InitPPinE(Pin, Drn);    break;
    case uCPORTF: CCpu_InitPPinF(Pin, Drn);    break;
    }
}

void CSys_GpioSetVal(int Port, int Pin, Bool Val)
{
    switch (Port) {
    case uCPORTA: CCpu_SetPPinAVal(Pin, Val);    break;
    case uCPORTB: CCpu_SetPPinBVal(Pin, Val);    break;
    case uCPORTC: CCpu_SetPPinCVal(Pin, Val);    break;
    case uCPORTD: CCpu_SetPPinDVal(Pin, Val);    break;
    case uCPORTE: CCpu_SetPPinEVal(Pin, Val);    break;
    case uCPORTF: CCpu_SetPPinFVal(Pin, Val);    break;
    }
}

Bool CSys_GpioGetVal(int Port, int Pin)
{
    Bool b = FALSE;
    switch (Port) {
    case uCPORTA: b = CCpu_GetPPinAVal(Pin);    break;
    case uCPORTB: b = CCpu_GetPPinBVal(Pin);    break;
    case uCPORTC: b = CCpu_GetPPinCVal(Pin);    break;
    case uCPORTD: b = CCpu_GetPPinDVal(Pin);    break;
    case uCPORTE: b = CCpu_GetPPinEVal(Pin);    break;
    case uCPORTF: b = CCpu_GetPPinFVal(Pin);    break;
    }
    return b;
}

void CSys_GpioTgl(int Port, int Pin)
{
    switch (Port) {
    case uCPORTA: CCpu_TglPPinAVal(Pin);    break;
    case uCPORTB: CCpu_TglPPinBVal(Pin);    break;
    case uCPORTC: CCpu_TglPPinCVal(Pin);    break;
    case uCPORTD: CCpu_TglPPinDVal(Pin);    break;
    case uCPORTE: CCpu_TglPPinEVal(Pin);    break;
    case uCPORTF: CCpu_TglPPinFVal(Pin);    break;
    }
}

Bool CSys_GpioIsHi(int Port, int Pin)
{
    return (CSys_GpioGetVal(Port, Pin) ? TRUE : FALSE);
}

Bool CSys_GpioIsLo(int Port, int Pin)
{
    return (CSys_GpioGetVal(Port, Pin) ? FALSE : TRUE);
}

void CSys_LedInit()
{
    CSys_GpioInit(LED_PORT, LED1, PORT_DRN_OUT);
    CSys_GpioInit(LED_PORT, LED2, PORT_DRN_OUT);
}

void CSys_LedOn(int iLed)
{
    switch (iLed) {// LED Port is in sinking
    case LED1: CSys_GpioSetVal(LED_PORT, LED1, FALSE); break;
    case LED2: CSys_GpioSetVal(LED_PORT, LED2, FALSE); break;
    }
}

void CSys_LedOff(int iLed)
{
    switch (iLed) {// LED Port is in sinking
    case LED1: CSys_GpioSetVal(LED_PORT, LED1, TRUE); break;
    case LED2: CSys_GpioSetVal(LED_PORT, LED2, TRUE); break;
    }
}

void CSys_LedTgl(int iLed)
{
    switch (iLed) {// LED Port is in sinking
    case LED1: CSys_GpioTgl(LED_PORT, LED1); break;
    case LED2: CSys_GpioTgl(LED_PORT, LED2); break;
    }
}


//*********************************************************************
// End
//*********************************************************************
