import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import QtMultiMedia3D 1.0

Page {
    objectName: "model3d"
    background: Rectangle { color: "#f5f5f5" }

    property string currentFile: ""
    property color modelColor: "#0078d4"
    property real zoom: 1.0
    property bool autoRotate: false

    Timer {
        running: autoRotate
        interval: 30
        repeat: true
        onTriggered: renderer3D.rotateByMouse(1, 0)
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // 左侧控制面板
        Rectangle {
            Layout.preferredWidth: 320
            Layout.fillHeight: true
            color: "#ffffff"
            border.color: "#e0e0e0"
            border.width: 1

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
                                    radius: 6
                                }

                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: fileDialog.open()
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
                                        radius: 4
                                        border.color: modelColor === modelData ? "#333333" : "transparent"
                                        border.width: 2

                                        MouseArea {
                                            anchors.fill: parent
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
                                    radius: 4
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
                                    to: 3.0
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
                                    renderer3D.resetView()
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
                            Label { text: "1,248"; font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }

                            Label { text: "面数:"; font.pixelSize: 12; color: "#666666" }
                            Label { text: "2,496"; font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }

                            Label { text: "格式:"; font.pixelSize: 12; color: "#666666" }
                            Label { text: "STL"; font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }

                            Label { text: "尺寸:"; font.pixelSize: 12; color: "#666666" }
                            Label { text: "100x80x60 mm"; font.pixelSize: 12; color: "#333333"; font.weight: Font.Medium }
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
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e1e1e"

            // 3D模型渲染
            Qml3DRenderer {
                id: renderer3D
                anchors.fill: parent
                modelPath: currentFile
                modelColor: modelColor
                zoom: zoom
            }

            // 网格背景
            Grid {
                anchors.fill: parent
                columns: 20
                rows: 20
                opacity: 0.1

                Repeater {
                    model: 400

                    Rectangle {
                        width: parent.width / 20
                        height: parent.height / 20
                        color: (index % 2 === 0) ? "#ffffff" : "transparent"
                    }
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
                radius: 8
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
                radius: 8
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

    FileDialog {
        id: fileDialog
        title: "选择3D模型文件"
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: ["3D模型文件 (*.stl *.ply *.obj)", "所有文件 (*)"]
        onAccepted: {
            var filePath = fileDialog.file.toString()
            if (Qt.platform.os === "windows") {
                currentFile = filePath.replace("file:///", "")
            } else {
                currentFile = filePath.replace("file://", "")
            }
        }
    }

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
                renderer3D.rotateByMouse(dx, dy)
            } else if (pressedButtons & Qt.RightButton) {
                renderer3D.panByMouse(dx, dy)
            }
            lastX = mouse.x
            lastY = mouse.y
        }

        onWheel: function(wheel) {
            zoom += wheel.angleDelta.y / 1200
            zoom = Math.max(0.1, Math.min(3.0, zoom))
            zoomSlider.value = zoom
        }
    }
}
