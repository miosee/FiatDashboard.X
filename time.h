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
#ifndef TIME_H
#define	TIME_H


typedef struct {
    int hour;
    int minute;
    int second;
} timeType;

typedef struct {
    int year;
    int month;
    int day;
    int weekDay;
} dateType;


timeType getTime(void);
void enableTimeChange(void);
void disableTimeChange(void);
void increaseHour(void);
void increaseMinute(void);
void decreaseHour(void);
void decreaseMinute(void);

dateType getDate(void);
void enableDateChange(void);
void disableDateChange(void);
void increaseDay(void);
void increaseMonth(void);
void increaseYear(void);
void decreaseDay(void);
void decreaseMonth(void);
void decreaseYear(void);


#endif	/* TIME_H */

