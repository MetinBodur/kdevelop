/* 
   Copyright (C) 2003 ian reinhart geiser <geiseri@kde.org> 

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   version 2, License as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qwhatsthis.h>
#include <qtimer.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qregexp.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdevelop/kdevcore.h>
#include <kdevelop/kdevpartcontroller.h>
#include <kdevelop/kdevmainwindow.h>
#include <kdevelop/domutil.h>
#include <kdevelop/codemodel.h>


#include <kdevelop/kdevproject.h>
#include <kaction.h>
#include <kdebug.h>
#include <kapplication.h>

#include <kjsembed/kjsembedpart.h>
#include <kjsembed/jsconsolewidget.h>

#include "kjssupport_part.h"
#include "kjsproblems.h"

typedef KGenericFactory<kjsSupportPart> kjsSupportFactory;
K_EXPORT_COMPONENT_FACTORY( libkdevkjssupport, kjsSupportFactory( "kdevkjssupport" ) );

kjsSupportPart::kjsSupportPart(QObject *parent, const char *name, const QStringList& )
: KDevLanguageSupport("KDevkjsSupport", "JavaScript", parent, name ? name : "kjsSupportPart" )
{
	setInstance(kjsSupportFactory::instance());
	setXMLFile("kdevlang_kjssupport.rc");


	m_build = new KAction( i18n("&Run"), "exec",Key_F9,this, SLOT(slotRun()),actionCollection(), "build_execute" );
	m_build->setStatusText( i18n("Test the active script.") );

	kdDebug() << "Creating kjssupport Part" << endl;

	connect( core(), SIGNAL(projectConfigWidget(KDialogBase*)),
		this, SLOT(projectConfigWidget(KDialogBase*)) );
	connect( core(), SIGNAL(projectOpened()), this, SLOT(projectOpened()) );
	connect( core(), SIGNAL(projectClosed()), this, SLOT(projectClosed()) );
	connect( partController(), SIGNAL(savedFile(const QString&)), this, SLOT(savedFile(const QString&)) );
	connect(partController(), SIGNAL(activePartChanged(KParts::Part*)),
		this, SLOT(slotActivePartChanged(KParts::Part *)));

     // Building kjs interpreter.
	m_js = new KJSEmbed::KJSEmbedPart();
	mainWindow()->embedOutputView( m_js->view() , i18n("KJS Console"),i18n("KJS Embed Console") );

    // get the problem reporter
        m_problemReporter = new KJSProblems( this );
	m_problemReporter->setIcon( SmallIcon("info") );
	mainWindow( )->embedOutputView( m_problemReporter, i18n("Problems"), i18n("Problem reporter"));

    }


kjsSupportPart::~kjsSupportPart()
{
	delete m_build;
	delete m_js;
}

KDevLanguageSupport::Features kjsSupportPart::features()
{
	return Features(Classes | Variables | Functions);
}

KMimeType::List kjsSupportPart::mimeTypes()
{
	KMimeType::List list;
	
	KMimeType::Ptr mime = KMimeType::mimeType( "application/x-javascript" );
	if( mime )
		list << mime;

	return list;
}
void kjsSupportPart::slotRun()
{
	// Execute the application here.

	kdDebug() << "app " << project()->mainProgram(true ) << endl;
	m_js->runFile( project()->mainProgram(true ));
}

void kjsSupportPart::projectConfigWidget(KDialogBase *dlg)
{
	Q_UNUSED( dlg );
	// Create your config dialog here.
}
void kjsSupportPart::projectOpened()
{
	kdDebug(9014) << "projectOpened()" << endl;

	connect( project(), SIGNAL(addedFilesToProject(const QStringList &)),
		this, SLOT(addedFilesToProject(const QStringList &)) );
	connect( project(), SIGNAL(removedFilesFromProject(const QStringList &)),
		this, SLOT(removedFilesFromProject(const QStringList &)) );

	// We want to parse only after all components have been
	// properly initialized
	QTimer::singleShot(0, this, SLOT(parse()));
}
void kjsSupportPart::projectClosed()
{

}
void kjsSupportPart::savedFile(const QString &fileName)
{


	if (project()->allFiles().contains(fileName.mid ( project()->projectDirectory().length() + 1 )))
	{
		kdDebug(9014) << "parse file " << fileName << endl;
		parse( fileName );
		emit addedSourceInfo( fileName );
	}
}
void kjsSupportPart::addedFilesToProject(const QStringList &fileList)
{
	kdDebug(9014) << "addedFilesToProject()" << endl;

	QStringList::ConstIterator it;

	for ( it = fileList.begin(); it != fileList.end(); ++it )
	{
		kdDebug(9014) << "maybe parse " << project()->projectDirectory() + "/" + ( *it ) << endl;
		parse( project()->projectDirectory() + "/" + (*it) );
	}

	emit updatedSourceInfo();
}
void kjsSupportPart::removedFilesFromProject(const QStringList &fileList)
{


	QStringList::ConstIterator it;

	for ( it = fileList.begin(); it != fileList.end(); ++it )
	{
		QString fileName = project()->projectDirectory() + "/" + ( *it );
		if( codeModel()->hasFile(fileName) )
		{
			kdDebug(9014) << "removed " << fileName << endl;
			emit aboutToRemoveSourceInfo( fileName );
			codeModel()->removeFile( codeModel()->fileByName(fileName) );
		}
	}

}
void kjsSupportPart::parse()
{
	kdDebug(9014) << "initialParse()" << endl;

	if (project())
	{
		kapp->setOverrideCursor(waitCursor);
		QStringList files = project()->allFiles();
		for (QStringList::Iterator it = files.begin(); it != files.end() ;++it)
		{
			kdDebug(9014) << "maybe parse " << project()->projectDirectory() + "/" + (*it) << endl;
			
			parse( project()->projectDirectory() + "/" + (*it) );
			
		}
		emit updatedSourceInfo();
		kapp->restoreOverrideCursor();
	} else {
		kdDebug(9014) << "No project" << endl;
	}
}
void kjsSupportPart::slotActivePartChanged(KParts::Part *part)
{
	kdDebug() << "Changeing active part..." << endl;
}

/*!
    \fn kjsSupportPart::parse(const QString &fileName)
 */
