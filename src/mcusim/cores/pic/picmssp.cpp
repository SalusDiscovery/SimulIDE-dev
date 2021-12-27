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

#include "picmssp.h"
#include "picspi.h"
#include "pictwi.h"
#include "datautils.h"

PicMssp::PicMssp( eMcu* mcu, QString name, int type )
       : McuModule( mcu, name )
       , eElement( name )
{
    //m_SSPMx = getRegBits( "SSPM0,SSPM1,SSPM2,SSPM3", mcu );
}
PicMssp::~PicMssp(){}

void PicMssp::initialize()
{

}

void PicMssp::configureA( uint8_t SSPCON )
{
    uint8_t mode = getRegBitsVal( SSPCON, m_SSPMx );
    if( m_mode != mode )
    {
        //uint8_t spiClk = 0;
        m_mode = mode;
        switch( m_mode ){
            case 0:                               // SPI Master, clock = FOSC/4
            case 1:                               // SPI Master, clock = FOSC/16
            case 2:                               // SPI Master, clock = FOSC/64
            case 3:  m_sspMode = sspSPI_M; break; // SPI Master, clock = TMR2
            case 4:                               // SPI Slave, SS pin enabled
            case 5:  m_sspMode = sspSPI_S; break; // SPI Slave, SS pin disabled
            case 6:                        break; // I2C Slave, 7-bit address
            case 7:  m_sspMode = sspI2C_S; break; // I2C Slave, 10-bit address
            case 8:  m_sspMode = sspI2C_M; break; // I2C Master, clock = FOSC/(4 *(SSPADD+1))
            case 9:                        break; // Load Mask function
            case 10:                       break; // Reserved
            case 11:                       break; // I2C firmware controlled Master (Slave idle)
            case 12:                       break; // Reserved
            case 13:                       break; // Reserved
            case 14:                       break; // I2C Slave,  7-bit address, Start & Stop interrupts enabled
            case 15: m_sspMode = sspI2C_S; break; // I2C Slave, 10-bit address, Start & Stop interrupts enabled
        }
    }

}
