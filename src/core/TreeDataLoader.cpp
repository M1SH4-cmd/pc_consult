#include "core/TreeDataLoader.h"

#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace core
{
namespace
{

std::string toUtf8(const QString& value)
{
    const QByteArray bytes = value.toUtf8();
    return std::string(bytes.constData(), static_cast<std::size_t>(bytes.size()));
}

TreeLoadResult fail(std::string message)
{
    TreeLoadResult result;
    result.error = TreeLoadError{std::move(message)};
    return result;
}

bool readNonEmptyString(const QJsonObject& object, const QString& key, const QString& context,
                        std::string& out, TreeLoadResult& result)
{
    const QJsonValue value = object.value(key);
    if (value.isUndefined())
    {
        result = fail(toUtf8(context) + " is missing field '" + toUtf8(key) + "'");
        return false;
    }
    if (!value.isString())
    {
        result = fail(toUtf8(context) + " field '" + toUtf8(key) + "' must be string");
        return false;
    }
    out = toUtf8(value.toString());
    if (out.empty())
    {
        result = fail(toUtf8(context) + " field '" + toUtf8(key) + "' must not be empty");
        return false;
    }
    return true;
}

bool parseQuestions(const QJsonObject& questions, DecisionTree::NodeMap& nodes,
                    TreeLoadResult& result)
{
    for (auto it = questions.constBegin(); it != questions.constEnd(); ++it)
    {
        const std::string id = toUtf8(it.key());
        if (nodes.find(id) != nodes.end())
        {
            result = fail("Question " + id + " conflicts with an existing node");
            return false;
        }

        const QJsonValue value = it.value();
        if (!value.isObject())
        {
            result = fail("Question " + id + " must be an object");
            return false;
        }
        const QJsonObject object = value.toObject();
        const QString context = QStringLiteral("Question ") + it.key();

        DecisionNode node;
        node.id = id;
        node.kind = NodeKind::Question;

        std::string yes;
        std::string no;
        if (!readNonEmptyString(object, QStringLiteral("text"), context, node.text, result))
        {
            return false;
        }
        if (!readNonEmptyString(object, QStringLiteral("yes"), context, yes, result))
        {
            return false;
        }
        if (!readNonEmptyString(object, QStringLiteral("no"), context, no, result))
        {
            return false;
        }

        node.yesTarget = std::move(yes);
        node.noTarget = std::move(no);
        nodes.emplace(id, std::move(node));
    }
    return true;
}

bool parseResults(const QJsonObject& results, DecisionTree::NodeMap& nodes,
                  TreeLoadResult& result)
{
    for (auto it = results.constBegin(); it != results.constEnd(); ++it)
    {
        const std::string id = toUtf8(it.key());
        if (nodes.find(id) != nodes.end())
        {
            result = fail("Result " + id + " conflicts with an existing node");
            return false;
        }

        const QJsonValue value = it.value();
        if (!value.isObject())
        {
            result = fail("Result " + id + " must be an object");
            return false;
        }
        const QJsonObject object = value.toObject();
        const QString context = QStringLiteral("Result ") + it.key();

        DecisionNode node;
        node.id = id;
        node.kind = NodeKind::Result;
        if (!readNonEmptyString(object, QStringLiteral("text"), context, node.text, result))
        {
            return false;
        }

        nodes.emplace(id, std::move(node));
    }
    return true;
}

}

TreeLoadResult TreeDataLoader::loadFromJson(const std::string& jsonUtf8)
{
    const QByteArray raw(jsonUtf8.data(), static_cast<int>(jsonUtf8.size()));
    QJsonParseError parseError{};
    const QJsonDocument document = QJsonDocument::fromJson(raw, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        return fail("JSON parse error at offset " + std::to_string(parseError.offset) + ": "
                    + toUtf8(parseError.errorString()));
    }
    if (!document.isObject())
    {
        return fail("JSON root must be an object");
    }

    const QJsonObject rootObject = document.object();

    const QJsonValue rootValue = rootObject.value(QStringLiteral("root"));
    if (rootValue.isUndefined())
    {
        return fail("JSON is missing field 'root'");
    }
    if (!rootValue.isString())
    {
        return fail("Root field 'root' must be string");
    }
    const std::string rootId = toUtf8(rootValue.toString());
    if (rootId.empty())
    {
        return fail("Root field 'root' must not be empty");
    }

    const QJsonValue questionsValue = rootObject.value(QStringLiteral("questions"));
    if (questionsValue.isUndefined())
    {
        return fail("JSON is missing field 'questions'");
    }
    if (!questionsValue.isObject())
    {
        return fail("Field 'questions' must be an object");
    }

    const QJsonValue resultsValue = rootObject.value(QStringLiteral("results"));
    if (resultsValue.isUndefined())
    {
        return fail("JSON is missing field 'results'");
    }
    if (!resultsValue.isObject())
    {
        return fail("Field 'results' must be an object");
    }

    DecisionTree::NodeMap nodes;
    nodes.reserve(static_cast<std::size_t>(questionsValue.toObject().size()
                                           + resultsValue.toObject().size()));

    TreeLoadResult result;
    if (!parseQuestions(questionsValue.toObject(), nodes, result))
    {
        return result;
    }
    if (!parseResults(resultsValue.toObject(), nodes, result))
    {
        return result;
    }

    if (nodes.find(rootId) == nodes.end())
    {
        return fail("Root '" + rootId + "' does not exist");
    }

    TreeLoadResult success;
    success.tree = DecisionTree(rootId, std::move(nodes));
    return success;
}

TreeLoadResult TreeDataLoader::loadFromResource(const std::string& resourcePath)
{
    QFile file(QString::fromUtf8(resourcePath.c_str(), static_cast<int>(resourcePath.size())));
    if (!file.open(QIODevice::ReadOnly))
    {
        return fail("Cannot open resource '" + resourcePath + "': " + toUtf8(file.errorString()));
    }
    const QByteArray bytes = file.readAll();
    file.close();
    return loadFromJson(std::string(bytes.constData(), static_cast<std::size_t>(bytes.size())));
}

}
