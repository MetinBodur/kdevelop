/* This file is part of the KDE project
   Copyright (C) 2001 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
   Copyright (C) 2002-2003 Roberto Raggi <roberto@kdevelop.org>
   Copyright (C) 2002 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2003 Mario Scalas <mario.scalas@libero.it>
   Copyright (C) 2004 Alexander Dymo <adymo@kdevelop.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef IVERSIONCONTROL_H
#define IVERSIONCONTROL_H

#include "iextension.h"
#include "kdevexport.h"
#include <QtDesigner/QAbstractExtensionFactory>

#include <kurl.h>

#include <qstring.h>
#include <qwidget.h>
#include <qmap.h>

class QMenu;

/**
@file iversioncontrol.h
Version control system interface and utility classes.
 */
namespace KDevelop{
    
/**State of the file.*/
enum VcsFileState {
    Unknown        /**<No VCS information about a file is known.*/,
    Added          /**<File was added to the repository but not commited.*/,
    Uptodate       /**<File was updated or it is already at up to date version.*/,
    Modified       /**<File was modified locally.*/,
    Conflict       /**<Local version conflicts with the one in a repository.*/,
    Sticky         /**<File is sticky.*/,
    NeedsPatch     /**<File needs a patch.*/,
    NeedsCheckout  /**<File needs to be checkout again.*/,
    Directory      /**<This is a directory.*/ ,
    Deleted        /**<File or Directory is scheduled to be deleted. */ ,
    Replaced       /**<File was scheduled for deletion, and then a new file with the same name was scheduled for addition in its place. */
};

class VcsFileInfoPrivate
{
public:
    /**The FULL file or directory path.*/
    KUrl filePath;
    /**The working revision number.*/
    QString workRevision;
    /**The repository revision number.*/
    QString repoRevision;
    /**The state of a file.*/
    VcsFileState state;
};

/**
 * Info about file state in VCS.
 * Used, for example in file views to display VCS related information about files.
 */
class VcsFileInfo
{
public:
    /**Constructor.*/
    VcsFileInfo() : d( new VcsFileInfoPrivate ) {}
    /**Constructor.
    @param fn The file name (without a path).
    @param workRev The current working revision of a file.
    @param repoRev The last revision of a file in the repository.
    @param aState The state of a file.*/    
    VcsFileInfo( KUrl &fn, QString workRev, QString repoRev, VcsFileState aState )
    : d( new VcsFileInfoPrivate )
    {
        d->filePath= fn;
        d->workRevision = workRev;
        d->repoRevision = repoRev;
        d->state = aState;
    }
    /**Destructor*/
    ~VcsFileInfo()
    {
        delete d;
    }
    /** Accessors */
    KUrl filePath() { return d->filePath; }
    QString workRev() { return d->workRevision; }
    QString repoRev() { return d->repoRevision; }
    VcsFileState state() { return d->state; }

    /**@return A descriptive string with all VCS related info about the file.*/
    QString toString() const
    {
        return "(" + d->filePath.fileName() + ", " + d->workRevision + ", " + d->repoRevision + ", " + state2String( d->state ) + ")";
    }