void kjsSupportPart::parse(const QString &fileName)
{
	QFileInfo fi(fileName);
	if (fi.extension() == "js")
	{
		if( codeModel()->hasFile(fileName) )
		{
			emit aboutToRemoveSourceInfo( fileName );
			codeModel()->removeFile( codeModel()->fileByName(fileName) );
		}
	
		FileDom m_file = codeModel()->create<FileModel>();
		m_file->setName( fileName );
		m_file->setFileName( fileName );
		
                QFile f(QFile::encodeName(fileName));
                if (!f.open(IO_ReadOnly))
                        return;
                QString rawline;
                QString line;
                uint lineNo = 0;
		QTextStream stream(&f);
		int depth = 0;
		bool inFunction = false;
		QString lastFunction = "";
		int lastLineNo = 0;
		ClassDom currentClass;
		
		QRegExp varRx("var[\\s]([_a-zA-Z\\d]+)");
		QRegExp classVarRx("this\\.([_a-zA-Z\\d]+)");
		QRegExp classMethRx("this\\.([_a-zA-Z\\d]+)[\\s]*=[\\s]*function(\\([^){}\\n\\r]*\\))");
		QRegExp methRx("function[\\s]+([_a-zA-Z\\d]+[\\s]*\\([^){}\\n\\r]*\\))");
		
                while (!stream.atEnd())
                {
                         rawline = stream.readLine();
                         line = rawline.stripWhiteSpace().local8Bit();
                         kdDebug() << "Trying line: " << line << endl;
			 
			if (methRx.search(line) != -1 && depth == 0)
			{
				if (lastFunction != "" )
					addMethod(lastFunction, m_file, lastLineNo);
				lastFunction = methRx.cap(1);
				lastLineNo = lineNo;
                        }
                        else if(varRx.search(line) != -1 && depth == 0)
                        {
                                addAttribute(varRx.cap(1), m_file, lineNo);
                        }
                        else if(classMethRx.search(line) != -1 && depth > 0)
                        {
				if ( lastFunction != "" )
				{
					currentClass = addClass(lastFunction, m_file, lastLineNo );
					lastFunction = "";
				}
                               addMethod(classMethRx.cap(1)+classMethRx.cap(2), currentClass, lineNo);
                        }
                        else if(classVarRx.search(line) != -1 && depth > 0)
                        {
				if ( lastFunction != "" )
				{
					currentClass = addClass(lastFunction, m_file, lastLineNo );
					lastFunction = "";
				}
                                addAttribute(classVarRx.cap(1), currentClass, lineNo);
                        }
			
			if( line.contains("{") )
				++depth;
				
			if( line.contains("}") )
				--depth;
                       ++lineNo;
                }
		
		if (lastFunction != "" )
			addMethod(lastFunction, m_file, lastLineNo);
					
                f.close();

		kdDebug() << "Trying to add list..." << endl;
                
                codeModel()->addFile( m_file );
        }
}

ClassDom kjsSupportPart::addClass(const QString &name, FileDom file, uint lineNo)
{
 	ClassDom clazz = codeModel()->create<ClassModel>();
	clazz->setName(name);
	clazz->setFileName(file->fileName());
	clazz->setStartPosition(lineNo, 0);

	if( !file->hasClass(clazz->name()) ){
		kdDebug() << "Add global class " << clazz->name() << endl;
		file->addClass( clazz );
	}
	return clazz;
}

void kjsSupportPart::addMethod(const QString &name, ClassDom clazz, uint lineNo)
{
 	FunctionDom method = codeModel()->create<FunctionModel>();
	method->setName(name);
	method->setFileName(clazz->fileName());
	method->setStartPosition(lineNo, 0);

	if( !clazz->hasFunction(method->name()) ){
		kdDebug() << "Add class method " << method->name() << endl;
		clazz->addFunction( method );
	}
}

void kjsSupportPart::addAttribute(const QString &name, ClassDom clazz, uint lineNo)
{
	VariableDom var = codeModel()->create<VariableModel>();
	var->setName(name);
	var->setFileName(clazz->fileName());
	var->setStartPosition( lineNo, 0 );
	var->setType(i18n("Variable"));

	if( !clazz->hasVariable(var->name()) ){
		kdDebug() << "Add class attribute " << var->name() << endl;
		clazz->addVariable(var);
	}
}

void kjsSupportPart::addMethod(const QString &name, FileDom file, uint lineNo)
{
 	FunctionDom method = codeModel()->create<FunctionModel>();
	method->setName(name);
	method->setFileName(file->fileName());
	method->setStartPosition(lineNo, 0);

	if( !file->hasFunction(method->name()) ){
		kdDebug() << "Add global method " << method->name() << endl;
		file->addFunction( method );
	}
}

void kjsSupportPart::addAttribute(const QString &name, FileDom file, uint lineNo)
{
	VariableDom var = codeModel()->create<VariableModel>();
	var->setName(name);
	var->setFileName(file->fileName());
	var->setStartPosition( lineNo, 0 );
	var->setType(i18n("Variable"));

	if( !file->hasVariable(var->name()) ){
		kdDebug() << "Add global attribute " << var->name() << endl;
		file->addVariable(var);
	}
}

#include "kjssupport_part.moc"



