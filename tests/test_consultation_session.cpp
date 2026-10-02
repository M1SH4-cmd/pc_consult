#include <QtTest/QtTest>
#include "core/ConsultationSession.h"
#include "core/DecisionNode.h"
#include "core/NodeId.h"

using namespace core;

namespace
{

DecisionNode makeQuestion(const NodeId& id, const std::string& text, const NodeId& yes,
                          const NodeId& no)
{
    DecisionNode node;
    node.id = id;
    node.kind = NodeKind::Question;
    node.text = text;
    node.yesTarget = yes;
    node.noTarget = no;
    return node;
}

DecisionNode makeResult(const NodeId& id, const std::string& text)
{
    DecisionNode node;
    node.id = id;
    node.kind = NodeKind::Result;
    node.text = text;
    return node;
}

}

class TestConsultationSession : public QObject
{
    Q_OBJECT

private slots:
    void testInitialState()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q02", makeQuestion("Q02", "Second question", "Q03", "Q49")},
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        ConsultationSession session(tree);
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q01"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(0));
        QVERIFY(session.isQuestion());
        QVERIFY(!session.isResult());
        QVERIFY(!session.canGoBack());
    }

    void testYesTransition()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q02", makeQuestion("Q02", "Second question", "Q03", "Q49")}
        };
        DecisionTree tree(rootId, nodes);
        ConsultationSession session(tree);
        QVERIFY(session.answerYes());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q02"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(1));

        // Verify history entry
        const auto& hist = session.history();
        QCOMPARE(hist.size(), static_cast<std::size_t>(1));
        QCOMPARE(QString::fromStdString(hist[0].nodeId), QString("Q01"));
        QCOMPARE(hist[0].answer, Answer::Yes);
        QCOMPARE(QString::fromStdString(hist[0].targetId), QString("Q02"));
    }

    void testNoTransition()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        ConsultationSession session(tree);
        QVERIFY(session.answerNo());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q48"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(1));

        // Verify history entry
        const auto& hist = session.history();
        QCOMPARE(hist.size(), static_cast<std::size_t>(1));
        QCOMPARE(QString::fromStdString(hist[0].nodeId), QString("Q01"));
        QCOMPARE(hist[0].answer, Answer::No);
        QCOMPARE(QString::fromStdString(hist[0].targetId), QString("Q48"));
    }

    void testBackTransition()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q02", makeQuestion("Q02", "Second question", "Q03", "Q49")}
        };
        DecisionTree tree(rootId, nodes);
        ConsultationSession session(tree);
        QVERIFY(session.answerYes());
        QVERIFY(session.goBack());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q01"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(0));
    }

    void testBackAtRoot()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")}
        };
        DecisionTree tree(rootId, nodes);
        ConsultationSession session(tree);
        QVERIFY(!session.goBack());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q01"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(0));
    }

    void testRestart()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q02", makeQuestion("Q02", "Second question", "Q03", "Q49")}
        };
        DecisionTree tree(rootId, nodes);
        ConsultationSession session(tree);
        QVERIFY(session.answerYes());
        session.restart();
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q01"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(0));
    }

    void testTerminalResult()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        ConsultationSession session(tree);
        QVERIFY(session.answerNo());
        QVERIFY(session.isResult());
        QVERIFY(!session.isQuestion());
        QVERIFY(!session.answerYes());
        QVERIFY(!session.answerNo());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q48"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(1));
    }

    void testBackFromResult()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        ConsultationSession session(tree);
        QVERIFY(session.answerNo());
        QVERIFY(session.goBack());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("Q01"));
        QCOMPARE(session.historySize(), static_cast<std::size_t>(0));
    }

    void testDagHistory()
    {
        NodeId rootId("Q0");
        DecisionTree::NodeMap nodes = {
            {"Q0", makeQuestion("Q0", "Root question", "A", "B")},
            {"A", makeQuestion("A", "Path A", "COMMON", "")},
            {"B", makeQuestion("B", "Path B", "COMMON", "")},
            {"COMMON", makeResult("COMMON", "Common result")}
        };
        DecisionTree tree(rootId, nodes);
        ConsultationSession session(tree);
        QVERIFY(session.answerYes());
        QVERIFY(session.answerYes());
        QVERIFY(session.goBack());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("A"));
        session.restart();
        QVERIFY(session.answerNo());
        QVERIFY(session.answerYes());
        QVERIFY(session.goBack());
        QCOMPARE(QString::fromStdString(session.currentId()), QString("B"));
    }
};

QTEST_MAIN(TestConsultationSession)
#include "test_consultation_session.moc"
