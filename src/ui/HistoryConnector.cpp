#include "ui/HistoryConnector.h"

#include <QBrush>
#include <QEvent>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>

namespace
{

constexpr int kRailWidth = 36;
constexpr int kDiameter = 12;
constexpr int kSmallDiameter = 9;
constexpr int kMargin = 6;

}

HistoryConnector::HistoryConnector(Marker marker, bool first, bool last,
                                   QLabel* anchor, QWidget* parent)
    : QWidget(parent)
    , m_marker(marker)
    , m_first(first)
    , m_last(last)
    , m_anchor(anchor)
{
    setFixedWidth(kRailWidth);
    setObjectName(QStringLiteral("historyConnector"));
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QSize HistoryConnector::sizeHint() const
{
    return minimumSizeHint();
}

QSize HistoryConnector::minimumSizeHint() const
{
    return QSize(kRailWidth, kDiameter + kMargin * 2);
}

void HistoryConnector::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QPalette pal = palette();
    const QColor accent = pal.highlight().color();

    QColor lineColor;
    QColor markerColor;
    QColor markerOutline;
    int diameter = kDiameter;

    switch (m_marker)
    {
    case Marker::Question:
        lineColor = accent;
        markerColor = accent;
        markerOutline = accent;
        break;

    case Marker::Answer:
        lineColor = pal.mid().color();
        markerColor = lineColor;
        markerOutline = lineColor;
        diameter = kSmallDiameter;
        break;

    case Marker::Result:
        lineColor = accent;
        markerColor = accent;
        markerOutline = accent.darker(130);
        break;
    }

    const qreal lw = 1.5;
    const qreal markerYVal = markerY();
    const qreal cx = kRailWidth / 2.0;
    const qreal r = diameter / 2.0;

    p.setPen(QPen(lineColor, lw, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);

    if (!m_first)
    {
        p.drawLine(QPointF(cx, 0.0), QPointF(cx, markerYVal - r));
    }
    if (!m_last)
    {
        p.drawLine(QPointF(cx, markerYVal + r),
                   QPointF(cx, static_cast<qreal>(height())));
    }

    if (m_marker == Marker::Result)
    {
        p.setPen(QPen(markerOutline, lw, Qt::SolidLine, Qt::RoundCap,
                      Qt::RoundJoin));
        p.setBrush(markerColor);

        const qreal dx = r * 0.75;
        const qreal dy = r * 0.75;
        const QPointF pts[4] = {
            QPointF(cx, markerYVal - dy),
            QPointF(cx + dx, markerYVal),
            QPointF(cx, markerYVal + dy),
            QPointF(cx - dx, markerYVal)
        };
        p.drawPolygon(pts, 4);
    }
    else
    {
        p.setPen(QPen(markerOutline, lw, Qt::SolidLine, Qt::RoundCap,
                      Qt::RoundJoin));
        p.setBrush(markerColor);
        p.drawEllipse(QPointF(cx, markerYVal), r, r);
    }
}

void HistoryConnector::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::EnabledChange ||
        event->type() == QEvent::PaletteChange ||
        event->type() == QEvent::FontChange)
    {
        update();
    }
}

qreal HistoryConnector::markerY() const
{
    return static_cast<qreal>(kMargin) + kDiameter / 2.0;
}
