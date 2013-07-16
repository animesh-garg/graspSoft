/*
 *  main.cpp
 *  qtdemo
 *
 *  Created by Erich Birngruber on 19.07.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <qobject.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qgraphicsview.h>
#include <qmenubar.h>

#include <qmenu.h>
#include <qaction.h>
#include <qpushbutton.h>

#include <qicon.h>
#include <qsystemtrayicon.h>
#include <qaction.h>

#include <SceneRegistry.h>
#include <SceneWindow.h>
#include <GuiServer.h>

#include <qdebug.h>
#include <matvtkProtocol.h>
#include <vtkMultiThreader.h>

//#ifdef WIN32
//extern "C" unsigned int _get_output_format() { return 1; };
//#endif


int main(int argc, char *argv[])
{

	qDebug() << "short size:" << sizeof(short);
	qDebug() << "int size:" << sizeof(int);
    qDebug() << "unsigned long size:" << sizeof(unsigned long);
    qDebug() << "vtkIdType size:" << sizeof(vtkIdType);
    
	qDebug() << "float size:" << sizeof(float);
	qDebug() << "double size:" << sizeof(double);
    qDebug() << "long double size:" << sizeof(long double);


    qDebug() << "vtkIdType size:" << sizeof(vtkIdType);
	qDebug() << "DataSize size:" << sizeof(DataSize);
	qDebug() << "CommandData size:" << sizeof(CommandData);
    qDebug() << "RegistryHandle size:" << sizeof(RegistryHandle);
    
    int maxThreads = vtkMultiThreader::GetGlobalMaximumNumberOfThreads();
    int defaultThreads = vtkMultiThreader::GetGlobalDefaultNumberOfThreads();
    
    qDebug() << "Threads:";
    qDebug() << "max Threads (0=unlimited):" << maxThreads;
    qDebug() << "default Threads:" << defaultThreads;
    qDebug() << "";
    
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);
		
	GuiServer server;
	server.startup();
	
	SceneWindow *win = SceneWindow::newWindow();
	
	
    QMenu *trayIconMenu = new QMenu();
	
    // tray icon: new window menu entry
    QAction *newWindowAction = new QAction(QObject::tr("New Window"), trayIconMenu);
    QObject::connect(newWindowAction, SIGNAL(triggered()), SceneRegistry::instance(), SLOT(newWindow()));  
    trayIconMenu->addAction(newWindowAction);

    // tray icon menu: close all windows
    QAction *closeAllWindowsAction = new QAction(QObject::tr("Close all Windows"), trayIconMenu);
    QObject::connect(closeAllWindowsAction, SIGNAL(triggered()), SceneRegistry::instance(), SLOT(closeAllWindows()));
    trayIconMenu->addAction(closeAllWindowsAction);
    
    trayIconMenu->addSeparator();
   
    // quit menu entry for tray icon
    QAction *quitAction = new QAction(QObject::tr("&Quit"), trayIconMenu);
    QObject::connect(quitAction, SIGNAL(triggered()), SceneRegistry::instance(), SLOT(shutdownApplication()));
    trayIconMenu->addAction(quitAction);
	


	QSystemTrayIcon *trayIcon = new QSystemTrayIcon();
    
	
	QIcon icon = QIcon(":/matvtklogo.png");
	
    trayIcon->setIcon(icon);
    trayIcon->setContextMenu(trayIconMenu);
	
	trayIcon->show();
	
    // run the application eventloop
	int appValue = app.exec();
	
    //delete win;
    
    SceneRegistry::instance()->closeAllWindows();
	delete SceneRegistry::instance();
	
	//win->closeWindow();
	
	return appValue;
}
