
#include <xc.h>
#include "libdspic/clock.h"
#include <libpic30.h>
#include <stdio.h>
#include "libdspic/uart.h"
#include "time.h"
#include "mainScreen.h"
#include "secondaryScreen.h"


#define SECONDARYSCREEN_UART_CH  (UART_CH2)
#define TIMEOUT_DELAY_US    (100000)

enum {
    DAY,
    MONTH,
    YEAR
} prDateSelected = DAY;

enum {
    HOUR,
    MINUTE
} prTimeSelected = HOUR;
 
volatile int prSecondaryScreenAck = 1;
volatile int prTouchEvent = 0;
volatile int prTouchEventIndex = 0;
kmToDisplayType prKmToDisplay;
// Customization related variables
int handbrake_asked = 1;
int opendoor_asked = 1;
int problem_modification = 0;



void initializeDateScreen(dateType date);
void selectDay(void);
void selectMonth(void);
void selectYear(void);
void initializeTimeScreen(timeType time);
void selectHour(void);
void selectMinute(void);

void display2(const char *string);
void callMacro2(const char *string);
void putCharacter2(const char character);
void writeString2(const char *string);


void secondaryScreenConfig(kmToDisplayType kmToDisplay) {
    prKmToDisplay = kmToDisplay;
    uartConfig(SECONDARYSCREEN_UART_CH, 115200, UART_8N1_NO_HANDSHAKE);
    uartRxIsrEnable(SECONDARYSCREEN_UART_CH);
}

void resetSecondaryScreen(void) {
    unsigned long i = TIMEOUT_DELAY_US;
    
    prSecondaryScreenAck = 0;
    while (prSecondaryScreenAck != 1) {
        if (i++ > TIMEOUT_DELAY_US) {
        writeString2("\xaa\x27");
        i = 0;
        }
        __delay_us(1);
    }
}

// Function reacting to all the touch events
void checkTouchEvent(void) {
    if (prTouchEventIndex != 0) {
        switch(prTouchEventIndex) {
            case 'a':       // User wants to modify the date
                enableDateChange();
                callMacro2("\x06");
                initializeDateScreen(getDate());
                break;
            case 'b':       // User wants to select the day
                selectDay();
                break;
            case 'c':       // User wants to select the month
                selectMonth();
                break;
            case 'd':       // User wants to select the year
                selectYear();
                break;
            case 'e':       // User want to increase the selected value
                switch (prDateSelected) {
                    case DAY:
                        increaseDay();
                        selectDay();
                        break;
                    case MONTH:
                        increaseMonth();
                        selectMonth();
                        break;
                    case YEAR:
                        increaseYear();
                        selectYear();
                    default:
                        break;
                }
                break;
            case 'f':       // User wants to decrease the selected value
                switch (prDateSelected) {
                    case DAY:
                        decreaseDay();
                        selectDay();
                        break;
                    case MONTH:
                        decreaseMonth();
                        selectMonth();
                        break;
                    case YEAR:
                        decreaseYear();
                        selectYear();
                    default:
                        break;
                }
                break;
            case 'g':       // User wants to modify the time
                   setRtccDate(getDate());
                   disableDateChange();
                   enableTimeChange();
                   callMacro2("\x07");
                   initializeTimeScreen(getTime());
                   break;
            case 'h':       // User wants to select the hour
                selectHour();
                break;
            case 'i':       // User wants to select the minutes
                selectMinute();
                break;
            case 'j':       // User wants to increase the selected value
                switch (prTimeSelected) {
                    case HOUR:
                        increaseHour();
                        selectHour();
                        break;
                    case MINUTE:
                        increaseMinute();
                        selectMinute();
                        break;
                    default:
                        break;
                }
                break;
            case 'q':       // User wants to decrease the selected value
                switch (prTimeSelected) {
                    case HOUR:
                        decreaseHour();
                        selectHour();
                        break;
                    case MINUTE:
                        decreaseMinute();
                        selectMinute();
                        break;
                    default:
                        break;
                }
                break;
            case 'r':       // User is done with the time and date modifications
                setRtccTime(getTime());
                disableTimeChange();
                callMacro2("\x04"); // Macro 4
                break;
            case 'l':       // User wants to activate or deactive the display of km_tot
                if (prKmToDisplay == ABSOLUTE_KM) {
                    prKmToDisplay = RELATIVE_KM;
                    callMacro2("\x0c"); // macro 12
                    callMacro2("\x0b"); // macro 11
                } else {
                    prKmToDisplay = ABSOLUTE_KM;
                    callMacro2("\x0a"); // macro 10
                    callMacro2("\x0d"); // macro 13
                }
                break;
               case 'm':    // User wants to activate or deactive the display of km_rel
                if (prKmToDisplay == RELATIVE_KM) {
                    prKmToDisplay = ABSOLUTE_KM;
                    callMacro2("\x0a"); // macro 10
                    callMacro2("\x0d"); // macro 13
                } else {
                    prKmToDisplay = RELATIVE_KM;
                    callMacro2("\x0c"); // macro 12
                    callMacro2("\x0b"); // macro 11
                }
                break;
            case 'n':       // User wants to activate or deactive the display handbrake
                if (handbrake_asked == 1) {
                    handbrake_asked = 0;
                    problem_modification = 1;
                    callMacro2("\x0f"); // macro 15
                } else {
                    handbrake_asked = 1;
                    problem_modification = 1;
                    callMacro2("\x0e"); // macro 14
                }
                break;
            case 'o':       // User wants to activate or deactive the display of opendoor
                   if(opendoor_asked == 1) {
                    opendoor_asked = 0;
                    problem_modification = 1;
                    callMacro2("\x11"); // macro 17
                } else {
                    opendoor_asked = 1;
                    problem_modification = 1;
                    callMacro2("\x10"); // macro 16
                }
                break;
            case 'p': // User wants to reinitialize the km value.
                setKmRelatif(0);
                storeKmRelatifToNvm(0);
                callMacro2("\x1d"); // macro 29
                break;
            default :
                break;
        }
        prTouchEventIndex = 0;
    }
}

