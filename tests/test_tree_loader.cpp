#include <QtTest/QtTest>
#include <QResource>
#include "core/TreeDataLoader.h"
#include "core/DecisionNode.h"

using namespace core;

class TestTreeLoader : public QObject
{
    Q_OBJECT

private slots:
    void testRealJson()
    {
        auto result = TreeDataLoader::loadFromResource(":/data/consultant_tree.json");
        QVERIFY(result.ok());
        QVERIFY(result.tree.has_value());
        QCOMPARE(QString::fromStdString(result.tree->rootId()), QString("Q01"));
        QCOMPARE(result.tree->size(), static_cast<std::size_t>(92));

        const DecisionNode& q01 = result.tree->at("Q01");
        QCOMPARE(q01.kind, NodeKind::Question);
        QVERIFY(q01.yesTarget.has_value());
        QVERIFY(q01.noTarget.has_value());
        QCOMPARE(QString::fromStdString(*q01.yesTarget), QString("Q02"));
        QCOMPARE(QString::fromStdString(*q01.noTarget), QString("Q48"));
        QCOMPARE(QString::fromStdString(q01.text),
                 QString::fromUtf8("Вы выбираете компонент для уже работающего ПК?"));

        const DecisionNode& gpu = result.tree->at("O_GPU1");
        QCOMPARE(gpu.kind, NodeKind::Result);
        QVERIFY(!gpu.yesTarget.has_value());
        QVERIFY(!gpu.noTarget.has_value());
    }

    void testMalformedJson()
    {
        auto result = TreeDataLoader::loadFromJson("{invalid json}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testMissingRoot()
    {
        auto result = TreeDataLoader::loadFromJson("{\"questions\": {}, \"results\": {}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testWrongRootType()
    {
        auto result = TreeDataLoader::loadFromJson(
            "{\"root\": 42, \"questions\": {}, \"results\": {}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testMissingQuestions()
    {
        auto result = TreeDataLoader::loadFromJson("{\"root\": \"Q01\", \"results\": {}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testMissingResults()
    {
        auto result = TreeDataLoader::loadFromJson("{\"root\": \"Q01\", \"questions\": {}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testMissingText()
    {
        auto result = TreeDataLoader::loadFromJson(
            "{\"root\": \"Q01\", \"questions\": {\"Q01\": {\"yes\": \"Q02\", \"no\": \"Q48\"}}, "
            "\"results\": {\"Q02\": {\"text\": \"R\"}, \"Q48\": {\"text\": \"R\"}}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testMissingYes()
    {
        auto result = TreeDataLoader::loadFromJson(
            "{\"root\": \"Q01\", \"questions\": {\"Q01\": {\"text\": \"Q\", \"no\": \"Q48\"}}, "
            "\"results\": {\"Q48\": {\"text\": \"R\"}}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testMissingNo()
    {
        auto result = TreeDataLoader::loadFromJson(
            "{\"root\": \"Q01\", \"questions\": {\"Q01\": {\"text\": \"Q\", \"yes\": \"Q02\"}}, "
            "\"results\": {\"Q02\": {\"text\": \"R\"}}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testWrongFieldType()
    {
        auto result = TreeDataLoader::loadFromJson(
            "{\"root\": \"Q01\", \"questions\": {\"Q01\": {\"text\": 123, \"yes\": \"Q02\", "
            "\"no\": \"Q48\"}}, \"results\": {\"Q02\": {\"text\": \"R\"}, \"Q48\": {\"text\": "
            "\"R\"}}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testIdConflict()
    {
        auto result = TreeDataLoader::loadFromJson(
            "{\"root\": \"Q01\", \"questions\": {\"Q01\": {\"text\": \"Q\", \"yes\": \"Q02\", "
            "\"no\": \"Q48\"}}, \"results\": {\"Q01\": {\"text\": \"R\"}}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }

    void testNonexistentRoot()
    {
        auto result = TreeDataLoader::loadFromJson(
            "{\"root\": \"Q99\", \"questions\": {}, \"results\": {\"Q01\": {\"text\": \"R\"}}}");
        QVERIFY(!result.ok());
        QVERIFY(result.error.has_value());
    }
};

QTEST_MAIN(TestTreeLoader)
#include "test_tree_loader.moc"