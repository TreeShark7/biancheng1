#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QTime>

class ClockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClockWidget(QWidget *parent = nullptr);

    void setDarkMode(bool dark);
    bool isDarkMode() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawDial(QPainter &painter, int side);
    void drawTicks(QPainter &painter, int side);
    void drawNumbers(QPainter &painter, int side);
    void drawHands(QPainter &painter, int side, const QTime &time);

    QTimer *m_timer;
    bool m_darkMode;

    // Color schemes
    QColor m_bgColor;
    QColor m_dialColor;
    QColor m_tickColor;
    QColor m_numberColor;
    QColor m_hourHandColor;
    QColor m_minuteHandColor;
    QColor m_secondHandColor;
};

#endif // CLOCKWIDGET_H