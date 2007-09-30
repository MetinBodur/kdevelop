/***************************************************************************
 *   Copyright 2007 Dukju Ahn <dukjuahn@gmail.com>                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SUBVERSIONPART_H
#define SUBVERSIONPART_H

#include "ibasicversioncontrol.h"
#include "vcsmapping.h"
#include <iplugin.h>
#include "subversioncore.h"
#include <qwidget.h>

#include <QtCore/QVariant>

class QMenu;
class KUrl;
class SvnStatusHolder;
class KTempDir;

using namespace KDevelop;

class KDevSubversionPart: public KDevelop::IPlugin, public KDevelop::IBasicVersionControl
{
    Q_OBJECT
    Q_INTERFACES( KDevelop::IBasicVersionControl )
public:
    KDevSubversionPart( QObject *parent, const QVariantList & = QVariantList() );
    virtual ~KDevSubversionPart();

    // IBasicVersionControl interfaces.
    bool isVersionControlled( const KUrl& localLocation );

    VcsJob* repositoryLocation( const KUrl& localLocation );

    VcsJob* status( const KUrl::List& localLocations, RecursionMode recursion );

    VcsJob* add( const KUrl::List& localLocations, RecursionMode recursion );

    VcsJob* remove( const KUrl::List& localLocations );

    VcsJob* edit( const KUrl& localLocation );

    VcsJob* unedit( const KUrl& localLocation );

    VcsJob* localRevision( const KUrl& localLocation,
                                   VcsRevision::RevisionType );

    VcsJob* copy( const KUrl& localLocationSrc,
                          const KUrl& localLocationDstn );

    VcsJob* move( const KUrl& localLocationSrc,
                          const KUrl& localLocationDst );

    VcsJob* revert( const KUrl::List& localLocations,
                            RecursionMode recursion );

    VcsJob* update( const KUrl::List& localLocations,
                            const VcsRevision& rev,
                            RecursionMode recursion );

    VcsJob* commit( const QString& message,
                            const KUrl::List& localLocations,
                            RecursionMode recursion );

    VcsJob* diff( const QVariant& localOrRepoLocationSrc,
                          const QVariant& localOrRepoLocationDst,
                          const VcsRevision& srcRevision,
                          const VcsRevision& dstRevision,
                          VcsDiff::Type );

    VcsJob* showDiff( const QVariant& localOrRepoLocationSrc,
                              const QVariant& localOrRepoLocationDst,
                              const VcsRevision& srcRevision,
                              const VcsRevision& dstRevision );

    VcsJob* log( const KUrl& localLocation,
                         const VcsRevision& rev,
                         unsigned long limit );

    VcsJob* log( const KUrl& localLocation,
                         const VcsRevision& rev,
                         const VcsRevision& limit );

    VcsJob* showLog( const KUrl& localLocation,
                             const VcsRevision& rev );

    VcsJob* annotate( const KUrl& localLocation,
                              const VcsRevision& rev );

    VcsJob* showAnnotate( const KUrl& localLocation,
                                  const VcsRevision& rev );

    VcsJob* merge( const QVariant& localOrRepoLocationSrc,
                           const QVariant& localOrRepoLocationDst,
                           const VcsRevision& srcRevision,
                           const VcsRevision& dstRevision,
                           const KUrl& localLocation );

    VcsJob* resolve( const KUrl::List& localLocations,
                             RecursionMode recursion );

    VcsJob* import( const KUrl& localLocation,
                            const QString& repositoryLocation,
                            RecursionMode recursion );

    VcsJob* checkout( const KDevelop::VcsMapping & mapping );

private:
    // context menu slots will be redirected to these methods,
    // because these two should be able to execute job internally.
    void checkout( const KUrl &targetDir );
    void add( const KUrl::List &wcPaths );
    void removeInternal( const KUrl::List &paths_or_urls );
    void revert( const KUrl::List &wcPaths );
    void commit( const KUrl::List &wcPaths );
    void update( const KUrl::List &wcPaths );
    void logview( const KUrl &wcPath_or_url );
    void annotate( const KUrl &path_or_url );
    void vcsInfo( const KUrl &path_or_url ); // not yet in interface
    void pegDiff( const KUrl &path );
    void diffToHead( const KUrl &path );
    void diffToBase( const KUrl &path );
    void svnStatus( const KUrl &wcPath );
    void copy( const KUrl &path_or_url );
    void move( const KUrl &path_or_url );
    void cat( const KUrl &path_or_url );

public:
    // SubversionPart internal methods
    QMap<KUrl, SvnStatusHolder> statusSync( const KUrl &dirPath, bool recurse,
        bool getall, bool contactReposit = false, bool noIgnore = true, bool ignoreExternals = false );
    SubversionCore* svncore();
    KTempDir *outputTmpDir();
    const KUrl urlFocusedDocument( );

    QPair<QString,QList<QAction*> > requestContextMenuActions( KDevelop::Context* );

public Q_SLOTS:
    // invoked by menubar, editor-context menu
    void checkout();
    void add();
    void remove();
    void commit();
    void update();
    void logView();
    void blame();
    void svnInfo();
    void cat();

    // invoked by context-menu
    void ctxLogView();
    void ctxBlame();
    void ctxCommit();
    void ctxUpdate();
    void ctxAdd();
    void ctxRemove();
    void ctxRevert();
    void ctxCheckout();
    void ctxDiff();
    void ctxDiffHead();
    void ctxDiffBase();
    void ctxInfo();
    void ctxStatus();
    void ctxCopy();
    void ctxMove();
    void ctxCat();

private:

    struct KDevSubversionPartPrivate * const d;
};
#endif

//kate: space-indent on; indent-width 4; replace-tabs on; auto-insert-doxygen on; indent-mode cstyle;
