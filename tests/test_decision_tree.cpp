#include <QtTest/QtTest>
#include "core/DecisionTree.h"
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

class TestDecisionTree : public QObject
{
    Q_OBJECT

private slots:
    void testRootId()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")}
        };
        DecisionTree tree(rootId, nodes);
        QCOMPARE(QString::fromStdString(tree.rootId()), QString("Q01"));
    }

    void testSize()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q02", makeQuestion("Q02", "Second question", "Q03", "Q49")}
        };
        DecisionTree tree(rootId, nodes);
        QCOMPARE(tree.size(), static_cast<std::size_t>(2));
    }

    void testContains()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")}
        };
        DecisionTree tree(rootId, nodes);
        QVERIFY(tree.contains("Q01"));
        QVERIFY(!tree.contains("Q99"));
    }

    void testFind()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")}
        };
        DecisionTree tree(rootId, nodes);
        QVERIFY(tree.find("Q01") != nullptr);
        QVERIFY(tree.find("Q99") == nullptr);
    }

    void testAt()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")}
        };
        DecisionTree tree(rootId, nodes);
        QCOMPARE(QString::fromStdString(tree.at("Q01").text), QString("Root question"));
        QVERIFY_EXCEPTION_THROWN(tree.at("Q99"), std::out_of_range);
    }

    void testInvalidRootConstructor()
    {
        NodeId rootId("Q99");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")}
        };
        QVERIFY_EXCEPTION_THROWN(DecisionTree(rootId, nodes), std::invalid_argument);
    }
};

QTEST_MAIN(TestDecisionTree)
#include "test_decision_tree.moc"