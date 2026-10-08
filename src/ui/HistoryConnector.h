#pragma once

#include <QPointer>
#include <QSize>
#include <QWidget>

class QLabel;
class QPaintEvent;

class HistoryConnector final : public QWidget
{
    Q_OBJECT

public:
    enum class Marker
    {
        Question,
        Answer,
        Result
    };

    // anchor: label первой строки контента — маркер центрируется по нему.
    // first/last: рисовать ли линию сверху/снизу.
    HistoryConnector(Marker marker, bool first, bool last, QLabel* anchor,
                     QWidget* parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    qreal markerY() const;

    Marker m_marker;
    bool m_first;
    bool m_last;
    QPointer<QLabel> m_anchor;
};
