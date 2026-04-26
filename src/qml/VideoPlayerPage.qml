import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import QtQml 2.15
import QtMultiMedia3D 1.0

Page {
    id: root
    objectName: "video"
    background: Rectangle { color: "#1a1a1a" }
    clip: true

    // ============================================================
    //  C++ Backend 对象
    // ============================================================

    VideoPlayer {
        id: videoPlayer
    }

    AudioPlayer {
        id: audioPlayer
    }

    // ============================================================
    //  QML 状态属性
    // ============================================================

    property string currentFile: ""
    property real playbackRate: 1.0
    property bool isMuted: false
    property real volume: 0.8
    property bool hasAudioStream: true

    // ============================================================
    //  错误信号连接
    // ============================================================

    Connections {
        target: videoPlayer
        function onErrorOccurred(msg) {
            console.warn("VideoPlayer:", msg)
            statusText.text = "视频错误: " + msg
        }
    }

    Connections {
        target: audioPlayer
        function onErrorOccurred(msg) {
            console.warn("AudioPlayer:", msg)
            // 音频错误不阻断视频播放（可能是无音频轨道的文件）
        }
    }

    // ============================================================
    //  主布局
    // ============================================================

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ============================================================
        //  视频画面区域
        // ============================================================

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 8
            color: "#000000"
            radius: 16

            // 双 Image 交替显示（double buffering），避免单 Image 切 source 闪烁
            Item {
                id: videoFrameContainer
                anchors.fill: parent
                anchors.margins: 4
                clip: true

                property int activeIndex: 0

                Image {
                    id: videoImageA
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    source: currentFile !== "" ? "image://videoframe/frame?a=0&v=" + (videoPlayer.frameSequence) : ""
                    visible: videoFrameContainer.activeIndex === 0
                }

                Image {
                    id: videoImageB
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    source: currentFile !== "" ? "image://videoframe/frame?b=0&v=" + (videoPlayer.frameSequence) : ""
                    visible: videoFrameContainer.activeIndex === 1
                }

                Connections {
                    target: videoPlayer
                    function onFrameSequenceChanged() {
                        // 先更新当前不可见图层的 source，再切换可见性
                        // 这样用户始终看到的是完整帧，不会有空白期
                        var nextIdx = 1 - videoFrameContainer.activeIndex
                        var seq = videoPlayer.frameSequence

                        if (nextIdx === 0) {
                            videoImageA.source = "image://videoframe/frame?a=" + seq + "&v=" + seq
                        } else {
                            videoImageB.source = "image://videoframe/frame?b=" + seq + "&v=" + seq
                        }
                        videoFrameContainer.activeIndex = nextIdx
                    }
                }
            }

            // ---- 视频信息浮层 ----
            Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 20
                width: 200
                height: 100
                color: "#000000"
                radius: 12
                opacity: 0.7
                visible: currentFile !== "" && videoPlayer.videoWidth > 0

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 4

                    Label {
                        text: "视频信息"
                        font.pixelSize: 12
                        font.weight: Font.Bold
                        color: "#ffffff"
                    }

                    Label {
                        text: "分辨率: " + videoPlayer.videoWidth + " × " + videoPlayer.videoHeight
                        font.pixelSize: 11
                        color: "#cccccc"
                        visible: videoPlayer.videoWidth > 0
                    }

                    Label {
                        text: "时长: " + formatTime(videoPlayer.duration)
                        font.pixelSize: 11
                        color: "#cccccc"
                    }

                    Label {
                        text: "帧率: " + (videoPlayer.frameRate > 0 ? videoPlayer.frameRate.toFixed(1) + " fps" : "")
                        font.pixelSize: 11
                        color: "#cccccc"
                        property real fps: 0
                    }
                }
            }

            // ---- 中央提示文字 ----
            Label {
                id: statusText
                anchors.centerIn: parent
                text: {
                    if (currentFile === "")
                        return "请点击下方按钮选择视频文件"
                    if (!videoPlayer.isPlaying && videoPlayer.position > 0)
                        return "已暂停"
                    if (videoPlayer.videoWidth === 0 && videoPlayer.duration > 0)
                        return "解码中..."
                    return ""
                }
                font.pixelSize: 16
                color: "#ffffff"
                visible: text !== ""
            }
        }

        // ============================================================
        //  控制栏
        // ============================================================

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 130
            Layout.margins: 8
            color: "#2d2d2d"
            border.color: "#3d3d3d"
            border.width: 1
            radius: 16

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                // ---- 进度条 ----
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Label {
                        text: formatTime(videoPlayer.position)
                        font.pixelSize: 12
                        color: "#ffffff"
                        Layout.preferredWidth: 56
                        horizontalAlignment: Text.AlignHCenter
                    }

                    // 自定义进度条：支持点击跳转 + 拖拽
                    Item {
                        id: positionSlider
                        Layout.fillWidth: true
                        Layout.preferredHeight: 28
                        enabled: currentFile !== "" && videoPlayer.duration > 0

                        // 当前显示值（0.0~1.0）
                        property real visualPos: videoPlayer.duration > 0
                            ? (videoPlayer.position / videoPlayer.duration) : 0.0
                        property bool isDragging: false
                        property real dragValue: 0.0

                        // 将像素坐标转为 0~1 比例
                        function posToRatio(mouseX) {
                            var w = width - leftMargin - rightMargin
                            if (w <= 0) return 0.0
                            return Math.max(0.0, Math.min(1.0, (mouseX - leftMargin) / w))
                        }

                        // 比例转为毫秒
                        function ratioToMs(ratio) {
                            return Math.round(ratio * videoPlayer.duration)
                        }

                        readonly property real leftMargin: 0
                        readonly property real rightMargin: 0

                        // === 轨道背景 ===
                        Rectangle {
                            x: parent.leftMargin
                            y: parent.height / 2 - 3
                            width: parent.width - parent.leftMargin - parent.rightMargin
                            height: 6
                            radius: 3
                            color: "#4d4d4d"

                            // 已播放部分
                            Rectangle {
                                width: Math.max(0, parent.width * (positionSlider.isDragging ? positionSlider.dragValue : positionSlider.visualPos))
                                height: parent.height
                                color: "#0078d4"
                                radius: 3
                            }

                            // 鼠标悬停预览圆点
                            Rectangle {
                                visible: positionSliderMouse.containsMouse && !positionSlider.isDragging
                                width: 12
                                height: 12
                                radius: 6
                                color: "#ffffff"
                                y: (parent.height - height) / 2
                                x: Math.max(0, Math.min(parent.width - width,
                                    positionSliderMouse.mouseX - width / 2))
                                opacity: 0.8
                            }
                        }

                        // === 拖拽手柄 ===
                        Rectangle {
                            x: parent.leftMargin
                               + (positionSlider.isDragging ? positionSlider.dragValue : positionSlider.visualPos) * (parent.width - parent.leftMargin - parent.rightMargin)
                               - width / 2
                            y: parent.height / 2 - height / 2
                            width: 14
                            height: 14
                            radius: 7
                            color: "#0078d4"
                            border.color: "#ffffff"
                            border.width: 2
                            visible: parent.enabled
                        }

                        // === 鼠标交互 ===
                        MouseArea {
                            id: positionSliderMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            enabled: positionSlider.enabled
                            cursorShape: Qt.PointingHandCursor

                            onPressed: function(mouse) {
                                positionSlider.isDragging = true
                                positionSlider.dragValue = positionSlider.posToRatio(mouse.x)
                            }

                            onPositionChanged: function(mouse) {
                                if (positionSlider.isDragging) {
                                    positionSlider.dragValue = positionSlider.posToRatio(mouse.x)
                                }
                            }

                            onReleased: function(mouse) {
                                if (positionSlider.isDragging) {
                                    var ratio = positionSlider.posToRatio(mouse.x)
                                    var ms = positionSlider.ratioToMs(ratio)
                                    videoPlayer.position = ms
                                    if (hasAudioStream)
                                        audioPlayer.position = ms
                                    positionSlider.isDragging = false
                                }
                            }

                            onClicked: function(mouse) {
                                // 点击跳转
                                var ratio = positionSlider.posToRatio(mouse.x)
                                var ms = positionSlider.ratioToMs(ratio)
                                videoPlayer.position = ms
                                if (hasAudioStream)
                                    audioPlayer.position = ms
                            }
                        }
                    }

                    Label {
                        text: formatTime(videoPlayer.duration)
                        font.pixelSize: 12
                        color: "#ffffff"
                        Layout.preferredWidth: 56
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                // ---- 控制按钮行 ----
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16
                    Layout.alignment: Qt.AlignHCenter

                    // 打开文件
                    Button {
                        id: openBtn
                        text: "📂"
                        font.pixelSize: 18
                        flat: true
                        Layout.preferredWidth: 40

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "#ffffff"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            videoFileDialog.open()
                        }
                    }

                    // 快退 10s
                    Button {
                        text: "⏪"
                        font.pixelSize: 20
                        flat: true
                        Layout.preferredWidth: 40
                        enabled: currentFile !== ""

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: enabled ? "#ffffff" : "#666666"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            var v = Math.max(0, videoPlayer.position - 10000)
                            videoPlayer.position = v
                            if (hasAudioStream)
                                audioPlayer.position = v
                        }
                    }

                    // 播放/暂停
                    Button {
                        text: videoPlayer.isPlaying ? "⏸" : "▶"
                        font.pixelSize: 28
                        flat: true
                        Layout.preferredWidth: 48
                        enabled: currentFile !== ""

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: enabled ? "#ffffff" : "#666666"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            if (videoPlayer.isPlaying) {
                                videoPlayer.pause()
                                audioPlayer.pause()
                            } else {
                                videoPlayer.play()
                                audioPlayer.play()
                            }
                        }
                    }

                    // 停止
                    Button {
                        text: "⏹"
                        font.pixelSize: 20
                        flat: true
                        Layout.preferredWidth: 40
                        enabled: currentFile !== ""

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: enabled ? "#ffffff" : "#666666"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            videoPlayer.stop()
                            audioPlayer.stop()
                        }
                    }

                    // 快进 10s
                    Button {
                        text: "⏩"
                        font.pixelSize: 20
                        flat: true
                        Layout.preferredWidth: 40
                        enabled: currentFile !== ""

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: enabled ? "#ffffff" : "#666666"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            var v = Math.min(videoPlayer.duration, videoPlayer.position + 10000)
                            videoPlayer.position = v
                            if (hasAudioStream)
                                audioPlayer.position = v
                        }
                    }

                    // 分隔符
                    Rectangle {
                        width: 1
                        height: 30
                        color: "#4d4d4d"
                    }

                    // 倍速选择
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
                            radius: 8
                            border.color: "#4d4d4d"
                            border.width: 1
                        }

                        onCurrentIndexChanged: {
                            var rates = [0.5, 1.0, 1.5, 2.0]
                            playbackRate = rates[currentIndex]
                            videoPlayer.playbackRate = playbackRate
                        }
                    }

                    // 分隔符
                    Rectangle {
                        width: 1
                        height: 30
                        color: "#4d4d4d"
                        visible: hasAudioStream
                    }

                    // 静音按钮
                    Button {
                        text: isMuted ? "🔇" : "🔊"
                        font.pixelSize: 18
                        flat: true
                        Layout.preferredWidth: 40
                        visible: hasAudioStream

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "#ffffff"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            isMuted = !isMuted
                            audioPlayer.muted = isMuted
                        }
                    }

                    // 自定义音量滑块（替换标准 Slider，拖拽更可靠）
                    Item {
                        id: volumeSlider
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 28
                        visible: hasAudioStream
                        enabled: hasAudioStream

                        property real visualVolume: root.volume
                        property bool isDragging: false
                        property real dragVolume: 0.0
                        readonly property real leftMargin: 0
                        readonly property real rightMargin: 0

                        function posToVolume(mouseX) {
                            var w = width - leftMargin - rightMargin
                            if (w <= 0) return 0.0
                            return Math.max(0.0, Math.min(1.0, (mouseX - leftMargin) / w))
                        }

                        // 轨道背景
                        Rectangle {
                            x: parent.leftMargin
                            y: parent.height / 2 - 3
                            width: parent.width - parent.leftMargin - parent.rightMargin
                            height: 6
                            radius: 3
                            color: "#4d4d4d"

                            // 已填充部分
                            Rectangle {
                                width: Math.max(0, parent.width * (volumeSlider.isDragging ? volumeSlider.dragVolume : volumeSlider.visualVolume))
                                height: parent.height
                                color: "#0078d4"
                                radius: 3
                            }
                        }

                        // 拖拽手柄
                        Rectangle {
                            x: parent.leftMargin
                               + (volumeSlider.isDragging ? volumeSlider.dragVolume : volumeSlider.visualVolume) * (parent.width - parent.leftMargin - parent.rightMargin)
                               - width / 2
                            y: parent.height / 2 - height / 2
                            width: 14
                            height: 14
                            radius: 7
                            color: "#ffffff"
                            border.color: "#0078d4"
                            border.width: 2
                        }

                        // 鼠标交互
                        MouseArea {
                            id: volumeSliderMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            enabled: volumeSlider.enabled
                            cursorShape: Qt.PointingHandCursor

                            onPressed: function(mouse) {
                                volumeSlider.isDragging = true
                                volumeSlider.dragVolume = volumeSlider.posToVolume(mouse.x)
                            }

                            onPositionChanged: function(mouse) {
                                if (volumeSlider.isDragging) {
                                    volumeSlider.dragVolume = volumeSlider.posToVolume(mouse.x)
                                }
                            }

                            onReleased: function(mouse) {
                                if (volumeSlider.isDragging) {
                                    var vol = volumeSlider.posToVolume(mouse.x)
                                    root.volume = vol
                                    audioPlayer.volume = vol
                                    volumeSlider.visualVolume = vol
                                    volumeSlider.isDragging = false
                                }
                            }

                            onClicked: function(mouse) {
                                var vol = volumeSlider.posToVolume(mouse.x)
                                root.volume = vol
                                audioPlayer.volume = vol
                                volumeSlider.visualVolume = vol
                            }
                        }
                    }

                    // 分隔符
                    Rectangle {
                        width: 1
                        height: 30
                        color: "#4d4d4d"
                        visible: hasAudioStream
                    }

                    // 音频设备选择
                    ComboBox {
                        id: audioDeviceCombo
                        Layout.preferredWidth: 140
                        visible: hasAudioStream

                        // 初始化设备列表
                        model: audioPlayer.availableAudioDevices()
                        currentIndex: {
                            // 找到当前默认设备的索引
                            var defaultDesc = audioPlayer.audioDevice
                            for (var i = 0; i < model.length; i++) {
                                if (model[i] === defaultDesc || (defaultDesc === "" && i === 0))
                                    return i
                            }
                            return 0
                        }

                        contentItem: Text {
                            text: audioDeviceCombo.displayText
                            font.pixelSize: 11
                            color: "#ffffff"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                        }

                        background: Rectangle {
                            color: "#3d3d3d"
                            radius: 8
                            border.color: "#4d4d4d"
                            border.width: 1
                        }

                        onCurrentIndexChanged: {
                            if (currentIndex >= 0 && currentIndex < model.length) {
                                var desc = model[currentIndex]
                                audioPlayer.audioDevice = desc
                                console.log("Audio device changed to:", desc)
                                // 如果正在播放，自动重启音频
                                if (audioPlayer.isPlaying) {
                                    audioPlayer.stop()
                                    audioPlayer.play()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ============================================================
    //  文件对话框（使用 Qt.labs.platform 的 StandardPaths + FileDialog）
    //  Qt.labs.platform 是跨 Qt 5/6 兼容的稳定模块
    // ============================================================

    // 计算默认文件夹 URL
    readonly property url defaultFolderUrl: {
        try {
            var docPath = StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
            // 确保路径以 / 结尾并转换为 URL
            var normalized = docPath.replace(/\\/g, "/")
            if (!normalized.endsWith("/"))
                normalized += "/"
            return "file:///" + normalized
        } catch(e) {
            return "file:///C:/Users/Public/Videos/"
        }
    }

    FileDialog {
        id: videoFileDialog
        title: "选择视频文件"
        folder: root.defaultFolderUrl
        nameFilters: ["视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv *.flv *.webm)", "所有文件 (*)"]

        onAccepted: {
            // Qt.labs.platform FileDialog 使用 .file 属性（单数 QUrl）
            var fileUrl = ""
            try {
                if (videoFileDialog.file !== null && videoFileDialog.file !== undefined)
                    fileUrl = videoFileDialog.file.toString()
            } catch(e) {
                console.warn("FileDialog.file access error:", e)
            }

            if (fileUrl && fileUrl.length > 0) {
                loadFile(fileUrl)
            }
        }
    }

    function loadFile(fileUrl) {
        if (!fileUrl || fileUrl.length === 0)
            return

        console.log("Loading file:", fileUrl)
        currentFile = fileUrl
        videoPlayer.filePath = fileUrl
        audioPlayer.filePath = fileUrl

        // 延迟检查音频是否可用
        hasAudioStream = (audioPlayer.duration > 0)
        console.log("File loaded, duration:", videoPlayer.duration, "hasAudio:", hasAudioStream)

        // 自动开始播放（延迟让解码器初始化）
        autoPlayTimer.retryCount = 0
        autoPlayTimer.start()
    }

    Timer {
        id: autoPlayTimer
        interval: 150
        repeat: false
        property int retryCount: 0

        onTriggered: {
            if (videoPlayer.duration > 0 || retryCount >= 5) {
                if (videoPlayer.duration > 0) {
                    videoPlayer.play()
                    audioPlayer.play()
                    console.log("Auto-play started after", retryCount, "retries")
                } else {
                    console.log("Auto-play: duration still 0 after retries, user must click play")
                }
            } else {
                retryCount++
                console.log("Auto-play: duration not ready (retry", retryCount, ")...")
                autoPlayTimer.start()
            }
        }
    }

    // ============================================================
    //  初始化
    // ============================================================

    Component.onCompleted: {
        audioPlayer.volume = root.volume
        audioPlayer.muted = root.isMuted
        videoPlayer.playbackRate = playbackRate
    }

    // ============================================================
    //  时间格式化工具
    // ============================================================

    function formatTime(ms) {
        if (ms === undefined || ms === null || isNaN(ms) || ms < 0)
            return "00:00"
        var totalSeconds = Math.floor(ms / 1000)
        if (totalSeconds === 0 && ms > 0)
            totalSeconds = 1
        var minutes = Math.floor(totalSeconds / 60)
        var hours = Math.floor(minutes / 60)
        var seconds = totalSeconds % 60
        minutes = minutes % 60

        if (hours > 0) {
            return _pad(hours, 2) + ":" + _pad(minutes, 2) + ":" + _pad(seconds, 2)
        }
        return _pad(minutes, 2) + ":" + _pad(seconds, 2)
    }

    function _pad(num, size) {
        var s = String(num)
        while (s.length < size)
            s = "0" + s
        return s
    }
}
