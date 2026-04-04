import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

Page {
    objectName: "video"
    background: Rectangle { color: "#1a1a1a" }

    // 视频数据
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

        // 视频显示区域
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#000000"

            // 视频信息显示
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

            // 状态标签
            Label {
                id: statusLabel
                anchors.centerIn: parent
                text: currentFile === "" ? "请点击下方按钮选择视频文件" : "视频播放区域"
                font.pixelSize: 16
                color: "#ffffff"
            }
        }

        // 控制栏
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

                // 进度条
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

                // 控制按钮
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 15
                    Layout.alignment: Qt.AlignHCenter

                    // 打开文件按钮
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

                    // 快退按钮
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

                    // 播放/暂停按钮
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

                    // 停止按钮
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

                    // 快进按钮
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

                    // 分隔线
                    Rectangle {
                        width: 1
                        height: 30
                        color: "#4d4d4d"
                    }

                    // 播放速度
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

                    // 分隔线
                    Rectangle {
                        width: 1
                        height: 30
                        color: "#4d4d4d"
                    }

                    // 音量控制
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

    // 视频文件对话框
    FileDialog {
        id: videoFileDialog
        title: "选择视频文件"
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: ["视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv)", "所有文件 (*)"]
        onAccepted: {
            currentFile = videoFileDialog.file.toString().replace("file:///", "")
            // 模拟加载视频
            duration = 300000 // 5分钟
            isPlaying = true
        }
    }

    // 格式化时间
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
