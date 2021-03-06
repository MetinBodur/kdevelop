#include <QTest>
#include <serialization/itemrepositoryregistry.h>
#include <shell/sessioncontroller.h>

#include <tests/testcore.h>
#include <tests/autotestshell.h>

using namespace KDevelop;

class TestItemRepositoryRegistryAutomaticDeletion
    : public QObject
{
    Q_OBJECT
    void initCore(const QString& sessionName = QString())
    {
        auto* core = new TestCore();
        core->initialize(Core::NoUi, sessionName);
    }

    void destroyCore()
    {
        TestCore::shutdown();
    }

private Q_SLOTS:
    void initTestCase()
    {
        AutoTestShell::init();
    }

    void testTemporarySessionDeletion()
    {
        // Create and shutdown a TestCore. The session created by it is temporary
        // and thus shall be deleted upon core shutdown together with its
        // item-repository directory.

        {
            initCore();

            // The session created by TestCore shall be temporary
            QVERIFY(Core::self()->activeSession()->isTemporary());

            // The repository shall exist
            QString repositoryPath = globalItemRepositoryRegistry().path();
            QVERIFY(QFile::exists(repositoryPath));

            // The repository shall die with the core shutdown
            destroyCore();
            QVERIFY(!QFile::exists(repositoryPath));
        }
    }

    void cleanupTestCase()
    {
    }
};

#include "test_itemrepositoryregistry_automatic.moc"

QTEST_MAIN(TestItemRepositoryRegistryAutomaticDeletion)
