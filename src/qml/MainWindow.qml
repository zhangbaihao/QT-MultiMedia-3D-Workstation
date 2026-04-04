import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick.Dialogs

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    title: "QT-MultiMedia 3D Workstation"
    visible: true
    color: "#f8f9fa"
    
    // 直接在MainWindow.qml中定义ModelViewerOpenGLPage组件
    Component {
        id: modelViewerOpenGLPage
        
        Page {
            objectName: "model3d"
            background: Rectangle { color: "#f8f9fa" }

            property string currentFile: ""
            property int vertexCount: 0
            property int faceCount: 0
            property string modelStatus: "未选择"
            
            onCurrentFileChanged: {
                console.log("QML: currentFile 变更为:", currentFile)
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
                                text: "3D模型控制 (OpenGL)"
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

                            Item { Layout.fillHeight: true }
                        }
                    }
                }

                // 右侧3D视图 要使用     qmlRegisterType<GLWidget>("CustomComponents", 1, 0, "GLWidget");注册一个widgetOpengl 然后import CustomComponents 使用
                Rectangle {
                    id: viewport3D
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#1e1e1e"
                    radius: 16

                    // 暂时使用简单的Rectangle代替View3D
                    Rectangle {
                        anchors.fill: parent
                        color: modelColor
                        opacity: 0.5
                        
                        Label {
                            text: "OpenGL 3D View"
                            font.pixelSize: 20
                            color: "#ffffff"
                            anchors.centerIn: parent
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
                }
            }

            // 文件对话框
            FileDialog {
                id: fileDialog
                title: "选择3D模型文件"
                nameFilters: ["3D模型文件 (*.stl *.ply *.obj)", "所有文件 (*)"]
                
                onAccepted: {
                    console.log("QML: 文件对话框被接受")
                    console.log("QML: fileDialog.fileUrl:", fileDialog.fileUrl)
                    currentFile = fileDialog.fileUrl
                }
                
                onRejected: {
                    console.log("QML: 文件对话框被取消")
                }
            }
        }
    }

    // 顶部导航栏
    header: Rectangle {
        id: headerBackground
        height: 80
        color: "#f8f9fa"

        Rectangle {
            anchors {
                fill: parent
                topMargin: 10
                leftMargin: 10
                rightMargin: 10
                bottomMargin: 10
            }
            color: "#ffffff"
            border.color: "#e0e0e0"
            border.width: 1
            radius: 10

            TabBar {
                id: tabBar
                anchors.fill: parent
                background: Rectangle {
                    color: "transparent"
                }

                TabButton {
                    text: "首页"
                    onClicked: {
                        stackLayout.currentIndex = 0
                        tabBar.currentIndex = 0
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 0 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 0 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
                TabButton {
                    text: "3D模型 (Qt Quick 3D)"
                    onClicked: {
                        stackLayout.currentIndex = 1
                        tabBar.currentIndex = 1
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 1 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 1 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
                TabButton {
                    text: "3D模型 (OpenGL)"
                    onClicked: {
                        stackLayout.currentIndex = 2
                        tabBar.currentIndex = 2
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 2 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 2 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
                TabButton {
                    text: "视频播放"
                    onClicked: {
                        stackLayout.currentIndex = 3
                        tabBar.currentIndex = 3
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 3 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 3 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
                TabButton {
                    text: "控件展示"
                    onClicked: {
                        stackLayout.currentIndex = 4
                        tabBar.currentIndex = 4
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 4 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 4 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
            }
        }
    }

    // 主内容区域
    StackLayout {
        id: stackLayout
        anchors {
            fill: parent
            margins: 10
        }
        currentIndex: 0
        clip: true

        // 首页
        HomePage {
            clip: true
            onNavigateToPage: function(pageIndex) {
                stackLayout.currentIndex = pageIndex
                tabBar.currentIndex = pageIndex
            }
        }

        // 3D模型页面
        ModelViewerPage {
            clip: true
        }
        // 3D模型页面 (OpenGL)
        Loader {
            sourceComponent: modelViewerOpenGLPage
            clip: true
        }
        // 视频播放页面
        VideoPlayerPage {
            clip: true
        }
        // 控件展示页面
        ControlsPage {
            clip: true
        }
    }

    function showToast(message) {
        console.log("Toast:", message)
    }
}
