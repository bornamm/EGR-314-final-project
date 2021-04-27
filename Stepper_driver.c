#include <xc.h>
#include "Stepper_driver.h"
#include <stdbool.h>
#include "mcc_generated_files/device_config.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"
#include "mcc_generated_files/eusart1.h"
#include "math.h"


void DRV_write_register(uint8_t reg_addr, uint8_t data)
{
   I2C2_Write1ByteRegister(DRV_W,  reg_addr,  data); 
}

uint8_t DRV_read_register(uint8_t reg_addr) 
{
	uint8_t out = I2C2_Read1ByteRegister(DRV_R, reg_addr);//DRV_R
    return out;
}


void DRV_initialize()
{
    DRV_write_register(DRV_F, DRV_F_CLEAR);
}


/*
 * speed should be from speed control definitions (reverse/ forward , the speed to rotate at)
 */
void DRV_drive(uint8_t speed)
{
    DRV_write_register(DRV_CNTRL, speed);
}


void DRV_stop()
{
    DRV_write_register(DRV_CNTRL, stapp_low);
}


/* 5.625/32 degrees per step??
 * FAULT pin is D6 change accordingly, if fault keep toggling
 * input speed
 * input # rotations
 * 
 */
void DRV_stepper(uint8_t speed)
{
        for(int x=0; x<2000; x++){
//            DRV_initialize();
            DRV_drive(speed);
            __delay_ms(10);
            DRV_stop();
            __delay_ms(10);
        }
    
}
