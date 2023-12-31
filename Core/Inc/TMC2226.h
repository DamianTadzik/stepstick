/*
 * TMC2226.h
 *
 *  Created on: Dec 13, 2023
 *      Author: brzan
 */

#ifndef INC_TMC2226_H_
#define INC_TMC2226_H_

#include "main.h"
#include "tim.h"
#include "usart.h"
#include "cmsis_os.h"

/**
 * \brief			Those are possible addresses of TMC2226 nodes
 */
typedef enum {
	TMC2226_ADDR_0 = 0x00u,					 	/* MS1 and MS2 pin set to LOW and LOW correspondingly */
	TMC2226_ADDR_1 = 0x01u,						/* MS1 and MS2 pin set to HIGH and LOW correspondingly */
	TMC2226_ADDR_2 = 0x02u,						/* MS1 and MS2 pin set to LOW and HIGH correspondingly */
	TMC2226_ADDR_3 = 0x03u						/* MS1 and MS2 pin set to HIGH and HIGH correspondingly */
} TMC2226_NodeAddress;

/**
 * \brief 			Those are possible READ registers in TMC2226
 */
typedef enum {
	/* General configuration registers */
	R_GCONF = 0x00u,
	R_GSTAT = 0x01u,							/* Status register */
	R_IFCNT = 0x02u,
	R_OTP_READ = 0x05u,
	R_IOIN = 0x06u,
	R_FACTORY_CONF = 0x07u,
	/* Velocity Dependent Control */
	R_TSTEP = 0x12u,
	/* StallGuard Control */
	R_SG_RESULT = 0x41u,
	/* Sequencer Registers */
	R_MSCNT = 0x6Au,
	R_MSCURACT = 0x6Bu,
	/*  Chopper Control Registers */
	R_CHOPCONF = 0x6Cu,
	R_DRV_STATUS = 0x6Fu,
	R_PWMCONF = 0x70u,
	R_PWM_SCALE = 0x71u,
	R_PWM_AUTO = 0x72u
} TMC2226_ReadRegisters;

/**
 * \brief 			Those are possible WRITE registers in TMC2226
 */
typedef enum {
	/* General configuration registers */
	W_GCONF = 0x00u,
	WC_GSTAT = 0x01u,							/* Status register. Write 1 bit to clear flag */
	W_NODECONF = 0x03u,
	W_OTP_PROG = 0x04u,
	W_FACTORY_CONF = 0x07u,
	/* Velocity Dependent Control */
	W_IHOLD_IRUN = 0x10u,
	W_TPOWERDOWN = 0x11u,
	W_TPWMTHRS = 0x13u,
	W_VACTUAL = 0x22u,
	/* StallGuard Control */
	W_TCOOLTHRS = 0x14u,
	W_SGTHRS = 0x40u,
	W_COOLCONF = 0x42u,
	/*  Chopper Control Registers */
	W_CHOPCONF = 0x6Cu,
	W_PWMCONF = 0x70u
} TMC2226_WriteRegisters;

/*
 * \brief			Possible values for MRES in CHOPCONF register
 * \note			For getting resolution just use simple calculation
 * 					resolution = 1 << (8 - TMC2226_MRES_steps);
 */
typedef enum {
	uSteps_256 = 0b0000u,
	uSteps_128 = 0b0001u,
	uSteps_64 = 0b0010u,
	uSteps_32 = 0b0011u,
	uSteps_16 = 0b0100u,
	uSteps_8 = 0b0101u,
	uSteps_4 = 0b0110u,
	uSteps_2 = 0b0111u,
	fullStep = 0b1000u
} TMC2226_MRES_steps;


/**
 * \brief			This is a basic TMC2226 structure type
 * \note 			No note yet ;)
 */
typedef struct {
	TIM_HandleTypeDef* htim;					/* TIMER handler pointer */
	UART_HandleTypeDef* huart;					/* UART handler pointer */
	TMC2226_NodeAddress	node_address;			/* This is a node address */
	uint16_t engine_steps_per_full_turn; 		/* engine resolution */
	TMC2226_MRES_steps microstep_resolution; 	/* micro-steps per full step */

	float clock_constant;

	uint32_t	reg_GCONF_val;
	uint32_t 	reg_NODECONF_val;
	uint32_t 	reg_CHOPCONF_val;
} TMC_HandleTypeDef;


/* ################ API ################ */
void TMC_Init(TMC_HandleTypeDef* htmc, TMC2226_NodeAddress node_addr, TIM_HandleTypeDef* htim,
		UART_HandleTypeDef* huart, uint16_t engine_steps_per_full_turn);

void TMC_set_speed_by_UART(TMC_HandleTypeDef* htmc, float rpm_speed);

void TMC_set_angle(TMC_HandleTypeDef* htmc, uint16_t angle);


// Not implemented yet
void TMC_enable_driver(uint8_t node_address);
void TMC_disable_driver(uint8_t node_address);



/* Basic definitions used in LL section */
#define TMC2226_SYNC 0x05
#define TMC2226_READ 0x00
#define TMC2226_WRITE 0x80

/* ################ Low Level functions ################ */
uint32_t read_access(TMC_HandleTypeDef* htmc, TMC2226_ReadRegisters register_address,
		uint64_t *received_datagram, uint32_t *sent_datagram);

void write_access(TMC_HandleTypeDef* htmc, TMC2226_WriteRegisters register_address,
		uint32_t data, uint64_t *sent_datagram);

uint8_t calculate_CRC(uint8_t* datagram, uint8_t datagram_length);

uint32_t get_mask_for_given_register(TMC2226_ReadRegisters register_address);

uint32_t apply_mask_and_convert(uint32_t mask, uint64_t received_datagram);

#endif /* INC_TMC2226_H_ */
