/*********************************************************************
 *
 *                Microchip USB C18 Firmware Version 1.0
 *
 *********************************************************************
 * FileName:        user.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rawin Rojvanit       11/19/04    Original.
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"

#include "system\usb\usb.h"

#include "io_cfg.h"             // I/O pin mapping
#include "user\user.h"
#include "user\temperature.h"
#include "def_ports.h"								// Port definitions

/** V A R I A B L E S ********************************************************/
#pragma udata
byte old_sw2,old_sw3;
byte counter;

DATA_PACKET dataPacket;

/** P R I V A T E  P R O T O T Y P E S ***************************************/

void BlinkUSBStatus(void);
BOOL Switch2IsPressed(void);
BOOL Switch3IsPressed(void);
void ServiceRequests(void);

// For board testing purpose only
void PICDEMFSUSBDemoBoardTest(void);

/** D E C L A R A T I O N S **************************************************/
#pragma code
void UserInit(void)
{
    mInitAllLEDs();
    mInitAllSwitches();
    old_sw2 = sw2;
    old_sw3 = sw3;
    
}//end UserInit


/******************************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user routines.
 *                  It is a mixture of both USB and non-USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void ProcessIO(void)
{   
    BlinkUSBStatus();
    // User Application USB tasks
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
    
    ServiceRequests();

}//end ProcessIO


void ServiceRequests(void)
{
    byte index;
    
    if(USBGenRead((byte*)&dataPacket,sizeof(dataPacket)))
    {
        counter = 0;
        switch(dataPacket.CMD)
        {
            case READ_VERSION:
                //dataPacket._byte[1] is len
                dataPacket._byte[2] = !PB4; // Switch 2 status
                dataPacket._byte[3] = !PB5; // Switch 3 status
                counter=0x04;
                break;

            default:
                break;
        }//end switch()
        if(counter != 0)
        {
            if(!mUSBGenTxIsBusy())
                USBGenWrite((byte*)&dataPacket,counter);
        }//end if
    }//end if

}//end ServiceRequests

/******************************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus turns on and off LEDs corresponding to
 *                  the USB device state.
 *
 * Note:            mLED macros can be found in io_cfg.h
 *                  usb_device_state is declared in usbmmap.c and is modified
 *                  in usbdrv.c, usbctrltrf.c, and usb9.c
 *****************************************************************************/
void BlinkUSBStatus(void)
{
    static word led_count=0;
    
    if(led_count == 0)led_count = 10000U;
    led_count--;

    #define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
    #define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
    #define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
    #define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}

    if(UCONbits.SUSPND == 1)
    {
        if(led_count==0)
        {
            mLED_1_Toggle();
            mLED_2 = mLED_1;        // Both blink at the same time
        }//end if
    }
    else
    {
        if(usb_device_state == DETACHED_STATE)
        {
            mLED_Both_Off();
            
            PICDEMFSUSBDemoBoardTest();
        }
        else if(usb_device_state == ATTACHED_STATE)
        {
            mLED_Both_On();
        }
        else if(usb_device_state == POWERED_STATE)
        {
            mLED_Only_1_On();
        }
        else if(usb_device_state == DEFAULT_STATE)
        {
            mLED_Only_2_On();
        }
        else if(usb_device_state == ADDRESS_STATE)
        {
            if(led_count == 0)
            {
                mLED_1_Toggle();
                mLED_2_Off();
            }//end if
        }
        else if(usb_device_state == CONFIGURED_STATE)
        {
            if(led_count==0)
            {
                mLED_1_Toggle();
                mLED_2 = !mLED_1;       // Alternate blink                
            }//end if
        }//end if(...)
    }//end if(UCONbits.SUSPND...)

}//end BlinkUSBStatus

BOOL Switch2IsPressed(void)
{
    if(sw2 != old_sw2)
    {
        old_sw2 = sw2;                  // Save new value
        if(sw2 == 0)                    // If pressed
            return TRUE;                // Was pressed
    }//end if
    return FALSE;                       // Was not pressed
}//end Switch2IsPressed

BOOL Switch3IsPressed(void)
{
    if(sw3 != old_sw3)
    {
        old_sw3 = sw3;                  // Save new value
        if(sw3 == 0)                    // If pressed
            return TRUE;                // Was pressed
    }//end if
    return FALSE;                       // Was not pressed
}//end Switch3IsPressed

void TXbyte(byte data)
{
    while(TXSTAbits.TRMT==0);
    TXREG = data;
}//end TXbyte

void PICDEMFSUSBDemoBoardTest(void)
{
    byte temp;
    
    //PICDEM FS USB Demo Board Test Procedure:
    if(Switch2IsPressed())
    {
        //LEDs and push buttons testing
        mLED_1_On();
        while(!Switch2IsPressed());
        mLED_1_Off();
        mLED_2_On();
        while(!Switch3IsPressed());
        mLED_2_Off();
        mLED_3_On();
        while(!Switch3IsPressed());
        mLED_3_Off();
        mLED_4_On();
        while(!Switch3IsPressed());
        mLED_4_Off();
        
        //RS-232 Setup
        SSPCON1 = 0;        // Make sure SPI is disabled
        TRISCbits.TRISC7=1; // RX
        TRISCbits.TRISC6=0; // TX
        SPBRG = 0x71;
        SPBRGH = 0x02;      // 0x0271 for 48MHz -> 19200 baud
        TXSTA = 0x24;       // TX enable BRGH=1
        RCSTA = 0x90;       // continuous RX
        BAUDCON = 0x08;     // BRG16 = 1
        temp = RCREG;       // Empty buffer
        temp = RCREG;       // Empty buffer
        
        //RS-232 Tx & Rx Tests
        while(!Switch3IsPressed());
        TXbyte('R');
        TXbyte('S');
        TXbyte('-');
        TXbyte('2');
        TXbyte('3');
        TXbyte('2');
        TXbyte(' ');
        TXbyte('T');
        TXbyte('X');
        TXbyte(' ');
        TXbyte('T');
        TXbyte('e');
        TXbyte('s');
        TXbyte('t');
        TXbyte(' ');
        TXbyte('O');
        TXbyte('K');
        TXbyte(',');
        TXbyte(' ');
        TXbyte('P');
        TXbyte('r');
        TXbyte('e');
        TXbyte('s');
        TXbyte('s');
        TXbyte(' ');
        TXbyte('"');
        TXbyte('r');
        TXbyte('"');
        TXbyte(',');
        while(PIR1bits.RCIF==0);        //Wait for data from RS232
        if(RCREG == 'r')
        {
            TXbyte(' ');
            TXbyte('R');
            TXbyte('X');
            TXbyte(' ');
            TXbyte('T');
            TXbyte('e');
            TXbyte('s');
            TXbyte('t');
            TXbyte(' ');
            TXbyte('O');
            TXbyte('K');
        }//end if
        UserInit();                     //Re-initialize default user fw
        //Test phase 1 done
    }//end if
}//end PICDEMFSUSBDemoBoardTest()

/** EOF user.c ***************************************************************/
