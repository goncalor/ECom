#include <iostream>


using namespace std;
/*********************************************************************
 *
 *                Example 02 - Run-time Linking
 *
 *********************************************************************
 * FileName:        console.cpp
 * Dependencies:    None
 * Compiler:        Borland C++ Builder 6
 * Company:         Copyright (C) 2004 by Microchip Technology, Inc.
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
 * Rawin Rojvanit       11/19/04
 ********************************************************************/

//---------------------------------------------------------------------------

#pragma hdrstop

#include <stdio.h>
#include "windows.h"
#include "mpusbapi.h"                   // MPUSBAPI Header File

//---------------------------------------------------------------------------
#pragma argsused

// Global Vars
char vid_pid[]= "vid_04d8&pid_000c";    // Default Demo Application Firmware
char out_pipe[]= "\\MCHP_EP1";
char in_pipe[]= "\\MCHP_EP1";

DWORD temp;

HINSTANCE libHandle;
HANDLE myOutPipe;
HANDLE myInPipe;

//---------------------------------------------------------------------------
// Prototypes
void GetSummary(void);
void LoadDLL(void);
void GetDataFromPIC(void);
char ReadAddress(char address);
void WriteToAddress(char address,char data);
void ReadAddress45(char address,char* cenas);
DWORD SendReceivePacket(BYTE *SendData, DWORD SendLength, BYTE *ReceiveData,
                    DWORD *ReceiveLength, UINT SendDelay, UINT ReceiveDelay);
void CheckInvalidHandle(void);