    /**@return A textual VCS state description.*/
    static QString state2String( VcsFileState state )
    {
        switch (state)
        {
            case Added: return "added";
            case Uptodate: return "up-to-date";
            case Modified: return "modified";
            case Conflict: return "conflict";
            case Sticky: return "sticky";
            case NeedsPatch: return "needs update";
            case NeedsCheckout: return "needs check-out";
            case Directory: return "directory";
            case Deleted: return "deleted";
            case Replaced: return "replaced";
            case Unknown:
            default:
                return "unknown";
        }
    }
private:
    struct VcsFileInfoPrivate *d;

};

/** @class VcsFileInfoMap
Info for a bunch of files that got modified.
This is a type definition: @code QMap<QString,VcsFileInfo> VcsFileInfoMap; @endcode
This class is obtained by IVcsFileInfoProvider::statusSync or IVcsFileInfoProvider::statusASync.
QString is the filename or directory which is relative to &dirPath parameter of these methods.

For example, for $PROJECT_TOP_SRC_DIR/dir1/dir2/file1.c,
we invoke statusSync( file://$PROJECT_TOP_SRC_DIR/dir1 ). Then
@code
vcsMap = statusSync( file://$PROJECT_TOP_SRC_DIR/dir1, true );
VcsFileInfo info1 = (vcsMap)["."]              // info about dir1 ('s property)
VcsFileInfo info2 = (vcsMap)["./dir2"]         // info about dir2 ('s property)
VcsFileInfo info3 = (vcsMap)["./dir2/file1.c]; // info about file1.c
@endcode
 */
typedef QMap<QString,VcsFileInfo> VcsFileInfoMap;

/**
KDevelop version control system interface.
This is the abstract base class which encapsulates everything
necessary for communicating with version control systems.
VCS support plugins should implement this interface.

@sa http://www.kdevelop.org/mediawiki/index.php/Extension_Interfaces_and_Plugins
*/
class IVersionControl
{

public:

/// Destructor
    virtual ~IVersionControl(){};

/**Creates a new project in the passed path @p dir. This should instantiate
    VCS infrastructure and import a project into the VCS in that directory.
    @param dir The absolute path to the directory where VCS infrastructure should be
    created.*/
    virtual void createNewProject(const KUrl &dir) = 0;

/**Fetches a module from remote repository, so it can be used for importing.
    @return true in case of success.*/
    virtual bool fetchFromRepository() = 0;

/**Checks if the directory is valid for this version control (for example
    CVS may check for the presence of "<dirPath>/CVS/" subdir and something else)
    @param dirPath The absolute path of the directory.
    @return true if the directory is valid for this version control
    <b>warning</b>: this returns false by default.*/
    virtual bool isValidDirectory(const KUrl &dirPath) const = 0;

/**Gets the status for local files in the specified directory (local copy): 
    the info are collected locally so they are necessarily in sync with the repository

    This is a <b>synchronous operation</b> (blocking).
    @param dirPath The ABSOLUTE directory path to stat.
    @param recursive If not set, retrieve information only for immediate child
    files and directories.
    @return Status for all files in dirPath
    @sa VcsFileInfoMap */
    virtual const VcsFileInfoMap& statusSync(const KUrl &dirPath, bool recursive) = 0;

/**Starts a request for directory status to the remote repository.
    Requests and answers are asynchronous.

    This is an <b>asynchronous operation for requesting data</b>, so
    for obvious reasons: the caller must connect the statusReady() signal and
    check for the return value of this method.
    @param dirPath The ABSOLUTE directory which status you are asking for.
    @param recursive If not set, retrieve information only for immediate child
    files and directories.
    @param infoMap The reference to VcsFileInfoMap you want the plugin will return
    to you when it has done.
    @return true if the request has been successfully started, false otherwise.*/
    virtual bool statusASync(const KUrl &dirPath, bool recursive, VcsFileInfoMap &infoMap) = 0;

/** Fill context menu. VCS plugins will fill out the caller's context menu appropriately
 *  It is VCS plugin's responsibility to connect menu signals to appropriate slots
 *  @param ctxUrl The ABSOLUTE URL (file or directory) for which the context menu was requested
 *  @param ctxMenu The reference to QMenu to be filled out by VCS plugins
 */
    virtual void fillContextMenu( const KUrl &ctxUrl, QMenu &ctxMenu ) = 0;

/** Action interfaces. These methods is supposed to be ASync (ie. non-blocking) */
    // TODO add more
    virtual void checkout( const KUrl &repository, const KUrl &targetDir, bool recurse ) = 0;
    virtual void add( const KUrl::List &wcPaths ) = 0;
    virtual void remove( const KUrl::List &paths_or_urls ) = 0;
    virtual void commit( const KUrl::List &wcPaths ) = 0;
    virtual void update( const KUrl::List &wcPaths ) = 0;
    virtual void logview( const KUrl &wcPath_or_url ) = 0;
    virtual void annotate( const KUrl &path_or_url ) = 0;

Q_SIGNALS:
/**Emitted when the Version Control has finished importing a module from remote
    repository
    @param destinationDir The directory where the module has been fetched.*/
    virtual void finishedFetching(KUrl destinationDir) = 0;

/**Emitted when the status request to remote repository has finished.
    @param fileInfoMap The status for <b>registered in repository</b> files.
    The status of files under dirPath, including informations from repository
    such as out-of-dateness, since this is async operation.
    @see statusASync for to find out when this signal should be used.*/
    virtual void statusReady(const VcsFileInfoMap &fileInfoMap) = 0;

};

} // end of namespace KDevelop

KDEV_DECLARE_EXTENSION_INTERFACE_NS( KDevelop, IVersionControl, "org.kdevelop.IVersionControl" )
Q_DECLARE_INTERFACE( KDevelop::IVersionControl, "org.kdevelop.IVersionControl" )

#endif

//kate: space-indent on; indent-width 4; replace-tabs on; auto-insert-doxygen on; indent-mode cstyle;
