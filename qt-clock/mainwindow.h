#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTimeEdit>
#include <QListWidget>
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>

class ClockWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateDateTime();
    void toggleTheme();
    void setAlarm();
    void checkAlarm();
    void stopwatchStart();
    void stopwatchStop();
    void stopwatchReset();
    void stopwatchLap();
    void updateStopwatchDisplay();
    void worldClockChanged();

private:
    void setupUI();
    void setupClockWidget();
    void setupDigitalClock();
    void setupDateDisplay();
    void setupThemeButton();
    void setupAlarmSection();
    void setupStopwatchSection();
    void setupWorldClockSection();

    // Clock widget
    ClockWidget *m_clockWidget;

    // Digital clock & date
    QLabel *m_digitalClockLabel;
    QLabel *m_dateLabel;

    // Theme
    QPushButton *m_themeButton;
    bool m_darkTheme;

    // Alarm
    QTimeEdit *m_alarmTimeEdit;
    QPushButton *m_setAlarmButton;
    QLabel *m_alarmStatusLabel;
    QTimer *m_alarmTimer;

    // Stopwatch
    QPushButton *m_stopwatchStartButton;
    QPushButton *m_stopwatchLapButton;
    QPushButton *m_stopwatchResetButton;
    QLabel *m_stopwatchDisplay;
    QListWidget *m_lapList;
    QTimer *m_stopwatchTimer;
    QElapsedTimer m_stopwatchElapsed;
    bool m_stopwatchRunning;
    qint64 m_stopwatchOffset;

    // World clock
    QComboBox *m_timezoneCombo;
    QLabel *m_worldClockLabel;
    QTimer *m_worldTimer;

    // Timezone offsets in seconds
    static const int TIMEZONE_OFFSETS[];
    static const QString TIMEZONE_NAMES[];
    static const int TIMEZONE_COUNT;
};

#endif // MAINWINDOW_H
</｜｜DSML｜｜parameter>
</write_to_file>