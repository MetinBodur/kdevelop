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

#ifndef _DOCTREEVIEWFACTORY_H_
#define _DOCTREEVIEWFACTORY_H_

#include <kgenericfactory.h>
#include "doctreeviewpart.h"


class DocTreeViewFactory : public KGenericFactory<DocTreeViewPart>
{
public:
    DocTreeViewFactory();

protected:
    virtual KInstance *createInstance();
};

#endif
