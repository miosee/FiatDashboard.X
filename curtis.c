///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Nom du projet : FiatDashboard                                            //
//  Nom du fichier : main.c                                                  //
//  Date de création : 27/11/2015                                            //
//  Auteur : Michel Osée                                                     //
//                                                                           //
//  Version : 0.1                                                            //
//  Date de la dernière modification : 27/11/2015                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#include <xc.h>
#include "libdspic/clock.h"
#include "libdspic/pps.h"
#include "libdspic/CanDspic.h"
#include "curtis.h"

#define SPEED_FACTOR    (1.0)

struct {
    unsigned char motorSpeedMsb;
    unsigned char motorSpeedLsb;
    unsigned char motorTemp;
    unsigned char controllerTemp;
    unsigned char currentMsb;
    unsigned char currentLsb;
    unsigned char voltageMsb;
    unsigned char voltageLsb;
} prCurtisInfo;

void curtisConfig(void) {
    prCurtisInfo.motorSpeedMsb = 0;
    prCurtisInfo.motorSpeedLsb = 0;
    prCurtisInfo.motorTemp = 0;
    prCurtisInfo.controllerTemp = 0;
    prCurtisInfo.currentMsb = 0;
    prCurtisInfo.currentLsb = 0;
    prCurtisInfo.voltageMsb = 0;
    prCurtisInfo.voltageLsb = 0;
    CanInitialisation(0x06);
    CanDeclarationConsommation(0x601, &prCurtisInfo, sizeof(prCurtisInfo));
    ACTIVATE_CAN_INTERRUPTS = 1;
}

float getSpeed(void) {
    float value;
    int tmp;
    
    ACTIVATE_CAN_INTERRUPTS = 0;
    tmp = prCurtisInfo.motorSpeedMsb;
    tmp = (tmp << 8) + prCurtisInfo.motorSpeedLsb;
    value = SPEED_FACTOR*tmp;
    ACTIVATE_CAN_INTERRUPTS = 1;
    return (value);
}

int getMotorTemp(void) {
    int value;
    
    ACTIVATE_CAN_INTERRUPTS = 0;
    value = prCurtisInfo.motorTemp;
    ACTIVATE_CAN_INTERRUPTS = 1;
    return (value);
}

int getControllerTemp(void) {
    int value;
    
    ACTIVATE_CAN_INTERRUPTS = 0;
    value = prCurtisInfo.controllerTemp;
    ACTIVATE_CAN_INTERRUPTS = 1;
    return (value);
}

float getCurrent(void) {
    float value;
    unsigned int tmp;
    
    ACTIVATE_CAN_INTERRUPTS = 0;
    tmp = prCurtisInfo.currentMsb;
    tmp = (tmp << 8) + prCurtisInfo.currentLsb;
    value = 0.1*tmp;
    ACTIVATE_CAN_INTERRUPTS = 1;
    return (value);
}

float getVoltage(void) {
    float value;
    unsigned int tmp;
    
    ACTIVATE_CAN_INTERRUPTS = 0;
    tmp = prCurtisInfo.voltageMsb;
    tmp = (tmp << 8) + prCurtisInfo.voltageLsb;
    value = 0.1*tmp;
    ACTIVATE_CAN_INTERRUPTS = 1;
    return (value);
}
