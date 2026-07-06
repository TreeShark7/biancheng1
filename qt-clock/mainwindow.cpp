#include "mainwindow.h"
#include "clockwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QFont>
#include <QApplication>

// 8 timezones: Beijing, London, New York, Los Angeles, Tokyo, Paris, Sydney, Dubai
const int MainWindow::TIMEZONE_OFFSETS[] = {
    8 * 3600,      // Beijing (UTC+8)
    0 * 3600,      // London (UTC+0)
    -5 * 3600,     // New York (UTC-5)
    -8 * 3600,     // Los Angeles (UTC-8)
    9 * 3600,      // Tokyo (UTC+9)
    1 * 3600,      // Paris (UTC+1)
    11 * 3600,     // Sydney (UTC+11)
    4 * 3600       // Dubai (UTC+4)
};

const QString MainWindow::TIMEZONE_NAMES[] = {
    "北京 (Beijing)",
    "伦敦 (London)",
    "纽约 (New York)",
    "洛杉矶 (Los Angeles)",
    "东京 (Tokyo)",
    "巴黎 (Paris)",
    "悉尼 (Sydney)",
    "迪拜 (Dubai)"
};

const int MainWindow::TIMEZONE_COUNT = 8;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_clockWidget(nullptr)
    , m_digitalClockLabel(nullptr)
    , m_dateLabel(nullptr)
    , m_themeButton(nullptr)
    , m_darkTheme(false)
    , m_alarmTimeEdit(nullptr)
    , m_setAlarmButton(nullptr)
    , m_alarmStatusLabel(nullptr)
    , m_alarmTimer(nullptr)
    , m_stopwatchStartButton(nullptr)
    , m_stopwatchLapButton(nullptr)
    , m_stopwatchResetButton(nullptr)
    , m_stopwatchDisplay(nullptr)
    , m_lapList(nullptr)
    , m_stopwatchTimer(nullptr)
    , m_stopwatchRunning(false)
    , m_stopwatchOffset(0)
    , m_timezoneCombo(nullptr)
    , m_worldClockLabel(nullptr)
    , m_worldTimer(nullptr)
{
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("Visual Clock - 可视化时钟");
    setMinimumSize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Left side: analog clock + digital clock + date
    QVBoxLayout *leftLayout = new QVBoxLayout();

    setupClockWidget();
    leftLayout->addWidget(m_clockWidget, 1);

    setupDigitalClock();
    leftLayout->addWidget(m_digitalClockLabel);

    setupDateDisplay();
    leftLayout->addWidget(m_dateLabel);

    setupThemeButton();
    leftLayout->addWidget(m_themeButton);

    mainLayout->addLayout(leftLayout, 2);

    // Right side: alarm, stopwatch, world clock
    QVBoxLayout *rightLayout = new QVBoxLayout();

    setupAlarmSection();
    rightLayout->addWidget(findChild<QGroupBox*>("alarmGroup"));

    setupStopwatchSection();
    rightLayout->addWidget(findChild<QGroupBox*>("stopwatchGroup"));

    setupWorldClockSection();
    rightLayout->addWidget(findChild<QGroupBox*>("worldGroup"));

    rightLayout->addStretch();

    mainLayout->addLayout(rightLayout, 1);

    // Timers
    QTimer *dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();

    m_alarmTimer = new QTimer(this);
    connect(m_alarmTimer, &QTimer::timeout, this, &MainWindow::checkAlarm);
    m_alarmTimer->start(1000);

    m_worldTimer = new QTimer(this);
    connect(m_worldTimer, &QTimer::timeout, this, &MainWindow::worldClockChanged);
    m_worldTimer->start(1000);
}

void MainWindow::setupClockWidget()
{
    m_clockWidget = new ClockWidget(this);
    m_clockWidget->setMinimumSize(300, 300);
    m_clockWidget->setMaximumSize(500, 500);
}

void MainWindow::setupDigitalClock()
{
    m_digitalClockLabel = new QLabel(this);
    m_digitalClockLabel->setAlignment(Qt::AlignCenter);
    m_digitalClockLabel->setStyleSheet("font-size: 36px; font-family: 'Courier New', monospace; font-weight: bold; color: #333;");
    m_digitalClockLabel->setText("00:00:00");
}

void MainWindow::setupDateDisplay()
{
    m_dateLabel = new QLabel(this);
    m_dateLabel->setAlignment(Qt::AlignCenter);
    m_dateLabel->setStyleSheet("font-size: 16px; color: #666;");
    m_dateLabel->setText("----年--月--日 星期-");
}

void MainWindow::setupThemeButton()
{
    m_themeButton = new QPushButton("切换深色主题", this);
    m_themeButton->setFixedHeight(40);
    connect(m_themeButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);
}

