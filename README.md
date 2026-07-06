# Visual Clock - 可视化时钟

基于 Qt5 开发的多功能可视化时钟桌面应用，具备模拟表盘、数字时钟、主题切换、闹钟、秒表、世界时钟六大功能模块。

## 功能特性

| 功能 | 说明 |
|------|------|
| 模拟表盘 | QPainter 绘制渐变表盘、60格刻度、1~12数字标记、三根指针，秒针毫秒级平滑动画 |
| 数字时钟 | 大号等宽字体同步显示当前时间 HH:mm:ss |
| 日期显示 | 年月日 + 星期，与表盘同步刷新 |
| 主题切换 | 浅色 / 深色双主题，一键切换，表盘与界面同步变色 |
| 闹钟提醒 | 自定义时间，到点弹窗提醒 |
| 秒表计时 | 开始/暂停/继续/重置/记圈，毫秒精度，圈数列表 |
| 世界时钟 | 8个常用时区一键切换（北京/伦敦/纽约/洛杉矶/东京/巴黎/悉尼/迪拜） |

## 技术栈

- **语言**: C++17
- **框架**: Qt 5 (Qt Widgets)
- **构建**: CMake 3.16+
- **编译器**: MinGW / MSVC
- **IDE**: Visual Studio Code

## 项目结构

```
qt-clock/
├── CMakeLists.txt        # CMake 构建配置
├── main.cpp              # 程序入口
├── clockwidget.h         # 时钟绘制控件头文件
├── clockwidget.cpp       # 时钟绘制实现（QPainter 表盘+指针）
├── mainwindow.h          # 主窗口头文件
├── mainwindow.cpp        # 主窗口实现（菜单/工具栏/闹钟/秒表/世界时钟）
├── README.md             # 项目说明
└── .gitignore
```

## 环境准备

### 1. 安装 Qt 5

下载 [Qt 在线安装器](https://www.qt.io/download-open-source)，安装时勾选：
- Qt 5.15.x（MinGW 版本）
- MinGW 编译器（或使用已安装的 MSVC）

### 2. 安装 VSCode 插件

在 VSCode 中安装：
- **C/C++** (ms-vscode.cpptools)
- **CMake Tools** (ms-vscode.cmake-tools)

### 3. 配置环境变量

将 Qt 和编译器路径加入系统 PATH，例如：
```
C:\Qt\5.15.2\mingw81_64\bin
C:\Qt\Tools\mingw810_64\bin
```

## 编译运行

### 方式一：VSCode + CMake Tools（推荐）

1. 用 VSCode 打开 `qt-clock` 文件夹
2. 按 `Ctrl+Shift+P`，选择 `CMake: Configure`
3. 选择 MinGW 或 MSVC 编译器
4. 按 `F7` 构建，`Shift+F5` 运行

### 方式二：命令行

```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/mingw81_64"
cmake --build .
./qt-clock.exe
```

## 核心实现说明

### 时钟绘制（ClockWidget）

继承 `QWidget`，重写 `paintEvent`，使用 `QPainter` 绘制：

- **表盘**: `QRadialGradient` 径向渐变填充，双圈装饰
- **刻度**: 60格，每5格为大刻度（粗线），其余小刻度（细线）
- **指针角度计算**:
  - 时针: `30° × (hour % 12) + 0.5° × minute`
  - 分针: `6° × minute + 0.1° × second`
  - 秒针: `6° × second + 6° × (ms/1000)` — 毫秒插值实现丝滑动画
- **刷新**: `QTimer` 每 50ms 触发 `update()`，秒针流畅无卡顿

### 主题系统

通过 `setStyleSheet` 动态切换全局 QSS，同时 `ClockWidget` 内部根据主题切换绘制颜色方案。

### 世界时钟

利用 `QDateTime::currentDateTimeUtc()` 获取 UTC 时间，加上时区偏移秒数得到当地时间，`QComboBox` 选择时区后即时刷新。

## 运行截图

> 运行程序后可在此处添加截图

## 开发者

崔子昂 - 中国地质大学（北京）人工智能学院

## 许可证

MIT License
