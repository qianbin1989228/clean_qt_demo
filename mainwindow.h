#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QTextEdit>
#include <QFile>

#include "algorithm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //覆写closeEvent函数
    void closeEvent(QCloseEvent *);
    void InitWindows();//初始化子窗口
    void ChangeLayout();//自定义样式
    void ShowOrgImg();//显示原始照片
    void ShowImg();//显示制作后的照片

public:
    QImage m_img;//当前图像
    QImage m_imgorg;//原始图像
    QString m_saveFolder;

private slots:
    void OpenImg();//打开图像
    void Process();//制作照片
    void SaveImgs();//保存图像
    void ChangeSaveFolder();//修改保存目录

private:
    Ui::MainWindow *ui;
    //状态栏
    QLabel *m_pStatusImgSize,*m_pStatusCompany;

    //主窗口
    QDockWidget* dock_Image;// 图像窗口
    QDockWidget* dock_Output;// 输出结果窗口
    QLabel *imgLabel;// 原始照片显示
    QLabel *outLabel;// 制作结果显示
    QScrollArea* scrollArea;
    QScrollArea* scrollOutArea;

    //菜单栏
    QMenu *menu_capture;//选择照片
    QMenu *menu_detection;// 制作
    QMenu *menu_turn;// 参数调整
    QMenu *menu_file;// 保存图像

    //工具栏
    QComboBox*combo;

    //结果输出列表
    QTableWidget *driveResTable;
};
#endif // MAINWINDOW_H
