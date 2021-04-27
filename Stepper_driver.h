
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */


#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> 
#include <stdint.h>
#include <stdbool.h>  

// device addr
#define DRV_W           0x68    //0xD0 //68 is correct
#define DRV_R           0x68    //0xD1 //68 is correct
#define DRV_CNTRL       0x00
#define DRV_F           0x01
// voltages (don't write these)
#define Five_V          0x3F
#define high            0x3D
#define mid             0x1F
#define low             0x10
// speed Commands for driver
#define FORWARD_Five    0xFE
#define FORWARD_high    0xF6
#define FORWARD_mid     0x7E
#define FORWARD_low     0x42

#define REVERSE_Five    0xFD
#define REVERSE_high    0xF5
#define REVERSE_mid     0x7D
#define REVERSE_low     0x41

#define stapp_high      0xF7 
#define stapp_low       0xF4 
//  fault status
#define DRV_F_CLEAR     0x80


//  Functions
void DRV_write_register(uint8_t reg_addr, uint8_t data);
uint8_t DRV_read_register(uint8_t reg_addr);
void DRV_initialize();
void DRV_drive(uint8_t speed);
void DRV_stop();
void DRV_stepper(uint8_t speed);



#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

