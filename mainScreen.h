/* 
 * File:   mainScreen.h
 * Author: Kevin
 *
 * Created on 7 septembre 2015, 11:37
 */

#ifndef MAINSCREEN_H
#define	MAINSCREEN_H


    #include "time.h"

    void mainScreenConfig(void);
    void resetMainScreen(void);
    void displaySpeed(int speed);
    void displayBatteryLevel(int baterryLevel);
    void displayRemainingKm(int remainingKm);
    void displayKm(long km);
    void displayHeadlights(int voltage);
    void displayHighBeams(int voltage);
    void displaySideLights(int voltage);
    void displayLeftBlinker(int voltage);
    void displayRightBlinker(int voltage);
    void displayOverHeat(int overHeatDetected);
    timeType getRtccTime();
    dateType getRtccDate(void);
    void setRtccTime(timeType time);
    void setRtccDate(dateType date);


#endif	/* MAINSCREEN_H */

