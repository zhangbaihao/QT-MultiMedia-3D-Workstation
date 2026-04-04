import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick3D.AssetUtils
import QtMultiMedia3D 1.0

Page {
    objectName: "model3d"
    background: Rectangle { color: "#f8f9fa" }

    property string currentFile: ""
    property int vertexCount: 0
    property int faceCount: 0
    property string modelStatus: "未选择"
    
    onCurrentFileChanged: {
        console.log("QML: currentFile 变更为:", currentFile)
        console.log("QML: currentFile 长度:", currentFile.length)
        if (currentFile) {
            modelStatus = "加载中"
        } else {
            modelStatus = "未选择"
            vertexCount = 0
            faceCount = 0
        }
    }
    
    property color modelColor: "#0078d4"
    property real zoom: 1.0
    property bool autoRotate: false

    // === 将相机声明移动到页面的顶层 ===
    PerspectiveCamera {
        id: mainCamera
        position: Qt.vector3d(0, 0, 20)
        fieldOfView: 45
        eulerRotation: Qt.vector3d(0, 0, 0)
        clipFar: 10000
        clipNear: 1
    }

    RowLayout {
        anchors.fill: parent
        spacing: 15
        anchors.margins: 10

        // 左侧控制面板
        Rectangle {
            Layout.preferredWidth: 320
            Layout.fillHeight: true
            color: "#ffffff"
            border.color: "#e0e0e0"
            border.width: 1
            radius: 16

            ScrollView {
                anchors.fill: parent
                contentWidth: parent.width

                ColumnLayout {
                    width: parent.width
                    spacing: 20
                    anchors.margins: 20

                    // 标题
                    Label {
                        text: "3D模型控制"
                        font.pixelSize: 20
                        font.weight: Font.Bold
                        color: "#333333"
                    }

                    // 文件选择
                    GroupBox {
                        Layout.fillWidth: true
                        title: "文件操作"
                        font.pixelSize: 14

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10

                            Button {
                                Layout.fillWidth: true
                                text: "📂 选择模型文件"
                                font.pixelSize: 14

                                background: Rectangle {
                                    color: parent.pressed ? "#005a9e" : "#0078d4"
                                    radius: 8
                                }

                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: {
                                    console.log("QML: 用户点击了选择模型文件按钮")
                                    fileDialog.open()
                                }
                            }

                            Label {
                                text: currentFile ? "当前文件: " + currentFile : "未选择文件"
                                font.pixelSize: 12
                                color: "#666666"
                                wrapMode: Text.WrapAnywhere
                                Layout.fillWidth: true
                            }
                        }
                    }

                    // 颜色控制
                    GroupBox {
                        Layout.fillWidth: true
                        title: "颜色控制"
                        font.pixelSize: 14

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 15

                            // 预设颜色
                            Flow {
                                Layout.fillWidth: true
                                spacing: 8

                                Repeater {
                                    model: ["#0078d4", "#e81123", "#107c10", "#ff8c00", "#5c2d91", "#00b294", "#ffffff", "#333333"]

                                    Rectangle {
                                        width: 32
                                        height: 32
                                        color: modelData
                                        radius: 8
                                        border.color: modelColor === modelData ? "#333333" : "transparent"
                                        border.width: 2
                                        Behavior on border.color {
                                            ColorAnimation { duration: 200 }
                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            onEntered: {
                                                parent.scale = 1.1
                                            }
                                            onExited: {
                                                parent.scale = 1.0
                                            }
                                            onClicked: modelColor = modelData
                                        }
                                    }
                                }
                            }

                            // 自定义颜色
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 10

                                Label {
                                    text: "自定义:"
                                    font.pixelSize: 12
                                    color: "#666666"
                                }

                                Rectangle {
                                    Layout.preferredWidth: 40
                                    Layout.preferredHeight: 30
                                    color: modelColor
                                    radius: 6
                                    border.color: "#cccccc"
                                    border.width: 1
                                }
                            }
                        }
                    }

                    // 视图控制
                    GroupBox {
                        Layout.fillWidth: true
                        title: "视图控制"
                        font.pixelSize: 14

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 15

                            // 自动旋转
                            CheckBox {
                                text: "自动旋转"
                                checked: autoRotate
                                onCheckedChanged: autoRotate = checked
                            }

                            // 缩放控制
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 10

                                Label {
                                    text: "缩放:"
                                    font.pixelSize: 12
                                    color: "#666666"
                                    Layout.preferredWidth: 40
                                }

                                Slider {
                                    id: zoomSlider
                                    Layout.fillWidth: true
                                    from: 0.1
                                    to: 5.0
                                    value: zoom
                                    onValueChanged: zoom = value
                                }

                                Label {
                                    text: (zoom * 100).toFixed(0) + "%"
                                    font.pixelSize: 12
                                    color: "#666666"
                                    Layout.preferredWidth: 45
                                }
                            }

                            // 重置视图按钮
                            Button {
                                Layout.fillWidth: true
                                text: "重置视图"
                                font.pixelSize: 13
                                flat: true

                                onClicked: {
                                    // 修正：这里应该使用 mainCamera
                                    mainCamera.position = Qt.vector3d(0, 0, 20)  // 改为 20
                                    mainCamera.eulerRotation = Qt.vector3d(0, 0, 0)
                                    zoom = 1.0
                                    zoomSlider.value = 1.0
                                    autoRotate = false
                                    console.log("重置视图完成")
                                }
                            }
                        }
                    }

                    // 模型信息
                    GroupBox {
                        Layout.fillWidth: true
                        title: "模型信息"
                        font.pixelSize: 14

                        GridLayout {
                            anchors.fill: parent
                            columns: 2
                            rowSpacing: 8
                            columnSpacing: 15

                            Label { text: "顶点数:"; font.pixelSize: 12; color: "#666666" }
                            Label { text: vertexCount.toString(); font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }

                            Label { text: "面数:"; font.pixelSize: 12; color: "#666666" }
                            Label { text: faceCount.toString(); font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }

                            Label { text: "格式:"; font.pixelSize: 12; color: "#666666" }
                            Label { text: currentFile ? (currentFile.toUpperCase().endsWith(".STL") ? "STL" : currentFile.toUpperCase().endsWith(".PLY") ? "PLY" : "Unknown") : "-"; font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }

                            Label { text: "状态:"; font.pixelSize: 12; color: "#666666" }
                            Label { 
                                text: modelStatus 
                                font.pixelSize: 12 
                                color: modelStatus === "已加载" ? "#107c10" : modelStatus === "加载中" ? "#ff8c00" : modelStatus === "加载失败" ? "#e81123" : "#666666" 
                                font.weight: Font.Medium 
                            }
                        }
                    }

                    // 渲染选项
                    GroupBox {
                        Layout.fillWidth: true
                        title: "渲染选项"
                        font.pixelSize: 14

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10

                            CheckBox { text: "显示网格"; checked: true }
                            CheckBox { text: "显示法线"; checked: false }
                            CheckBox { text: "线框模式"; checked: false }
                            CheckBox { text: "双面渲染"; checked: true }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }
        }

        // 右侧3D视图
        Rectangle {
            id: viewport3D
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e1e1e"
            radius: 16

            View3D {
                id: view3D
                anchors.fill: parent
                camera: mainCamera  // 引用顶层声明的相机

                // 设置场景环境
                environment: SceneEnvironment {
                    clearColor: "#1e1e1e"
                    backgroundMode: SceneEnvironment.Color
                }

                // 添加光源
                DirectionalLight {
                    id: light
                    position: Qt.vector3d(5, 5, 5)
                    eulerRotation.x: -30
                    brightness: 1.5
                }

                // 添加相机控制器
                OrbitCameraController {
                    id: orbitController
                    camera: mainCamera  // 引用同一个相机
                    origin: Qt.vector3d(0, 0, 0)
                    anchors.fill: parent
                }

                // 使用自定义的 ModelLoader 加载模型
                Model {
                    id: model
                    materials: [material]
                    scale: Qt.vector3d(zoom, zoom, zoom)
                    position: Qt.vector3d(0, 0, 0)
                    rotation: Qt.vector3d(0, 0, 0)
                    visible: modelLoader.loaded
                    
                    // 使用自定义的 ModelLoader 作为几何数据
                    geometry: ModelLoader {
                        id: modelLoader
                        // 不直接绑定到 currentFile，而是通过手动设置
                        source: ""
                        onLoadedChanged: {
                            if (modelLoader.loaded) {
                                modelStatus = "已加载"
                                console.log("QML: 模型加载成功:", modelLoader.vertexCount, "顶点,", modelLoader.faceCount, "面")
                                console.log("QML: 模型边界框:", modelLoader.boundingBoxMin, "到", modelLoader.boundingBoxMax)
                            } else {
                                modelStatus = "加载失败"
                                console.log("QML: 模型加载失败")
                            }
                        }
                        onSourceChanged: {
                            console.log("QML: ModelLoader source 变更为:", source)
                            modelStatus = "加载中"
                        }
                    }
                    
                    Component.onCompleted: {
                        console.log("QML: Model 组件完成初始化")
                        if (currentFile) {
                            modelStatus = "加载中"
                        }
                    }
                }

                DefaultMaterial {
                    id: material
                    diffuseColor: modelColor
                }
                
                // 组件完成时添加调试信息
                Component.onCompleted: {
                    console.log("View3D 组件完成初始化")
                    console.log("相机对象:", mainCamera)
                    console.log("相机位置:", mainCamera.position)
                }
            }

            // 鼠标交互由 OrbitCameraController 处理

            // 视图控制提示
            Rectangle {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 20
                width: 200
                height: 100
                color: "#000000"
                radius: 12
                opacity: 0.7

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 5

                    Label {
                        text: "操作说明"
                        font.pixelSize: 12
                        font.weight: Font.Bold
                        color: "#ffffff"
                    }

                    Label {
                        text: "🖱️ 左键拖动: 旋转"
                        font.pixelSize: 11
                        color: "#cccccc"
                    }

                    Label {
                        text: "🖱️ 右键拖动: 平移"
                        font.pixelSize: 11
                        color: "#cccccc"
                    }

                    Label {
                        text: "🖱️ 滚轮: 缩放"
                        font.pixelSize: 11
                        color: "#cccccc"
                    }
                }
            }

            // 坐标轴指示器
            Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 20
                width: 80
                height: 80
                color: "#000000"
                radius: 12
                opacity: 0.5

                Label {
                    anchors.centerIn: parent
                    text: "X Y Z"
                    font.pixelSize: 14
                    font.weight: Font.Bold
                    color: "#ffffff"
                }
            }
        }
    }

    // 文件对话框 - 使用 Qt.labs.platform 1.1
    FileDialog {
        id: fileDialog
        title: "选择3D模型文件"
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: ["3D模型文件 (*.stl *.ply *.obj)", "所有文件 (*)"]
        
        onAccepted: {
            console.log("QML: 文件对话框被接受")
            console.log("QML: fileDialog.file type:", typeof fileDialog.file)
            console.log("QML: fileDialog.file:", fileDialog.file)
            console.log("QML: fileDialog.files:", fileDialog.files)
            console.log("QML: fileDialog.files.length:", fileDialog.files ? fileDialog.files.length : 0)
            
            // 确保获取正确的文件URL格式
            var fileUrl = ""
            if (fileDialog.files && fileDialog.files.length > 0) {
                // 优先使用files数组
                fileUrl = fileDialog.files[0].toString()
                console.log("QML: 从files数组获取文件URL:", fileUrl)
            } else if (fileDialog.file && fileDialog.file.toString) {
                fileUrl = fileDialog.file.toString()
                console.log("QML: 获取文件URL:", fileUrl)
            }
            
            console.log("QML: 最终文件URL:", fileUrl)
            
            // 转换 file:// URL 为本地文件路径
            var localPath = fileUrl.toString().replace("file:///", "")
            console.log("QML: 转换为本地文件路径:", localPath)
            
            // 同时更新 currentFile 以便在界面上显示和加载模型
            currentFile = fileUrl // 保持原始 URL 用于显示
            console.log("QML: 更新 currentFile 为:", currentFile)
            
            // 手动触发模型加载 - 使用本地文件路径
            modelLoader.source = localPath
            console.log("QML: 手动设置 ModelLoader source 为:", modelLoader.source)
        }
        
        onRejected: {
            console.log("QML: 文件对话框被取消")
        }
    }
    
    // 页面完成时添加调试信息
    Component.onCompleted: {
        console.log("=== 页面初始化完成 ===")
        console.log("mainCamera:", mainCamera)
        console.log("相机位置:", mainCamera.position)
        console.log("相机旋转:", mainCamera.eulerRotation)
    }
}