//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    INT8 deltax,deltay;
    POINT point;
    BOOLEAN bQuit;
    DWORD selection;
    bQuit = false;

    int d;
    UINT16 i,j,k;
    char buffer[64];
    INT8 cor;
    HDC hdcScreen;
    HDC MemDCExercising;
    HBITMAP bm;
    HPEN hPen;
    HBRUSH hBrush;

    // Load DLL when it is necessary, i.e. on start-up!
    LoadDLL();

    // Always a good idea to initialize the handles
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;

    printf("Microchip Technology Inc., 2004\r\n");
    printf("===============================\r\n");
    while(!bQuit)
    {
        printf("Select an option\r\n");/*
        printf("[1] Get this application version\r\n");
        */printf("[0] List boards present\r\n");/*
        printf("[3] Asks the PIC for the status of the switches S2 and S3\r\n");
        */
        printf("[1] Read from address 0x00\r\n");
        printf("[2] Write to address 0x0d\r\n");
        printf("[3] Read from address 0x0d\r\n");
        printf("[4] Read from addresses 0x03 e 0x04\r\n");
        printf("[5] Quit\r\n");
        printf("[6] Begin Mouse Operation\r\n");
        printf("[7] Camera thingy\r\n>>");
        scanf("%d",&selection);

        switch(selection)
        {/*
            case 1:
                temp = MPUSBGetDLLVersion();
                printf("MPUSBAPI Version: %d.%d\r\n",HIWORD(temp),LOWORD(temp));
                break;*/
            case 0:
                GetSummary();
                break;/*
            case 3:
                GetDataFromPIC();
                break;

        */
            case 1:
                ReadAddress(0x00);
                break;
            case 2:
                WriteToAddress(0x0d,0x01);
                break;
            case 3:
                ReadAddress(0x0d);
                break;
            case 4:
                ReadAddress(0x03);
                ReadAddress(0x04);
                break;
            case 5:
                bQuit = true;
                break;
            case 6:
                while(1){
                    deltax = (INT8) ReadAddress(0x03);
                    deltay = (INT8) ReadAddress(0x04);

                    if(GetCursorPos(&point)){
                        if(!SetCursorPos(point.x - deltax,point.y + deltay)){
                            puts("Erro: Escrita da posicao do rato");
                            break;
                        }
                    }else{
                        puts("Erro: Leitura da posicao do rato");
                        break;
                    }
                }
                break;
            case 7:
while(1)
{
                WriteToAddress(0x0b,0x00);  // write any value to reset (start reading from pixel 0)
                d = 20;
                cor = 0;
                hdcScreen = GetDC( NULL );
                MemDCExercising = CreateCompatibleDC(hdcScreen);
                bm = CreateCompatibleBitmap(hdcScreen, 15*d,15*d);
                /*for(i=0;i<15;i++){
                    for(j=0;j<15;j++){
                        cor = (UINT8) ReadAddress(0x0b);
                        cor = cor & 0x7f;
                        SelectObject(MemDCExercising, bm);
                        hBrush = CreateSolidBrush(RGB(cor,cor,cor));
                        SelectObject(MemDCExercising, hBrush);
                        hPen = CreatePen(PS_SOLID ,1,RGB(cor,cor,cor));
                        SelectObject(MemDCExercising, hPen);
                        Rectangle(MemDCExercising, i*d,j*d,(i+1)*d,(j+1)*d);
                    }
                }*/
                //for(i=4;i>=0;i--)
                for(i=0;i<5;i++)
                {
                	ReadAddress45(0x0b,buffer);
                	//for(j=2;j>=0;j--)
                	for(j=0;j<3;j++)
                    {
                		//for(k=14;k>=0;k--)
                		for(k=0;k<15;k++)
                		{
                			cor = buffer[(j*15+k)];
                			cor = cor & 0x7f;
                            SelectObject(MemDCExercising, bm);
                            hBrush = CreateSolidBrush(RGB(cor,cor,cor));
                            SelectObject(MemDCExercising, hBrush);
                            hPen = CreatePen(PS_SOLID ,1,RGB(cor,cor,cor));
                            SelectObject(MemDCExercising, hPen);
                            //Rectangle(MemDCExercising, (i*3+j)*d,(14-k)*d,((i*3+j)+1)*d,(15-k)*d);
                            //Rectangle(MemDCExercising, (j+3*i)*d, (k)*d, (j+3*i+1)*d, (k+1)*d);
                            if(Rectangle(MemDCExercising, ((j+3*i))*d, (k)*d, ((j+3*i+1))*d, ((k+1))*d)==0)
                                puts("Rectangle() error.");
                            DeleteObject(hBrush);
                            DeleteObject(hPen);
                		}
                	}
                }
                BitBlt(hdcScreen, 10, 10, 15*d, 15*d, MemDCExercising, 0, 0, SRCCOPY);
                DeleteObject(bm);
                DeleteDC(MemDCExercising);
}
                break;
            default:
                break;
        }// end switch

        fflush(stdin);printf("\r\n");
    }//end while

    // Always check to close all handles before exiting!
    if (myOutPipe != INVALID_HANDLE_VALUE) MPUSBClose(myOutPipe);
    if (myInPipe != INVALID_HANDLE_VALUE) MPUSBClose(myInPipe);
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;

    // Always check to close the library too.
    if (libHandle != NULL) FreeLibrary(libHandle);

    return 0;
}//end main

//---------------------------------------------------------------------------

void GetSummary(void)
{
    HANDLE tempPipe = INVALID_HANDLE_VALUE;
    DWORD count = 0;
    DWORD max_count;

    max_count = MPUSBGetDeviceCount(vid_pid);

    printf("\r\n%d device(s) with %s currently attached\r\n",max_count,vid_pid);

    // Note:
    // The total number of devices using the generic driver could be
    // bigger than max_count. They could have different vid & pid numbers.
    // This means if max_count is 2, the valid instance index do not
    // necessary have to be '0' and '1'.
    //
    // Below is a sample code for searching for all valid instance indexes.
    // MAX_NUM_MPUSB_DEV is defined in _mpusbapi.h

    count = 0;
    for(int i = 0; i < MAX_NUM_MPUSB_DEV; i++)
    {
        tempPipe = MPUSBOpen(i,vid_pid,NULL,MP_READ,0);
        if(tempPipe != INVALID_HANDLE_VALUE)
        {
            printf("Instance Index # %d\r\n",i);
            MPUSBClose(tempPipe);
            count++;
        }
        if(count == max_count) break;
    }//end for
    printf("\r\n");
}//end GetSummary

