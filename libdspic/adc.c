/**
 * @file adc.c
 * @author bULBot 2011-2012
 * @author Olivier Desenfans
 * @brief Impl�mentation des fonctions de contr�le du module ADC pour dSPIC33F.
 */

#include "adc.h"


/**
 * @brief Choix d'une entr�e analogique pour le canal 0.
 */
#define SET_CH0_TO_INPUT(input)		AD1CHS0bits.CH0SA = input

/**
 * @brief Lancement d'un �chantillonnage sur l'ADC1.
 */
#define START_SAMPLING()	AD1CON1bits.SAMP = 1

/**
 * @brief Bool�en indiquant si le module ADC est en train de r�aliser une conversion.
 */
#define ADC_CONVERTING		!AD1CON1bits.DONE

/**
 * @brief Buffer de sortie de l'ADC.
 */
#define ADC_BUFFER			ADC1BUF0


void initAdc(void) {
    AD1CON1bits.ADON = 0;       	// D�sactivation du module
    AD1CON1bits.AD12B = 0;          // Conversion sur 10 bits
    AD1CON3bits.ADRC = 0;           // Choix de l'horloge interne comme source d'horloge pour l'ADC
    AD1CON3bits.ADCS = 3;           // Choix de la p�riode Tad = 4*Tcy (marge de s�curit� sur la valeur min)
    AD1CON3bits.SAMC = 15;          // Choix du temps d'attente entre d�but de l'�chantillonnage et conversion
    AD1CON1bits.SSRC = 0b111;       // Lancement de la conversion via l'horloge de l'ADC
    AD1CON1bits.FORM = 0b00;        // R�sultat sur un entier
    AD1CON2bits.VCFG = 0b000;       // Tensions de r�f�rence: Vdd et Vss.
    AD1CON2bits.CHPS = 0b00;        // 4 canaux pour l'�chantillonnage simultan�
    AD1CON1bits.ADON = 1;           // Activation du module
}

int conversionAdc(unsigned char canal) {
    if (canal < 32) {
        SET_CH0_TO_INPUT(canal);
    } else {
        return 0;
    }
    START_SAMPLING();
    while (ADC_CONVERTING);
    return (ADC_BUFFER);
}
