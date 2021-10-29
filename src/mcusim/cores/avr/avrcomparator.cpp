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

#include "avrcomparator.h"
#include "datautils.h"
#include "e_mcu.h"
#include "mcupin.h"

AvrComp::AvrComp( eMcu* mcu, QString name )
       : McuComp( mcu, name )
{
    //m_ACSR = mcu->getReg( "ACSR" );
    m_ACD  = getRegBits( "ACD", mcu );
    m_ACBG = getRegBits( "ACBG", mcu );
    m_ACO  = getRegBits( "ACO", mcu );
    m_ACI  = getRegBits( "ACI", mcu );
    m_ACIC = getRegBits( "ACIC", mcu );
    m_ACIS = getRegBits( "ACIS0,ACIS1", mcu );

    //m_DIDR1 = mcu->getReg( "DIDR1" );
    m_AIN0D = getRegBits( "AIN0D", mcu );
    m_AIN1D = getRegBits( "AIN1D", mcu );

    m_pinP = m_pins[0];
    m_pinN = m_pins[1];

    //mcu->watchBitNames( "ACO", R_READ, this, &AvrComp::configureB );
}
AvrComp::~AvrComp(){}

/*void AvrComp::initialize()
{
    m_fixVref = false;
    m_enabled = true;
    m_compOut = false;
    m_mode = 0;
}*/

void AvrComp::configureA( uint8_t newACSR ) // ACSR is being written
{
    if( newACSR & m_ACI.mask )
        m_mcu->m_regOverride = newACSR & ~(m_ACI.mask); // Clear ACI by writting it to 1

    m_enabled = getRegBitsVal( newACSR, m_ACD );
    if( !m_enabled ) m_mcu->m_regOverride = newACSR & ~m_ACO.mask; // Clear ACO

    m_fixVref = getRegBitsVal( newACSR, m_ACBG );

    /// TODO: ACIC: Analog Comparator Input Capture Enable

    m_mode = getRegBitsVal( newACSR, m_ACIS );
}

void AvrComp::configureB( uint8_t newDIDR1 ) // AIN0D,AIN1D being written
{
    m_pinP->changeCallBack( m_pinP, getRegBitsVal( newDIDR1, m_AIN0D ) );
    m_pinN->changeCallBack( m_pinN, getRegBitsVal( newDIDR1, m_AIN1D ) );

    //// ?????
    /// m_mcu->m_regOverride = newDIDR1; // Keep rest of bits at 0
}

void AvrComp::compare( uint8_t ) // Performed only when ACO is readed
{
    if( !m_enabled ) return;

    double vRef = m_fixVref ? 1.1 : m_pinP->getVolt();
    bool compOut = vRef > m_pinN->getVolt() ;

    if( m_compOut != compOut )
    {
        if( compOut ) setRegBits( m_ACO );
        else          clearRegBits( m_ACO );

        switch( m_mode )
        {
            case 0:     // Comparator Interrupt on Output Toggle.
                m_interrupt->raise();
                break;
            case 1:     // Reserved
                break;
            case 2:     // Comparator Interrupt on Falling Output Edge.
                if( m_compOut & !compOut ) m_interrupt->raise();
                break;
            case 3:     // Comparator Interrupt on Rising Output Edge.
                if( !m_compOut & !compOut ) m_interrupt->raise();
                break;
        }
        m_compOut = compOut;
    }
}