kmToDisplayType getKmToDisplay(void) {
    return (prKmToDisplay);
}


void displayTime(void) {
    char timeStr[6];
    timeType time;
    static timeType oldTime = {-1, -1, -1};
    
    time = getTime();
    if ((time.minute != oldTime.minute) || (time.hour != oldTime.hour)) {
        oldTime.minute = time.minute;
        oldTime.hour = time.hour;
        sprintf(timeStr, "%02dh%02d", time.hour, time.minute);
        callMacro2("\x02");
        display2(timeStr);
    }
}

void displayDate(void) {
    char dateStr[10];
    dateType date;
    static dateType oldDate = {-1, -1, -1};
    
    date = getDate();
    if ((date.day != oldDate.day) || (date.month != oldDate.month) || (date.year != oldDate.year)) {
        oldDate.day = date.day;
        oldDate.month = date.month;
        oldDate.year = date.year;
        sprintf(dateStr,"%02d/%02d/20%02d", date.day, date.month, date.year);
        callMacro2("\x01");
        display2(dateStr);
    }
}


void displayTemperature(int temperature) {
    char tempStr[6];
    static int displayedTemperature = -100;

    if (displayedTemperature != temperature) {
        displayedTemperature = temperature;
        if (temperature < 0) {
            sprintf(tempStr,"T\xb0%d", temperature);
        } else {
            sprintf(tempStr,"T\xb0+%d", temperature);
        }
        callMacro2("\x03");
        display2(tempStr);
    }
}


// Prepares the time setting screens and writes everything in black
void initializeTimeScreen(timeType time) {
    char hourStr[4];
    char minuteStr[4];

    sprintf(hourStr, "%02dh", time.hour);
    sprintf(minuteStr, "%02dm", time.minute);
    callMacro2("\x15");
    display2(hourStr);
    callMacro2("\x16");
    display2(minuteStr);
}

void selectHour(void) {
    char hourStr[4];
    timeType curTime;

    prTimeSelected = HOUR;
    curTime = getTime();
    initializeTimeScreen(curTime);
    sprintf(hourStr, "%02dh", curTime.hour);
    callMacro2("\x1a");
    display2(hourStr);
}

