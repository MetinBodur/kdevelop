#ifndef KROSSCONTEXT_H
#define KROSSCONTEXT_H

#include<QtCore/QVariant>

//This is file has been generated by xmltokross, you should not edit this file but the files used to generate it.

#include <context.h>
namespace Handlers
{
	QVariant _fileContextHandler(void* type);
	QVariant fileContextHandler(KDevelop::FileContext* type) { return _fileContextHandler((void*) type); }
	QVariant fileContextHandler(const KDevelop::FileContext* type) { return _fileContextHandler((void*) type); }

	QVariant _projectItemContextHandler(void* type);
	QVariant projectItemContextHandler(KDevelop::ProjectItemContext* type) { return _projectItemContextHandler((void*) type); }
	QVariant projectItemContextHandler(const KDevelop::ProjectItemContext* type) { return _projectItemContextHandler((void*) type); }

	QVariant _contextHandler(void* type);
	QVariant contextHandler(KDevelop::Context* type) { return _contextHandler((void*) type); }
	QVariant contextHandler(const KDevelop::Context* type) { return _contextHandler((void*) type); }

	QVariant _editorContextHandler(void* type);
	QVariant editorContextHandler(KDevelop::EditorContext* type) { return _editorContextHandler((void*) type); }
	QVariant editorContextHandler(const KDevelop::EditorContext* type) { return _editorContextHandler((void*) type); }

}

#endif
