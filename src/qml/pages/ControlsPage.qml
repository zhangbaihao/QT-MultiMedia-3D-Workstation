import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 2.15

Page {
    objectName: "controls"
    background: Rectangle { color: "#f5f5f5" }

    // 示例数据
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

            // 页面标题
            Label {
                text: "QML 控件展示"
                font.pixelSize: 28
                font.weight: Font.Bold
                color: "#333333"
            }

            // 第一行：基本控件
            RowLayout {
                Layout.fillWidth: true
                spacing: 20

                // 按钮展示
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

                // 输入控件
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

            // 第二行：选择控件和滑块
            RowLayout {
                Layout.fillWidth: true
                spacing: 20

                // 选择控件
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

                // 滑块和进度条
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

            // 第三行：列表和表格
            RowLayout {
                Layout.fillWidth: true
                spacing: 20

                // 列表视图
                GroupBox {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 250
                    title: "列表视图"
                    font.pixelSize: 14

                    ListView {
                        anchors.fill: parent
                        clip: true
                        model: listModelData

                        delegate: Rectangle {
                            width: parent.width
                            height: 50
                            color: index % 2 === 0 ? "#f8f9fa" : "#ffffff"
                            border.color: "#e0e0e0"
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 10

                                Label {
                                    text: modelData.name
                                    font.pixelSize: 14
                                    font.weight: Font.Medium
                                    Layout.preferredWidth: 80
                                }

                                ProgressBar {
                                    Layout.fillWidth: true
                                    value: modelData.value / 100
                                }

                                Label {
                                    text: modelData.value + "%"
                                    font.pixelSize: 12
                                    color: "#666666"
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
                                        color: modelData.status === "Active" ? "#0078d4" :
                                               modelData.status === "Completed" ? "#2e7d32" : "#ef6c00"
                                    }
                                }
                            }
                        }
                    }
                }

                // 树形视图（简化版）
                GroupBox {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 250
                    title: "树形结构"
                    font.pixelSize: 14

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 5

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
                                height: 30
                                color: "transparent"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: level * 20 + 10
                                    spacing: 5

                                    Text {
                                        text: expanded !== undefined ? (expanded ? "▼" : "▶") : ""
                                        font.pixelSize: 10
                                        color: "#666666"
                                        visible: expanded !== undefined
                                    }

                                    Text {
                                        text: expanded !== undefined ? "📁" : "📄"
                                        font.pixelSize: 14
                                    }

                                    Label {
                                        text: name
                                        font.pixelSize: 13
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if (expanded !== undefined) {
                                            expanded = !expanded
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // 第四行：自定义控件
            GroupBox {
                Layout.fillWidth: true
                title: "自定义控件"
                font.pixelSize: 14

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 15

                    RowLayout {
                        spacing: 20

                        // 自定义卡片
                        Rectangle {
                            Layout.preferredWidth: 200
                            Layout.preferredHeight: 120
                            color: "#ffffff"
                            radius: 12
                            border.color: "#e0e0e0"
                            border.width: 1

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 8

                                RowLayout {
                                    spacing: 10

                                    Rectangle {
                                        Layout.preferredWidth: 40
                                        Layout.preferredHeight: 40
                                        color: "#e3f2fd"
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
                                        }

                                        Label {
                                            text: "实时更新"
                                            font.pixelSize: 12
                                            color: "#666666"
                                        }
                                    }
                                }

                                Label {
                                    text: "1,234 条记录"
                                    font.pixelSize: 24
                                    font.weight: Font.Bold
                                    color: "#0078d4"
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
                                    color: model.index === 1 ? "#0078d4" : "#ffffff"
                                    border.color: "#e0e0e0"
                                    border.width: 1

                                    Label {
                                        anchors.centerIn: parent
                                        text: modelData
                                        font.pixelSize: 14
                                        color: model.index === 1 ? "#ffffff" : "#333333"
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: showToast("切换到" + modelData + "视图")
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
                                    color: model.index === 0 ? "#ffffff" : "#f5f5f5"
                                    border.color: "#e0e0e0"
                                    border.width: 1

                                    Label {
                                        anchors.centerIn: parent
                                        text: modelData
                                        font.pixelSize: 13
                                        color: model.index === 0 ? "#0078d4" : "#666666"
                                        font.weight: model.index === 0 ? Font.Medium : Font.Normal
                                    }

                                    Rectangle {
                                        visible: model.index === 0
                                        anchors.bottom: parent.bottom
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        height: 3
                                        color: "#0078d4"
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: showToast("切换到" + modelData + "标签")
                                    }
                                }
                            }
                        }
                    }

                    // 动态创建的控件示例
                    Flow {
                        Layout.fillWidth: true
                        spacing: 10

                        Repeater {
                            model: 8

                            Rectangle {
                                width: 80
                                height: 80
                                color: Qt.rgba(Math.random(), Math.random(), Math.random(), 0.7)
                                radius: 8

                                Label {
                                    anchors.centerIn: parent
                                    text: index + 1
                                    font.pixelSize: 20
                                    font.weight: Font.Bold
                                    color: "#ffffff"
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: showToast("点击了色块 " + (index + 1))
                                }
                            }
                        }
                    }
                }
            }

            // 第五行：动画效果
            GroupBox {
                Layout.fillWidth: true
                title: "动画效果"
                font.pixelSize: 14

                RowLayout {
                    anchors.fill: parent
                    spacing: 20

                    // 旋转动画
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

                    // 缩放动画
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

                    // 位移动画
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

                    // 透明度动画
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

                    // 颜色动画
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

    // Toast提示
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