//---------------------------------------------------------------------------

void LoadDLL(void)
{
    libHandle = NULL;
    libHandle = LoadLibrary("mpusbapi");
    if(libHandle == NULL)
    {
        printf("Error loading mpusbapi.dll\r\n");
    }
    else
    {
        MPUSBGetDLLVersion=(DWORD(*)(void))\
                    GetProcAddress(libHandle,"_MPUSBGetDLLVersion");
        MPUSBGetDeviceCount=(DWORD(*)(PCHAR))\
                    GetProcAddress(libHandle,"_MPUSBGetDeviceCount");
        MPUSBOpen=(HANDLE(*)(DWORD,PCHAR,PCHAR,DWORD,DWORD))\
                    GetProcAddress(libHandle,"_MPUSBOpen");
        MPUSBWrite=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))\
                    GetProcAddress(libHandle,"_MPUSBWrite");
        MPUSBRead=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))\
                    GetProcAddress(libHandle,"_MPUSBRead");
        MPUSBReadInt=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))\
                    GetProcAddress(libHandle,"_MPUSBReadInt");
        MPUSBClose=(BOOL(*)(HANDLE))GetProcAddress(libHandle,"_MPUSBClose");
        MPUSBSetConfiguration=(DWORD(*)(HANDLE,USHORT))\
                    GetProcAddress(libHandle,"_MPUSBSetConfiguration");
        MPUSBGetStringDescriptor = \
                    (DWORD(*)(HANDLE,UCHAR,USHORT,PVOID,DWORD,PDWORD))\
                    GetProcAddress(libHandle,"_MPUSBGetStringDescriptor");
        MPUSBGetConfigurationDescriptor = \
                   (DWORD(*)(HANDLE,UCHAR,PVOID,DWORD,PDWORD))\
                   GetProcAddress(libHandle,"_MPUSBGetConfigurationDescriptor");
        MPUSBGetDeviceDescriptor = (DWORD(*)(HANDLE,PVOID,DWORD,PDWORD))\
                   GetProcAddress(libHandle,"_MPUSBGetDeviceDescriptor");

        if((MPUSBGetDeviceCount == NULL) || (MPUSBOpen == NULL) ||
            (MPUSBWrite == NULL) || (MPUSBRead == NULL) ||
            (MPUSBClose == NULL) || (MPUSBGetDLLVersion == NULL) ||
            (MPUSBReadInt == NULL) || (MPUSBSetConfiguration == NULL) ||
            (MPUSBGetStringDescriptor == NULL) ||
            (MPUSBGetConfigurationDescriptor == NULL) ||
            (MPUSBGetDeviceDescriptor == NULL))
            printf("GetProcAddress Error\r\n");
    }//end if else
}//end LoadDLL

//---------------------------------------------------------------------------

void GetDataFromPIC(void)
{
    // First we need to open data pipes...
    DWORD selection;
    selection = 0; // Assumes only one board is connected to PC through USB and it has index 0
    fflush(stdin);

    myOutPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_WRITE,0);
    myInPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_READ,0);
    if(myOutPipe == INVALID_HANDLE_VALUE || myInPipe == INVALID_HANDLE_VALUE)
    {
        printf("Failed to open data pipes.\r\n");
        return;
    }//end if


    // This Computer Electronics Course Demo has a simple communication protocol.
    // To read the status of switches S2 and S3 one has to send the MY_COMMAND command
    // which is defined as 0x00, follows by the length of the
    // expected result, in this case is 2 bytes, S2 status, and S3 status.
    // i.e. <MY_COMMAND><0x02>
    //
    // The response packet from the board has the following format:
    // <MY_COMMAND><0x02><S2><S3>

    // The receive buffer size must be equal to or larger than the maximum
    // endpoint size it is communicating with. In this case, 64 bytes.

    BYTE send_buf[64],receive_buf[64];
    DWORD RecvLength=4;

    #define MY_COMMAND    0
    send_buf[0] = MY_COMMAND;      // Command
    send_buf[1] = 0x02;              // Expected length of the result

    if(SendReceivePacket(send_buf,2,receive_buf,&RecvLength,1000,1000) == 1)
    {
        if(RecvLength == 4 && receive_buf[0] == MY_COMMAND &&
            receive_buf[1] == 0x02)
        {
            if(receive_buf[2]==1 && receive_buf[3]==1) printf("\nS2 and S3 pressed\r\n");
            else if(receive_buf[2]==1 && receive_buf[3]==0) printf("\nS2 pressed\r\n");
            else if(receive_buf[2]==0 && receive_buf[3]==1) printf("\nS3 pressed\r\n");
            else printf("\nno switch is pressed\r\n");
        }
    }
    else
        printf("USB Operation Failed\r\n");

    // Let's close the data pipes since we have nothing left to do..
    MPUSBClose(myOutPipe);
    MPUSBClose(myInPipe);
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;

}//end GetUSBDemoFWVersion