void MainWindow::setupAlarmSection()
{
    QGroupBox *alarmGroup = new QGroupBox("闹钟提醒", this);
    alarmGroup->setObjectName("alarmGroup");
    QVBoxLayout *layout = new QVBoxLayout(alarmGroup);

    QHBoxLayout *timeLayout = new QHBoxLayout();
    m_alarmTimeEdit = new QTimeEdit(QTime(8, 0), this);
    m_alarmTimeEdit->setDisplayFormat("HH:mm:ss");
    m_setAlarmButton = new QPushButton("设置闹钟", this);
    timeLayout->addWidget(m_alarmTimeEdit);
    timeLayout->addWidget(m_setAlarmButton);
    layout->addLayout(timeLayout);

    m_alarmStatusLabel = new QLabel("闹钟未设置", this);
    m_alarmStatusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_alarmStatusLabel);

    connect(m_setAlarmButton, &QPushButton::clicked, this, &MainWindow::setAlarm);
}

void MainWindow::setupStopwatchSection()
{
    QGroupBox *stopwatchGroup = new QGroupBox("秒表计时", this);
    stopwatchGroup->setObjectName("stopwatchGroup");
    QVBoxLayout *layout = new QVBoxLayout(stopwatchGroup);

    m_stopwatchDisplay = new QLabel("00:00:00.000", this);
    m_stopwatchDisplay->setAlignment(Qt::AlignCenter);
    m_stopwatchDisplay->setStyleSheet("font-size: 24px; font-family: 'Courier New', monospace; font-weight: bold;");
    layout->addWidget(m_stopwatchDisplay);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_stopwatchStartButton = new QPushButton("开始", this);
    m_stopwatchLapButton = new QPushButton("记圈", this);
    m_stopwatchResetButton = new QPushButton("重置", this);
    m_stopwatchLapButton->setEnabled(false);
    m_stopwatchResetButton->setEnabled(false);
    btnLayout->addWidget(m_stopwatchStartButton);
    btnLayout->addWidget(m_stopwatchLapButton);
    btnLayout->addWidget(m_stopwatchResetButton);
    layout->addLayout(btnLayout);

    m_lapList = new QListWidget(this);
    m_lapList->setMaximumHeight(120);
    layout->addWidget(m_lapList);

    connect(m_stopwatchStartButton, &QPushButton::clicked, this, &MainWindow::stopwatchStart);
    connect(m_stopwatchLapButton, &QPushButton::clicked, this, &MainWindow::stopwatchLap);
    connect(m_stopwatchResetButton, &QPushButton::clicked, this, &MainWindow::stopwatchReset);
}

void MainWindow::setupWorldClockSection()
{
    QGroupBox *worldGroup = new QGroupBox("世界时钟", this);
    worldGroup->setObjectName("worldGroup");
    QVBoxLayout *layout = new QVBoxLayout(worldGroup);

    m_timezoneCombo = new QComboBox(this);
    for (int i = 0; i < TIMEZONE_COUNT; ++i) {
        m_timezoneCombo->addItem(TIMEZONE_NAMES[i]);
    }
    layout->addWidget(m_timezoneCombo);

    m_worldClockLabel = new QLabel(this);
    m_worldClockLabel->setAlignment(Qt::AlignCenter);
    m_worldClockLabel->setStyleSheet("font-size: 20px; font-family: 'Courier New', monospace; font-weight: bold;");
    layout->addWidget(m_worldClockLabel);

    connect(m_timezoneCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::worldClockChanged);
}

void MainWindow::updateDateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    m_digitalClockLabel->setText(now.toString("HH:mm:ss"));

    // Chinese weekday
    QStringList weekdays = {"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"};
    int dayOfWeek = now.date().dayOfWeek(); // 1=Monday ... 7=Sunday
    m_dateLabel->setText(now.toString("yyyy年M月d日 ") + weekdays[dayOfWeek - 1]);
}

void MainWindow::toggleTheme()
{
    m_darkTheme = !m_darkTheme;
    m_clockWidget->setDarkMode(m_darkTheme);

    if (m_darkTheme) {
        qApp->setStyleSheet(
            "QMainWindow { background-color: #1e1e23; }"
            "QLabel { color: #ddd; }"
            "QGroupBox { color: #ddd; border: 1px solid #555; border-radius: 5px; margin-top: 10px; }"
            "QGroupBox::title { color: #ddd; }"
            "QPushButton { background-color: #3a3a40; color: #ddd; border: 1px solid #555; padding: 5px; border-radius: 3px; }"
            "QPushButton:hover { background-color: #4a4a50; }"
            "QTimeEdit { background-color: #2a2a30; color: #ddd; border: 1px solid #555; }"
            "QComboBox { background-color: #2a2a30; color: #ddd; border: 1px solid #555; }"
            "QListWidget { background-color: #2a2a30; color: #ddd; border: 1px solid #555; }"
        );
        m_themeButton->setText("切换浅色主题");
    } else {
        qApp->setStyleSheet("");
        m_themeButton->setText("切换深色主题");
    }

    // Update colors for digital clock and date
    if (m_darkTheme) {
        m_digitalClockLabel->setStyleSheet("font-size: 36px; font-family: 'Courier New', monospace; font-weight: bold; color: #ddd;");
        m_dateLabel->setStyleSheet("font-size: 16px; color: #aaa;");
    } else {
        m_digitalClockLabel->setStyleSheet("font-size: 36px; font-family: 'Courier New', monospace; font-weight: bold; color: #333;");
        m_dateLabel->setStyleSheet("font-size: 16px; color: #666;");
    }
}

