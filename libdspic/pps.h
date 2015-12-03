/*!
 * @file pps.h
 * @author Michel Os�e
 * @brief Header file pour la configuration du peripheral pin select
 */


#ifndef PPS_H
#define	PPS_H


/**
 * \enum ppsStatus
 * \brief sp�cifie si une erreur est apparue lors de l'ex�cution d'une fonction li�e au peripheral pin select
 */
typedef enum {
    PPS_SUCCESS = 0, //!< Tout s'est bien pass�
    PPS_PERIPH_ERROR = 1, //!< l'I/O du p�riph�rique fournie est inconnue
    PPS_PIN_ERROR = 2, //!< la patte RP fournie n'existe pas
} ppsStatus;

/**
 * \enum ppsOutput
 * \brief d�finit les sorties des p�riph�riques configurables par le Peripheral Pin Select
 */
typedef enum {
	PPS_C1OUT		= 0x01,
	PPS_C2OUT		= 0x02,
	PPS_U1TX		= 0x03,
	PPS_U1RTS		= 0x04,
	PPS_U2TX		= 0x05,
	PPS_U2RTS		= 0x06,
	PPS_SDO1		= 0x07,
	PPS_OUT_SCK1	= 0x08,
	PPS_OUT_SS1		= 0x09,
	PPS_SDO2		= 0x0A,
	PPS_OUT_SCK2	= 0x0B,
	PPS_OUT_SS2		= 0x0C,
	PPS_CSDO		= 0x0D,
	PPS_CSCKOUT		= 0x0E,
	PPS_COFSOUT		= 0x0F,
	PPS_C1TX		= 0x10,
	PPS_OC1			= 0x12,
	PPS_OC2			= 0x13,
	PPS_OC3			= 0x14,
	PPS_OC4			= 0x15,
	PPS_UPDN1		= 0x1A,
	PPS_UPDN2		= 0x1B
} ppsOutput;


/**
 * @brief Assignation d'une patte reconfigurable (RPx) du dsPIC � une sortie de p�riph�rique
 * @param periphOutput d�finit la sortie de p�riph�rique � connecter
 * @param rpPin d�finit le num�ro de la patte RP � assigner � la sortie du p�riph�rique
 * @returns indication du succ�s ou de l'�chec (et raison de l'�chec)
 */
ppsStatus ppsOutConfig(ppsOutput periphOutput, int rpPin);


/**
 * \enum ppsInput
 * \brief d�finit les entr�es des p�riph�riques configurables par le Peripheral Pin Select
 */
typedef enum {
	PPS_U1RX	= 1,	// UART1 Receive (U1RX)
	PPS_U1CTS	= 2,	// UART1 Clear to Send (U1CTS)
	PPS_U2RX	= 3,	// UART2 Receive (U2RX)
	PPS_U2CTS	= 4,	// UART2 Clear to Send (U2CTS)
	PPS_SDI1	= 5,	// SPI1 Data Input (SDI1)
	PPS_IN_SCK1	= 6,	// SPI1 Clock Input (SCK1IN)
	PPS_IN_SS1	= 7,	// SPI1 Slave Select Input (SS1IN)
	PPS_SDI2	= 8,	// SPI2 Data Input (SDI2)
	PPS_IN_SCK2	= 9,	// SPI2 Clock Input (SCK2IN)
	PPS_IN_SS2	= 10,	// SPI2 Slave Select Input (SS2IN)
	PPS_IN_C1RX = 11,
#if defined(__dsPIC33FJ128MC802__) ||defined(__dsPIC33FJ128MC804__)
	PPS_QEA1	= 12,	// QEI1 chanel A (QEA)
	PPS_QEB1	= 13,	// QEI1 chanel B (QEB)
	PPS_QEA2	= 14,	// QEI2 chanel A (QEA)
	PPS_QEB2	= 15	// QEI2 chanel B (QEB)
#endif
} ppsInput;


/**
 * @brief Assignation d'une patte reconfigurable (RPx) du dsPIC � une entr�e de p�riph�rique
 * @param periphInput d�finit l'entr�e de p�riph�rique � connecter
 * @param rpPin d�finit le num�ro de la patte RP � assigner � l'entr�e du p�riph�rique
 * @returns indication du succ�s ou de l'�chec (et raison de l'�chec)
 */
ppsStatus ppsInConfig(ppsInput periphInput, int rpPin);

#endif	/* PPS_H */

