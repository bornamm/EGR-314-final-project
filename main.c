#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/eusart1.h"
#include "mcc_generated_files/examples/i2c1_master_example.h"
#include "RFID.h"
#include "Stepper_driver.h"
#include "math.h"
#include <stdio.h>
#include <string.h>

// timer interrupt
double time;
uint16_t ms=0;
uint16_t s=0;
bool spin=0;
// RFID stuff
uint8_t *RFID_Buff[64];
unsigned char status;
unsigned char str[MAX_LEN];
// mic stuff
bool start=0;
typedef uint16_t adc_result_t;
adc_result_t valueOfADCC;
double voltage;
uint8_t voltageC;

// EUSART stuff
char id='B';                               // CHANGE THIS , FLAG SHOULD BE IN BOUND OF "ID*10" & "ID*10+5"
int request_key;
int key_read=0;
volatile uint8_t RxData=15;
uint8_t TxData;
// functions
void timer_callback();
void check_data();
void empty_read();
void DRV_rotate( uint8_t rotations);
void ESUART_read();
void ESUART_write_long(double publish);
void ESUART_write_str(char strTX[80]);
void ESUART_write(uint8_t publish);


void main(void){
    SYSTEM_Initialize();
    INTERRUPT_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART1_Initialize();
    I2C1_Initialize();
    I2C2_Initialize();
    TMR2_Initialize();
    TMR2_SetInterruptHandler(timer_callback);    
    __delay_ms(200);
    ADCC_Initialize();
    MFRC522_Init();
    TMR2_Start();
    
    while(1){         
        empty_read();
        //mic stuff
//        ADCC_StartConversion(channel_AND7_mic);
//        if(!ADCC_IsConversionDone()){};
//        valueOfADCC     =  ADCC_GetConversionResult();
//        voltage         = (valueOfADCC*(3.30))/1024;
//        valueOfADCC = valueOfADCC & 0x00FF;
//        uint8_t test1 = (uint8_t)(valueOfADCC);
//        voltageC=(voltage*100)-200;
//        ESUART_write_str(valueOfADCC); // print mic voltage to MQTT
//        ESUART_write(test1);
//        ESUART_write(voltageC);
        
        while(RxData!=id){ // && 11>RxData
            ESUART_read();
        }
        ESUART_write_str("request key received, spinning");
        
        while(key_read==0){

            DRV_rotate(1); // turn quarter of the way ONCE
            // RFID stuff
            __delay_ms(200);
            status = MFRC522_Request(PICC_REQIDL, str); // return status value to status, populate str with FIFO data. Find cards, return card type - Don't remove this sub
            status = MFRC522_Anticoll(str);             // Anti-collision, return card serial number 4 bytes
            memcpy(serNum, str, 5);                     // serNum is serial number with bits 1:PICC_ANTICOLL and bit2:0x20 copy over 5 bytes from raw SN to modified SN
            __delay_ms(200);
            if (status == MI_OK )                       // if there is a new card detected  if (status == MI_OK || status == MI_NOTAGERR)
            {
             SetFormatRDM630();                         // take serNum and format it and return read     
             check_data();                              // check tag data
             ESUART_write_str("n");
//             char str2[50];
//             sprintf(str2, "key serial number: %d%d%d%d%d%d%d%d",read[0],read[1],read[2],read[3], read[4], read[5], read[6], read[7]); //sprintf(str2, "ID1:%d %d %d %d", read[4], read[5], read[6], read[7]);//prints 450
//             ESUART_write_str(str2);                    // print tag
            }
            else
            {  
             MFRC522_Halt();                            // Command card into hibernation
             __delay_ms(50);                            // wait for low consumption
            }
        }
        
        RxData=0;
        key_read=0;
        spin=0;
        
    }
}


void empty_read()
{
    for(int r=0; r<11; r++){
        read[r]=0;
    }
}

void timer_callback()
{ 
    
    ms= ms+1;
    if(ms >= 1000){
        IO_RD0_LED1_Toggle();
        ms= ms-1000;
        s=s+1;
//        time = (ms/1000.00) + s;
    }
     
}

