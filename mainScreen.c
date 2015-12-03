#include <xc.h>
#include "libdspic/clock.h"
#include <libpic30.h>
#include <stdio.h>
#include "libdspic/uart.h"
#include "time.h"


#define MAINSCREEN_UART_CH  (UART_CH1)
#define TIMEOUT_DELAY_US    (100000)
#define VOLTAGE_THRESHOLD   (512)

void callMacro1(const char *string);
void display1(const char *string);
void writeString1(const char *string);
void putCharacter1(const char character);


volatile int prMainScreenAck = 1;      // flag activé par l'ISR de réception de l'UART
volatile int prMainScreenTimeReq = 0;
volatile int prMainScreenDateReq = 0;
volatile timeType prMainScreenTime;
volatile dateType prMainScreenDate;


void mainScreenConfig(void) {
    uartConfig(MAINSCREEN_UART_CH, 115200, UART_8N1_NO_HANDSHAKE);
    uartRxIsrEnable(MAINSCREEN_UART_CH);
    prMainScreenDate.year = 0;
    prMainScreenDate.month = 1;
    prMainScreenDate.day = 1;
    prMainScreenTime.hour = 0;
    prMainScreenTime.minute = 0;
}


//Writes the speed on the main screen
void displaySpeed(int speed) {
    static int displayedSpeed = -1;
    char tmpStr[4] ;

    if (speed < 0) {
        speed = 0;
    } else if (speed > 999) {
        speed = 999;
    }
    if (speed != displayedSpeed) {
        displayedSpeed = speed;
        sprintf(tmpStr, "%d", speed);
        callMacro1("\x01");
        display1(tmpStr);
    }
}

/** display the battery level
 * &param batteryLevel :  0% -> 100%
 */
void displayBatteryLevel(int batteryLevel) {
    static int displayedBatteryLevel = -1;
    const char macro[10][4] = {"\x04", "\x05", "\x06", "\x07", "\x08", "\x09", "\x0a", "\x0b", "\x0c", "\x0d"};

    batteryLevel /= 10;                             // notre bargraph ne possède que 10 valeurs
    if (batteryLevel > 9) {                         // on sature la valeur à afficher
        batteryLevel = 9;
    } else if (batteryLevel < 0) {
        batteryLevel = 0;
    }
    if (batteryLevel != displayedBatteryLevel) {    // si la valeur a changé
        callMacro1(macro[batteryLevel]);            // on affiche la nouvelle valeur
        displayedBatteryLevel = batteryLevel;       // et on la mémorise
    }
}

// Writes the remaining km on the main screen
void displayRemainingKm(int remainingKm) {
    static int displayedRemainingKm = -1;
    char tmpStr[4];

    if (remainingKm < 0) {
        remainingKm = 0;
    } else if (remainingKm > 999) {
        remainingKm = 999;
    }
    if (remainingKm != displayedRemainingKm) {
        displayedRemainingKm = remainingKm;
        sprintf(tmpStr, "%d", remainingKm);
        callMacro1("\x02");
        display1(tmpStr);
    }
}

// Writes km (rel or tot) on the main screen
void displayKm(long km) {
    static long displayedKm = -1;
    char tmpStr[7];

    if (km <0) {
        km = 0;
    } else if (km > 999999) {
        km = 999999;
    }
    if (km != displayedKm) {
        displayedKm = km;
        sprintf(tmpStr, "%ld", km);
        callMacro1("\x03");
        display1(tmpStr);
    }
}

void displayHeadlights(int voltage) {
    static int headLightsOn = -1;
    
    if (voltage > VOLTAGE_THRESHOLD) {
        if(headLightsOn != 1) {
            headLightsOn = 1;
            callMacro1("\x0e");
        }
    } else {
        if (headLightsOn != 0) {
            headLightsOn = 0;
            callMacro1("\x0f");
        }
    }
}

void displayHighBeams(int voltage) {
    static int highBeamsOn = -1;
    
    if (voltage > VOLTAGE_THRESHOLD) {
        if(highBeamsOn != 1) {
            highBeamsOn = 1;
            callMacro1("\x10");
        }
    } else {
        if (highBeamsOn != 0) {
            highBeamsOn = 0;
            callMacro1("\x11");
        }
    }
}

void displaySideLights(int voltage) {
    static int sideLightsOn = -1;
    
    if (voltage > VOLTAGE_THRESHOLD) {
        if(sideLightsOn != 1) {
            sideLightsOn = 1;
            callMacro1("\x12");
        }
    } else {
        if (sideLightsOn != 0) {
            sideLightsOn = 0;
            callMacro1("\x13");
        }
    }
}

void displayLeftBlinker(int voltage) {
    static int leftBlinkerOn = -1;
    
    if (voltage > VOLTAGE_THRESHOLD) {
        if(leftBlinkerOn != 1) {
            leftBlinkerOn = 1;
            callMacro1("\x14");
        }
    } else {
        if (leftBlinkerOn != 0) {
            leftBlinkerOn = 0;
            callMacro1("\x15");
        }
    }
}

void displayRightBlinker(int voltage) {
    static int rightBlinkerOn = -1;
    
    if (voltage > VOLTAGE_THRESHOLD) {
        if(rightBlinkerOn != 1) {
            rightBlinkerOn = 1;
            callMacro1("\x16");
        }
    } else {
        if (rightBlinkerOn != 0) {
            rightBlinkerOn = 0;
            callMacro1("\x17");
        }
    }
}

