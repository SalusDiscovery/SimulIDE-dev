/***************************************************************************
 *   Copyright (C) 2021 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#include "twimodule.h"
#include "iopin.h"
#include "simulator.h"

TwiModule::TwiModule( QString name )
         : eElement( name )
         , eClockedDevice()

{
    m_sda = NULL;
    m_scl = NULL;
    m_addrBits = 7;
}
TwiModule::~TwiModule( ){}

void TwiModule::initialize()
{
    m_mode      = TWI_OFF;
    m_twiState  = TWI_NO_STATE;
    m_i2cState  = I2C_IDLE;
    m_lastState = I2C_IDLE;

    m_sheduleSDA = false;
    m_toggleScl  = false;

    m_lastSDA = true; // SDA High = inactive
}

void TwiModule::stamp()      // Called at Simulation Start
{ /* We are just avoiding eClockedDevice::stamp() call*/ }

void TwiModule::keepClocking()
{
    m_toggleScl = true;
    Simulator::self()->addEvent( m_clockPeriod/2, this );
}

void TwiModule::runEvent()
{
    if( m_sheduleSDA && (m_mode == TWI_SLAVE) ) // Used by Slave to set SDA state at 1/2 Clock
    {
        setSDA( m_nextSDA );
        m_sheduleSDA = false;
        return;
    }
    if( m_mode != TWI_MASTER ) return;

    clkState_t sclState = getClockState();
    bool clkLow = ((sclState == Clock_Low) | (sclState == Clock_Falling));

    if( m_toggleScl )
    {
        setSCL( clkLow );     // High if is LOW, LOW if is HIGH
        m_toggleScl = false;
        return;
    }
    Simulator::self()->addEvent( m_clockPeriod, this );
    if( m_i2cState == I2C_IDLE ) return;

    getSdaState();                     // Update state of SDA pin

    twiState_t twiState;
    switch( m_i2cState )
    {
        case I2C_IDLE: return;

        case I2C_START :               // Send Start Condition
        {
            if( m_sdaState ) setSDA( false ); // Step 1: SDA is High, Lower it
            else if( !clkLow )                // Step 2: SDA Already Low, Lower Clock
            {
                //if( m_comp ) m_comp->inStateChanged( TWI_MSG+TWI_COND_START ); // Set TWINT
                setSCL( false ); //keepClocking();
                setTwiState( TWI_START );
                m_i2cState = I2C_IDLE;
            }
        }break;

        case I2C_WRITE :                // We are Writting data
        {
            if( clkLow ) writeBit();    // Set SDA while clk is Low
            keepClocking();
        }break;

        case I2C_READ:                  // We are Reading data
        {
            if( !clkLow )               // Read bit while clk is high
            {
                readBit();
                if( m_bitPtr == 8 ) readByte();
            }
            keepClocking();
        }break;

        case I2C_ACK:                   // Send ACK
        {
            if( clkLow )
            {
                if( m_masterACK ) setSDA( false);
                m_i2cState = I2C_ENDACK;
            }
            keepClocking();
        }break;

        case I2C_ENDACK:               // We sent ACK, release SDA
        {
            if( clkLow )
            {
                setSDA( true ); //if( m_lastState == I2C_READ )

                twiState = m_masterACK ? TWI_MRX_DATA_ACK : TWI_MRX_DATA_NACK ;
                setTwiState( twiState );
                //m_comp->inStateChanged( TWI_MSG+TWI_COND_READ );
                //m_comp->inStateChanged( TWI_MSG+TWI_COND_ACK+m_masterACK ); // ACK/NACK sent
                m_i2cState = I2C_IDLE;
            }
            else keepClocking();
        }break;

        case I2C_READACK:            // Read ACK
        {
            if( m_isAddr ) // ACK after sendind Slave address
            {
                if( m_write ) twiState = m_sdaState ? TWI_MTX_ADR_NACK : TWI_MTX_ADR_ACK; // Transmition started
                else          twiState = m_sdaState ? TWI_MRX_ADR_NACK : TWI_MRX_ADR_ACK; // Reception started
            }
            else           // ACK after sendind data
                twiState = m_sdaState ? TWI_MTX_DATA_NACK : TWI_MTX_DATA_ACK;

            setTwiState( twiState );
            //if( m_comp ) m_comp->inStateChanged( TWI_MSG+TWI_COND_ACK+ack );

            m_i2cState = I2C_IDLE;
            keepClocking();
        }break;

        case I2C_STOP:           // Send Stop Condition
        {
            if     (  m_sdaState && clkLow )  setSDA( false ); // Step 1: Lower SDA
            else if( !m_sdaState && clkLow )  keepClocking();  // Step 2: Raise Clock
            else if( !m_sdaState && !clkLow ) setSDA( true );  // Step 3: Raise SDA
            else if(  m_sdaState && !clkLow )                  // Step 4: Operation Finished
            {
                setTwiState( TWI_NO_STATE ); // Set State first so old m_i2cState is still avilable
                m_i2cState = I2C_IDLE;
                //if( m_comp ) m_comp->inStateChanged( 128+I2C_STOPPED ); // Set TWINT ( set to 0 )
            }
        }
    }
}

