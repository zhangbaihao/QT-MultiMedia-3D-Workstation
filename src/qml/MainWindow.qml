import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Rectangle {
    id: mainWindow
    color: "#f8f9fa"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // 左侧控制面板
        ColumnLayout {
            width: 240
            spacing: 16

            GroupBox {
                title: "3D Model"
                Layout.fillWidth: true
                background: Rectangle {
                    color: "#ffffff"
                    radius: 8
                    border.color: "#e0e0e0"
                    border.width: 1
                }

                ColumnLayout {
                    spacing: 10
                    // ColumnLayout不支持padding属性

                    TextField {
                        id: modelPath
                        placeholderText: "Model path"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: "#f5f5f5"
                            radius: 6
                            border.color: "#e0e0e0"
                            border.width: 1
                        }
                        font.pointSize: 14
                    }
                    Button {
                        text: "Load Model"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: "#0078d4"
                            radius: 6
                        }
                        font.pointSize: 14
                        onClicked: {
                            // TODO: Load model
                        }
                    }
                }
            }

            GroupBox {
                title: "Video"
                Layout.fillWidth: true
                background: Rectangle {
                    color: "#ffffff"
                    radius: 8
                    border.color: "#e0e0e0"
                    border.width: 1
                }

                ColumnLayout {
                    spacing: 10
                    // ColumnLayout不支持padding属性

                    TextField {
                        id: videoPath
                        placeholderText: "Video path"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: "#f5f5f5"
                            radius: 6
                            border.color: "#e0e0e0"
                            border.width: 1
                        }
                        font.pointSize: 14
                    }
                    Button {
                        text: "Load Video"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: "#0078d4"
                            radius: 6
                        }
                        font.pointSize: 14
                        onClicked: {
                            // TODO: Load video
                        }
                    }
                }
            }
        }

        // 右侧显示区域
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16

            // 3D View
            Rectangle {
                id: view3D
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#1e1e1e"
                radius: 8
                border.color: "#333333"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: "3D View"
                    color: "#ffffff"
                    font.pointSize: 18
                    font.weight: Font.Medium
                }
            }

            // Video View
            Rectangle {
                id: viewVideo
                Layout.fillWidth: true
                height: 200
                color: "#1e1e1e"
                radius: 8
                border.color: "#333333"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: "Video View"
                    color: "#ffffff"
                    font.pointSize: 16
                    font.weight: Font.Medium
                }
            }
        }
    }
}