void displayOverHeat(int overHeatDetected) {
    static int overHeatOn = -1;
    
    if (overHeatDetected == 1) {
        if(overHeatOn != 1) {
            overHeatOn = 1;
            callMacro1("\x1d");
        }
    } else {
        if (overHeatOn != 0) {
            overHeatOn = 0;
            callMacro1("\x18");
        }
    }
}


// Sets the time of the RTCC
void setRtccTime(timeType time) {
    unsigned long i = TIMEOUT_DELAY_US;
    
    prMainScreenAck = 0;
    while (prMainScreenAck != 1) {
        if (i++ > TIMEOUT_DELAY_US) {
            writeString1("\xaa\x49\x54");
            putCharacter1(time.hour);
            putCharacter1(time.minute);
            putCharacter1('\x00');
            i = 0;
        }
        __delay_us(1);
    }
}

void setRtccDate(dateType date) {
    unsigned long i = TIMEOUT_DELAY_US;
    
    prMainScreenAck = 0;
    while (prMainScreenAck != 1) {
        if (i++ > TIMEOUT_DELAY_US) {
            writeString1("\xaa\x49\x44");
            putCharacter1(date.year);
            putCharacter1(date.month);
            putCharacter1(date.day);
            putCharacter1('\x00');
            i = 0;
        }
        __delay_us(1);
    }
}

// Requests the time from the RTCC
timeType getRtccTime(void) {
    unsigned long i = TIMEOUT_DELAY_US;
    
    prMainScreenTimeReq = 1;
    while(prMainScreenTimeReq != 0) {
        if (i++ > TIMEOUT_DELAY_US) {
            writeString1("\xaa\x49\x3f\x54");
            i = 0;
        }
    }
    return (prMainScreenTime);
}

dateType getRtccDate(void) {
    unsigned long i = TIMEOUT_DELAY_US;

    prMainScreenDateReq = 1;
    while(prMainScreenDateReq != 0) {
        if (i++ > TIMEOUT_DELAY_US) {
            writeString1("\xaa\x49\x3f\x44");
            i = 0;
        }
    }
    return (prMainScreenDate);
}

// Displays a string on the main screen
void display1(const char *string) {
    unsigned long i = TIMEOUT_DELAY_US;
    
    prMainScreenAck = 0;
    while (prMainScreenAck != 1) {
        if (i++ > TIMEOUT_DELAY_US) {
        writeString1("\xaa\x44\x54");
        writeString1(string);
        putCharacter1('\x00');
        i=0;
        }
        __delay_us(1);
    }
}

void resetMainScreen(void) {
    unsigned long i = TIMEOUT_DELAY_US;
    
    prMainScreenAck = 0;
    while (prMainScreenAck != 1) {
        if (i++ > TIMEOUT_DELAY_US) {
        writeString1("\xaa\x24");
        i = 0;
        }
        __delay_us(1);
    }
}


// Calls a macro from the main screen
void callMacro1(const char *string) {
    unsigned long i = TIMEOUT_DELAY_US;
    
    prMainScreenAck = 0;
    while (prMainScreenAck != 1) {
        if (i++ > TIMEOUT_DELAY_US) {
        writeString1("\xaa\x4f\x45");
        putCharacter1('\x00');
        writeString1(string);
        i = 0;
        }
        __delay_us(1);
    }
}

// Sends a string to the main screen
void writeString1(const char *string) {
    while (*string != '\0') {
        while (!uartTxReady(MAINSCREEN_UART_CH));
        uartSendChar(MAINSCREEN_UART_CH, *string);
        string++;
        while (!uartTxIdle(MAINSCREEN_UART_CH));
    }
}

// Sends a character to the main screen
void putCharacter1(const char character) {
    while (!uartTxReady(MAINSCREEN_UART_CH));
    uartSendChar(MAINSCREEN_UART_CH, character);
    while (!uartTxIdle(MAINSCREEN_UART_CH));
}


enum {
    IDLE,
    YEAR_READ,
    MONTH_READ,
    DAY_READ,
    WEEKDAY_READ,
    HOUR_READ,
    MINUTE_READ,
    SECOND_READ,
    END_OF_READ
} u1rxState = IDLE;
void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) {
    int response1;

    
    IFS0bits.U1RXIF = 0;
    response1 = uartGetChar(MAINSCREEN_UART_CH);
    switch (u1rxState) {
        case IDLE:
            if (response1 == '\x06') {
                prMainScreenAck = 1;
                if (u1rxState == IDLE) {
                    if (prMainScreenTimeReq == 1) {
                        u1rxState = HOUR_READ;
                    } else if (prMainScreenDateReq == 1) {
                        u1rxState = YEAR_READ;
                    }
                }
            }
            break;
        case YEAR_READ:
            prMainScreenDate.year = response1;
            u1rxState = MONTH_READ;
            break;
        case MONTH_READ:
            prMainScreenDate.month = response1;
            u1rxState = DAY_READ;
            break;
        case DAY_READ:
            prMainScreenDate.day = response1;
            u1rxState = WEEKDAY_READ;
            break;
        case WEEKDAY_READ:
            prMainScreenDate.weekDay = response1;
            u1rxState = END_OF_READ;
            break;
        case HOUR_READ:
            prMainScreenTime.hour = response1;
            u1rxState = MINUTE_READ;
            break;
        case MINUTE_READ:
            prMainScreenTime.minute = response1;
            u1rxState = SECOND_READ;
            break;
        case SECOND_READ:
            prMainScreenTime.second = response1;
            u1rxState = END_OF_READ;
            break;
        case END_OF_READ:
            prMainScreenTimeReq = 0;
            prMainScreenDateReq = 0;
            u1rxState = IDLE;
        default:
            break;
    }
}
