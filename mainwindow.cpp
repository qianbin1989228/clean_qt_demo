#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //删除中央窗体
    QWidget* p = takeCentralWidget();
    if (p)
        delete p;
    //设置标题
    this->setWindowTitle(QString::fromLocal8Bit("证件照制作小工具"));
    this->setWindowIcon(QIcon(":/logo.ico"));
    //允许嵌套dock
    setDockNestingEnabled(true);
    //初始化大小
    this->resize(QApplication::desktop()->width()*0.6,QApplication::desktop()->height()*0.7);
    //初始化子窗口
    InitWindows();
    //修改样式
    ChangeLayout();
    //当前文件保存目录（默认设置为桌面）
    m_saveFolder = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0];
}

/*************************************************
// 修改全局样式
*************************************************/
void MainWindow::ChangeLayout()
{
    QFile file(":/style.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = tr(file.readAll());
    qApp->setStyleSheet(styleSheet);
    file.close();
}

/*************************************************
// 窗体关闭事件
*************************************************/
void MainWindow::closeEvent(QCloseEvent *e){
    //添加结束时操作
    e->accept();
}

MainWindow::~MainWindow()
{
    delete ui; 
}

void MainWindow::InitWindows()
{
    //菜单栏
    menu_capture = menuBar()->addMenu(QString::fromLocal8Bit("打开"));
    menu_detection= menuBar()->addMenu(QString::fromLocal8Bit("制作"));
    menu_file= menuBar()->addMenu(QString::fromLocal8Bit("保存"));

    QAction * act_open_img = new QAction(QIcon(":/icon_openfile.png"),QString::fromLocal8Bit("选择照片"),this);
    menu_capture->addAction(act_open_img);

    QAction * act_detection = new QAction(QIcon(":/icon_det.png"),QString::fromLocal8Bit("制作"),this);
    menu_detection->addAction(act_detection);

    QAction * act_save = new QAction(QIcon(":/icon_save.png"),QString::fromLocal8Bit("保存图像"),this);
    menu_file->addAction(act_save);

    //工具栏
    QToolBar *toolBar = new QToolBar();
    toolBar->setFloatable(false);       // 设置是否浮动
    toolBar->setMovable(false);         // 设置工具栏不允许移动
    toolBar->addAction(act_open_img);
    toolBar->addSeparator();
    toolBar->addAction(act_detection);
    toolBar->addSeparator();
    toolBar->addAction(act_save);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(toolBar);

    //建立 信号-槽 连接
    connect(act_open_img,SIGNAL(triggered()),this,SLOT(OpenImg()));
    connect(act_detection,SIGNAL(triggered()),this,SLOT(Process()));
    connect(act_save,SIGNAL(triggered()),this,SLOT(SaveImgs()));

    // 图像面板
    dock_Image = new QDockWidget(QString::fromLocal8Bit(""), this);
    dock_Image->setMinimumSize(300, 300);   // 设置最小宽高
    dock_Image->setFeatures(QDockWidget::NoDockWidgetFeatures);//设置不可移动并取消标题菜单

    QLabel *label1 = new QLabel(QString::fromLocal8Bit("原始照片"), dock_Image);
    label1->setStyleSheet("padding: 5px 0;;color: rgb(0,0,0); font-size: 9pt; font-weight: normal;background-color: qlineargradient(spread: pad, x1:1, y1:0, x2:1, y2:1, stop:0 rgb(255,255,255), stop:1 rgb(211,218,222));");
    dock_Image->setTitleBarWidget(label1);

    // 填充图像展示区域
    imgLabel = new QLabel(dock_Image);
    imgLabel->setScaledContents(true);  // 设置QLabel自动适应图像大小

    //图像面板滚动条
    scrollArea = new QScrollArea(this);// 增加滚动条,如果图像比imgLabel大，就会出现滚动条
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setWidget(imgLabel);
    dock_Image->setWidget(scrollArea);

    //检测结果面板
    dock_Output = new QDockWidget(QString::fromLocal8Bit(""), this);          // 输出窗口
    dock_Output->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock_Output->setFixedWidth(QApplication::desktop()->width()*0.2);

    QLabel *label2 = new QLabel(QString::fromLocal8Bit("制作效果"), dock_Image);
    label2->setStyleSheet("padding: 5px 0;;color: rgb(0,0,0); font-size: 9pt; font-weight: normal;background-color: qlineargradient(spread: pad, x1:1, y1:0, x2:1, y2:1, stop:0 rgb(255,255,255), stop:1 rgb(211,218,222));");
    dock_Output->setTitleBarWidget(label2);

    // 进行布局
    setCentralWidget(dock_Image);       // 指定为中心窗口
    addDockWidget(Qt::RightDockWidgetArea, dock_Output);

    // 制作结果展示区域
    outLabel = new QLabel(dock_Output);
    outLabel->setScaledContents(true);

    //输出结果面板滚动条
    scrollOutArea = new QScrollArea(this);// 增加滚动条,如果图像比imgLabel大，就会出现滚动条
    scrollOutArea->setBackgroundRole(QPalette::Midlight);
    scrollOutArea->setAlignment(Qt::AlignCenter);
    scrollOutArea->setWidget(outLabel);
    dock_Output->setWidget(scrollOutArea);

    //状态栏
    m_pStatusImgSize=new QLabel;
    m_pStatusCompany=new QLabel;
    ui->statusbar->addWidget(m_pStatusImgSize);
    ui->statusbar->addPermanentWidget(m_pStatusCompany);
    m_pStatusImgSize->setText(QString::fromLocal8Bit("《深度学习与图像处理PaddlePaddle》"));
    m_pStatusCompany->setText(QString::fromLocal8Bit("版本：V1.0"));
}

/*************************************************
显示原始照片
*************************************************/
void MainWindow::ShowOrgImg()
{
    QImage showimg = m_imgorg;
    double ImgRatio = showimg.width()*1.0 / showimg.height();     // 图像宽高比
    double WinRatio = 1.0 * (scrollArea->width() - 2) / (scrollArea->height() - 2); // 窗口宽高比
    if (ImgRatio > WinRatio)        // 图像宽高比大于图像宽高比
    {
        showimg = showimg.scaled((scrollArea->width() - 2), (scrollArea->width() - 2) / ImgRatio, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    else                            // 图像宽高比小于等于图像宽高比
    {
        showimg = showimg.scaled((scrollArea->height() - 2) * ImgRatio, (scrollArea->height() - 2), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    imgLabel->setPixmap(QPixmap::fromImage(showimg));
    imgLabel->resize(showimg.width(), showimg.height());
}

/*************************************************
显示制作后的照片
*************************************************/
void MainWindow::ShowImg()
{
    QImage showimg = m_img;
    double ImgRatio = showimg.width()*1.0 / showimg.height();     // 图像宽高比
    double WinRatio = 1.0 * (scrollOutArea->width() - 2) / (scrollOutArea->height() - 2); // 窗口宽高比
    if (ImgRatio > WinRatio)        // 图像宽高比大于图像宽高比
    {
        showimg = showimg.scaled((scrollOutArea->width() - 2), (scrollOutArea->width() - 2) / ImgRatio, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    else                            // 图像宽高比小于等于图像宽高比
    {
        showimg = showimg.scaled((scrollOutArea->height() - 2) * ImgRatio, (scrollOutArea->height() - 2), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    outLabel->setPixmap(QPixmap::fromImage(showimg));
    outLabel->resize(showimg.width(), showimg.height());
}

/*************************************************
打开图像
*************************************************/
void MainWindow::OpenImg()
{
    QString path = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("选择图像"),".",tr("Images(*.jpg *.png *.bmp)"));
    if(path.isEmpty())
        return;
    QImage img;
    if(!(img.load(path)))
    {
        QMessageBox::warning(this,QString::fromLocal8Bit("错误"),QString::fromLocal8Bit("图像打开失败"));
        return;
    }
    m_imgorg = img;
    m_img = QImage();//清除上一张制作好的照片内容
    ShowOrgImg();
}

/*************************************************
制作照片
*************************************************/
void MainWindow::Process()
{
    if(m_imgorg.isNull())
    {
        QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("请先导入原始照片"));
        return;
    }
    //调用智能算法完整证件照制作
    m_img = MakeIDPhoto(m_imgorg);
    ShowImg();
}

/*************************************************
保存图像
*************************************************/
void MainWindow::SaveImgs()
{
    if(m_img.isNull())
    {
        QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("请先制作标准证件照片"));
        return;
    }
    QString savefolder = m_saveFolder+"/Images";
    QFileInfo fileInfo(savefolder);
    if(!fileInfo.isDir())
        QDir().mkdir(savefolder);
    QDateTime time = QDateTime::currentDateTime();
    QString folderPath = savefolder+"/"+time.toString("yyyy_MM_dd");

    QDir dir(folderPath);
    if(!dir.exists())
        QDir().mkdir(folderPath);

    QString subname = time.toString("HH_mm_ss");
    if(!m_img.isNull())
        m_img.save(folderPath+"/"+subname+".jpg");

    QMessageBox::information(this,QString::fromLocal8Bit("保存图片成功"),QString::fromLocal8Bit("保存在：")+folderPath);
}