char ReadAddress(char address)
{
    // First we need to open data pipes...
    DWORD selection;
    selection = 0; // Assumes only one board is connected to PC through USB and it has index 0
    fflush(stdin);

    myOutPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_WRITE,0);
    myInPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_READ,0);
    if(myOutPipe == INVALID_HANDLE_VALUE || myInPipe == INVALID_HANDLE_VALUE)
    {
        printf("Failed to open data pipes.\r\n");
        return 0;
    }//end if


    // Read command: 3
    // Request format: <Read Command><Address><Expected Reply Length>
    // Reply format: <Read Command><Address><Expected Reply Length><Address content>

    BYTE send_buf[64],receive_buf[64];
    DWORD RecvLength=4;

    send_buf[0] = 3;      // Command
    send_buf[1] = (BYTE) address;
    send_buf[2] = 0x01;              // Expected length of the result

    if(SendReceivePacket(send_buf,3,receive_buf,&RecvLength,1000,1000) == 1)
    {
        /*if(*/RecvLength == 4 && receive_buf[0] == 3 &&
            receive_buf[2] == 0x01/*)*/;
        //{
            //printf("Value %x read from address %x\r\n",receive_buf[3],receive_buf[1]);
        //}
    }
    else
        printf("USB Operation Failed\r\n");

    // Let's close the data pipes since we have nothing left to do..
    MPUSBClose(myOutPipe);
    MPUSBClose(myInPipe);
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;
    return(receive_buf[3]);
}
void WriteToAddress(char address,char data)
{
    // First we need to open data pipes...
    DWORD selection;
    selection = 0; // Assumes only one board is connected to PC through USB and it has index 0
    fflush(stdin);

    myOutPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_WRITE,0);
    myInPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_READ,0);
    if(myOutPipe == INVALID_HANDLE_VALUE || myInPipe == INVALID_HANDLE_VALUE)
    {
        printf("Failed to open data pipes.\r\n");
        return;
    }//end if


    // Write command: 4
    // Request format: <Command><Address><Content><Expected Reply Length>
    // Reply Format: <Command><Address><Content><Expected Reply Length><Success>
    BYTE send_buf[64],receive_buf[64];
    DWORD RecvLength=5;

    send_buf[0] = 4;      // Command
    send_buf[1] = (BYTE) address | 0x80;
    send_buf[2] = (BYTE) data;
    send_buf[3] = 0x01;              // Expected length of the result

    if(SendReceivePacket(send_buf,4,receive_buf,&RecvLength,1000,1000) == 1)
    {
        if(RecvLength == 5 && receive_buf[0] == 4 &&
            receive_buf[3] == 0x01)
        {
            if(receive_buf[4]==1)
            {
                /*printf("Write success.\r\n");*/
            }
            else printf("Write failure.\r\n");
        }
    }
    else
        printf("USB Operation Failed\r\n");

    // Let's close the data pipes since we have nothing left to do..
    MPUSBClose(myOutPipe);
    MPUSBClose(myInPipe);
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;

}


