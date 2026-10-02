#pragma once

#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <string>
#include <vector>

namespace core
{
class ConsultationSession;
class DecisionTree;
}

class HistoryView final : public QScrollArea
{
    Q_OBJECT

public:
    explicit HistoryView(const core::ConsultationSession& session,
                         const core::DecisionTree& tree,
                         QWidget* parent = nullptr);

    void refresh();

private:
    void buildEmpty();
    void buildTimeline();

    const core::ConsultationSession& m_session;
    const core::DecisionTree& m_tree;
    QVBoxLayout* m_layout = nullptr;
    QWidget* m_widget = nullptr;
};
