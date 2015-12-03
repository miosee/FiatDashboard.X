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
#include <libpic30.h>
#include "libdspic/pps.h"
#include "libdspic/adc.h"
#include "libdspic/timers.h"
#include "libdspic/DEE Emulation 16-bit.h"
#include "mainScreen.h"
#include "secondaryScreen.h"
#include "curtis.h"


/**
 * @brief Configuration de la PLL interne à  40MHz (!! ne pas oublier les config bits
 */
 _FWDT(FWDTEN_OFF) // on désactive le Watchdog
_FOSCSEL(FNOSC_FRC);
// enables clock switching and configure the primary oscillator for a 10MHz crystal
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);
_FGS(GSS_OFF & //
	GCP_OFF & //
	GWRP_OFF); //
_FICD(ICS_PGD2 & JTAGEN_OFF); // <-- à  changer selon les cas


#define TIMER1_PERIOD_MS    (100)
#define MAX_TEMPERATURE     (100)
#define NOMINAL_VOLTAGE     (80)
#define KM_TOTAL_ADDR       (0)
#define KM_RELATIF_ADDR     (2)
#define KM_TO_DISPLAY       (4)

#define getHeadLightsVoltage()      conversionAdc(0)
#define getHighBeamsVoltage()       conversionAdc(1)
#define getSideLightsVoltage()      conversionAdc(2)
#define getLeftBlinkerVoltage()     conversionAdc(3)
#define getRightBlinkerVoltage()    conversionAdc(4)
#define getTemperature()            (int)( (1324.0 - 3300.0*conversionAdc(5)/1024)/8.2 )


void storeKmToDisplayToNvm(kmToDisplayType value);
kmToDisplayType readKmToDisplayFromNvm(void);
int isOverHeatDetected(void);

void kmConfig(void);
void storeKmTotalToNvm(float value);
float readKmTotalFromNvm(void);
void setKmTotal(float value);
float getKmTotal(void);
void storeKmRelatifToNvm(float value);
float readKmRelatifFromNvm(void);
float getKmRelatif(void);
void timer1CallBack(void);
// variables privées utlisées par la fonction CallBack du timer1
volatile float kmTotal;
volatile float kmRelatif;
volatile int timer1Flag = 0;


int main(void) {
    kmToDisplayType kmToDisplay;
    int timeCounter = 0;
    
    pllConfig();
    initAdc();
    // configuration de l'écran principal
    ppsOutConfig(PPS_U1TX,6);
    ppsInConfig(PPS_U1RX,7);
    mainScreenConfig();
    // Configuration de l'écran secondaire
    ppsOutConfig(PPS_U2TX,8);
    ppsInConfig(PPS_U2RX,9);
    kmToDisplay = readKmToDisplayFromNvm();
    secondaryScreenConfig(kmToDisplay);
    // Configuration du timer pour le calcul de la distance
    timerSetup(TIMER_1, TIMER1_PERIOD_MS);
    timerInterrupt(TIMER_1, timer1CallBack);
    timerStart(TIMER_1);
    AD1PCFGLbits.PCFG11 = 1;
    AD1PCFGLbits.PCFG12 = 1;
    ppsOutConfig(PPS_C1TX,12);
    ppsInConfig(PPS_IN_C1RX,13);
    curtisConfig();
    // Initialisation des compteurs kilométriques
    kmConfig();
    __delay_ms(2000);      // Laisse le temps aux écrans de s'initialiser
    resetSecondaryScreen();
    while (1) {
        while (timer1Flag == 0);
        timer1Flag = 0;
        checkTouchEvent();
        if (kmToDisplay != getKmToDisplay()) {
            kmToDisplay = getKmToDisplay();
            storeKmToDisplayToNvm(kmToDisplay);
        }
        displaySpeed(getSpeed());
        displayBatteryLevel((100*getVoltage())/NOMINAL_VOLTAGE);
        displayRemainingKm(50);
        displayHeadlights(getHeadLightsVoltage());
        displayHighBeams(getHighBeamsVoltage());
        displaySideLights(getSideLightsVoltage());
        displayLeftBlinker(getLeftBlinkerVoltage());
        displayRightBlinker(getRightBlinkerVoltage());
        displayOverHeat(isOverHeatDetected());
        if (kmToDisplay == RELATIVE_KM) {         
            displayKm((long)getKmRelatif());
        } else {
            displayKm((long)getKmTotal());
        }
        displayTime();
        displayDate();
        timeCounter++;
        // Action exécutées toutes les secondes
        if (timeCounter >= 10) {
            timeCounter = 0;
            displayTemperature(getTemperature());
            storeKmTotalToNvm(getKmTotal());
            storeKmRelatifToNvm(getKmRelatif());
        }
//        // Check problems every 2 seconds
//        if(problem_check ==1) {
//            problem_check = 0;
//            CheckProblems();
//        }
    }
}



void timer1CallBack(void) {
    float speed, distance;
    
    speed = getSpeed();
    distance = (speed/3600)*(1E-3*TIMER1_PERIOD_MS);
    kmRelatif += distance;
    kmTotal += distance;
    timer1Flag = 1;
}

void storeKmToDisplayToNvm(kmToDisplayType value) {
    DataEEWrite((unsigned int)value, KM_TO_DISPLAY);
}

