import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import Qt.labs.platform 1.1
import QtMultiMedia3D 1.0

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    title: "QT-MultiMedia 3D Workstation"
    visible: true
    color: "#f5f5f5"

    // 当前页面索引
    property int currentPageIndex: 0

    // 顶部导航栏
    header: ToolBar {
        height: 60
        background: Rectangle {
            color: "#ffffff"
            border.color: "#e0e0e0"
            border.width: 1
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            spacing: 10

            Label {
                text: "3D Workstation"
                font.pixelSize: 20
                font.weight: Font.Bold
                color: "#0078d4"
            }

            Item { Layout.fillWidth: true }

            Repeater {
                model: [
                    { text: "首页", icon: "🏠" },
                    { text: "3D模型", icon: "🎲" },
                    { text: "视频播放", icon: "🎬" },
                    { text: "控件展示", icon: "🎨" }
                ]

                Button {
                    text: modelData.icon + " " + modelData.text
                    flat: true
                    font.pixelSize: 14
                    Layout.preferredHeight: 40
                    
                    background: Rectangle {
                        color: currentPageIndex === model.index ? "#e3f2fd" : "transparent"
                        radius: 6
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        font: parent.font
                        color: currentPageIndex === model.index ? "#0078d4" : "#333333"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: currentPageIndex = model.index
                }
            }
        }
    }

    // 主内容区域
    StackLayout {
        anchors.fill: parent
        currentIndex: currentPageIndex

        // 首页
        Page {
            background: Rectangle { color: "#f5f5f5" }

            ScrollView {
                anchors.fill: parent
                contentWidth: parent.width

                ColumnLayout {
                    width: parent.width
                    spacing: 30
                    anchors.margins: 40

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 200
                        color: "#ffffff"
                        radius: 12
                        border.color: "#e0e0e0"
                        border.width: 1

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 15

                            Label {
                                text: "欢迎使用 QT-MultiMedia 3D Workstation"
                                font.pixelSize: 32
                                font.weight: Font.Bold
                                color: "#333333"
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Label {
                                text: "一个功能强大的多媒体3D工作站，支持模型查看、视频播放和QML控件展示"
                                font.pixelSize: 16
                                color: "#666666"
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 20

                        Repeater {
                            model: [
                                { title: "3D模型查看", desc: "支持STL/PLY格式，提供颜色控制、信息显示等功能", icon: "🎲", color: "#e3f2fd", page: 1 },
                                { title: "视频播放器", desc: "支持多种格式，提供进度条、快进快退等控制功能", icon: "🎬", color: "#fce4ec", page: 2 },
                                { title: "控件展示", desc: "展示各种QML控件用法，包括自定义控件", icon: "🎨", color: "#f3e5f5", page: 3 }
                            ]

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 220
                                color: "#ffffff"
                                radius: 12
                                border.color: "#e0e0e0"
                                border.width: 1

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 20
                                    spacing: 10

                                    Rectangle {
                                        Layout.preferredWidth: 60
                                        Layout.preferredHeight: 60
                                        color: modelData.color
                                        radius: 30
                                        Layout.alignment: Qt.AlignHCenter

                                        Label {
                                            anchors.centerIn: parent
                                            text: modelData.icon
                                            font.pixelSize: 28
                                        }
                                    }

                                    Label {
                                        text: modelData.title
                                        font.pixelSize: 20
                                        font.weight: Font.Bold
                                        color: "#333333"
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    Label {
                                        text: modelData.desc
                                        font.pixelSize: 14
                                        color: "#666666"
                                        wrapMode: Text.WordWrap
                                        Layout.fillWidth: true
                                        horizontalAlignment: Text.AlignHCenter
                                    }

                                    Item { Layout.fillHeight: true }

                                    Button {
                                        text: "进入"
                                        Layout.alignment: Qt.AlignHCenter
                                        Layout.preferredWidth: 120
                                        Layout.preferredHeight: 40

                                        background: Rectangle {
                                            color: parent.pressed ? "#005a9e" : "#0078d4"
                                            radius: 6
                                        }

                                        contentItem: Text {
                                            text: parent.text
                                            font.pixelSize: 14
                                            color: "#ffffff"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                        onClicked: currentPageIndex = modelData.page
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: currentPageIndex = modelData.page
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 150
                        color: "#ffffff"
                        radius: 12
                        border.color: "#e0e0e0"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 15

                            Label {
                                text: "系统信息"
                                font.pixelSize: 18
                                font.weight: Font.Bold
                                color: "#333333"
                            }

                            GridLayout {
                                columns: 3
                                columnSpacing: 40
                                rowSpacing: 10

                                Label { text: "Qt版本: 6.10.2"; color: "#666666"; font.pixelSize: 14 }
                                Label { text: "OpenGL: 支持"; color: "#666666"; font.pixelSize: 14 }
                                Label { text: "线程池: 已启用"; color: "#666666"; font.pixelSize: 14 }
                                Label { text: "缓存系统: 三级缓存"; color: "#666666"; font.pixelSize: 14 }
                                Label { text: "内存管理: 内存池"; color: "#666666"; font.pixelSize: 14 }
                                Label { text: "异步流水线: 已启用"; color: "#666666"; font.pixelSize: 14 }
                            }
                        }
                    }

                    Item { Layout.preferredHeight: 40 }
                }
            }
        }

        // 3D模型页面
        Page {
            background: Rectangle { color: "#f5f5f5" }

            // 页面属性
            property string currentFile: ""
            property color modelColor: "#0078d4"
            property real rotationX: 0
            property real rotationY: 0
            property real zoom: 1.0
            property bool autoRotate: false

            // 主布局
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

                            // 文件操作
                            GroupBox {
                                Layout.fillWidth: true
                                title: "文件操作"
                                font.pixelSize: 14

                                ColumnLayout {
                                    anchors.fill: parent
                                    spacing: 10

                                    // 选择文件按钮
                                    Button {
                                        Layout.fillWidth: true
                                        text: "📂 选择模型文件"
                                        font.pixelSize: 14

                                        onClicked: fileDialog.open()
                                    }

                                    // 文件状态
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
                                                border.color: parent.parent.parent.parent.modelColor === modelData ? "#333333" : "transparent"
                                                border.width: 2

                                                MouseArea {
                                                    anchors.fill: parent
                                                    onClicked: parent.parent.parent.parent.modelColor = modelData
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
                                            color: parent.parent.parent.modelColor
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
                                        checked: parent.parent.parent.autoRotate
                                        onCheckedChanged: parent.parent.parent.autoRotate = checked
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
                                            value: parent.parent.parent.zoom
                                            onValueChanged: parent.parent.parent.zoom = value
                                        }

                                        Label {
                                            text: (parent.parent.parent.zoom * 100).toFixed(0) + "%"
                                            font.pixelSize: 12
                                            color: "#666666"
                                            Layout.preferredWidth: 45
                                        }
                                    }

                                    // 重置视图
                                    Button {
                                        Layout.fillWidth: true
                                        text: "重置视图"
                                        font.pixelSize: 13
                                        flat: true

                                        onClicked: {
                                            parent.parent.parent.rotationX = 0
                                            parent.parent.parent.rotationY = 0
                                            parent.parent.parent.zoom = 1.0
                                            zoomSlider.value = 1.0
                                            parent.parent.parent.autoRotate = false
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
                        anchors.fill: parent
                        modelPath: currentFile
                        modelColor: modelColor
                        rotationX: rotationX
                        rotationY: rotationY
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

                    // 操作说明
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

            // 文件选择对话框
            FileDialog {
                id: fileDialog
                title: "选择3D模型文件"
                nameFilters: ["3D模型文件 (*.stl *.ply *.obj)", "所有文件 (*)"]
                modality: Qt.WindowModal
                onAccepted: {
                    currentFile = fileDialog.file.toString().replace("file:///", "")
                }
            }

            // 鼠标控制 - 只在3D视图区域
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                propagateComposedEvents: true

                property real lastX: 0
                property real lastY: 0

                onPressed: {
                    // 只有在3D视图区域才处理
                    if (mouse.x > 320) {
                        lastX = mouse.x
                        lastY = mouse.y
                    } else {
                        mouse.accepted = false
                    }
                }

                onPositionChanged: {
                    if (pressedButtons & Qt.LeftButton && mouse.x > 320) {
                        parent.rotationY += (mouse.x - lastX) * 0.5
                        parent.rotationX += (mouse.y - lastY) * 0.5
                        lastX = mouse.x
                        lastY = mouse.y
                    } else {
                        mouse.accepted = false
                    }
                }

                onWheel: {
                    if (mouse.x > 320) {
                        parent.zoom += wheel.angleDelta.y / 1200
                        parent.zoom = Math.max(0.1, Math.min(3.0, parent.zoom))
                    } else {
                        mouse.accepted = false
                    }
                }
            }
        }

        // 视频播放页面
        Page {
            background: Rectangle { color: "#1a1a1a" }

            property string currentFile: ""
            property bool isPlaying: false
            property real playbackRate: 1.0
            property bool isMuted: false
            property real volume: 0.8
            property real position: 0
            property real duration: 0

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#000000"

                    Rectangle {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.margins: 20
                        width: 200
                        height: 80
                        color: "#000000"
                        radius: 8
                        opacity: 0.7
                        visible: currentFile !== ""

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 5

                            Label {
                                text: "视频信息"
                                font.pixelSize: 12
                                font.weight: Font.Bold
                                color: "#ffffff"
                            }

                            Label {
                                text: "分辨率: 1920x1080"
                                font.pixelSize: 11
                                color: "#cccccc"
                            }

                            Label {
                                text: "时长: " + formatTime(duration)
                                font.pixelSize: 11
                                color: "#cccccc"
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent
                        text: currentFile === "" ? "请点击下方按钮选择视频文件" : "视频播放区域"
                        font.pixelSize: 16
                        color: "#ffffff"
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    color: "#2d2d2d"
                    border.color: "#3d3d3d"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            Label {
                                text: formatTime(position)
                                font.pixelSize: 12
                                color: "#ffffff"
                                Layout.preferredWidth: 60
                            }

                            Slider {
                                id: positionSlider
                                Layout.fillWidth: true
                                from: 0
                                to: duration
                                value: position
                                enabled: currentFile !== ""

                                onValueChanged: position = value

                                background: Rectangle {
                                    x: positionSlider.leftPadding
                                    y: positionSlider.topPadding + positionSlider.availableHeight / 2 - height / 2
                                    width: positionSlider.availableWidth
                                    height: 4
                                    radius: 2
                                    color: "#4d4d4d"

                                    Rectangle {
                                        width: positionSlider.visualPosition * parent.width
                                        height: parent.height
                                        color: "#0078d4"
                                        radius: 2
                                    }
                                }

                                handle: Rectangle {
                                    x: positionSlider.leftPadding + positionSlider.visualPosition * (positionSlider.availableWidth - width)
                                    y: positionSlider.topPadding + positionSlider.availableHeight / 2 - height / 2
                                    width: 16
                                    height: 16
                                    radius: 8
                                    color: positionSlider.pressed ? "#ffffff" : "#0078d4"
                                    border.color: "#ffffff"
                                    border.width: 2
                                }
                            }

                            Label {
                                text: formatTime(duration)
                                font.pixelSize: 12
                                color: "#ffffff"
                                Layout.preferredWidth: 60
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 15
                            Layout.alignment: Qt.AlignHCenter

                            Button {
                                text: "📂"
                                font.pixelSize: 18
                                flat: true

                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: videoFileDialog.open()
                            }

                            Button {
                                text: "⏪"
                                font.pixelSize: 20
                                flat: true
                                enabled: currentFile !== ""

                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: enabled ? "#ffffff" : "#666666"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: {
                                    position = Math.max(0, position - 10000)
                                }
                            }

                            Button {
                                text: isPlaying ? "⏸" : "▶"
                                font.pixelSize: 28
                                flat: true
                                enabled: currentFile !== ""

                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: enabled ? "#ffffff" : "#666666"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: {
                                    isPlaying = !isPlaying
                                }
                            }

                            Button {
                                text: "⏹"
                                font.pixelSize: 20
                                flat: true
                                enabled: currentFile !== ""

                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: enabled ? "#ffffff" : "#666666"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: {
                                    isPlaying = false
                                    position = 0
                                }
                            }

                            Button {
                                text: "⏩"
                                font.pixelSize: 20
                                flat: true
                                enabled: currentFile !== ""

                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: enabled ? "#ffffff" : "#666666"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: {
                                    position = Math.min(duration, position + 10000)
                                }
                            }

                            Rectangle {
                                width: 1
                                height: 30
                                color: "#4d4d4d"
                            }

                            ComboBox {
                                id: speedCombo
                                Layout.preferredWidth: 80
                                model: ["0.5x", "1.0x", "1.5x", "2.0x"]
                                currentIndex: 1
                                enabled: currentFile !== ""

                                contentItem: Text {
                                    text: speedCombo.displayText
                                    font.pixelSize: 12
                                    color: enabled ? "#ffffff" : "#666666"
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                background: Rectangle {
                                    color: "#3d3d3d"
                                    radius: 4
                                    border.color: "#4d4d4d"
                                    border.width: 1
                                }

                                onCurrentIndexChanged: {
                                    var rates = [0.5, 1.0, 1.5, 2.0]
                                    playbackRate = rates[currentIndex]
                                }
                            }

                            Rectangle {
                                width: 1
                                height: 30
                                color: "#4d4d4d"
                            }

                            Button {
                                text: isMuted ? "🔇" : "🔊"
                                font.pixelSize: 18
                                flat: true

                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: isMuted = !isMuted
                            }

                            Slider {
                                id: volumeSlider
                                Layout.preferredWidth: 100
                                from: 0
                                to: 1
                                value: volume

                                onValueChanged: volume = value

                                background: Rectangle {
                                    x: volumeSlider.leftPadding
                                    y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                                    width: volumeSlider.availableWidth
                                    height: 4
                                    radius: 2
                                    color: "#4d4d4d"

                                    Rectangle {
                                        width: volumeSlider.visualPosition * parent.width
                                        height: parent.height
                                        color: "#0078d4"
                                        radius: 2
                                    }
                                }

                                handle: Rectangle {
                                    x: volumeSlider.leftPadding + volumeSlider.visualPosition * (volumeSlider.availableWidth - width)
                                    y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                                    width: 12
                                    height: 12
                                    radius: 6
                                    color: "#ffffff"
                                }
                            }
                        }
                    }
                }
            }

            FileDialog {
                id: videoFileDialog
                title: "选择视频文件"
                nameFilters: ["视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv)", "所有文件 (*)"]
                onAccepted: {
                    currentFile = videoFileDialog.file.toString().replace("file:///", "")
                    duration = 300000
                    isPlaying = true
                }
            }

            function formatTime(ms) {
                if (isNaN(ms) || ms < 0) return "00:00"
                var seconds = Math.floor(ms / 1000)
                var minutes = Math.floor(seconds / 60)
                var hours = Math.floor(minutes / 60)
                seconds = seconds % 60
                minutes = minutes % 60

                if (hours > 0) {
                    return hours.toString().padStart(2, '0') + ":" + 
                           minutes.toString().padStart(2, '0') + ":" + 
                           seconds.toString().padStart(2, '0')
                } else {
                    return minutes.toString().padStart(2, '0') + ":" + 
                           seconds.toString().padStart(2, '0')
                }
            }
        }

        // 控件展示页面
        Page {
            background: Rectangle { color: "#f5f5f5" }

            property var listModelData: [
                { name: "Item 1", value: 85, status: "Active" },
                { name: "Item 2", value: 62, status: "Pending" },
                { name: "Item 3", value: 94, status: "Completed" },
                { name: "Item 4", value: 45, status: "Active" },
                { name: "Item 5", value: 78, status: "Pending" }
            ]

            ScrollView {
                anchors.fill: parent
                contentWidth: parent.width

                ColumnLayout {
                    width: parent.width
                    spacing: 25
                    anchors.margins: 30

                    Label {
                        text: "QML 控件展示"
                        font.pixelSize: 28
                        font.weight: Font.Bold
                        color: "#333333"
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 20

                        GroupBox {
                            Layout.fillWidth: true
                            title: "按钮控件"
                            font.pixelSize: 14

                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 10

                                RowLayout {
                                    spacing: 10

                                    Button {
                                        text: "默认按钮"
                                        onClicked: showToast("默认按钮被点击")
                                    }

                                    Button {
                                        text: "主要按钮"
                                        highlighted: true
                                        onClicked: showToast("主要按钮被点击")
                                    }

                                    Button {
                                        text: "扁平按钮"
                                        flat: true
                                        onClicked: showToast("扁平按钮被点击")
                                    }
                                }

                                RowLayout {
                                    spacing: 10

                                    RoundButton {
                                        text: "+"
                                        onClicked: showToast("圆形按钮被点击")
                                    }

                                    ToolButton {
                                        text: "🔧"
                                        onClicked: showToast("工具按钮被点击")
                                    }

                                    Button {
                                        text: checkableButton.checked ? "已选中" : "未选中"
                                        checkable: true
                                        id: checkableButton
                                    }
                                }
                            }
                        }

                        GroupBox {
                            Layout.fillWidth: true
                            title: "输入控件"
                            font.pixelSize: 14

                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 10

                                TextField {
                                    Layout.fillWidth: true
                                    placeholderText: "请输入文本..."
                                }

                                RowLayout {
                                    spacing: 10

                                    SpinBox {
                                        from: 0
                                        to: 100
                                        value: 50
                                    }

                                    ComboBox {
                                        model: ["选项 1", "选项 2", "选项 3"]
                                    }
                                }

                                TextArea {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 60
                                    placeholderText: "多行文本输入..."
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 20

                        GroupBox {
                            Layout.fillWidth: true
                            title: "选择控件"
                            font.pixelSize: 14

                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 10

                                CheckBox {
                                    text: "复选框 1"
                                    checked: true
                                }

                                CheckBox {
                                    text: "复选框 2"
                                }

                                RowLayout {
                                    spacing: 20

                                    RadioButton {
                                        text: "选项 A"
                                        checked: true
                                    }

                                    RadioButton {
                                        text: "选项 B"
                                    }

                                    RadioButton {
                                        text: "选项 C"
                                    }
                                }

                                Switch {
                                    text: "开关控制"
                                }
                            }
                        }

                        GroupBox {
                            Layout.fillWidth: true
                            title: "滑块和进度"
                            font.pixelSize: 14

                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 15

                                Slider {
                                    id: demoSlider
                                    Layout.fillWidth: true
                                    from: 0
                                    to: 100
                                    value: 50
                                }

                                ProgressBar {
                                    Layout.fillWidth: true
                                    value: demoSlider.value / 100
                                }

                                ProgressBar {
                                    Layout.fillWidth: true
                                    value: demoSlider.value / 100
                                    indeterminate: true
                                }

                                Dial {
                                    Layout.alignment: Qt.AlignHCenter
                                    from: 0
                                    to: 100
                                    value: demoSlider.value
                                }
                            }
                        }
                    }

                    GroupBox {
                        Layout.fillWidth: true
                        title: "动画效果"
                        font.pixelSize: 14

                        RowLayout {
                            anchors.fill: parent
                            spacing: 20

                            Rectangle {
                                Layout.preferredWidth: 100
                                Layout.preferredHeight: 100
                                color: "#e3f2fd"
                                radius: 8

                                Rectangle {
                                    width: 40
                                    height: 40
                                    color: "#0078d4"
                                    radius: 4
                                    anchors.centerIn: parent

                                    RotationAnimation on rotation {
                                        loops: Animation.Infinite
                                        from: 0
                                        to: 360
                                        duration: 2000
                                    }
                                }

                                Label {
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottomMargin: 5
                                    text: "旋转"
                                    font.pixelSize: 11
                                    color: "#666666"
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: 100
                                Layout.preferredHeight: 100
                                color: "#fce4ec"
                                radius: 8

                                Rectangle {
                                    width: 40
                                    height: 40
                                    color: "#e91e63"
                                    radius: 20
                                    anchors.centerIn: parent

                                    SequentialAnimation on scale {
                                        loops: Animation.Infinite
                                        NumberAnimation { to: 1.5; duration: 500 }
                                        NumberAnimation { to: 1.0; duration: 500 }
                                    }
                                }

                                Label {
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottomMargin: 5
                                    text: "脉冲"
                                    font.pixelSize: 11
                                    color: "#666666"
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: 100
                                Layout.preferredHeight: 100
                                color: "#f3e5f5"
                                radius: 8

                                Rectangle {
                                    width: 30
                                    height: 30
                                    color: "#9c27b0"
                                    radius: 4

                                    SequentialAnimation on x {
                                        loops: Animation.Infinite
                                        NumberAnimation { to: 60; duration: 1000 }
                                        NumberAnimation { to: 10; duration: 1000 }
                                    }

                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                Label {
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottomMargin: 5
                                    text: "移动"
                                    font.pixelSize: 11
                                    color: "#666666"
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: 100
                                Layout.preferredHeight: 100
                                color: "#e8f5e9"
                                radius: 8

                                Rectangle {
                                    width: 50
                                    height: 50
                                    color: "#4caf50"
                                    radius: 4
                                    anchors.centerIn: parent

                                    SequentialAnimation on opacity {
                                        loops: Animation.Infinite
                                        NumberAnimation { to: 0.2; duration: 1000 }
                                        NumberAnimation { to: 1.0; duration: 1000 }
                                    }
                                }

                                Label {
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottomMargin: 5
                                    text: "淡入淡出"
                                    font.pixelSize: 11
                                    color: "#666666"
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: 100
                                Layout.preferredHeight: 100
                                radius: 8

                                SequentialAnimation on color {
                                    loops: Animation.Infinite
                                    ColorAnimation { to: "#ff5722"; duration: 1000 }
                                    ColorAnimation { to: "#ffeb3b"; duration: 1000 }
                                    ColorAnimation { to: "#4caf50"; duration: 1000 }
                                    ColorAnimation { to: "#2196f3"; duration: 1000 }
                                }

                                Label {
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottomMargin: 5
                                    text: "变色"
                                    font.pixelSize: 11
                                    color: "#ffffff"
                                }
                            }
                        }
                    }

                    Item { Layout.preferredHeight: 30 }
                }
            }

            Rectangle {
                id: toast
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 50
                width: toastText.width + 40
                height: 40
                color: "#333333"
                radius: 20
                opacity: 0
                visible: opacity > 0

                Label {
                    id: toastText
                    anchors.centerIn: parent
                    text: ""
                    font.pixelSize: 14
                    color: "#ffffff"
                }

                Behavior on opacity {
                    NumberAnimation { duration: 300 }
                }
            }

            Timer {
                id: toastTimer
                interval: 2000
                onTriggered: toast.opacity = 0
            }

            function showToast(message) {
                toastText.text = message
                toast.opacity = 1
                toastTimer.restart()
            }
        }
    }
}