void selectMinute(void) {
    char minuteStr[4];
    timeType curTime;

    prTimeSelected = MINUTE;
    curTime = getTime();
    initializeTimeScreen(curTime);
    sprintf(minuteStr, "%02dm", curTime.minute);
    callMacro2("\x1B");
    display2(minuteStr);
}

// Prepares the date setting screens and writes everything in black
void initializeDateScreen(dateType date) {
    char dayStr[3];
    char monthStr[3];
    char yearStr[3];

    sprintf(dayStr, "%02d", date.day);
    sprintf(monthStr, "%02d", date.month);
    sprintf(yearStr, "20%02d", date.year);
    callMacro2("\x12");
    display2(dayStr);
    callMacro2("\x13");
    display2(monthStr);
    callMacro2("\x14");
    display2(yearStr);
}

// updates the value of Day and highlights it in red
void selectDay(void) {
    char dayStr[3];
    dateType curDate;

    prDateSelected = DAY;
    curDate = getDate();
    initializeDateScreen(curDate);
    sprintf(dayStr, "%02d", curDate.day);
    callMacro2("\x17");
    display2(dayStr);
}

// updates the value of month and highlights it in red
void selectMonth(void) {
    char monthStr[3];
    dateType curDate;

    prDateSelected = MONTH;
    curDate = getDate();
    initializeDateScreen(curDate);
    sprintf(monthStr, "%02d", curDate.month);
    callMacro2("\x18");
    display2(monthStr);
}

// updates the value of year and highlights it in red
void selectYear() {
    char yearStr[3];
    dateType curDate;

    prDateSelected = YEAR;
    curDate = getDate();
    initializeDateScreen(curDate);
    sprintf(yearStr, "20%02d", curDate.year);
    callMacro2("\x19");
    display2(yearStr);
}


// Displays a string on the secondary screen
void display2(const char *string) {
    unsigned long i = TIMEOUT_DELAY_US;
    
    prSecondaryScreenAck = 0;
    while (prSecondaryScreenAck != 1) {
        if (i++ > TIMEOUT_DELAY_US) {
            writeString2("\xaa\x44\x54");
            writeString2(string);
            putCharacter2('\x00');
            i = 0;
        }
        __delay_us(1);
    }
}

// Calls a macro from the secondary screen
void callMacro2(const char *string) {
    unsigned long i = TIMEOUT_DELAY_US;
    
    prSecondaryScreenAck = 0;
    while (prSecondaryScreenAck != 1) {
        if (i++ > TIMEOUT_DELAY_US) {
        writeString2("\xaa\x4f\x45");
        putCharacter2('\x00');
        writeString2(string);
        i = 0;
        }
        __delay_us(1);
    }
}

// Sends a string to the secondary screen
void writeString2(const char *string) {
    while (*string != '\0') {
        while (!uartTxReady(SECONDARYSCREEN_UART_CH));
        uartSendChar(SECONDARYSCREEN_UART_CH, *string);
        string++;
        while (!uartTxIdle(SECONDARYSCREEN_UART_CH));
    }
}

// Sends a characgter to the secondary screen
void putCharacter2(const char character) {
    while (!uartTxReady(SECONDARYSCREEN_UART_CH));
    uartSendChar(SECONDARYSCREEN_UART_CH, character);
    while (!uartTxIdle(SECONDARYSCREEN_UART_CH));
}

// Interrupt function for the secondary screen
void __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void) {
    int response2;
    static int tmpTouchEventIndex;
    
    IFS1bits.U2RXIF = 0;
    response2 = uartGetChar(SECONDARYSCREEN_UART_CH);
    if(response2 =='\x06') {
        if (prTouchEvent == 1) {
            prTouchEvent = 0;
            prTouchEventIndex = tmpTouchEventIndex;
        } else {
            prSecondaryScreenAck = 1;
        }
    } else if (response2 == 'k') {
        prTouchEvent = 1;
    } else if (prTouchEvent == 1) {
        tmpTouchEventIndex = response2;
    }
}