kmToDisplayType readKmToDisplayFromNvm(void) {
    unsigned int tmp;
    
    
    tmp = DataEERead(KM_TO_DISPLAY);
    if (tmp == 0xFF) {
        tmp = ABSOLUTE_KM;
        storeKmToDisplayToNvm(tmp);
    }
    return(tmp);
}

int isOverHeatDetected(void) {
    return ((getMotorTemp() > MAX_TEMPERATURE) || (getControllerTemp() > MAX_TEMPERATURE));
}

void kmConfig(void) {
    DataEEInit();
    dataEEFlags.val = 0;
    kmTotal = readKmTotalFromNvm();
    kmRelatif = readKmRelatifFromNvm();
}

void storeKmTotalToNvm(float value) {
    unsigned long valueLong;
    
    if (value < 0) {
        value = 0;
    } else if (value > 999999) {
        value = 999999;
    }
    valueLong = (long)(1000*value);
    DataEEWrite((unsigned int)(valueLong),KM_TOTAL_ADDR);
    DataEEWrite((unsigned int)(valueLong >> 16),KM_TOTAL_ADDR+1);
}

float readKmTotalFromNvm(void) {
    unsigned long valueLong;
    float valueFloat;
    
    valueLong = DataEERead(KM_TOTAL_ADDR);
    valueLong += (long)DataEERead(KM_TOTAL_ADDR+1) << 16;
    if (valueLong == 0xFFFFFFFF) {          // 1er accès à la NVM (qui est initialisée à FF à chaque programmation)
        valueFloat = 0;                     // on initialise à 0
        storeKmTotalToNvm(valueFloat);     // et on initialise la NVM
    } else {
        valueFloat = 1E-3*valueLong;        // on covertit la valeur stockée (m -> km)
    }
    return (valueFloat);
}

void setKmTotal(float value) {
    timerIsrDisable(TIMER_1);
    kmTotal = value;
    timerIsrEnable(TIMER_1);
}

float getKmTotal(void) {
    float value;
    
    timerIsrDisable(TIMER_1);
    value = kmTotal;
    timerIsrEnable(TIMER_1);
    return(value);
}

void storeKmRelatifToNvm(float value) {
    unsigned long valueLong;
    
    if (value < 0) {
        value = 0;
    } else if (value > 999999) {
        value = 999999;
    }
    valueLong = (long)(1000*value);
    DataEEWrite((unsigned int)(valueLong),KM_RELATIF_ADDR);
    DataEEWrite((unsigned int)(valueLong >> 16),KM_RELATIF_ADDR+1);
}

float readKmRelatifFromNvm(void) {
    unsigned long valueLong;
    float valueFloat;
    
    valueLong = DataEERead(KM_RELATIF_ADDR);
    valueLong += (long)DataEERead(KM_RELATIF_ADDR+1) << 16;
    if (valueLong == 0xFFFFFFFF) {          // 1er accès à la NVM (qui est initialisée à FF à chaque programmation)
        valueFloat = 0;                     // on initialise à 0
        storeKmRelatifToNvm(valueFloat);     // et on initialise la NVM
    } else {
        valueFloat = 1E-3*valueLong;        // on covertit la valeur stockée (m -> km)
    }
    return (valueFloat);
}

void setKmRelatif(float value) {
    timerIsrDisable(TIMER_1);
    kmRelatif = value;
    timerIsrEnable(TIMER_1);
}

float getKmRelatif(void) {
    float value;
    
    timerIsrDisable(TIMER_1);
    value = kmRelatif;
    timerIsrEnable(TIMER_1);
    return(value);
}


//// Checks the display of the problems
//void CheckProblems()
//{
//
//    if(problem_light == 1 && (problem_index == 1 || problem_modification == 1))
//    {
//        CallMacro1("\x1b"); // macro 27
//
//        if(problem_handbrake_on ==1 && handbrake_asked == 1)
//            problem_index = 4;
//
//        if(problem_door_open ==1 && opendoor_asked == 1)
//            problem_index = 3;
//
//        if(problem_motor_overheat ==1)
//            problem_index = 2;
//        problem_modification = 0;
//    }
//    else if(opendoor_asked && problem_door_open == 1 && (problem_index == 3 || problem_modification ==1))
//    {
//        CallMacro1("\x1c"); // macro 28
//
//        if(problem_motor_overheat == 1)
//            problem_index = 2;
//
//        if(problem_light == 1)
//            problem_index = 1;
//
//        if(problem_handbrake_on == 1 && handbrake_asked == 1)
//            problem_index = 4;
//
//        problem_modification = 0;
//    }
//
//    else if(handbrake_asked && problem_handbrake_on == 1 && (problem_index == 4 || problem_modification ==1))
//    {
//        CallMacro1("\x19"); // macro 25
//
//        if(problem_door_open == 1 && opendoor_asked == 1)
//            problem_index = 3;
//
//        if(problem_motor_overheat == 1)
//            problem_index = 2;
//
//        if(problem_light == 1)
//            problem_index = 1;
//
//        problem_modification = 0;
//    }
//
//    else
//    {
//        CallMacro1("\x18"); // macro 24
//    }
//
//}
