/* 
 * File:   secondaryScreen.h
 * Author: Kevin
 *
 * Created on 7 septembre 2015, 13:16
 */

#ifndef SECONDARYSCREEN_H
#define	SECONDARYSCREEN_H


    typedef enum {
        RELATIVE_KM,
        ABSOLUTE_KM
    } kmToDisplayType;

    void secondaryScreenConfig(kmToDisplayType kmToDisplay);
    void resetSecondaryScreen(void);
    void displayTime(void);
    void displayDate(void);
    void displayTemperature(int temperature);
    kmToDisplayType getKmToDisplay(void);
    void checkTouchEvent(void);
    

#endif	/* SECONDARYSCREEN_H */