void check_data()
{
    int checkByte1=0;
    int checkByte2=0;
    int checkByte3=0;
    int checkByte4=0;
    while ( checkByte1!=9 && checkByte2!=9 && checkByte3!=9 && checkByte4!=9)//checkByte1!=9 && checkByte2!=9 && checkByte3!=9 && checkByte4!=9
    {
        for(int d=0; d<8; d++){
            if(keyI[d] == read[d])
            {
                checkByte1++;
                if(checkByte1>7){
                    key_read=1;
                    ESUART_write_str("keyI");
                    DRV_rotate(1);
                    checkByte1=9;
                }
            }
            if(keyII[d] == read[d])
            {
                checkByte2++;
                if(checkByte2>7){
                    key_read=2;
                    ESUART_write_str("keyII");
                    DRV_rotate(1);
                    checkByte2=9;
                }
            }
            if(keyIII[d] == read[d])
            {
                checkByte3++;
                if(checkByte3>7){
                    key_read=3;
                    ESUART_write_str("keyIII");
                    DRV_rotate(1);
                    checkByte3=9;
                }
            }
            if(keyIV[d] == read[d])
            {
                checkByte4++;
                if(checkByte4>7){
                    key_read=4;
                    ESUART_write_str("keyIV");
                    DRV_rotate(1);
                    checkByte4=9;
                }
            }
        }
    }
}

void DRV_rotate( uint8_t rotations)
{
    DRV_initialize();
    if (IO_RD6_FAULT_GetValue()==0)
    {
        ESUART_write_str("Fault exists");
        uint8_t Fault_reg_val;
        Fault_reg_val = DRV_read_register(DRV_F);
        __delay_ms(200);
        ESUART_write(Fault_reg_val);
        __delay_ms(200);
        IO_RD0_LED1_SetHigh();
        IO_RD1_LED2_SetHigh();
        __delay_ms(200);
        IO_RD0_LED1_SetLow();
        IO_RD1_LED2_SetLow();
        __delay_ms(200);
    }
    for(int r=0; r<rotations; r++ )
    {
        for(int x=0; x<37; x++) // full rotation is 256 , 64 is quarter, 32 eight
        {
            DRV_write_register(DRV_CNTRL, FORWARD_high); //I2C2_Write1ByteRegister(DRV_W,  reg_addr,  data); FORWARD_high
            __delay_ms(3);
            DRV_write_register(DRV_CNTRL, REVERSE_high); //I2C2_Write1ByteRegister(DRV_W,  reg_addr,  data); REVERSE_high
            __delay_ms(3);
            
        }
        DRV_write_register(DRV_CNTRL, stapp_low);
    }
}

void ESUART_read()
{
    if(EUSART1_is_rx_ready()){
        RxData=EUSART1_Read();
    }
    //add code for parsing musical notes?
}

void ESUART_write_long(double publish)
{
    if(EUSART1_is_tx_ready())
    {
//        double Vf= (double)(publish);
        char strTX[80];
        sprintf(strTX,"%0.9f",publish);
        for(int i=0; i<80; i++)
        {
            TxData=strTX[i];  
            EUSART1_Write(TxData);
            __delay_ms(20);
        }
        EUSART1_Write('\r');
    }
}

void ESUART_write_str(char strTX[80])
{
    if(EUSART1_is_tx_ready())
    {
        
        for(int i=0; i<80; i++)
        {
            TxData=strTX[i];  
            EUSART1_Write(TxData);
            __delay_ms(20);
        }
        EUSART1_Write('\r');
    }
}

void ESUART_write(uint8_t publish)
{
    if(EUSART1_is_tx_ready())
    {
        char strTX[8];
        sprintf(strTX,"%u",publish);
        for(int i=0; i<8; i++)
        {
            TxData=strTX[i];  
            EUSART1_Write(TxData);
            __delay_ms(20);
        }
        EUSART1_Write('\r');
    }
}