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

        // Q01 --no--> Q48
        QVERIFY(session.answerNo());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q48"));

        // Verify history entry
        const auto& hist1 = session.history();
        QCOMPARE(hist1.size(), static_cast<std::size_t>(1));
        QCOMPARE(QString::fromStdString(hist1[0].nodeId), QString("Q01"));
        QCOMPARE(hist1[0].answer, Answer::No);
        QCOMPARE(QString::fromStdString(hist1[0].targetId), QString("Q48"));

        // Q48 --yes--> Q10 --> Q11 --> Q12 --> O_GPU1
        QVERIFY(session.answerYes());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q10"));
        QVERIFY(session.answerYes());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q11"));
        QVERIFY(session.answerYes());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q12"));
        QVERIFY(session.answerYes());

        QVERIFY(session.isResult());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("O_GPU1"));

        // Verify full history
        const auto& hist2 = session.history();
        QCOMPARE(hist2.size(), static_cast<std::size_t>(5));
        QCOMPARE(QString::fromStdString(hist2[0].nodeId), QString("Q01"));
        QCOMPARE(hist2[0].answer, Answer::No);
        QCOMPARE(QString::fromStdString(hist2[0].targetId), QString("Q48"));
        QCOMPARE(QString::fromStdString(hist2[1].nodeId), QString("Q48"));
        QCOMPARE(hist2[1].answer, Answer::Yes);
        QCOMPARE(QString::fromStdString(hist2[1].targetId), QString("Q10"));
        QCOMPARE(QString::fromStdString(hist2[2].nodeId), QString("Q10"));
        QCOMPARE(hist2[2].answer, Answer::Yes);
        QCOMPARE(QString::fromStdString(hist2[2].targetId), QString("Q11"));
        QCOMPARE(QString::fromStdString(hist2[3].nodeId), QString("Q11"));
        QCOMPARE(hist2[3].answer, Answer::Yes);
        QCOMPARE(QString::fromStdString(hist2[3].targetId), QString("Q12"));
        QCOMPARE(QString::fromStdString(hist2[4].nodeId), QString("Q12"));
        QCOMPARE(hist2[4].answer, Answer::Yes);
        QCOMPARE(QString::fromStdString(hist2[4].targetId), QString("O_GPU1"));

        QVERIFY(session.canGoBack());
        QVERIFY(session.goBack());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q12"));

        session.restart();
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q01"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(0));
    }
};

QTEST_MAIN(TestIntegration)
#include "test_integration.moc"
