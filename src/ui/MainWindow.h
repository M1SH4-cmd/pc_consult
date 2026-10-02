#pragma once

#include <QMainWindow>

#include <functional>

class QLabel;
class QPushButton;
class QScrollArea;
class QGraphicsOpacityEffect;
class QPropertyAnimation;
class QToolButton;

namespace core
{
class ConsultationSession;
class DecisionTree;
}

namespace settings
{
class AppSettings;
}

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(core::ConsultationSession& session,
                         const core::DecisionTree& tree,
                         settings::AppSettings& settings,
                         QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void buildUi();
    void refreshUi();

    void performTransition(const std::function<void()>& stateChange);
    void setInteractionEnabled(bool enabled);
    void startFadeIn();

    void openSettings();

    core::ConsultationSession& m_session;
    const core::DecisionTree& m_tree;
    settings::AppSettings& m_settings;

    QWidget* m_card = nullptr;
    QLabel* m_contextLabel = nullptr;
    QLabel* m_textLabel = nullptr;
    QScrollArea* m_textArea = nullptr;
    QPushButton* m_yesButton = nullptr;
    QPushButton* m_noButton = nullptr;
    QPushButton* m_backButton = nullptr;
    QPushButton* m_restartButton = nullptr;
    QToolButton* m_settingsButton = nullptr;

    QGraphicsOpacityEffect* m_fadeEffect = nullptr;
    QPropertyAnimation* m_fadeAnimation = nullptr;
    bool m_transitionRunning = false;
};
