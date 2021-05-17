/***************************************************************************
 *   Copyright (C) 2018 by santiago González                               *
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

#include "memory.h"
#include "itemlibrary.h"
#include "circuitwidget.h"
#include "simulator.h"
#include "circuit.h"
#include "iopin.h"
#include "memtable.h"
#include "utils.h"

static const char* Memory_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Data Bits"),
    QT_TRANSLATE_NOOP("App::Property","Persistent")
};

Component* Memory::construct( QObject* parent, QString type, QString id )
{
    return new Memory( parent, type, id );
}

LibraryItem* Memory::libraryItem()
{
    return new LibraryItem(
        tr( "Ram/Rom" ),
        tr( "Logic/Memory" ),
        "2to3g.png",
        "Memory",
        Memory::construct );
}

Memory::Memory( QObject* parent, QString type, QString id )
      : LogicComponent( parent, type, id )
      , eElement( id )
      , MemData()
{
    Q_UNUSED( Memory_properties );

    m_width  = 4;
    m_height = 11;
    
    m_WePin = new IoPin( 180, QPoint( 0,0 ), m_id+"-Pin-We", 0, this, input );
    m_WePin->setLabelText( " WE" );
    m_WePin->setLabelColor( QColor( 0, 0, 0 ) );
    m_WePin->setInverted( true );
    
    m_CsPin = new IoPin(  0, QPoint( 0,0 ), m_id+"-Pin-Cs", 0, this, input );
    m_CsPin->setLabelText( "CS " );
    m_CsPin->setLabelColor( QColor( 0, 0, 0 ) );
    m_WePin->setInverted( true );
    
    m_oePin = new IoPin( 180, QPoint( 0,0 ), m_id+"-Pin-outEnable" , 0, this, input );
    m_oePin->setLabelText( " OE" );
    m_oePin->setLabelColor( QColor( 0, 0, 0 ) );
    m_WePin->setInverted( true );

    m_dataBytes = 1;
    m_addrBits = 0;
    m_dataBits = 0;
    setAddrBits( 8 );
    setDataBits( 8 );

    Simulator::self()->addToUpdateList( this );
}
Memory::~Memory(){}

QList<propGroup_t> Memory::propGroups()
{
    propGroup_t mainGroup { tr("Main") };
    mainGroup.propList.append( {"Address_Bits", tr("Address Size"),"Bits"} );
    mainGroup.propList.append( {"Data_Bits", tr("Data Size"),"Bits"} );
    mainGroup.propList.append( {"Persistent", tr("Persistent"),""} );

    QList<propGroup_t> pg = LogicComponent::propGroups();
    pg.prepend( mainGroup );
    return pg;
}

void Memory::stamp()                   // Called at Simulation Start
{
    for( int i=0; i<2+m_addrBits; ++i ) // Initialize control pins
    {
        eNode* enode =  m_inPin[i]->getEnode();
        if( enode ) enode->voltChangedCallback( this );
    }
    LogicComponent::stamp( this );
}

void Memory::updateStep()
{
    if( m_memTable ) m_memTable->updateTable( &m_ram );
}

void Memory::initialize()
{
    m_we = true;
    m_cs = true;
    m_oe = true;
    m_read = false;

    for( uint i=0; i<m_outPin.size(); ++i )
    {
        m_outPin[i]->setPinMode( input );
    }
    if( !m_persistent ) m_ram.fill( 0 );

    LogicComponent::initState();
}

void Memory::voltChanged()        // Some Pin Changed State, Manage it
{
    bool CS = m_CsPin->getInpState();
    bool csTrig = false;

    if( CS != m_cs )
    {
        if( CS && !m_cs ) csTrig = true;
        m_cs = CS;

        if( !CS && m_oe )
        {
            m_oe = false;
            LogicComponent::setOutputEnabled( false ); // Deactivate
        }
    }
    if( !CS ) return;

    bool WE = m_WePin->getInpState();
    bool oe = LogicComponent::outputEnabled();// && !WE;

    if( oe != m_oe )
    {
        m_oe = oe;
        setOutputEnabled( oe );
    }

    m_address = 0;
    for( int i=0; i<m_addrBits; ++i )        // Get Address
    {
        bool state = m_inPin[i+2]->getInpState();//getInputState(i+2);
        if( state ) m_address += pow( 2, i );
    }

    bool weTrig = WE && !m_we;
    m_we = WE;
    if( WE )                                // Write
    {
        for( uint i=0; i<m_outPin.size(); ++i ) m_outPin[i]->setPinMode( input );
        Simulator::self()->addEvent( 1, NULL );

        if( csTrig || weTrig )  // Write action triggered
        {
            m_read = false;
            Simulator::self()->addEvent( m_propDelay, this );
        }
    }else                                  // Read
    {
        for( uint i=0; i<m_outPin.size(); ++i ) m_outPin[i]->setPinMode( output );
        Simulator::self()->addEvent( 1, NULL );
        m_read = true;
        m_nextOutVal = m_ram[m_address];
        IoComponent::sheduleOutPuts( this );
    }
}

void Memory::runEvent()
{
    if( m_read ) IoComponent::runOutputs();
    else
    {
        int value = 0;
        for( uint i=0; i<m_outPin.size(); ++i )
        {
            bool state = m_outPin[i]->getInpState();
            if( state ) value += pow( 2, i );
            m_outPin[i]->setPinState( state? input_high:input_low ); // High-Low colors
        }
        m_ram[m_address] = value;
    }
}

void Memory::setMem( QVector<int> m )
{
    if( m.size() == 1 ) return;       // Avoid loading data if not saved
    m_ram = m;
}

QVector<int> Memory::mem()
{
    if( !m_persistent )
    {
        QVector<int> null;
        return null;
    }
    return m_ram;
}

void Memory::updatePins()
{
    int h = m_addrBits+1;
    if( m_dataBits > h ) h = m_dataBits;
    
    m_height = h+2;
    int origY = -(m_height/2)*8;
    
    for( int i=0; i<m_addrBits; i++ )
    {
        m_inPin[i]->setPos( QPoint(-24,origY+8+i*8 ) );
        m_inPin[i]->setLabelPos();
        m_inPin[i]->isMoved();
    }
    
    for( int i=0; i<m_dataBits; i++ )
    {
        m_outPin[i]->setPos( QPoint(24,origY+8+i*8 ) ); 
        m_outPin[i]->setLabelPos();
        m_outPin[i]->isMoved();
    }
    
    m_WePin->setPos( QPoint(-24,origY+h*8 ) );          // WE
    m_WePin->isMoved();
    m_WePin->setLabelPos();
    
    m_CsPin->setPos( QPoint( 24,origY+8+h*8 ) );        // CS
    m_CsPin->isMoved();
    m_CsPin->setLabelPos();
    
    m_oePin->setPos( QPoint(-24,origY+8+h*8 ) );        // OE
    m_oePin->isMoved();
    m_oePin->setLabelPos();
    
    m_area   = QRect( -(m_width/2)*8, origY, m_width*8, m_height*8 );
}

void Memory::setAddrBits( int bits )
{
    if( bits == m_addrBits ) return;
    if( bits == 0 ) bits = 8;
    if( bits > 18 ) bits = 18;

    m_ram.resize( pow( 2, bits ) );
    
    if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();
    
    if     ( bits < m_addrBits ) deleteAddrBits( m_addrBits-bits );
    else if( bits > m_addrBits ) createAddrBits( bits-m_addrBits );

    m_addrBits = bits;

    if( m_memTable ) m_memTable->setData( &m_ram );

    updatePins();
    Circuit::self()->update();
}

void Memory::createAddrBits( int bits )
{
    int chans = m_addrBits + bits;
    
    int origY = -(m_height/2)*8;
    
    m_inPin.resize( chans );
    
    for( int i=m_addrBits; i<chans; i++ )
    {
        QString number = QString::number(i);

        m_inPin[i] = new IoPin( 180, QPoint(-24,origY+8+i*8 ), m_id+"-in"+number, i, this, input );
        m_inPin[i]->setLabelText( " A"+number );
        m_inPin[i]->setLabelColor( QColor( 0, 0, 0 ) );
    }
}

void Memory::deleteAddrBits( int bits )
{
    LogicComponent::deletePins( &m_inPin, bits );
}

void Memory::setDataBits( int bits )
{
    if( Simulator::self()->isRunning() ) CircuitWidget::self()->powerCircOff();

    if( bits == m_dataBits ) return;
    if( bits == 0 ) bits = 8;
    if( bits > 32 ) bits = 32;

    if     ( bits < m_dataBits ) deleteDataBits( m_dataBits-bits );
    else if( bits > m_dataBits ) createDataBits( bits-m_dataBits );
    
    m_dataBits = bits;
    updatePins();
    Circuit::self()->update();
}

void Memory::createDataBits( int bits )
{
    int chans = m_dataBits + bits;
    int origY = -(m_height/2)*8;
    
    m_outPin.resize( chans );
    
    for( int i=m_dataBits; i<chans; i++ )
    {
        QString number = QString::number(i);
        
        m_outPin[i] = new IoPin( 0, QPoint(24,origY+8+i*8 ), m_id+"-out"+number, i, this, output );
        m_outPin[i]->setLabelText( "D"+number+" " );
        m_outPin[i]->setLabelColor( QColor( 0, 0, 0 ) );
    }
}

void Memory::deleteDataBits( int bits )
{
    LogicComponent::deletePins( &m_outPin, bits );
}

void Memory::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    if( !acceptedMouseButtons() ) event->ignore();
    else
    {
        event->accept();
        QMenu* menu = new QMenu();
        contextMenu( event, menu );
        Component::contextMenu( event, menu );
        menu->deleteLater();
    }
}

void Memory::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    QAction* loadAction = menu->addAction( QIcon(":/load.png"),tr("Load data") );
    connect( loadAction, SIGNAL(triggered()),
                   this, SLOT(loadData()), Qt::UniqueConnection );

    QAction* saveAction = menu->addAction(QIcon(":/save.png"), tr("Save data") );
    connect( saveAction, SIGNAL(triggered()),
                   this, SLOT(saveData()), Qt::UniqueConnection );

    QAction* showEepAction = menu->addAction(QIcon(":/save.png"), tr("Show Memory Table") );
    connect( showEepAction, SIGNAL(triggered()),
                      this, SLOT(showTable()), Qt::UniqueConnection );

    menu->addSeparator();
}

void Memory::loadData()
{
    MemData::loadData( &m_ram, false, m_dataBits );
    if( m_memTable ) m_memTable->setData( &m_ram );
}

void Memory::saveData()
{
    MemData::saveData( &m_ram, m_dataBits );
}

void Memory::showTable()
{
    MemData::showTable( m_ram.size(), m_dataBytes );
    if( m_persistent ) m_memTable->setWindowTitle( "ROM: "+m_idLabel->toPlainText() );
    else               m_memTable->setWindowTitle( "RAM: "+m_idLabel->toPlainText() );
    m_memTable->setData( &m_ram );
}

void Memory::remove()
{
    m_CsPin->removeConnector();
    m_WePin->removeConnector();
    m_oePin->removeConnector();
    
    LogicComponent::remove();
}

#include "moc_memory.cpp"
