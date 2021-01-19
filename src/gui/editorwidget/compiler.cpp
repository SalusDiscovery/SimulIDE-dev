/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
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

#include "compiler.h"
#include "baseprocessor.h"
#include "editorwindow.h"
#include "mainwindow.h"
#include "simulator.h"

static const char* Compiler_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Drive Circuit"),
    QT_TRANSLATE_NOOP("App::Property","Compiler Path")
};

bool Compiler::m_loadStatus = false;

Compiler::Compiler( CodeEditor* parent, OutPanelText* outPane, QString filePath )
        : QObject( parent )
        , m_compProcess( 0l )
{
    Q_UNUSED( Compiler_properties );

    m_editor  = parent;
    m_outPane = outPane;
    m_appPath = QCoreApplication::applicationDirPath();
    
    QFileInfo fi = QFileInfo( filePath );
    m_file     = filePath;
    m_fileDir  = fi.absolutePath();
    m_fileExt  = "."+fi.suffix();
    m_fileName = fi.completeBaseName();

    m_processorType = 0;
    type = 0;
    
    connect( &m_compProcess, SIGNAL(readyRead()), SLOT(ProcRead()), Qt::UniqueConnection );
}
Compiler::~Compiler( )
{
    //if( BaseProcessor::self() ) BaseProcessor::self()->getRamTable()->remDebugger( this );
}

bool Compiler::loadFirmware()
{
    if ( m_firmware == "" )  return false;
    
    upload();
    if( m_loadStatus ) return false;
    m_loadStatus = true;
    return true;
}

void Compiler::upload()
{
    if( m_loadStatus )
    {
        QMessageBox::warning( 0, "Compiler::loadFirmware",
                                tr("Debugger already running")+"\n"+tr("Stop active session") );
        return;
    }
    m_outPane->writeText( "-------------------------------------------------------\n" );
    m_outPane->appendText( "\n"+tr("Uploading: ")+"\n" );
    m_outPane->appendText( m_firmware );
    m_outPane->writeText( "\n\n" );
    
    if( McuComponent::self() ) 
    {
        McuComponent::self()->load( m_firmware );
        m_outPane->appendText( "\n"+tr("FirmWare Uploaded to ")+McuComponent::self()->device()+"\n" );
        m_outPane->writeText( "\n\n" );

        ///BaseProcessor::self()->getRamTable()->setDebugger( this );
        mapFlashToSource();
        ///BaseProcessor::self()->m_debugger = this;
    }
    else m_outPane->writeText( "\n"+tr("Error: No Mcu in Simulator... ")+"\n" );
}

void Compiler::stop()
{
    m_loadStatus = false;
}

int Compiler::getValidLine( int line )
{
    while( !m_sourceToFlash.contains(line) && line<=m_lastLine ) line++;
    return line;
}

QString Compiler::getVarType( QString var )
{
    var= var.toUpper();
    return m_varList[ var ];
}

QStringList Compiler::getVarList()
{
    /*QStringList varList = m_varList.keys();
    varList.sort();
    return varList;*/
    return m_varNames;
}

void Compiler::ProcRead()
{
    while( m_compProcess.canReadLine() ) 
    {
        m_outPane->appendText( QString::fromLocal8Bit( m_compProcess.readLine()) );
        //m_outPane->writeText( "\n" );
    }
}

void Compiler::readSettings()
{
    QSettings* settings = MainWindow::self()->settings();
    
    if( settings->contains( m_compSetting ) )
        m_compilerPath = settings->value( m_compSetting ).toString();
}

void Compiler::getCompilerPath()
{
        m_compilerPath = QFileDialog::getExistingDirectory( 0L,
                               tr("Select Compiler toolchain directory"),
                               m_compilerPath,
                               QFileDialog::ShowDirsOnly
                             | QFileDialog::DontResolveSymlinks);

        if( m_compilerPath != "" ) m_compilerPath += "/";

        MainWindow::self()->settings()->setValue( m_compSetting, m_compilerPath);

        m_outPane->appendText( "\n"+tr("Using Compiler Path: ")+"\n" );
        m_outPane->writeText( m_compilerPath+"\n\n" );
}

bool Compiler::driveCirc()
{
    CodeEditor* ce = EditorWindow::self()->getCodeEditor();
    return ce->driveCirc();
}

void Compiler::setDriveCirc( bool drive )
{
    CodeEditor* ce = EditorWindow::self()->getCodeEditor();
    ce->setDriveCirc( drive );
}

QString Compiler::compilerPath()
{
    return m_compilerPath;
}

void Compiler::setCompilerPath( QString path )
{
    m_compilerPath = path;
    MainWindow::self()->settings()->setValue( m_compSetting, m_compilerPath );
}

void Compiler::toolChainNotFound()
{
    m_outPane->appendText( tr(": ToolChain not found")+"\n" );
    m_outPane->writeText( "\n"+tr("Right-Click on Document Tab to set Path")+"\n\n" );
    QApplication::restoreOverrideCursor();
}
#include "moc_compiler.cpp"

