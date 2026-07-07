#include "floating_window.h"
#include "ui_floating_window.h"
#include <QMenu>

#ifdef Q_OS_WIN
#include <windows.h>
HWND f_hwnd;
#endif

#define PROGRESS_BAR_WIDTH          55
#define PROGRESS_BAR_HEIGHT         20
#define WINDOW_WIDTH                (PROGRESS_BAR_WIDTH*m_bar_show_count + m_bar_show_count-1)
#define WINDOW_HEIGHT               PROGRESS_BAR_HEIGHT

/**
  * @brief 构造函数
  * @param parent:父窗口
  * @retval 无
  * 	@arg
 */
FloatingWindow::FloatingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloatingWindow)
{
    ui->setupUi(this);

    //设置窗口标志 工具窗口 无标题栏 顶层显示
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //背景透明
    this->setAttribute(Qt::WA_TranslucentBackground);

    //默认不隐藏时显示的进度条个数为2
    m_bar_show_count = 2;
    //设置窗口大小
    setMinimumSize(1,1);
    resize(WINDOW_WIDTH,WINDOW_HEIGHT);

//windows平台设置
#ifdef Q_OS_WIN
    //获取窗口句柄
    f_hwnd = (HWND)this->winId();
    //置顶显示,不聚焦
    SetWindowPos(f_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif

    //进度条样式设置
    ui->B_time->setStyleSheet(
                "QProgressBar { "
                " border: 1px solid grey; "
                " text-align: center; "
                " font: 12pt 'MiSans'; "
                "} "
                "QProgressBar::chunk { "
                " background-color: lightgreen; "
                " width: 1px; "
                "}"
            );
    ui->B_time->setTextVisible(true);
    this->set_clock_range(0,1500);
    this->set_clock_value(0);

    ui->B_drink->setStyleSheet(
                "QProgressBar { "
                " border: 1px solid grey; "
                " text-align: center; "
                " font: 12pt 'MiSans'; "
                "} "
                "QProgressBar::chunk { "
                " background-color: lightblue; "
                " width: 1px; "
                "}"
            );
    ui->B_drink->setTextVisible(true);
    this->set_drink_range(0,1800);
    this->set_drink_value(0);

    //进度条右键点击槽连接
    ui->B_time->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->B_time, &QWidget::customContextMenuRequested, this,&FloatingWindow::slot_bar_clock_right_clicked);

}

/**
  * @brief 析构函数
  * @param 无
  * @retval 无
  * 	@arg
 */
FloatingWindow::~FloatingWindow()
{
    delete ui;
}

/**
  * @brief 设置秒数值
  * @param value 当前剩余秒数
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_clock_value(int value)
{
    ui->B_time->setValue(value);
    int minutes = value / 60;
    int seconds = value % 60;
    ui->B_time->setFormat(QString("%1:%2") .arg(minutes, 2, 10, QChar('0')) .arg(seconds, 2, 10, QChar('0')));
    refresh_windows();
}

/**
  * @brief 设置秒范围
  * @param min 通常为0
  * @param max 进度条最大时的秒值
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_clock_range(int min, int max)
{
    ui->B_time->setRange(min,max);
}

/**
  * @brief 时钟进度条显示状态切换
  * @param status 是否显示
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_bar_clock_show(bool status)
{
    if(true==status)
    {
        if(ui->B_time->isHidden())
        {
            m_bar_show_count++;
        }
    }
    else
    {
        if(!ui->B_time->isHidden())
        {
            m_bar_show_count--;
        }
    }
    ui->B_time->setHidden(!status);
    refresh_windows();
}

/**
  * @brief 设置饮水量
  * @param value 当前饮水量
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_drink_value(int value)
{
    ui->B_drink->setValue(value);
    ui->B_drink->setFormat(QString("%1").arg(value));
}

/**
  * @brief 设置饮水进度条范围
  * @param min 最小值
  * @param max 最大值
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_drink_range(int min, int max)
{
    ui->B_drink->setRange(min,max);
}

/**
  * @brief 饮水量进度条显示状态切换
  * @param status 是否显示
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_drink_clock_show(bool status)
{
    if(true==status)
    {
        if(ui->B_drink->isHidden())
        {
            m_bar_show_count++;
        }
    }
    else
    {
        if(!ui->B_drink->isHidden())
        {
            m_bar_show_count--;
        }
    }
    ui->B_drink->setHidden(!status);
    refresh_windows();
}

/**
  * @brief 时钟进度条右键点击槽
  * @param pos
  * @retval 无
  * 	@arg
 */
void FloatingWindow::slot_bar_clock_right_clicked(const QPoint &pos)
{
    QMenu menu;
    QAction *satrt_action = menu.addAction("开始");
    QAction *pause_action = menu.addAction("暂停");
    QAction *continue_action = menu.addAction("继续");
    QAction *quit_action = menu.addAction("退出");
    QAction *selected = menu.exec(ui->B_time->mapToGlobal(pos));
    if (selected == quit_action)
    {
        qApp->quit();
    }
    else if (selected == satrt_action)
    {
        emit sig_clock_start_loop();
    }
    else if (selected == pause_action)
    {
        emit sig_clock_pause_loop();
    }
    else if (selected == continue_action)
    {
        emit sig_clock_continue_loop();
    }
}

/**
  * @brief 饮水量进度条右键点击槽
  * @param pos
  * @retval 无
  * 	@arg
 */
void FloatingWindow::slot_bar_drink_right_clicked(const QPoint &pos)
{

}

/**
  * @brief 鼠标点击事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void FloatingWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

/**
  * @brief 鼠标移动事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void FloatingWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

/**
  * @brief 鼠标双击事件
  * @param event 事件
  * @retval 无
  *     @arg
 */
void FloatingWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit sig_double_clicked();  // 发射你定义的双击信号
        event->accept();
    }
}

/**
  * @brief 刷新窗口显示
  * @param 无
  * @retval 无
  *     @arg
 */
void FloatingWindow::refresh_windows()
{
#ifdef Q_OS_WIN
    if(!this->isHidden())
    {
        resize(WINDOW_WIDTH,WINDOW_HEIGHT);
        QRect rect = this->geometry();
        int x = rect.x();
        int y = rect.y();
        int w = rect.width();
        int h = rect.height();
        SetWindowPos(f_hwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
    }
#endif
}
