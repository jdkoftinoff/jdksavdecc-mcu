#pragma once
/*
  Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   3. Neither the name of J.D. Koftinoff Software, Ltd. nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef JDKSAVDECCMCU_BARE_METAL

#include "JDKSAvdeccMCU_World.hpp"

#include "Arduino.h"
#include "JDKSAvdeccMCU.hpp"
#include "JDKSAvdeccMCU_WizNetIO.hpp"
#include "Ethernet.h"
#include "SPI.h"

#else

#include "JDKSAvdeccMCU_World.hpp"
#include "JDKSAvdeccMCU_RawSocket.hpp"
#include "JDKSAvdeccMCU_RawSocketWin32.hpp"
#include "JDKSAvdeccMCU_RawSocketPcap.hpp"
#include "JDKSAvdeccMCU_RawSocketPcapFile.hpp"
#include "JDKSAvdeccMCU.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void yield( void );

#define HIGH 0x1
#define LOW 0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define SERIAL 0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define INTERNAL1V1 2
#define INTERNAL2V56 3

#define DEFAULT 1
#define EXTERNAL 0

#define min( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define max( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define abs( x ) ( ( x ) > 0 ? ( x ) : -( x ) )
#define constrain( amt, low, high ) ( ( amt ) < ( low ) ? ( low ) : ( ( amt ) > ( high ) ? ( high ) : ( amt ) ) )
#define round( x ) ( ( x ) >= 0 ? (long)( (x)+0.5 ) : (long)( (x)-0.5 ) )
#define radians( deg ) ( (deg)*DEG_TO_RAD )
#define degrees( rad ) ( (rad)*RAD_TO_DEG )
#define sq( x ) ( ( x ) * ( x ) )

#define interrupts()
#define noInterrupts()

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds( a ) ( ( a ) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles( a ) ( (a)*clockCyclesPerMicrosecond() )

#define lowByte( w ) ( ( uint8_t )( (w)&0xff ) )
#define highByte( w ) ( ( uint8_t )( ( w ) >> 8 ) )

#define bitRead( value, bit ) ( ( ( value ) >> ( bit ) ) & 0x01 )
#define bitSet( value, bit ) ( ( value ) |= ( 1UL << ( bit ) ) )
#define bitClear( value, bit ) ( ( value ) &= ~( 1UL << ( bit ) ) )
#define bitWrite( value, bit, bitvalue ) ( bitvalue ? bitSet( value, bit ) : bitClear( value, bit ) )

#ifndef _NOP
#define _NOP()                                                                                                                 \
    do                                                                                                                         \
    {                                                                                                                          \
    } while ( 0 )
#endif

typedef unsigned int word;

#define bit( b ) ( 1UL << ( b ) )

typedef uint8_t boolean;
typedef uint8_t byte;

void init( void );
void initVariant( void );

void pinMode( uint8_t, uint8_t );
void digitalWrite( uint8_t, uint8_t );
int digitalRead( uint8_t );
int analogRead( uint8_t );
void analogReference( uint8_t mode );
void analogWrite( uint8_t, int );

unsigned long millis( void );
unsigned long micros( void );
void delay( unsigned long );
void delayMicroseconds( unsigned int us );
unsigned long pulseIn( uint8_t pin, uint8_t state, unsigned long timeout );

void shiftOut( uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val );
uint8_t shiftIn( uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder );

void attachInterrupt( uint8_t, void ( * )( void ), int mode );
void detachInterrupt( uint8_t );

void setup( void );
void loop( void );

#define analogInPinToBit( P ) ( P )

#define NOT_ON_TIMER 0
#define TIMER0A 1
#define TIMER0B 2
#define TIMER1A 3
#define TIMER1B 4
#define TIMER1C 5
#define TIMER2 6
#define TIMER2A 7
#define TIMER2B 8

#define TIMER3A 9
#define TIMER3B 10
#define TIMER3C 11
#define TIMER4A 12
#define TIMER4B 13
#define TIMER4C 14
#define TIMER4D 15
#define TIMER5A 16
#define TIMER5B 17
#define TIMER5C 18

unsigned long pulseIn( uint8_t pin, uint8_t state, unsigned long timeout = 1000000L );

void tone( uint8_t _pin, unsigned int frequency, unsigned long duration = 0 );
void noTone( uint8_t _pin );

#ifdef __cplusplus
}
#endif

#define NUM_DIGITAL_PINS 70
#define NUM_ANALOG_INPUTS 16
#define analogInputToDigitalPin( p ) ( ( p < 16 ) ? (p)+54 : -1 )
#define digitalPinHasPWM( p ) ( ( ( p ) >= 2 && ( p ) <= 13 ) || ( ( p ) >= 44 && ( p ) <= 46 ) )

static const uint8_t SS = 53;
static const uint8_t MOSI = 51;
static const uint8_t MISO = 50;
static const uint8_t SCK = 52;

static const uint8_t SDA = 20;
static const uint8_t SCL = 21;
#define LED_BUILTIN 13

static const uint8_t A0 = 54;
static const uint8_t A1 = 55;
static const uint8_t A2 = 56;
static const uint8_t A3 = 57;
static const uint8_t A4 = 58;
static const uint8_t A5 = 59;
static const uint8_t A6 = 60;
static const uint8_t A7 = 61;
static const uint8_t A8 = 62;
static const uint8_t A9 = 63;
static const uint8_t A10 = 64;
static const uint8_t A11 = 65;
static const uint8_t A12 = 66;
static const uint8_t A13 = 67;
static const uint8_t A14 = 68;
static const uint8_t A15 = 69;

inline uint16_t makeWord( uint16_t w ) { return w; }
inline uint16_t makeWord( byte h, byte l ) { return ( ( (uint16_t)h ) << 8 ) + l; }

class Serial_
{
  public:
    Serial_( FILE *inf, FILE *outf ) : m_inf( inf ), m_outf( outf ) {}
    virtual ~Serial_() {}
    void begin( unsigned long ) {}
    void begin( unsigned long, uint8_t ) {}
    void end( void ) {}

    virtual int available( void ) { return 0; }
    virtual int peek( void ) { return 0; }
    virtual int read( void ) { return 0; }
    virtual void flush( void ) {}
    virtual size_t write( uint8_t c )
    {
        fputc( c, m_outf );
        return 1;
    }
    virtual size_t write( const uint8_t *p, size_t cnt )
    {
        for ( size_t i = 0; i < cnt; ++i )
        {
            write( *p++ );
        }
        return cnt;
    }
    virtual void println( const char *s ) { fprintf( m_outf, "%s\n", s ); }
    virtual void print( const char *s ) { fprintf( m_outf, "%s", s ); }

    operator bool();

  private:
    FILE *m_inf;
    FILE *m_outf;
};
extern Serial_ Serial;

#endif
