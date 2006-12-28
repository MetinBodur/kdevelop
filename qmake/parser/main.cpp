/* KDevelop QMake Support
 *
 * Copyright 2006 Andreas Pakulat <apaku@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include "qmakedriver.h"
#include "qmakeast.h"

#include <QtGlobal>
#include <QtCore/QDebug>
#include <QtCore/QString>

static void usage( char const* argv0 );

int main( int argc, char** argv )
{
    if ( argc < 1 ) {
        usage( argv[0] );
        exit( EXIT_FAILURE );
    }

    int begin = 1;
    int debug = 0;

    if( strcmp( argv[begin], "--debug" ) == 0 )
    {
        std::cerr << "found dbg" << std::endl;
        begin = 2;
        debug = 1;
    }
    for( ; begin < argc ; begin++ ) {
        char const* arg = argv[begin];
        QMake::ProjectAST* ast;
        if ( !strncmp( arg, "--", 2 ) ) {
            std::cerr << "Unknown option: " << arg << std::endl;
            usage( argv[0] );
            exit( EXIT_FAILURE );
        } else if ( QMake::Driver::parseFile( arg, &ast, debug ) != 0 ) {
            exit( EXIT_FAILURE );
        }else if( debug )
        {
            QString buf;
            ast->writeToString( buf );
            qWarning() << "Project Read: "<< ast->statements().count() << "\n-------------\n" << buf << "\n-------------\n";
        }
    }

}

static void usage( char const* argv0 )
{
    std::cerr << "usage: " << argv0 << " [options] file.pro [file2.pro ...]"
    << std::endl << std::endl
    << "Options:" << std::endl
    << "  --debug" << std::endl;
}

// kate: space-indent on; indent-width 4; tab-width 4; replace-tabs on

