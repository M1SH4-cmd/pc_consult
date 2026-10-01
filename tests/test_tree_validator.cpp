#include <QtTest/QtTest>

#include "core/TreeValidator.h"
#include "test_helpers.h"

class TreeValidatorTest : public QObject
{
    Q_OBJECT

private slots:
    void testValidTree();
    void testRootMustBeQuestion();
    void testEmptyText();
    void testMissingYes();
    void testMissingNo();
    void testEmptyTarget();
    void testDanglingYes();
    void testDanglingNo();
    void testResultWithYesTransition();
    void testResultWithNoTransition();
    void testUnreachableNode();
    void testDirectCycle();
    void testIndirectCycle();
};

void TreeValidatorTest::testValidTree()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "R1", "R1"),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(result.valid());
}

void TreeValidatorTest::testRootMustBeQuestion()
{
    const core::DecisionTree tree = testing_helpers::makeTree("R1",
        {testing_helpers::makeQuestion("Q1", "text", "R1", "R1"),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("must be a question") != std::string::npos);
}

void TreeValidatorTest::testEmptyText()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "", "R1", "R1"),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("empty text") != std::string::npos);
}

void TreeValidatorTest::testMissingYes()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", std::nullopt, "R1"),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("missing target for answer 'yes'") != std::string::npos);
}

void TreeValidatorTest::testMissingNo()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "R1", std::nullopt),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("missing target for answer 'no'") != std::string::npos);
}

void TreeValidatorTest::testEmptyTarget()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "", "R1"),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("empty target for answer 'yes'") != std::string::npos);
}

void TreeValidatorTest::testDanglingYes()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "MISSING", "R1"),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("does not exist") != std::string::npos);
}

void TreeValidatorTest::testDanglingNo()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "R1", "MISSING"),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("does not exist") != std::string::npos);
}

void TreeValidatorTest::testResultWithYesTransition()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "R1", "R1"),
         testing_helpers::makeResult("R1", "result"),
         tree.nodes().at("R1")});
    auto r1 = tree.nodes().at("R1");
    r1.yesTarget = "Q1";  // break the Result node
    const core::DecisionTree tree2 = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "R1", "R1"), r1});
    const core::ValidationResult result = core::TreeValidator::validate(tree2);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("must not have target for answer 'yes'") != std::string::npos);
}

void TreeValidatorTest::testResultWithNoTransition()
{
    core::DecisionNode r1 = testing_helpers::makeResult("R1", "result");
    r1.noTarget = "Q1";  // break the Result node
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "R1", "R1"), r1});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("must not have target for answer 'no'") != std::string::npos);
}

void TreeValidatorTest::testUnreachableNode()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "R1", "R1"),
         testing_helpers::makeResult("R1", "result"),
         testing_helpers::makeResult("R2", "orphan")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("not reachable from the root") != std::string::npos);
}

void TreeValidatorTest::testDirectCycle()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "Q1", "Q1"),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("cycle") != std::string::npos);
}

void TreeValidatorTest::testIndirectCycle()
{
    const core::DecisionTree tree = testing_helpers::makeTree("Q1",
        {testing_helpers::makeQuestion("Q1", "text", "Q2", "R1"),
         testing_helpers::makeQuestion("Q2", "text", "Q1", "R1"),
         testing_helpers::makeResult("R1", "result")});
    const core::ValidationResult result = core::TreeValidator::validate(tree);
    QVERIFY(!result.valid());
    QVERIFY(result.errors.at(0).find("cycle") != std::string::npos);
}

QTEST_APPLESS_MAIN(TreeValidatorTest)