void ReadAddress45(char address,char* cenas)
{
    // First we need to open data pipes...
    DWORD selection;
    selection = 0; // Assumes only one board is connected to PC through USB and it has index 0
    fflush(stdin);

    myOutPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_WRITE,0);
    myInPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_READ,0);
    if(myOutPipe == INVALID_HANDLE_VALUE || myInPipe == INVALID_HANDLE_VALUE)
    {
        printf("Failed to open data pipes.\r\n");
        return;
    }//end if


    // Read command: 3
    // Request format: <Read Command><Address><Expected Reply Length>
    // Reply format: <Read Command><Address><Expected Reply Length><Address content>

    BYTE send_buf[64],receive_buf[64];
    DWORD RecvLength=49;
    UINT16 i;

    send_buf[0] = 7;      // Command
    send_buf[1] = (BYTE) address;
    send_buf[2] = 46;              // Expected length of the result

    if(SendReceivePacket(send_buf,3,receive_buf,&RecvLength,1000,1000) == 1)
    {
        /*if(RecvLength == 4 && receive_buf[0] == 3 &&
            receive_buf[2] == 0x01)*/;
        //{
            //printf("Value %x read from address %x\r\n",receive_buf[3],receive_buf[1]);
        //}
        /*if(receive_buf[3+45]=='r'){
            printf("A mensagem secreta chegou\r\n");
        }else{
            printf("A mensagem secreta NAO chegou\r\n");
        }*/
        for(i=0;i<45;i++){
            cenas[i] = receive_buf[3+i];/*
            if(i!=0){
                if(receive_buf[i]==receive_buf[i-1]){
                    printf("O problema é do PIC\r\n");
                }else{
                    printf("ILIBEM O PIC, É INOCENTE\r\n");
                }
            }*/
        }
    }
    else
        printf("USB Operation Failed\r\n");

    // Let's close the data pipes since we have nothing left to do..
    MPUSBClose(myOutPipe);
    MPUSBClose(myInPipe);
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;


    return;
}


//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
//
// A typical application would send a command to the target device and expect
// a response.
// SendReceivePacket is a wrapper function that facilitates the
// send command / read response paradigm
//
// SendData - pointer to data to be sent
// SendLength - length of data to be sent
// ReceiveData - Points to the buffer that receives the data read from the call
// ReceiveLength - Points to the number of bytes read
// SendDelay - time-out value for MPUSBWrite operation in milliseconds
// ReceiveDelay - time-out value for MPUSBRead operation in milliseconds
//

DWORD SendReceivePacket(BYTE *SendData, DWORD SendLength, BYTE *ReceiveData,
                    DWORD *ReceiveLength, UINT SendDelay, UINT ReceiveDelay)
{
    DWORD SentDataLength;
    DWORD ExpectedReceiveLength = *ReceiveLength;

    if(myOutPipe != INVALID_HANDLE_VALUE && myInPipe != INVALID_HANDLE_VALUE)
    {
        if(MPUSBWrite(myOutPipe,SendData,SendLength,&SentDataLength,SendDelay))
        {

            if(MPUSBRead(myInPipe,ReceiveData, ExpectedReceiveLength,
                        ReceiveLength,ReceiveDelay))
            {
                if(*ReceiveLength == ExpectedReceiveLength)
                {
                    return 1;   // Success!
                }
                else if(*ReceiveLength < ExpectedReceiveLength)
                {
                    return 2;   // Partially failed, incorrect receive length
                }//end if else
            }
            else
                CheckInvalidHandle();
        }
        else
            CheckInvalidHandle();
    }//end if

    return 0;  // Operation Failed
}//end SendReceivePacket

//---------------------------------------------------------------------------

void CheckInvalidHandle(void)
{
    if(GetLastError() == ERROR_INVALID_HANDLE)
    {
        // Most likely cause of the error is the board was disconnected.
        MPUSBClose(myOutPipe);
        MPUSBClose(myInPipe);
        myOutPipe = myInPipe = INVALID_HANDLE_VALUE;
    }//end if
    else
        printf("*Error Code %d\r\n",GetLastError());
}//end CheckInvalidHandle

//---------------------------------------------------------------------------
