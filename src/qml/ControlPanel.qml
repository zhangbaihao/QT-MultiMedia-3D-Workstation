import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultiMedia3D 1.0

Page {
    objectName: "controls"
    background: Rectangle { color: "#f5f5f5" }

    // 示例数据
    property real lightIntensity: 1.0
    property real ambientLight: 0.5
    property real modelScale: 1.0
    property real rotationSpeed: 1.0
    property real filterStrength: 0.5

    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        anchors.margins: 20

        // 标题
        Label {
            text: "控件展示"
            font.pixelSize: 24
            font.weight: Font.Bold
            color: "#333333"
        }

        // 光照参数
        GroupBox {
            Layout.fillWidth: true
            title: "光照参数"
            font.pixelSize: 16

            ColumnLayout {
                anchors.fill: parent
                spacing: 15

                // 主光源强度
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Label {
                        text: "主光源强度:";
                        font.pixelSize: 14
                        color: "#666666"
                        Layout.preferredWidth: 100
                    }

                    Slider {
                        id: lightSlider
                        Layout.fillWidth: true
                        from: 0
                        to: 2
                        value: lightIntensity
                        onValueChanged: lightIntensity = value
                    }

                    Label {
                        text: lightIntensity.toFixed(2)
                        font.pixelSize: 14
                        color: "#333333"
                        Layout.preferredWidth: 50
                    }
                }

                // 环境光强度
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Label {
                        text: "环境光强度:";
                        font.pixelSize: 14
                        color: "#666666"
                        Layout.preferredWidth: 100
                    }

                    Slider {
                        id: ambientSlider
                        Layout.fillWidth: true
                        from: 0
                        to: 1
                        value: ambientLight
                        onValueChanged: ambientLight = value
                    }

                    Label {
                        text: ambientLight.toFixed(2)
                        font.pixelSize: 14
                        color: "#333333"
                        Layout.preferredWidth: 50
                    }
                }
            }
        }

        // 模型参数
        GroupBox {
            Layout.fillWidth: true
            title: "模型参数"
            font.pixelSize: 16

            ColumnLayout {
                anchors.fill: parent
                spacing: 15

                // 模型缩放
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Label {
                        text: "模型缩放:";
                        font.pixelSize: 14
                        color: "#666666"
                        Layout.preferredWidth: 100
                    }

                    Slider {
                        id: scaleSlider
                        Layout.fillWidth: true
                        from: 0.1
                        to: 3
                        value: modelScale
                        onValueChanged: modelScale = value
                    }

                    Label {
                        text: modelScale.toFixed(2)
                        font.pixelSize: 14
                        color: "#333333"
                        Layout.preferredWidth: 50
                    }
                }

                // 旋转速度
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Label {
                        text: "旋转速度:";
                        font.pixelSize: 14
                        color: "#666666"
                        Layout.preferredWidth: 100
                    }

                    Slider {
                        id: speedSlider
                        Layout.fillWidth: true
                        from: 0
                        to: 5
                        value: rotationSpeed
                        onValueChanged: rotationSpeed = value
                    }

                    Label {
                        text: rotationSpeed.toFixed(2)
                        font.pixelSize: 14
                        color: "#333333"
                        Layout.preferredWidth: 50
                    }
                }
            }
        }

        // 滤镜参数
        GroupBox {
            Layout.fillWidth: true
            title: "滤镜参数"
            font.pixelSize: 16

            ColumnLayout {
                anchors.fill: parent
                spacing: 15

                // 滤镜强度
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Label {
                        text: "滤镜强度:";
                        font.pixelSize: 14
                        color: "#666666"
                        Layout.preferredWidth: 100
                    }

                    Slider {
                        id: filterSlider
                        Layout.fillWidth: true
                        from: 0
                        to: 1
                        value: filterStrength
                        onValueChanged: filterStrength = value
                    }

                    Label {
                        text: filterStrength.toFixed(2)
                        font.pixelSize: 14
                        color: "#333333"
                        Layout.preferredWidth: 50
                    }
                }

                // 滤镜类型
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Label {
                        text: "滤镜类型:";
                        font.pixelSize: 14
                        color: "#666666"
                        Layout.preferredWidth: 100
                    }

                    ComboBox {
                        id: filterCombo
                        Layout.fillWidth: true
                        model: ["无", "灰度", "sepia", "模糊", "锐化"]
                        currentIndex: 0
                    }
                }
            }
        }

        // 颜色选择
        GroupBox {
            Layout.fillWidth: true
            title: "颜色选择"
            font.pixelSize: 16

            Flow {
                Layout.fillWidth: true
                spacing: 10

                Repeater {
                    model: ["#0078d4", "#e81123", "#107c10", "#ff8c00", "#5c2d91", "#00b294", "#ffffff", "#333333"]

                    Rectangle {
                        width: 50
                        height: 50
                        color: modelData
                        radius: 4
                        border.color: "#cccccc"
                        border.width: 1

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("Selected color:", modelData)
                            }
                        }
                    }
                }
            }
        }

        // 按钮示例
        GroupBox {
            Layout.fillWidth: true
            title: "按钮示例"
            font.pixelSize: 16

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Button {
                    text: "按钮1"
                    Layout.fillWidth: true
                }

                Button {
                    text: "按钮2"
                    Layout.fillWidth: true
                }

                Button {
                    text: "按钮3"
                    Layout.fillWidth: true
                }
            }
        }

        // 复选框和单选按钮
        GroupBox {
            Layout.fillWidth: true
            title: "选择控件"
            font.pixelSize: 16

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    CheckBox {
                        text: "选项1"
                    }

                    CheckBox {
                        text: "选项2"
                    }

                    CheckBox {
                        text: "选项3"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    RadioButton {
                        text: "选项A"
                    }

                    RadioButton {
                        text: "选项B"
                    }

                    RadioButton {
                        text: "选项C"
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}