void MainWindow::setAlarm()
{
    QTime alarmTime = m_alarmTimeEdit->time();
    m_alarmStatusLabel->setText("闹钟已设置: " + alarmTime.toString("HH:mm:ss"));
    m_alarmStatusLabel->setStyleSheet("color: green;");
}

void MainWindow::checkAlarm()
{
    if (m_alarmStatusLabel && m_alarmStatusLabel->text().contains("闹钟已设置")) {
        QTime now = QTime::currentTime();
        QTime alarmTime = m_alarmTimeEdit->time();

        if (now.hour() == alarmTime.hour() &&
            now.minute() == alarmTime.minute() &&
            now.second() == alarmTime.second()) {
            QMessageBox::information(this, "闹钟提醒", "时间到！⏰");
            m_alarmStatusLabel->setText("闹钟未设置");
        }
    }
}

void MainWindow::stopwatchStart()
{
    if (!m_stopwatchRunning) {
        m_stopwatchRunning = true;
        m_stopwatchElapsed.start();
        m_stopwatchStartButton->setText("暂停");
        m_stopwatchLapButton->setEnabled(true);
        m_stopwatchResetButton->setEnabled(true);

        if (m_stopwatchTimer == nullptr) {
            m_stopwatchTimer = new QTimer(this);
            connect(m_stopwatchTimer, &QTimer::timeout, this, &MainWindow::updateStopwatchDisplay);
        }
        m_stopwatchTimer->start(50);
    } else {
        // Pause
        m_stopwatchRunning = false;
        m_stopwatchOffset += m_stopwatchElapsed.elapsed();
        m_stopwatchTimer->stop();
        m_stopwatchStartButton->setText("继续");
        m_stopwatchLapButton->setEnabled(false);
    }
}

void MainWindow::stopwatchStop()
{
    // This is handled by stopwatchStart() toggle
}

void MainWindow::stopwatchReset()
{
    m_stopwatchRunning = false;
    m_stopwatchOffset = 0;
    if (m_stopwatchTimer) {
        m_stopwatchTimer->stop();
    }
    m_stopwatchStartButton->setText("开始");
    m_stopwatchLapButton->setEnabled(false);
    m_stopwatchResetButton->setEnabled(false);
    m_stopwatchDisplay->setText("00:00:00.000");
    m_lapList->clear();
}

void MainWindow::stopwatchLap()
{
    if (m_stopwatchRunning) {
        qint64 elapsed = m_stopwatchOffset + m_stopwatchElapsed.elapsed();
        int ms = static_cast<int>(elapsed % 1000);
        int totalSec = static_cast<int>(elapsed / 1000);
        int h = totalSec / 3600;
        int m = (totalSec % 3600) / 60;
        int s = totalSec % 60;
        QString lapTime = QString("%1:%2:%3.%4")
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'))
            .arg(ms, 3, 10, QChar('0'));
        m_lapList->addItem("第 " + QString::number(m_lapList->count() + 1) + " 圈: " + lapTime);
    }
}

void MainWindow::updateStopwatchDisplay()
{
    qint64 elapsed = m_stopwatchOffset + m_stopwatchElapsed.elapsed();
    int ms = static_cast<int>(elapsed % 1000);
    int totalSec = static_cast<int>(elapsed / 1000);
    int h = totalSec / 3600;
    int m = (totalSec % 3600) / 60;
    int s = totalSec % 60;

    m_stopwatchDisplay->setText(
        QString("%1:%2:%3.%4")
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'))
            .arg(ms, 3, 10, QChar('0'))
    );
}

void MainWindow::worldClockChanged()
{
    int index = m_timezoneCombo->currentIndex();
    if (index < 0 || index >= TIMEZONE_COUNT) return;

    QDateTime utc = QDateTime::currentDateTimeUtc();
    QDateTime localTime = utc.addSecs(TIMEZONE_OFFSETS[index]);
    m_worldClockLabel->setText(localTime.toString("HH:mm:ss"));
}
</｜｜DSML｜｜parameter>
</｜｜DSML｜｜write_to_file>