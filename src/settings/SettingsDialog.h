#pragma once

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>

#include "core/ConsultationSession.h"
#include "core/DecisionTree.h"
#include "settings/AppSettings.h"
#include "settings/ThemeManager.h"
#include "ui/HistoryView.h"

class QComboBox;
class QFontComboBox;
class QPushButton;
class QToolButton;

namespace settings
{

class SettingsDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(const core::ConsultationSession& session,
                            const core::DecisionTree& tree,
                            AppSettings& settings,
                            QWidget* parent = nullptr);
    ~SettingsDialog() override;

private slots:
    void onNavClicked(QListWidget* list, int index);
    void onThemeChanged(int index);
    void onFontChanged(const QFont& font);
    void onAccentSelected(int index);
    void onCloseClicked();

private:
    void buildUi();
    void applySettings();

    const core::ConsultationSession& m_session;
    const core::DecisionTree& m_tree;

    QFrame* m_sidebar = nullptr;
    QStackedWidget* m_pages = nullptr;
    QListWidget* m_nav = nullptr;
    QPushButton* m_closeButton = nullptr;

    QComboBox* m_themeCombo = nullptr;
    QFontComboBox* m_fontCombo = nullptr;
    QWidget* m_accentContainer = nullptr;

    HistoryView* m_historyView = nullptr;

    AppSettings& m_settings;
    int m_currentPage = 0;
};

}
