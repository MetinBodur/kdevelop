/*
    Interface description for KDevelop OutputView Filter strategies
    Copyright (C) 2012  Morten Danielsen Volden mvolden2@gmail.com
    Copyright (C) 2016  Kevin Funk <kfunk@kde.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "ifilterstrategy.h"

namespace KDevelop
{


IFilterStrategy::IFilterStrategy()
{
}

IFilterStrategy::~IFilterStrategy()
{
}

IFilterStrategy::Progress IFilterStrategy::progressInLine(const QString& line)
{
    Q_UNUSED(line);
    return {};
}

}
