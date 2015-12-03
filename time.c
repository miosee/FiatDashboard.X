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
//                                                                           //
//  Contient les fonctions nécessaires à la gestion de l'horloge de la Fiat  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#include <xc.h>
#include "mainScreen.h"
#include "time.h"


int prTimeChangeActive = 0;     // flag indiquant que l'heure est en train d'être modifiée par l'utilisateur
int prDateChangeActive = 0;     // flag indiquant que l'heure est en train d'être modifiée par l'utilisateur
timeType prCurTime;
dateType prCurDate;


timeType getTime(void) {
    if (prTimeChangeActive == 0) {      // si l'utilisateur n'est pas en train de modifier l'heure,
        prCurTime = getRtccTime();      // on l'actualise en interrogeant la RTCC de l'écran principal
    }                                   // Sinon, on renvoie la valeur actuelle
    return (prCurTime);
}

void enableTimeChange(void) {
    prTimeChangeActive = 1;
}

void disableTimeChange(void) {
    prTimeChangeActive = 0;
}

void increaseHour(void) {
    if (prCurTime.hour >= 23) {
        prCurTime.hour = 0;
    } else {
        prCurTime.hour++;
    }
}

void increaseMinute(void) {
    if (prCurTime.minute >= 59) {
        prCurTime.minute = 0;
    } else {
        prCurTime.minute++;
    }
}

void decreaseHour(void) {
    if (prCurTime.hour <= 0) {
        prCurTime.hour = 23;
    } else {
        prCurTime.hour--;
    }
}

void decreaseMinute(void) {
    if (prCurTime.minute <= 0) {
        prCurTime.minute = 59;
    } else {
        prCurTime.minute--;
    }
}


dateType getDate(void) {
    if (prDateChangeActive == 0) {      // si l'utilisateur n'est pas en train de modifier la date,
        prCurDate = getRtccDate();      // on l'actualise en interrogeant la RTCC de l'écran principal
    }                                   // Sinon, on renvoie la valeur actuelle
    return (prCurDate);
}

void enableDateChange(void) {
    prDateChangeActive = 1;
}

void disableDateChange(void) {
    prDateChangeActive = 0;
}

void increaseDay(void) {
    if (prCurDate.day >= 31) {
        prCurDate.day = 1;
    } else {
        prCurDate.day++;
    }
}

void increaseMonth(void) {
    if (prCurDate.month >= 12) {
        prCurDate.month = 1;
    } else {
        prCurDate.month++;
    }
}

void increaseYear(void) {
    if (prCurDate.year >= 99) {
        prCurDate.year = 0;
    } else {
        prCurDate.year++;
    }
}

void decreaseDay(void) {
    if (prCurDate.day <= 1) {
        prCurDate.day = 31;
    } else {
        prCurDate.day--;
    }
}

void decreaseMonth(void) {
    if (prCurDate.month <= 1) {
        prCurDate.month = 12;
    } else {
        prCurDate.month--;
    }
}

void decreaseYear(void) {
    if (prCurDate.year <= 0) {
        prCurDate.year = 99;
    } else {
        prCurDate.year--;
    }
}
