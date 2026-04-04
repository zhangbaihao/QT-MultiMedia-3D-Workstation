import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import QtQuick3D
import QtQuick3D.AssetUtils

Page {
    objectName: "model3d"
    background: Rectangle { color: "#f8f9fa" }

    property string currentFile: ""
    onCurrentFileChanged: {
        console.log("QML: currentFile 变更为:", currentFile)
        console.log("QML: currentFile 长度:", currentFile.length)
    }
    property color modelColor: "#0078d4"
    property real zoom: 1.0
    property bool autoRotate: false

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
                                    camera.position = Qt.vector3d(0, 0, 10)
                                    camera.rotation = Qt.vector3d(0, 0, 0)
                                    zoom = 1.0
                                    zoomSlider.value = 1.0
                                    autoRotate = false
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
                            Label { text: "-"; font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }

                            Label { text: "面数:"; font.pixelSize: 12; color: "#666666" }
                            Label { text: "-"; font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }

                            Label { text: "格式:"; font.pixelSize: 12; color: "#666666" }
                            Label { text: currentFile ? (currentFile.toUpperCase().endsWith(".STL") ? "STL" : currentFile.toUpperCase().endsWith(".PLY") ? "PLY" : "Unknown") : "-"; font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }

                            Label { text: "状态:"; font.pixelSize: 12; color: "#666666" }
                            Label { text: currentFile ? "已加载" : "未选择"; font.pixelSize: 12; color: currentFile ? "#107c10" : "#666666"; font.weight: Font.Medium }
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
                anchors.fill: parent
                camera: camera

                PerspectiveCamera {
                    id: camera
                    position: Qt.vector3d(0, 0, 10)
                    fieldOfView: 45
                }

                DirectionalLight {
                    id: light
                    position: Qt.vector3d(5, 5, 5)
                }

                // 3D模型
                Model {
                    id: model
                    source: currentFile
                    materials: [material]
                    scale: Qt.vector3d(zoom, zoom, zoom)
                    visible: currentFile !== ""
                }

                DefaultMaterial {
                    id: material
                    diffuseColor: modelColor
                }
            }

            // 鼠标交互
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                property real lastX: 0
                property real lastY: 0

                onPressed: function(mouse) {
                    lastX = mouse.x
                    lastY = mouse.y
                }

                onPositionChanged: function(mouse) {
                    var dx = mouse.x - lastX
                    var dy = mouse.y - lastY
                    if (pressedButtons & Qt.LeftButton) {
                        // 旋转相机
                        camera.rotation.x -= dy * 0.5
                        camera.rotation.y += dx * 0.5
                    } else if (pressedButtons & Qt.RightButton) {
                        // 平移相机
                        camera.position.x -= dx * 0.01
                        camera.position.y += dy * 0.01
                    }
                    lastX = mouse.x
                    lastY = mouse.y
                }

                onWheel: function(wheel) {
                    zoom += wheel.angleDelta.y / 300
                    zoom = Math.max(0.1, Math.min(5.0, zoom))
                }
            }

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
            
            // 确保获取正确的文件URL格式
            var fileUrl = ""
            if (fileDialog.file && fileDialog.file.toString) {
                fileUrl = fileDialog.file.toString()
                console.log("QML: 获取文件URL:", fileUrl)
            } else if (fileDialog.files && fileDialog.files.length > 0) {
                // 尝试使用files数组
                fileUrl = fileDialog.files[0].toString()
                console.log("QML: 从files数组获取文件URL:", fileUrl)
            }
            
            console.log("QML: 最终文件URL:", fileUrl)
            
            // 同时更新 currentFile 以便在界面上显示和加载模型
            currentFile = fileUrl
            console.log("QML: 更新 currentFile 为:", currentFile)
        }
        
        onRejected: {
            console.log("QML: 文件对话框被取消")
        }
    }
}