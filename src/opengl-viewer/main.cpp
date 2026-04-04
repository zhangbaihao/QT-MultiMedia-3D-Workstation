#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QFileDialog>
#include <QColorDialog>
#include "GLWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 主窗口
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("OpenGL 3D 模型查看器");
    mainWindow.resize(1280, 720);
    
    // 中心部件
    QWidget *centralWidget = new QWidget(&mainWindow);
    mainWindow.setCentralWidget(centralWidget);
    
    // 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    // 左侧控制面板
    QWidget *controlPanel = new QWidget();
    controlPanel->setFixedWidth(300);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    controlLayout->setContentsMargins(20, 20, 20, 20);
    controlLayout->setSpacing(20);
    
    // 标题
    QLabel *titleLabel = new QLabel("3D模型查看器");
    titleLabel->setFont(QFont("Arial", 20, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignCenter);
    controlLayout->addWidget(titleLabel);
    
    // 文件操作
    QGroupBox *fileGroup = new QGroupBox("文件操作");
    QVBoxLayout *fileLayout = new QVBoxLayout(fileGroup);
    QPushButton *loadButton = new QPushButton("选择模型文件");
    fileLayout->addWidget(loadButton);
    QLabel *fileLabel = new QLabel("未选择文件");
    fileLabel->setWordWrap(true);
    fileLayout->addWidget(fileLabel);
    controlLayout->addWidget(fileGroup);
    
    // 模型信息
    QGroupBox *infoGroup = new QGroupBox("模型信息");
    QGridLayout *infoLayout = new QGridLayout(infoGroup);
    infoLayout->setColumnStretch(0, 1);
    infoLayout->setColumnStretch(1, 2);
    QLabel *vertexLabel = new QLabel("顶点数:");
    QLabel *vertexValueLabel = new QLabel("0");
    QLabel *faceLabel = new QLabel("面数:");
    QLabel *faceValueLabel = new QLabel("0");
    QLabel *boundingBoxLabel = new QLabel("包围盒:");
    QLabel *boundingBoxValueLabel = new QLabel("");
    boundingBoxValueLabel->setWordWrap(true);
    infoLayout->addWidget(vertexLabel, 0, 0);
    infoLayout->addWidget(vertexValueLabel, 0, 1);
    infoLayout->addWidget(faceLabel, 1, 0);
    infoLayout->addWidget(faceValueLabel, 1, 1);
    infoLayout->addWidget(boundingBoxLabel, 2, 0);
    infoLayout->addWidget(boundingBoxValueLabel, 2, 1, 1, 2);
    controlLayout->addWidget(infoGroup);
    
    // 视图控制
    QGroupBox *viewGroup = new QGroupBox("视图控制");
    QVBoxLayout *viewLayout = new QVBoxLayout(viewGroup);
    QCheckBox *autoRotateCheckBox = new QCheckBox("自动旋转");
    viewLayout->addWidget(autoRotateCheckBox);
    QComboBox *renderModeComboBox = new QComboBox();
    renderModeComboBox->addItems({"实体", "线框", "点云", "实体+线框"});
    viewLayout->addWidget(renderModeComboBox);
    QPushButton *resetButton = new QPushButton("重置视图");
    viewLayout->addWidget(resetButton);
    QPushButton *fitButton = new QPushButton("适配模型");
    viewLayout->addWidget(fitButton);
    controlLayout->addWidget(viewGroup);
    
    // 材质控制
    QGroupBox *materialGroup = new QGroupBox("材质控制");
    QVBoxLayout *materialLayout = new QVBoxLayout(materialGroup);
    QHBoxLayout *colorLayout = new QHBoxLayout();
    QLabel *colorLabel = new QLabel("颜色:");
    QPushButton *colorButton = new QPushButton();
    colorButton->setFixedSize(40, 30);
    colorButton->setStyleSheet("background-color: #0078d4;");
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(colorButton);
    materialLayout->addLayout(colorLayout);
    QHBoxLayout *metalnessLayout = new QHBoxLayout();
    QLabel *metalnessLabel = new QLabel("金属度:");
    QSlider *metalnessSlider = new QSlider(Qt::Horizontal);
    metalnessSlider->setRange(0, 100);
    metalnessSlider->setValue(10);
    metalnessLayout->addWidget(metalnessLabel);
    metalnessLayout->addWidget(metalnessSlider);
    materialLayout->addLayout(metalnessLayout);
    QHBoxLayout *roughnessLayout = new QHBoxLayout();
    QLabel *roughnessLabel = new QLabel("粗糙度:");
    QSlider *roughnessSlider = new QSlider(Qt::Horizontal);
    roughnessSlider->setRange(0, 100);
    roughnessSlider->setValue(50);
    roughnessLayout->addWidget(roughnessLabel);
    roughnessLayout->addWidget(roughnessSlider);
    materialLayout->addLayout(roughnessLayout);
    controlLayout->addWidget(materialGroup);
    
    // 光照控制
    QGroupBox *lightGroup = new QGroupBox("光照控制");
    QVBoxLayout *lightLayout = new QVBoxLayout(lightGroup);
    QHBoxLayout *ambientLayout = new QHBoxLayout();
    QLabel *ambientLabel = new QLabel("环境光:");
    QSlider *ambientSlider = new QSlider(Qt::Horizontal);
    ambientSlider->setRange(0, 100);
    ambientSlider->setValue(50);
    ambientLayout->addWidget(ambientLabel);
    ambientLayout->addWidget(ambientSlider);
    lightLayout->addLayout(ambientLayout);
    QHBoxLayout *directionalLayout = new QHBoxLayout();
    QLabel *directionalLabel = new QLabel("平行光:");
    QSlider *directionalSlider = new QSlider(Qt::Horizontal);
    directionalSlider->setRange(0, 200);
    directionalSlider->setValue(100);
    directionalLayout->addWidget(directionalLabel);
    directionalLayout->addWidget(directionalSlider);
    lightLayout->addLayout(directionalLayout);
    controlLayout->addWidget(lightGroup);
    
    // 快捷键提示
    QGroupBox *shortcutGroup = new QGroupBox("快捷键");
    QVBoxLayout *shortcutLayout = new QVBoxLayout(shortcutGroup);
    QLabel *shortcut1Label = new QLabel("R: 重置视图");
    QLabel *shortcut2Label = new QLabel("F: 适配模型");
    QLabel *shortcut3Label = new QLabel("左键: 旋转模型");
    QLabel *shortcut4Label = new QLabel("右键: 平移模型");
    QLabel *shortcut5Label = new QLabel("滚轮: 缩放模型");
    shortcutLayout->addWidget(shortcut1Label);
    shortcutLayout->addWidget(shortcut2Label);
    shortcutLayout->addWidget(shortcut3Label);
    shortcutLayout->addWidget(shortcut4Label);
    shortcutLayout->addWidget(shortcut5Label);
    controlLayout->addWidget(shortcutGroup);
    
    // 占位符
    controlLayout->addStretch();
    
    // 右侧3D视图
    GLWidget *glWidget = new GLWidget();
    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(glWidget);
    
    // 信号连接
    QObject::connect(loadButton, &QPushButton::clicked, [&]() {
        QString filePath = QFileDialog::getOpenFileName(
            &mainWindow, 
            "选择3D模型文件", 
            "", 
            "3D模型文件 (*.stl *.ply *.obj *.gltf *.glb);;所有文件 (*)"
        );
        if (!filePath.isEmpty()) {
            fileLabel->setText("当前文件: " + filePath);
            glWidget->loadModel(filePath);
        }
    });
    
    QObject::connect(glWidget, &GLWidget::modelLoaded, [&](int vertexCount, int faceCount, const QString &boundingBox) {
        vertexValueLabel->setText(QString::number(vertexCount));
        faceValueLabel->setText(QString::number(faceCount));
        boundingBoxValueLabel->setText(boundingBox);
    });
    
    QObject::connect(autoRotateCheckBox, &QCheckBox::toggled, [&](bool checked) {
        glWidget->setAutoRotate(checked);
    });
    
    QObject::connect(renderModeComboBox, &QComboBox::currentIndexChanged, [&](int index) {
        glWidget->setRenderMode(index);
    });
    
    QObject::connect(resetButton, &QPushButton::clicked, [&]() {
        glWidget->resetView();
    });
    
    QObject::connect(fitButton, &QPushButton::clicked, [&]() {
        glWidget->fitModelToView();
    });
    
    QObject::connect(colorButton, &QPushButton::clicked, [&]() {
        QColor color = QColorDialog::getColor(QColor(0, 120, 212), &mainWindow, "选择模型颜色");
        if (color.isValid()) {
            colorButton->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        }
    });
    
    // 显示窗口
    mainWindow.show();
    
    return app.exec();
}