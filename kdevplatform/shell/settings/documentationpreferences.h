/* This file is part of KDevelop
 *
 * Copyright 2016 Anton Anikin <anton.anikin@htower.ru>
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

#ifndef KDEVPLATFORM_DOCUMENTATION_PREFERENCES_H
#define KDEVPLATFORM_DOCUMENTATION_PREFERENCES_H

#include <interfaces/configpage.h>

namespace KDevelop
{

class DocumentationPreferences : public ConfigPage
{
    Q_OBJECT
public:
    explicit DocumentationPreferences(QWidget* parent);
    ~DocumentationPreferences() override;

    QString name() const override;
    QIcon icon() const override;
    QString fullName() const override;

public Q_SLOTS:
    void apply() override;
    void defaults() override;
    void reset() override;
};

}

#endif
