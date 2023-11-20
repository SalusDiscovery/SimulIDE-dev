/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QMenu>

#include "serialterm.h"
#include "serialmon.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "circuit.h"
#include "usarttx.h"
#include "usartrx.h"
#include "iopin.h"
#include "utils.h"
#include "mainwindow.h"
#include "custombutton.h"

#include "stringprop.h"
#include "boolprop.h"
#include "intprop.h"

#define tr(str) simulideTr("SerialTerm",str)

Component* SerialTerm::construct( QString type, QString id )
{ return new SerialTerm( type, id ); }

LibraryItem* SerialTerm::libraryItem()
{
    return new LibraryItem(
        tr("Serial Terminal"),
        "Peripherals",
        "serialterm.png",
        "SerialTerm",
        SerialTerm::construct );
}

SerialTerm::SerialTerm( QString type, QString id )
          : Component( type, id )
          , UsartModule( NULL, id+"-Uart" )
          , eElement( (id+"-eElement") )
{
    m_area = QRect(-16,-16, 32, 32 );
    setLabelPos(-20,-32 );

    m_pin.resize(2);

    IoPin* pinTx = new IoPin( 180, QPoint(-24,-8), id+"-pin0", 0, this, output );
    pinTx->setLabelText( "Tx" );
    pinTx->setOutHighV( 5 );
    m_pin[0] = pinTx;
    m_sender->setPins( {pinTx} );

    IoPin* pinRx = new IoPin( 180, QPoint(-24, 8), id+"-pin1", 0, this, input );
    pinRx->setLabelText( "Rx" );
    m_pin[1] = pinRx;
    m_receiver->setPins( {pinRx} );

    m_receiving = false;

    setBaudRate( 9600 );

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
//new BoolProp  <Chip>( "Logic_Symbol","","", this, &Chip::logicSymbol, &Chip::setLogicSymbol ),
    }, 0 } );

    addPropGroup( { "Config", {
new IntProp<SerialTerm>("Baudrate", tr("Baudrate"),"_Bauds", this, &SerialTerm::baudRate, &SerialTerm::setBaudRate,0,"uint" ),
new IntProp<SerialTerm>("DataBits", tr("Data Bits"),"_Bits", this, &SerialTerm::dataBits, &SerialTerm::setDataBits,0,"uint" ),
new IntProp<SerialTerm>("StopBits", tr("Stop Bits"),"_Bits", this, &SerialTerm::stopBits, &SerialTerm::setStopBits,0,"uint" ),
    }, 0 } );
    addPropGroup( {"Hidden", {
new BoolProp<SerialTerm>("SerialMon","","", this, &SerialTerm::serialMon, &SerialTerm::setSerialMon ),
}, groupHidden} );
}
SerialTerm::~SerialTerm(){}

void SerialTerm::stamp()
{
    m_uartData.clear();
    m_sender->enable( true );
    m_receiver->enable( true );
    m_sending   = false;
    m_receiving = false;
}

void SerialTerm::updateStep()
{
    if( m_uartData.size() ){
        if( !m_sending ){
            m_sending = true;
            Simulator::self()->addEvent( 1, this );
        }
    } else m_sending = false;

    update();
}

void SerialTerm::runEvent()
{
    if( m_uartData.isEmpty() ) return;
    UsartModule::sendByte( m_uartData.at( 0 ) ); // Start transaction
    m_uartData = m_uartData.right( m_uartData.size()-1 );
}

void SerialTerm::sendByte( uint8_t data )
{
    m_uartData += data;
}

void SerialTerm::byteReceived( uint8_t byte )
{
    m_receiver->getData();
    if( m_monitor ) m_monitor->printIn( byte );
    m_receiving = true;
}

void SerialTerm::frameSent( uint8_t data )
{
    if( m_monitor ) m_monitor->printOut( data );
    if( m_uartData.size() )
    {
        uint8_t byte = m_uartData.at( 0 );
        m_uartData = m_uartData.right( m_uartData.size()-1 );
        UsartModule::sendByte( byte );
    }
    else m_sending = false;
}

void SerialTerm::slotOpenTerm()
{
    openMonitor( m_id, 0 );
    m_monitor->activateSend();
}

void SerialTerm::setSerialMon( bool s )
{
    if( s ) slotOpenTerm();
}

void SerialTerm::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    QAction* openSerMon = menu->addAction( QIcon(":/terminal.svg"),tr("Open Serial Monitor.") );
    QObject::connect( openSerMon, &QAction::triggered, [=](){ slotOpenTerm(); } );

    menu->addSeparator();
    Component::contextMenu( event, menu );
}

void SerialTerm::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Component::paint( p, option, widget );
    
    p->setBrush( Qt::darkBlue );
    p->drawRoundedRect( m_area, 4, 4 );

    if( m_pin[0]->connector() )
    {
        if( m_sending ) p->setBrush( Qt::yellow );
        else            p->setBrush( Qt::red );
    }
    else p->setBrush( Qt::black );
    p->drawRoundedRect( -5,-11, 8, 6, 2, 2 ); // Tx led

    if( m_pin[1]->connector() )
    {
        if( m_receiving ){
            m_receiving = false;
            p->setBrush( Qt::yellow );
        }else p->setBrush( Qt::red );
    }
    else p->setBrush( Qt::black );
    p->drawRoundedRect(-5, 5, 8, 6, 2, 2 ); // Rx led
}