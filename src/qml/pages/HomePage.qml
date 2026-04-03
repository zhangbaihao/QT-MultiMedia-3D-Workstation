import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    objectName: "home"
    background: Rectangle { color: "#f5f5f5" }

    ScrollView {
        anchors.fill: parent
        contentWidth: parent.width

        ColumnLayout {
            width: parent.width
            spacing: 30
            anchors.margins: 40

            // 欢迎区域
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

            // 功能卡片区域
            RowLayout {
                Layout.fillWidth: true
                spacing: 20

                Repeater {
                    model: [
                        {
                            title: "3D模型查看",
                            desc: "支持STL/PLY格式，提供颜色控制、信息显示等功能",
                            icon: "🎲",
                            color: "#e3f2fd",
                            page: "model3d"
                        },
                        {
                            title: "视频播放器",
                            desc: "支持多种格式，提供进度条、快进快退等控制功能",
                            icon: "🎬",
                            color: "#fce4ec",
                            page: "video"
                        },
                        {
                            title: "控件展示",
                            desc: "展示各种QML控件用法，包括自定义控件",
                            icon: "🎨",
                            color: "#f3e5f5",
                            page: "controls"
                        }
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

                                onClicked: {
                                    switch(modelData.page) {
                                        case "model3d": stackView.replace("qrc:/QtMultiMedia3D/qml/pages/Model3DPage.qml"); break;
                                        case "video": stackView.replace("qrc:/QtMultiMedia3D/qml/pages/VideoPlayerPage.qml"); break;
                                        case "controls": stackView.replace("qrc:/QtMultiMedia3D/qml/pages/ControlsPage.qml"); break;
                                    }
                                }
                            }
                        }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        switch(modelData.page) {
                                            case "model3d": stackView.replace("qrc:/QtMultiMedia3D/qml/pages/Model3DPage.qml"); break;
                                            case "video": stackView.replace("qrc:/QtMultiMedia3D/qml/pages/VideoPlayerPage.qml"); break;
                                            case "controls": stackView.replace("qrc:/QtMultiMedia3D/qml/pages/ControlsPage.qml"); break;
                                        }
                                    }
                                }
                    }
                }
            }

            // 系统信息
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