void TwiModule::voltChanged() // Used by slave
{
    if( m_mode != TWI_SLAVE ) return;

    clkState_t sclState = getClockState();           // Get Clk to don't miss any clock changes
    getSdaState();                             // State of SDA pin

    if(( sclState == Clock_High )&&( m_i2cState != I2C_ACK ))
    {
        if( m_lastSDA && !m_sdaState ) {       // We are in a Start Condition
            m_bitPtr = 0;
            m_rxReg = 0;
            m_i2cState = I2C_START;
        }
        else if( !m_lastSDA && m_sdaState ) {  // We are in a Stop Condition
           I2Cstop();
        }
    }
    else if( sclState == Clock_Rising )        // We are in a SCL Rissing edge
    {
        if( m_i2cState == I2C_START )             // Get Transaction Info
        {
            readBit();
            if( m_bitPtr > m_addrBits )
            {
                bool rw = m_rxReg % 2;         //Last bit is R/W
                m_rxReg >>= 1;
                if( m_rxReg == m_address ) {   // Address match
                    if( rw ) {                 // Master is Reading
                        m_i2cState = I2C_READ;
                        writeByte();
                    } else {                   // Master is Writting
                        m_i2cState = I2C_WRITE;
                        m_bitPtr = 0;
                        startWrite(); // Notify posible child class
                    }
                    ACK();
                } else {
                    m_i2cState = I2C_STOP;
                    m_rxReg = 0;
                }
            }
        }else if( m_i2cState == I2C_WRITE ){
            readBit();
            if( m_bitPtr == 8 ) readByte();
        }
        else if( m_i2cState == I2C_READACK )      // We wait for Master ACK
        {
            if( !m_sdaState ) {                // ACK: Continue Sending
                m_i2cState = m_lastState;
                writeByte();
            } else m_i2cState = I2C_IDLE;
        }
    }
    else if( sclState == Clock_Falling )
    {
        if( m_i2cState == I2C_ACK ) {             // Send ACK
            sheduleSDA( false );
            m_i2cState = I2C_ENDACK;
        }
        else if( m_i2cState == I2C_ENDACK ) {     // We sent ACK, release SDA
            m_i2cState = m_lastState;
            bool releaseSda = true;
            if( m_i2cState == I2C_READ ) releaseSda = m_txReg>>m_bitPtr & 1; // Keep Sending
            sheduleSDA( releaseSda );
            m_rxReg = 0;
        }
        if( m_i2cState == I2C_READ ) writeBit();
    }
    m_lastSDA = m_sdaState;
}

void TwiModule::setMode( twiMode_t mode )
{
    if( mode == TWI_MASTER ) Simulator::self()->addEvent( m_clockPeriod, this ); // Start Clock

    m_scl->changeCallBack( this, mode == TWI_SLAVE );
    m_sda->changeCallBack( this, mode == TWI_SLAVE );

    setSCL( true );
    setSDA( true );

    m_mode = mode;
    m_i2cState = I2C_IDLE;
    m_sheduleSDA = false;
    m_toggleScl  = false;
}

void TwiModule::setTwiState( twiState_t state )
{
    m_twiState = state;
    //twiState.emitValue( state );
}

void TwiModule::getSdaState()
{
    m_sdaState = m_sda->getInpState();

    m_sda->setPinState( m_sdaState? input_high:input_low ); // High : Low colors
}

void TwiModule::setSCL( bool st ) { m_scl->setOutState( st, true ); }
void TwiModule::setSDA( bool st ) { m_sda->setOutState( st, true ); }

void TwiModule::sheduleSDA( bool state )
{
    m_sheduleSDA = true;
    m_nextSDA = state;
    Simulator::self()->addEvent( m_clockPeriod/2, this );
}

void TwiModule::readBit()
{
    if( m_bitPtr > 0 ) m_rxReg <<= 1;
    m_rxReg += m_sdaState;            //Read one bit from sda
    m_bitPtr++;
}

void TwiModule::writeBit()
{
    if( m_bitPtr < 0 ) { waitACK(); return; }

    bool bit = m_txReg>>m_bitPtr & 1;
    m_bitPtr--;

    setSDA( bit );
}

void TwiModule::readByte()
{
    m_bitPtr = 0;
    ACK();
}

void TwiModule::writeByte() { m_bitPtr = 7;}

void TwiModule::waitACK()
{
    setSDA( true );
    m_lastState = m_i2cState;
    m_i2cState = I2C_READACK;
}

void TwiModule::ACK()
{
    m_lastState = m_i2cState;
    m_i2cState = I2C_ACK;
}

void TwiModule::masterStart() { m_i2cState = I2C_START; }

void TwiModule::masterWrite( uint8_t data , bool isAddr, bool write )
{
    m_isAddr = isAddr;
    m_write  = write;

    m_i2cState = I2C_WRITE;
    m_txReg = data;
    writeByte();
}

void TwiModule::masterRead( bool ack )
{
    m_masterACK = ack;

    setSDA( true );
    m_bitPtr = 0;
    m_rxReg = 0;
    m_i2cState = I2C_READ;
}

void TwiModule::I2Cstop() { m_i2cState = I2C_STOP; }

void TwiModule::setFreqKHz( double f )
{
    m_freq = f*1e3;
    double stepsPerS = 1e12;
    m_clockPeriod = stepsPerS/m_freq/2;
}
void TwiModule::setSdaPin( IoPin* pin ) { m_sda = pin; }
void TwiModule::setSclPin( IoPin* pin )
{
    m_scl = pin;
    m_clockPin = pin;
}
