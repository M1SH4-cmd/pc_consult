#include <QtTest/QtTest>

#include "core/DecisionTree.h"
#include "test_helpers.h"

class DecisionTreeTest : public QObject
{
    Q_OBJECT

private slots:
    void testRootId();
    void testSize();
    void testContains();
    void testFind();
    void testAt();
    void testInvalidRoot();
    void testNodesReadOnly();
};

void DecisionTreeTest::testRootId()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text")});
    QCOMPARE(tree.rootId(), "Q1");
}

void DecisionTreeTest::testSize()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text"),
         testing_helpers::makeResult("R1", "result")});
    QCOMPARE(tree.size(), 2);
}

void DecisionTreeTest::testContains()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text")});
    QVERIFY(tree.contains("Q1"));
    QVERIFY(!tree.contains("MISSING"));
}

void DecisionTreeTest::testFind()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text")});
    QVERIFY(tree.find("Q1") != nullptr);
    QVERIFY(tree.find("MISSING") == nullptr);
}

void DecisionTreeTest::testAt()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text")});
    QCOMPARE(tree.at("Q1").id, "Q1");
    QVERIFY_EXCEPTION_THROWN(tree.at("MISSING"), std::out_of_range);
}

void DecisionTreeTest::testInvalidRoot()
{
    QVERIFY_EXCEPTION_THROWN(
        testing_helpers::makeTree("MISSING",
            {testing_helpers::makeQuestion("Q1", "text")}),
        std::invalid_argument);
}

void DecisionTreeTest::testNodesReadOnly()
{
    core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text")});
    const core::DecisionTree::NodeMap& nodes = tree.nodes();
    QVERIFY_EXCEPTION_THROWN(nodes.clear(), std::exception);
}

QTEST_APPLESS_MAIN(DecisionTreeTest)
