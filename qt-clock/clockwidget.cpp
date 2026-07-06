#include "clockwidget.h"
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QRadialGradient>
#include <QtMath>

ClockWidget::ClockWidget(QWidget *parent)
    : QWidget(parent)
    , m_darkMode(false)
{
    setMinimumSize(300, 300);
    setMaximumSize(600, 600);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    m_timer->start(50); // 50ms refresh for smooth second hand animation

    // Default light theme colors
    m_bgColor = QColor(245, 245, 245);
    m_dialColor = QColor(255, 255, 255);
    m_tickColor = QColor(50, 50, 50);
    m_numberColor = QColor(30, 30, 30);
    m_hourHandColor = QColor(30, 30, 30);
    m_minuteHandColor = QColor(60, 60, 60);
    m_secondHandColor = QColor(220, 50, 50);
}

void ClockWidget::setDarkMode(bool dark)
{
    m_darkMode = dark;
    if (dark) {
        m_bgColor = QColor(30, 30, 35);
        m_dialColor = QColor(45, 45, 50);
        m_tickColor = QColor(200, 200, 200);
        m_numberColor = QColor(220, 220, 220);
        m_hourHandColor = QColor(220, 220, 220);
        m_minuteHandColor = QColor(180, 180, 180);
        m_secondHandColor = QColor(255, 80, 80);
    } else {
        m_bgColor = QColor(245, 245, 245);
        m_dialColor = QColor(255, 255, 255);
        m_tickColor = QColor(50, 50, 50);
        m_numberColor = QColor(30, 30, 30);
        m_hourHandColor = QColor(30, 30, 30);
        m_minuteHandColor = QColor(60, 60, 60);
        m_secondHandColor = QColor(220, 50, 50);
    }
    update();
}

bool ClockWidget::isDarkMode() const
{
    return m_darkMode;
}

void ClockWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int side = qMin(width(), height());
    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(side / 200.0, side / 200.0);

    // Background
    painter.setBrush(m_bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(-width() / 2, -height() / 2, width(), height());

    drawDial(painter, side);
    drawTicks(painter, side);
    drawNumbers(painter, side);

    QTime currentTime = QTime::currentTime();
    drawHands(painter, side, currentTime);
}

void ClockWidget::drawDial(QPainter &painter, int side)
{
    Q_UNUSED(side);

    // Outer ring gradient
    QRadialGradient gradient(0, 0, 95);
    gradient.setColorAt(0.0, m_dialColor);
    gradient.setColorAt(0.85, m_dialColor);
    gradient.setColorAt(0.9, m_darkMode ? QColor(60, 60, 65) : QColor(220, 220, 220));
    gradient.setColorAt(1.0, m_darkMode ? QColor(35, 35, 40) : QColor(200, 200, 200));

    painter.setBrush(gradient);
    painter.setPen(QPen(m_tickColor, 2));
    painter.drawEllipse(QPoint(0, 0), 95, 95);

    // Inner ring
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(m_tickColor, 1));
    painter.drawEllipse(QPoint(0, 0), 88, 88);
}

void ClockWidget::drawTicks(QPainter &painter, int side)
{
    Q_UNUSED(side);

    for (int i = 0; i < 60; ++i) {
        double angle = i * 6.0; // 6 degrees per tick
        double rad = qDegreesToRadians(angle - 90.0);

        if (i % 5 == 0) {
            // Major tick (every 5 minutes)
            int outer = 82;
            int inner = 72;
            painter.setPen(QPen(m_tickColor, 3));
            painter.drawLine(
                static_cast<int>(inner * qCos(rad)),
                static_cast<int>(inner * qSin(rad)),
                static_cast<int>(outer * qCos(rad)),
                static_cast<int>(outer * qSin(rad))
            );
        } else {
            // Minor tick
            int outer = 82;
            int inner = 76;
            painter.setPen(QPen(m_tickColor, 1));
            painter.drawLine(
                static_cast<int>(inner * qCos(rad)),
                static_cast<int>(inner * qSin(rad)),
                static_cast<int>(outer * qCos(rad)),
                static_cast<int>(outer * qSin(rad))
            );
        }
    }
}

void ClockWidget::drawNumbers(QPainter &painter, int side)
{
    Q_UNUSED(side);

    painter.setPen(m_numberColor);
    QFont font = painter.font();
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);

    for (int i = 1; i <= 12; ++i) {
        double angle = i * 30.0; // 30 degrees per hour
        double rad = qDegreesToRadians(angle - 90.0);
        int radius = 62;

        double x = radius * qCos(rad);
        double y = radius * qSin(rad);

        QRectF rect(x - 15, y - 15, 30, 30);
        painter.drawText(rect, Qt::AlignCenter, QString::number(i));
    }
}

void ClockWidget::drawHands(QPainter &painter, int side, const QTime &time)
{
    Q_UNUSED(side);

    int ms = QTime::currentTime().msec();
    double hourAngle = 30.0 * (time.hour() % 12) + 0.5 * time.minute();
    double minuteAngle = 6.0 * time.minute() + 0.1 * time.second();
    double secondAngle = 6.0 * time.second() + 6.0 * (ms / 1000.0);

    // Draw hour hand
    painter.save();
    painter.rotate(hourAngle);
    painter.setBrush(m_hourHandColor);
    painter.setPen(QPen(m_hourHandColor, 2));
    QPolygonF hourHand;
    hourHand << QPointF(-6, 10) << QPointF(0, -50) << QPointF(6, 10);
    painter.drawPolygon(hourHand);
    painter.restore();

    // Draw minute hand
    painter.save();
    painter.rotate(minuteAngle);
    painter.setBrush(m_minuteHandColor);
    painter.setPen(QPen(m_minuteHandColor, 2));
    QPolygonF minuteHand;
    minuteHand << QPointF(-4, 12) << QPointF(0, -70) << QPointF(4, 12);
    painter.drawPolygon(minuteHand);
    painter.restore();

    // Draw second hand
    painter.save();
    painter.rotate(secondAngle);
    painter.setBrush(m_secondHandColor);
    painter.setPen(QPen(m_secondHandColor, 1));
    painter.drawLine(QPointF(0, 15), QPointF(0, -75));

    // Second hand circle
    painter.setBrush(m_secondHandColor);
    painter.drawEllipse(QPointF(0, -75), 4, 4);
    painter.restore();

    // Center dot
    painter.setBrush(m_darkMode ? QColor(220, 220, 220) : QColor(30, 30, 30));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(0, 0), 5, 5);
    painter.setBrush(m_secondHandColor);
    painter.drawEllipse(QPoint(0, 0), 3, 3);
}
</｜｜DSML｜｜parameter>
</write_to_file>