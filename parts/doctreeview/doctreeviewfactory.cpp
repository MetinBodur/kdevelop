/***************************************************************************
 *   Copyright (C) 2000-2001 by Bernd Gehrmann                             *
 *   bernd@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kinstance.h>
#include <kstandarddirs.h>
#include "doctreeviewfactory.h"

K_EXPORT_COMPONENT_FACTORY( libkdevdoctreeview, DocTreeViewFactory );

DocTreeViewFactory::DocTreeViewFactory()
    : KGenericFactory<DocTreeViewPart>( "kdevdoctreeview" )
{
}

KInstance *DocTreeViewFactory::createInstance()
{
    KInstance *instance = KGenericFactory<DocTreeViewPart>::createInstance();
    KStandardDirs *dirs = instance->dirs();
    dirs->addResourceType("docindices", KStandardDirs::kde_default("data") + "kdevdoctreeview/indices/");
    dirs->addResourceType("doctocs", KStandardDirs::kde_default("data") + "kdevdoctreeview/tocs/");
    return instance;
}

