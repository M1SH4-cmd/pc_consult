#pragma once

#include <QMainWindow>

class QLabel;
class QPushButton;
class QScrollArea;

namespace core
{
class ConsultationSession;
}

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(core::ConsultationSession& session, QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void buildUi();
    void refreshUi();

    core::ConsultationSession& m_session;

    QLabel* m_textLabel = nullptr;
    QScrollArea* m_textArea = nullptr;
    QPushButton* m_yesButton = nullptr;
    QPushButton* m_noButton = nullptr;
    QPushButton* m_backButton = nullptr;
    QPushButton* m_restartButton = nullptr;
};
