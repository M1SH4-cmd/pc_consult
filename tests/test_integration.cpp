#include <QtTest/QtTest>
#include <QResource>
#include "core/TreeDataLoader.h"
#include "core/TreeValidator.h"
#include "core/ConsultationSession.h"

using namespace core;

class TestIntegration : public QObject
{
    Q_OBJECT

private slots:
    void testRealPathToResult()
    {
        auto loadResult = TreeDataLoader::loadFromResource(":/data/consultant_tree.json");
        QVERIFY(loadResult.ok());
        QVERIFY(loadResult.tree.has_value());
        QCOMPARE(loadResult.tree->size(), static_cast<std::size_t>(92));

        auto validationResult = TreeValidator::validate(*loadResult.tree);
        QVERIFY2(validationResult.valid(), "Loaded tree must pass validation");

        ConsultationSession session(*loadResult.tree);

        QVERIFY(session.isQuestion());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q01"));

        QVERIFY(session.answerNo());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q48"));
        QVERIFY(session.answerNo());
        QVERIFY(session.isResult());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("O_CPU1"));

        QVERIFY(session.canGoBack());
        QVERIFY(session.goBack());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q48"));

        session.restart();
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q01"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(0));
    }
};

QTEST_MAIN(TestIntegration)
#include "test_integration.moc"