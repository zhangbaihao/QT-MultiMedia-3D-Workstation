import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Page {
    objectName: "controls"
    background: Rectangle { color: isDarkMode ? "#1a1a1a" : "#f5f7fa" }

    // 示例数据
    property var listModelData: [
        { name: "Item 1", value: 85, status: "Active" },
        { name: "Item 2", value: 62, status: "Pending" },
        { name: "Item 3", value: 94, status: "Completed" },
        { name: "Item 4", value: 45, status: "Active" },
        { name: "Item 5", value: 78, status: "Pending" }
    ]

    // 主题模式
    property bool isDarkMode: false

    // 主题颜色
    property color primaryColor: "#3498db"
    property color secondaryColor: "#2ecc71"
    property color accentColor: "#e74c3c"
    property color backgroundColor: isDarkMode ? "#1a1a1a" : "#f5f7fa"
    property color cardColor: isDarkMode ? "#2d2d2d" : "#ffffff"
    property color textColor: isDarkMode ? "#ffffff" : "#333333"
    property color textSecondaryColor: isDarkMode ? "#b0b0b0" : "#666666"
    property color borderColor: isDarkMode ? "#404040" : "#e0e0e0"
    
    // 自定义控件状态
    property int selectedTimeRange: 1 // 0:日, 1:周, 2:月, 3:年
    property int selectedTab: 0 // 0:概览, 1:详情, 2:设置

    ScrollView {
        anchors.fill: parent
        contentWidth: parent.width

        ColumnLayout {
            width: parent.width
            spacing: 30
            anchors.margins: 20

            // 页面标题和主题切换
            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                spacing: 20

                Label {
                    text: "QML 控件展示"
                    font.pixelSize: 28
                    font.weight: Font.Bold
                    color: textColor
                    padding: 10
                }

                Rectangle {
                    width: 50
                    height: 50
                    color: cardColor
                    radius: 25
                    border.color: borderColor
                    border.width: 1
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            isDarkMode = !isDarkMode
                            showToast(isDarkMode ? "已切换到暗色模式" : "已切换到亮色模式")
                        }
                    }

                    Label {
                        anchors.centerIn: parent
                        text: isDarkMode ? "🌙" : "☀️"
                        font.pixelSize: 24
                    }
                }
            }

            // 第一行：基本控件
            GridLayout {
                columns: 2
                columnSpacing: 20
                rowSpacing: 20
                Layout.fillWidth: true

                // 按钮展示
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    color: cardColor
                    radius: 12
                    border.color: borderColor
                    border.width: 1
                    layer.enabled: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 15

                        Label {
                            text: "按钮控件"
                            font.pixelSize: 16
                            font.weight: Font.Medium
                            color: textColor
                        }

                        RowLayout {
                            spacing: 12

                            Button {
                                text: "默认按钮"
                                onClicked: showToast("默认按钮被点击")
                                contentItem: Text {
                                    text: parent.text
                                    color: textColor
                                }
                            }

                            Button {
                                text: "主要按钮"
                                onClicked: showToast("主要按钮被点击")
                                contentItem: Text {
                                    text: parent.text
                                    color: "#ffffff"
                                }
                                background: Rectangle {
                                    color: primaryColor
                                    radius: 4
                                }
                            }

                            Button {
                                text: "扁平按钮"
                                onClicked: showToast("扁平按钮被点击")
                                contentItem: Text {
                                    text: parent.text
                                    color: primaryColor
                                }
                            }
                        }

                        RowLayout {
                            spacing: 12

                            RoundButton {
                                text: "+"
                                onClicked: showToast("圆形按钮被点击")
                                contentItem: Text {
                                    text: parent.text
                                    color: "#ffffff"
                                }
                                background: Rectangle {
                                    color: primaryColor
                                    radius: parent.width / 2
                                }
                            }

                            ToolButton {
                                text: "🔧"
                                onClicked: showToast("工具按钮被点击")
                                contentItem: Text {
                                    text: parent.text
                                    color: textColor
                                }
                            }

                            Button {
                                text: checkableButton.checked ? "已选中" : "未选中"
                                checkable: true
                                id: checkableButton
                                onClicked: showToast("复选按钮被点击")
                                contentItem: Text {
                                    text: parent.text
                                    color: checkableButton.checked ? "#ffffff" : textColor
                                }
                                background: Rectangle {
                                    color: checkableButton.checked ? primaryColor : "transparent"
                                    border.color: primaryColor
                                    border.width: 1
                                    radius: 4
                                }
                            }
                        }
                    }
                }

                // 输入控件
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    color: cardColor
                    radius: 12
                    border.color: borderColor
                    border.width: 1
                    layer.enabled: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 15

                        Label {
                            text: "输入控件"
                            font.pixelSize: 16
                            font.weight: Font.Medium
                            color: textColor
                        }

                        TextField {
                            Layout.fillWidth: true
                            placeholderText: "请输入文本..."
                            color: textColor
                            placeholderTextColor: textSecondaryColor
                            background: Rectangle {
                                color: isDarkMode ? "#3a3a3a" : "#f8f9fa"
                                border.color: borderColor
                                border.width: 1
                                radius: 4
                            }
                        }

                        RowLayout {
                            spacing: 12

                            SpinBox {
                                from: 0
                                to: 100
                                value: 50
                                contentItem: Text {
                                    text: parent.text
                                    color: textColor
                                }
                                background: Rectangle {
                                    color: isDarkMode ? "#3a3a3a" : "#f8f9fa"
                                    border.color: borderColor
                                    border.width: 1
                                    radius: 4
                                }
                            }

                            ComboBox {
                                model: ["选项 1", "选项 2", "选项 3"]
                                contentItem: Text {
                                    text: parent.displayText
                                    color: textColor
                                }
                                background: Rectangle {
                                    color: isDarkMode ? "#3a3a3a" : "#f8f9fa"
                                    border.color: borderColor
                                    border.width: 1
                                    radius: 4
                                }
                            }
                        }

                        TextArea {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 60
                            placeholderText: "多行文本输入..."
                            color: textColor
                            placeholderTextColor: textSecondaryColor
                            background: Rectangle {
                                color: isDarkMode ? "#3a3a3a" : "#f8f9fa"
                                border.color: borderColor
                                border.width: 1
                                radius: 4
                            }
                        }
                    }
                }
            }

            // 第二行：选择控件和滑块
            GridLayout {
                columns: 2
                columnSpacing: 20
                rowSpacing: 20
                Layout.fillWidth: true

                // 选择控件
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    color: cardColor
                    radius: 12
                    border.color: borderColor
                    border.width: 1
                    layer.enabled: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 15

                        Label {
                            text: "选择控件"
                            font.pixelSize: 16
                            font.weight: Font.Medium
                            color: textColor
                        }

                        CheckBox {
                            text: "复选框 1"
                            checked: true
                            contentItem: Text {
                                text: parent.text
                                color: textColor
                                leftPadding: 24
                            }
                        }

                        CheckBox {
                            text: "复选框 2"
                            contentItem: Text {
                                text: parent.text
                                color: textColor
                                leftPadding: 24
                            }
                        }

                        RowLayout {
                            spacing: 20

                            RadioButton {
                                text: "选项 A"
                                checked: true
                                contentItem: Text {
                                    text: parent.text
                                    color: textColor
                                    leftPadding: 24
                                }
                            }

                            RadioButton {
                                text: "选项 B"
                                contentItem: Text {
                                    text: parent.text
                                    color: textColor
                                    leftPadding: 24
                                }
                            }

                            RadioButton {
                                text: "选项 C"
                                contentItem: Text {
                                    text: parent.text
                                    color: textColor
                                    leftPadding: 24
                                }
                            }
                        }

                        Switch {
                            text: "开关控制"
                            contentItem: Text {
                                text: parent.text
                                color: textColor
                                leftPadding: 60
                            }
                        }
                    }
                }

                // 滑块和进度条
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    color: cardColor
                    radius: 12
                    border.color: borderColor
                    border.width: 1
                    layer.enabled: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 15

                        Label {
                            text: "滑块和进度"
                            font.pixelSize: 16
                            font.weight: Font.Medium
                            color: textColor
                        }

                        Slider {
                            id: demoSlider
                            Layout.fillWidth: true
                            from: 0
                            to: 100
                            value: 50
                            background: Rectangle {
                                width: parent.width
                                height: 6
                                color: "#4d4d4d"
                                radius: 3
                            }
                            handle: Rectangle {
                                width: 16
                                height: 16
                                color: primaryColor
                                radius: 8
                            }
                        }

                        ProgressBar {
                            Layout.fillWidth: true
                            value: demoSlider.value / 100
                            background: Rectangle {
                                width: parent.width
                                height: 8
                                color: "#4d4d4d"
                                radius: 4
                            }
                            contentItem: Rectangle {
                                width: parent.width * parent.value
                                height: 8
                                color: primaryColor
                                radius: 4
                            }
                        }

                        ProgressBar {
                            Layout.fillWidth: true
                            value: demoSlider.value / 100
                            indeterminate: true
                            background: Rectangle {
                                width: parent.width
                                height: 8
                                color: "#4d4d4d"
                                radius: 4
                            }
                            contentItem: Rectangle {
                                width: parent.width * 0.3
                                height: 8
                                color: primaryColor
                                radius: 4
                                SequentialAnimation on x {
                                    loops: Animation.Infinite
                                    NumberAnimation { from: -parent.width * 0.3; to: parent.width; duration: 1000 }
                                }
                            }
                        }

                        Dial {
                            Layout.alignment: Qt.AlignHCenter
                            from: 0
                            to: 100
                            value: demoSlider.value
                            background: Rectangle {
                                color: "#4d4d4d"
                                radius: 40
                            }
                            handle: Rectangle {
                                color: primaryColor
                                radius: 8
                            }
                        }
                    }
                }
            }

            // 第三行：列表和表格
            GridLayout {
                columns: 2
                columnSpacing: 20
                rowSpacing: 20
                Layout.fillWidth: true

                // 列表视图
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 280
                    color: cardColor
                    radius: 12
                    border.color: borderColor
                    border.width: 1
                    layer.enabled: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 15

                        Label {
                            text: "列表视图"
                            font.pixelSize: 16
                            font.weight: Font.Medium
                            color: textColor
                        }

                        ListView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            model: listModelData

                            delegate: Rectangle {
                                width: parent.width
                                height: 56
                                color: index % 2 === 0 ? (isDarkMode ? "#3a3a3a" : "#f8f9fa") : (isDarkMode ? "#2d2d2d" : "#ffffff")
                                border.color: borderColor
                                border.width: 1
                                radius: 6
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: parent.color = isDarkMode ? "#4a4a4a" : "#f0f4f8"
                                    onExited: parent.color = index % 2 === 0 ? (isDarkMode ? "#3a3a3a" : "#f8f9fa") : (isDarkMode ? "#2d2d2d" : "#ffffff")
                                }

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 12

                                    Label {
                                        text: modelData.name
                                        font.pixelSize: 14
                                        font.weight: Font.Medium
                                        color: textColor
                                        Layout.preferredWidth: 80
                                    }

                                    ProgressBar {
                                        Layout.fillWidth: true
                                        value: modelData.value / 100
                                    }

                                    Label {
                                        text: modelData.value + "%"
                                        font.pixelSize: 12
                                        color: textSecondaryColor
                                        Layout.preferredWidth: 40
                                    }

                                    Rectangle {
                                        Layout.preferredWidth: 70
                                        Layout.preferredHeight: 24
                                        radius: 12
                                        color: modelData.status === "Active" ? "#e3f2fd" :
                                               modelData.status === "Completed" ? "#e8f5e9" : "#fff3e0"

                                        Label {
                                            anchors.centerIn: parent
                                            text: modelData.status
                                            font.pixelSize: 11
                                            color: modelData.status === "Active" ? primaryColor :
                                                   modelData.status === "Completed" ? secondaryColor : accentColor
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // 树形视图（简化版）
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 280
                    color: cardColor
                    radius: 12
                    border.color: borderColor
                    border.width: 1
                    layer.enabled: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 15

                        Label {
                            text: "树形结构"
                            font.pixelSize: 16
                            font.weight: Font.Medium
                            color: textColor
                        }

                        ListView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true

                            model: ListModel {
                                ListElement { name: "项目根目录"; expanded: true; level: 0 }
                                ListElement { name: "源代码"; expanded: false; level: 1 }
                                ListElement { name: "资源文件"; expanded: false; level: 1 }
                                ListElement { name: "文档"; expanded: false; level: 1 }
                                ListElement { name: "main.cpp"; expanded: false; level: 2 }
                                ListElement { name: "mainwindow.cpp"; expanded: false; level: 2 }
                                ListElement { name: "images"; expanded: false; level: 2 }
                                ListElement { name: "icons"; expanded: false; level: 2 }
                                ListElement { name: "README.md"; expanded: false; level: 2 }
                            }

                            delegate: Rectangle {
                                width: parent.width
                                height: 36
                                color: "transparent"
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: parent.color = isDarkMode ? "#4a4a4a" : "#f0f4f8"
                                    onExited: parent.color = "transparent"
                                    onClicked: {
                                        if (expanded !== undefined) {
                                            expanded = !expanded
                                        }
                                    }
                                }

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: level * 20 + 10
                                    spacing: 8

                                    Text {
                                        text: expanded !== undefined ? (expanded ? "▼" : "▶") : ""
                                        font.pixelSize: 10
                                        color: textSecondaryColor
                                        visible: expanded !== undefined
                                    }

                                    Text {
                                        text: expanded !== undefined ? "📁" : "📄"
                                        font.pixelSize: 14
                                    }

                                    Label {
                                        text: name
                                        font.pixelSize: 13
                                        color: textColor
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // 第四行：自定义控件
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 300
                color: cardColor
                radius: 12
                border.color: borderColor
                border.width: 1
                layer.enabled: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    Label {
                        text: "自定义控件"
                        font.pixelSize: 16
                        font.weight: Font.Medium
                        color: textColor
                    }

                    RowLayout {
                        spacing: 20
                        Layout.fillWidth: true

                        // 自定义卡片
                        Rectangle {
                            Layout.preferredWidth: 200
                            Layout.preferredHeight: 120
                            color: cardColor
                            radius: 12
                            border.color: borderColor
                            border.width: 1
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: parent.border.color = primaryColor
                                onExited: parent.border.color = borderColor
                            }

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 8

                                RowLayout {
                                    spacing: 10

                                    Rectangle {
                                        Layout.preferredWidth: 40
                                        Layout.preferredHeight: 40
                                        color: primaryColor + "20"
                                        radius: 20

                                        Label {
                                            anchors.centerIn: parent
                                            text: "📊"
                                            font.pixelSize: 20
                                        }
                                    }

                                    ColumnLayout {
                                        spacing: 2

                                        Label {
                                            text: "数据统计"
                                            font.pixelSize: 16
                                            font.weight: Font.Bold
                                            color: textColor
                                        }

                                        Label {
                                            text: "实时更新"
                                            font.pixelSize: 12
                                            color: textSecondaryColor
                                        }
                                    }
                                }

                                Label {
                                    text: "1,234 条记录"
                                    font.pixelSize: 24
                                    font.weight: Font.Bold
                                    color: primaryColor
                                }
                            }
                        }

                        // 自定义按钮组
                        RowLayout {
                            spacing: 0

                            Repeater {
                                model: ["日", "周", "月", "年"]

                                Rectangle {
                                    Layout.preferredWidth: 50
                                    Layout.preferredHeight: 36
                                    color: model.index === selectedTimeRange ? primaryColor : cardColor
                                    border.color: borderColor
                                    border.width: 1
                                    MouseArea {
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onEntered: {
                                            if (model.index !== selectedTimeRange) parent.color = primaryColor + "20"
                                        }
                                        onExited: {
                                            if (model.index !== selectedTimeRange) parent.color = cardColor
                                        }
                                        onClicked: {
                                            selectedTimeRange = model.index
                                            showToast("切换到" + modelData + "视图")
                                        }
                                    }

                                    Label {
                                        anchors.centerIn: parent
                                        text: modelData
                                        font.pixelSize: 14
                                        color: model.index === selectedTimeRange ? "#ffffff" : textColor
                                    }
                                }
                            }
                        }

                        // 标签页
                        RowLayout {
                            spacing: 0

                            Repeater {
                                model: ["概览", "详情", "设置"]

                                Rectangle {
                                    Layout.preferredWidth: 80
                                    Layout.preferredHeight: 36
                                    color: model.index === selectedTab ? cardColor : (isDarkMode ? "#3a3a3a" : "#f8f9fa")
                                    border.color: borderColor
                                    border.width: 1
                                    MouseArea {
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onEntered: {
                                            if (model.index !== selectedTab) parent.color = primaryColor + "10"
                                        }
                                        onExited: {
                                            if (model.index !== selectedTab) parent.color = isDarkMode ? "#3a3a3a" : "#f8f9fa"
                                        }
                                        onClicked: {
                                            selectedTab = model.index
                                            showToast("切换到" + modelData + "标签")
                                        }
                                    }

                                    Label {
                                        anchors.centerIn: parent
                                        text: modelData
                                        font.pixelSize: 13
                                        color: model.index === selectedTab ? primaryColor : textSecondaryColor
                                        font.weight: model.index === selectedTab ? Font.Medium : Font.Normal
                                    }

                                    Rectangle {
                                        visible: model.index === selectedTab
                                        anchors.bottom: parent.bottom
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        height: 3
                                        color: primaryColor
                                    }
                                }
                            }
                        }
                    }

                    // 动态创建的控件示例
                    Flow {
                        Layout.fillWidth: true
                        spacing: 12

                        Repeater {
                            model: 8

                            Rectangle {
                                width: 80
                                height: 80
                                color: Qt.rgba(Math.random(), Math.random(), Math.random(), 0.7)
                                radius: 12
                                layer.enabled: true
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: parent.scale = 1.05
                                    onExited: parent.scale = 1.0
                                    onClicked: showToast("点击了色块 " + (index + 1))
                                }

                                Label {
                                    anchors.centerIn: parent
                                    text: index + 1
                                    font.pixelSize: 20
                                    font.weight: Font.Bold
                                    color: "#ffffff"
                                }
                            }
                        }
                    }
                }
            }

            // 第五行：动画效果
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 180
                color: cardColor
                radius: 12
                border.color: borderColor
                border.width: 1
                layer.enabled: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Label {
                        text: "动画效果"
                        font.pixelSize: 16
                        font.weight: Font.Medium
                        color: textColor
                    }

                    RowLayout {
                        anchors.fill: parent
                        spacing: 20

                        // 旋转动画
                        Rectangle {
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: 100
                            color: primaryColor + "20"
                            radius: 12
                            border.color: primaryColor + "40"
                            border.width: 1

                            Rectangle {
                                width: 40
                                height: 40
                                color: primaryColor
                                radius: 8
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
                                anchors.bottomMargin: 8
                                text: "旋转"
                                font.pixelSize: 12
                                color: textSecondaryColor
                            }
                        }

                        // 缩放动画
                        Rectangle {
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: 100
                            color: accentColor + "20"
                            radius: 12
                            border.color: accentColor + "40"
                            border.width: 1

                            Rectangle {
                                width: 40
                                height: 40
                                color: accentColor
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
                                anchors.bottomMargin: 8
                                text: "脉冲"
                                font.pixelSize: 12
                                color: textSecondaryColor
                            }
                        }

                        // 位移动画
                        Rectangle {
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: 100
                            color: secondaryColor + "20"
                            radius: 12
                            border.color: secondaryColor + "40"
                            border.width: 1

                            Rectangle {
                                width: 30
                                height: 30
                                color: secondaryColor
                                radius: 6

                                SequentialAnimation on x {
                                    loops: Animation.Infinite
                                    NumberAnimation { to: 60; duration: 1000 }
                                    NumberAnimation { to: 10; duration: 1000 }
                                }

                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 10
                            }

                            Label {
                                anchors.bottom: parent.bottom
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.bottomMargin: 8
                                text: "移动"
                                font.pixelSize: 12
                                color: textSecondaryColor
                            }
                        }

                        // 透明度动画
                        Rectangle {
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: 100
                            color: "#f3e5f5"
                            radius: 12
                            border.color: "#9c27b0" + "40"
                            border.width: 1

                            Rectangle {
                                width: 50
                                height: 50
                                color: "#9c27b0"
                                radius: 8
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
                                anchors.bottomMargin: 8
                                text: "淡入淡出"
                                font.pixelSize: 12
                                color: textSecondaryColor
                            }
                        }

                        // 颜色动画
                        Rectangle {
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: 100
                            radius: 12
                            border.color: borderColor
                            border.width: 1

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
                                anchors.bottomMargin: 8
                                text: "变色"
                                font.pixelSize: 12
                                color: "#ffffff"
                                font.weight: Font.Medium
                            }
                        }
                    }
                }
            }

            Item { Layout.preferredHeight: 40 }
        }
    }

    // Toast提示
    Rectangle {
        id: toast
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 50
        width: toastText.width + 40
        height: 44
        color: "#333333"
        radius: 22
        opacity: 0
        visible: opacity > 0
        layer.enabled: true

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
