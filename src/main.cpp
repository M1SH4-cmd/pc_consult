#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QStringList>

#include <cstdlib>

#include "core/ConsultationSession.h"
#include "core/DecisionTree.h"
#include "core/TreeDataLoader.h"
#include "core/TreeValidator.h"
#include "ui/MainWindow.h"

namespace
{

QString toQString(const std::string& utf8)
{
    return QString::fromUtf8(utf8.data(), static_cast<qsizetype>(utf8.size()));
}

int reportCriticalError(const QString& title, const QString& text)
{
    QMessageBox::critical(nullptr, title, text);
    return EXIT_FAILURE;
}

}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName(QStringLiteral("PCConsultant"));
    QCoreApplication::setApplicationName(QStringLiteral("PC Consultant"));

    const core::TreeLoadResult loaded =
        core::TreeDataLoader::loadFromResource(":/data/consultant_tree.json");

    if (!loaded.ok())
    {
        return reportCriticalError(
            QStringLiteral("Ошибка загрузки"),
            QStringLiteral("Не удалось загрузить дерево консультанта:\n\n%1")
                .arg(toQString(loaded.error->message)));
    }

    const core::DecisionTree& tree = *loaded.tree;

    const core::ValidationResult validation = core::TreeValidator::validate(tree);
    if (!validation.valid())
    {
        QStringList details;
        details.reserve(static_cast<int>(validation.errors.size()));
        for (const std::string& error : validation.errors)
        {
            details << QStringLiteral("- ") + toQString(error);
        }

        return reportCriticalError(
            QStringLiteral("Дерево консультанта некорректно"),
            QStringLiteral("Проверка дерева не пройдена:\n\n%1").arg(details.join('\n')));
    }

    core::ConsultationSession session(tree);
    MainWindow window(session);
    window.show();

    return app.exec();
}
