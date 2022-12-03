/*
 * audio_demo.c
 *
 *  Created on: May 8, 2013
 *      Author: jxciee
 */

#include <stdio.h>
#include "io.h"
#include "system.h"
#include "alt_types.h"
#include "math.h"
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"



// create standard embedded type definitions
typedef   signed char   sint8;              // signed 8 bit values
typedef unsigned char   uint8;              // unsigned 8 bit values
typedef   signed short  sint16;             // signed 16 bit values
typedef unsigned short  uint16;             // unsigned 16 bit values
typedef   signed long   sint32;             // signed 32 bit values
typedef unsigned long   uint32;             // unsigned 32 bit values
typedef         float   real32;             // 32 bit real values

// Global variables
#define MAX_SAMPLES 				 0x10000  //max sample data (16 bits each) for SDRAM

uint32 ECHO_CNT = 0;                      // index into buffer
uint32 SAMPLE_CNT = 0;                    //keep track of which sample is being read from SDRAM
uint32 CHANNELS = 0;
volatile uint16 TOGGLE = 0;
volatile uint16 sample = 0;
volatile uint16 filtered_data = 0;



#define FIRST_TIME         1                // 1= means it is the first time running, so the file is loaded in SRAM

//set up pointers to peripherals
uint16* SdramPtr    = (uint16*)NEW_SDRAM_CONTROLLER_0_BASE;
uint32* AudioPtr    = (uint32*)AUDIO_0_BASE;
uint32* TimerPtr    = (uint32*)TIMER_0_BASE;
uint32* SWPtr       = (uint32*)SWITCHES_BASE;

uint16* FilterPtr   = (uint16*)AUDIO_FILTER_0_BASE;
uint16* FilterAddr  = (uint16*)AUDIO_FILTER_0_BASE + 2;

  //In this ISR, most of the processing is performed.  The timer interrupt is set for 20.83 us which is
  // 1/48000.  By setting the timer interrupt at the sampling rate, a new sample is never missed and the
  // audio output fifo never gets overloaded.  this is easier than using the interrupts provided with the
  // audio core
void timer_isr(void *context)
{
		//clear timer interrupt
        *TimerPtr = 0;

	    if (SAMPLE_CNT < MAX_SAMPLES)
	    {
	    	sample = SdramPtr[SAMPLE_CNT++];//read sample first
	    	*FilterPtr = sample;
	    	filtered_data = *FilterPtr;

	    	if (CHANNELS == 2)                       //only read right sample if stereo mode
	    	{
	    		AudioPtr[3] = filtered_data;       //in stereo, output to both sides
	    	    AudioPtr[2] = filtered_data;
	    	}
	    	else
	    	{
	    		AudioPtr[3] = sample;
	    		AudioPtr[2] = sample;
	    	}
	    }
	    else      //this will allow continuous looping of audio.  comment this out to only play once
	    {
	    	SAMPLE_CNT = 0;
	    }
	return;
}

void switch_isr()
{
	*FilterAddr = 1;
	uint32 temp = *SWPtr;

	if(temp == 1)
	{
		*SWPtr = 1;
	}
	else
	{
		*SWPtr = 2;
	}
	*FilterAddr = 0; // setting the audio filter Ip to write mode
	SWPtr[3] = 0;// clearing edge capture
}


//this function reads a .wav file and stores the data in the SDRAM
//first it parses the header and stores that information in variables.
//Only files with 48K sample rates and 16 bit data will work with this program.

void read_file(void)
{
	//buffers for the header information
	uint8 ChunkID[4], Format[4], Subchunk1ID[4], Subchunk2ID[4];
	uint32 ChunkSize,Subchunk1Size, SampleRate, ByteRate, Subchunk2Size;
	uint16 AudioFormat, NumChannels, BlockAlign, BitsPerSample;
	uint16 Data;
	FILE* fp;
	uint32 i = 0;

	//start reading
	  fp = fopen("/mnt/host/audioTones.wav", "r");

	  if(fp == NULL)
	  {
	    printf("error, no file open!\n");
	  }

	  else
	  {
	    printf("file opened.\n");
	    fread(ChunkID,1,4,fp);
	    fread(&ChunkSize,4,1,fp);
	    fread(Format,1,4,fp);
	    fread(Subchunk1ID,1,4,fp);
	    fread(&Subchunk1Size,4,1,fp);
	    fread(&AudioFormat,2,1,fp);
	    fread(&NumChannels,2,1,fp);
	    fread(&SampleRate,4,1,fp);
	    fread(&ByteRate,4,1,fp);
	    fread(&BlockAlign,2,1,fp);
	    fread(&BitsPerSample,2,1,fp);
	    fread(&Subchunk2ID,1,4,fp);
	    fread(&Subchunk2Size,4,1,fp);

	    CHANNELS = NumChannels;

	    while (i < MAX_SAMPLES)
	    {

	    	fread(&Data, 2, 1, fp);  //read the file in one short int at a time
	    	SdramPtr[i++] = Data;   //store in sdram.

	    }
	    printf("file read \n");  //let user know file was read
	  }
}

int main(void)
{
	printf("ESD-I Audio Filter Program Running.\n");

#if (FIRST_TIME)
	read_file();
#endif

   //Register interrupts
   //Use legacy register because the audio core forces the system to legacy
	//alt_irq_register(TIMER_0_IRQ,0,timer_isr);
	alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,TIMER_0_IRQ,timer_isr,0,0);
	alt_ic_isr_register(SWITCHES_IRQ_INTERRUPT_CONTROLLER_ID, SWITCHES_IRQ, switch_isr, 0, 0);
   //initialize timer interrupt 48Khz
	TimerPtr[4] = 3;
	SWPtr[2] = 3;
	while (1)
	{
	}
	return 0;
}