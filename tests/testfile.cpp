/* This file is part of KDevelop

   Copyright 2010 Niko Sams <niko.sams@gmail.com>
   Copyright 2011 Milian Wolff <mail@milianw.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "testfile.h"

#include "testproject.h"

#include <KTemporaryFile>
#include <QTime>
#include <QTest>

#include <language/duchain/duchainlock.h>
#include <language/duchain/duchain.h>

using namespace KDevelop;

struct TestFile::TestFilePrivate {
    TestFilePrivate()
    : ready(false)
    {
    }

    void updateReady(const IndexedString& _url, ReferencedTopDUContext _topContext)
    {
        Q_ASSERT(_url == url);
        topContext = _topContext;
        ready = true;
    }

    KTemporaryFile file;
    bool ready;
    ReferencedTopDUContext topContext;
    IndexedString url;
    TestProject* project;
};

TestFile::TestFile (const QString& contents, const QString& fileExtension, TestProject* project, const QString& dir)
: d(new TestFilePrivate())
{
    d->file.setSuffix('.' + fileExtension);
    d->file.setPrefix(dir);
    d->file.open();
    Q_ASSERT(d->file.isOpen());
    Q_ASSERT(d->file.isWritable());
    d->file.write(contents.toLocal8Bit());
    d->file.close();

    QFileInfo info(d->file.fileName());
    Q_ASSERT(info.exists());
    Q_ASSERT(info.isFile());
    d->url = IndexedString(info.absoluteFilePath());

    d->project = project;
    project->addToFileSet(d->url);
}

TestFile::~TestFile()
{
    if (d->topContext) {
        DUChainWriteLocker lock;
        DUChain::self()->removeDocumentChain(d->topContext.data());
    }
    d->project->removeFromFileSet(d->url);
    delete d;
}

IndexedString TestFile::url() const
{
    return d->url;
}

void TestFile::parse (TopDUContext::Features features, int priority)
{
    d->ready = false;
    DUChain::self()->updateContextForUrl(d->url, features, this, priority);
}

bool TestFile::waitForParsed(int timeout)
{
    QTime t;
    t.start();
    while (!d->ready && t.elapsed() < timeout) {
        QTest::qWait(10);
    }
    return d->ready;
}

ReferencedTopDUContext TestFile::topContext()
{
    waitForParsed();
    return d->topContext;
}

#include "testfile.moc"
