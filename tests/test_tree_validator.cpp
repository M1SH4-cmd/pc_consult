#include <QtTest/QtTest>
#include "core/TreeValidator.h"
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

class TestTreeValidator : public QObject
{
    Q_OBJECT

private slots:
    void testValidTree()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q02", makeQuestion("Q02", "Second question", "Q48", "Q48")},
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(result.valid());
    }

    void testRootIsResult()
    {
        NodeId rootId("Q48");
        DecisionTree::NodeMap nodes = {
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testEmptyText()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "", "Q02", "Q48")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testMissingYes()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "", "Q48")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testMissingNo()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testDanglingYes()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testDanglingNo()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q48", "Q99")},
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testResultWithYesTransition()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q48", "Q48")},
            {"Q48", makeResult("Q48", "Result")}
        };
        nodes.at("Q48").yesTarget = "Q48";
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testResultWithNoTransition()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q48", "Q48")},
            {"Q48", makeResult("Q48", "Result")}
        };
        nodes.at("Q48").noTarget = "Q48";
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testUnreachableNode()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q48", "Q48")},
            {"Q48", makeResult("Q48", "Result")},
            {"Q99", makeQuestion("Q99", "Unreachable", "Q48", "Q48")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testDirectCycle()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q01", "Q48")},
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }

    void testIndirectCycle()
    {
        NodeId rootId("Q01");
        DecisionTree::NodeMap nodes = {
            {"Q01", makeQuestion("Q01", "Root question", "Q02", "Q48")},
            {"Q02", makeQuestion("Q02", "Second question", "Q03", "Q48")},
            {"Q03", makeQuestion("Q03", "Third question", "Q01", "Q48")},
            {"Q48", makeResult("Q48", "Result")}
        };
        DecisionTree tree(rootId, nodes);
        auto result = TreeValidator::validate(tree);
        QVERIFY(!result.valid());
    }
};

QTEST_MAIN(TestTreeValidator)
#include "test_tree_validator.moc"