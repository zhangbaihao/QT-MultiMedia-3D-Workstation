import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import QtQuick.Dialogs
import QtMultiMedia3D 1.0

Page {
    objectName: "model3dOther"
    background: Rectangle { color: "#f8f9fa" }

    // 1. 重构属性，明确区分状态
    property string currentFilePath: ""
    property int vertexCount: 0
    property int faceCount: 0
    property string modelStatus: "未选择"
    property color modelColor: "#0078d4"
    property real zoom: 1.0
    property bool autoRotate: false
    property real rotationX: 0
    property real rotationY: 0
    property real positionX: 0
    property real positionY: 0
    property bool isLoading: false  // 新增加载状态

    // 2. 将文件处理封装为函数
    function handleFileSelected(filePath) {
        console.log("QML: 处理文件选择:", filePath)
        
        if (!filePath || filePath === "") {
            resetModel()
            return
        }
        
        // 更新状态
        modelStatus = "加载中"
        isLoading = true
        currentFilePath = filePath
        
        // 调用C++加载
        console.log("QML: 调用C++加载模型:", filePath)
        modelLoader.loadModel(filePath)
        
        // 更新3D视图（如果有预览功能）
        viewer.setModel(filePath)
    }
    
    function resetModel() {
        currentFilePath = ""
        vertexCount = 0
        faceCount = 0
        modelStatus = "未选择"
        isLoading = false
        
        // 重置视图
        rotationX = 0
        rotationY = 0
        positionX = 0
        positionY = 0
        zoom = 1.0
        zoomSlider.value = 1.0
        
        // 更新3D视图
        updateTransform()
    }
    
    // 3. 连接C++ ModelLoader的信号
    Connections {
        target: modelLoader
        
        onModelLoaded: (vCount, fCount, modelData) => {
            console.log("QML: 模型加载成功, 顶点数:", vCount, "面数:", fCount)
            vertexCount = vCount
            faceCount = fCount
            modelStatus = "已加载"
            isLoading = false
            
            // 可选：处理modelData
            if (modelData) {
                console.log("收到模型数据:", modelData)
            }
            
            // 加载完成后的初始化
            updateTransform()
        }
        
        onModelLoadFailed: (errorMsg) => {
            console.error("QML: 模型加载失败:", errorMsg)
            modelStatus = "加载失败: " + errorMsg
            isLoading = false
            
            // 显示错误提示
            errorPopup.text = "加载失败: " + errorMsg
            errorPopup.open()
        }
        
        onLoadProgress: (progress, message) => {
            console.log("QML: 加载进度:", progress + "%", "消息:", message)
            // 可以在这里更新进度条
            if (progressBar) {
                progressBar.value = progress
            }
        }
    }

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
                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                ColumnLayout {
                    width: parent.width
                    spacing: 20
                    anchors.margins: 20

                    // 标题
                    Label {
                        text: "3D模型控制 (bgfx)"
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

                            // 加载按钮
                            Button {
                                id: loadButton
                                Layout.fillWidth: true
                                text: isLoading ? "加载中..." : "📂 选择模型文件"
                                font.pixelSize: 14
                                enabled: !isLoading

                                background: Rectangle {
                                    color: parent.enabled ? (parent.pressed ? "#005a9e" : "#0078d4") : "#cccccc"
                                    radius: 8
                                    Behavior on color { ColorAnimation { duration: 200 } }
                                }

                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    opacity: parent.enabled ? 1.0 : 0.7
                                }

                                onClicked: {
                                    console.log("QML: 用户点击了选择模型文件按钮")
                                    fileDialog.open()
                                }
                            }

                            // 文件信息
                            Label {
                                text: {
                                    if (isLoading) return "正在加载..."
                                    if (currentFilePath) {
                                        var fileName = currentFilePath.toString().split('/').pop()
                                        return "📄 " + fileName
                                    }
                                    return "未选择文件"
                                }
                                font.pixelSize: 12
                                color: isLoading ? "#ff8c00" : "#666666"
                                wrapMode: Text.WrapAnywhere
                                Layout.fillWidth: true
                            }
                            
                            // 进度条（可选）
                            ProgressBar {
                                id: progressBar
                                Layout.fillWidth: true
                                visible: isLoading
                                from: 0
                                to: 100
                                value: 0
                                indeterminate: true
                            }
                            
                            // 清空按钮
                            Button {
                                Layout.fillWidth: true
                                text: "🗑️ 清空模型"
                                font.pixelSize: 13
                                enabled: !isLoading && (currentFilePath !== "" || vertexCount > 0)
                                flat: true
                                
                                onClicked: {
                                    console.log("QML: 清空模型")
                                    resetModel()
                                }
                            }
                        }
                    }

                    // 颜色控制
                    GroupBox {
                        Layout.fillWidth: true
                        title: "颜色控制"
                        font.pixelSize: 14
                        enabled: !isLoading && modelStatus === "已加载"

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
                                        opacity: parent.parent.enabled ? 1.0 : 0.5
                                        
                                        Behavior on border.color { ColorAnimation { duration: 200 } }
                                        Behavior on scale { NumberAnimation { duration: 200 } }

                                        MouseArea {
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            enabled: parent.parent.enabled
                                            onEntered: {
                                                parent.scale = 1.1
                                            }
                                            onExited: {
                                                parent.scale = 1.0
                                            }
                                            onClicked: {
                                                modelColor = modelData
                                                var color = Qt.color(modelData)
                                                viewer.setColor(color.r, color.g, color.b)
                                                console.log("QML: 颜色更改为:", modelData)
                                            }
                                        }
                                    }
                                }
                            }

                            // 自定义颜色
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 10
                                opacity: parent.enabled ? 1.0 : 0.5

                                Label {
                                    text: "自定义:"
                                    font.pixelSize: 12
                                    color: "#666666"
                                }

                                Rectangle {
                                    Layout.preferredWidth: 40
                                    Layout.preferredHeight: 30
                                    color: modelColor
                                    radius: 6
                                    border.color: "#cccccc"
                                    border.width: 1
                                    
                                    MouseArea {
                                        anchors.fill: parent
                                        enabled: parent.parent.parent.enabled
                                        onClicked: {
                                            colorDialog.open()
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
                        enabled: !isLoading && modelStatus === "已加载"

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 15

                            // 自动旋转
                            CheckBox {
                                text: "自动旋转"
                                checked: autoRotate
                                enabled: parent.parent.enabled
                                onCheckedChanged: {
                                    autoRotate = checked
                                    console.log("QML: 自动旋转", checked ? "开启" : "关闭")
                                }
                            }

                            // 缩放控制
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 10
                                opacity: parent.enabled ? 1.0 : 0.5

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
                                    enabled: parent.parent.enabled
                                    onValueChanged: {
                                        if (!pressed) return
                                        zoom = value
                                        updateTransform()
                                        console.log("QML: 缩放调整为", (zoom * 100).toFixed(0) + "%")
                                    }
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
                                enabled: parent.parent.enabled
                                
                                onClicked: {
                                    rotationX = 0
                                    rotationY = 0
                                    positionX = 0
                                    positionY = 0
                                    zoom = 1.0
                                    zoomSlider.value = 1.0
                                    autoRotate = false
                                    updateTransform()
                                    console.log("QML: 重置视图完成")
                                }
                            }
                        }
                    }

                    // 模型信息
                    GroupBox {
                        Layout.fillWidth: true
                        title: "模型信息"
                        font.pixelSize: 14
                        enabled: !isLoading

                        GridLayout {
                            anchors.fill: parent
                            columns: 2
                            rowSpacing: 8
                            columnSpacing: 15
                            opacity: parent.enabled ? 1.0 : 0.5

                            Label { text: "顶点数:"; font.pixelSize: 12; color: "#666666" }
                            Label { 
                                text: vertexCount > 0 ? vertexCount.toLocaleString() : "-"
                                font.pixelSize: 12; 
                                color: "#333333"; 
                                font.weight: Font.Medium 
                            }

                            Label { text: "面数:"; font.pixelSize: 12; color: "#666666" }
                            Label { 
                                text: faceCount > 0 ? faceCount.toLocaleString() : "-"
                                font.pixelSize: 12; 
                                color: "#333333"; 
                                font.weight: Font.Medium 
                            }

                            Label { text: "格式:"; font.pixelSize: 12; color: "#666666" }
                            Label { 
                                text: {
                                    if (!currentFilePath) return "-"
                                    var path = currentFilePath.toString().toLowerCase()
                                    if (path.endsWith(".stl")) return "STL"
                                    if (path.endsWith(".ply")) return "PLY"
                                    if (path.endsWith(".obj")) return "OBJ"
                                    if (path.endsWith(".gltf")) return "GLTF"
                                    if (path.endsWith(".fbx")) return "FBX"
                                    return "Unknown"
                                }
                                font.pixelSize: 12; 
                                color: "#333333"; 
                                font.weight: Font.Medium 
                            }

                            Label { text: "状态:"; font.pixelSize: 12; color: "#666666" }
                            Label { 
                                text: modelStatus
                                font.pixelSize: 12 
                                color: {
                                    if (modelStatus.includes("已加载")) return "#107c10"
                                    if (modelStatus.includes("加载中")) return "#ff8c00"
                                    if (modelStatus.includes("加载失败")) return "#e81123"
                                    return "#666666"
                                }
                                font.weight: Font.Medium 
                            }
                        }
                    }

                    // 渲染选项
                    GroupBox {
                        Layout.fillWidth: true
                        title: "渲染选项"
                        font.pixelSize: 14
                        enabled: !isLoading && modelStatus === "已加载"

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10
                            opacity: parent.enabled ? 1.0 : 0.5

                            CheckBox { 
                                text: "显示网格"
                                checked: true
                                enabled: parent.parent.enabled
                            }
                            CheckBox { 
                                text: "显示法线"
                                checked: false
                                enabled: parent.parent.enabled
                            }
                            CheckBox { 
                                text: "线框模式"
                                checked: false
                                enabled: parent.parent.enabled
                            }
                            CheckBox { 
                                text: "双面渲染"
                                checked: true
                                enabled: parent.parent.enabled
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }
        }

        // 右侧3D视图
        Rectangle {
            id: viewport3D
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e1e1e"
            radius: 16

            FramebufferObject {
                id: viewer
                anchors.fill: parent
                onHeightChanged: console.log("Viewer height:", height)
                onWidthChanged: console.log("Viewer width:", width)
            }

            // 鼠标交互
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                enabled: !isLoading && modelStatus === "已加载"

                property point lastMousePosLeft: Qt.point(0, 0)
                property point lastMousePosRight: Qt.point(0, 0)

                onPressed: function (mouse) {
                    if (!enabled) return
                    
                    if (mouse.button === Qt.LeftButton) {
                        lastMousePosLeft = Qt.point(mouse.x, mouse.y)
                    } else if (mouse.button === Qt.RightButton) {
                        lastMousePosRight = Qt.point(mouse.x, mouse.y)
                    }
                }

                onPositionChanged: function (mouse) {
                    if (!enabled) return
                    
                    if (mouse.buttons & Qt.LeftButton) {
                        var dx = mouse.x - lastMousePosLeft.x
                        var dy = mouse.y - lastMousePosLeft.y
                        
                        rotationY += dx * 0.5
                        rotationX += dy * 0.5
                        
                        updateTransform()
                        lastMousePosLeft = Qt.point(mouse.x, mouse.y)
                    } else if (mouse.buttons & Qt.RightButton) {
                        var dx = mouse.x - lastMousePosRight.x
                        var dy = mouse.y - lastMousePosRight.y
                        
                        positionX += dx * 0.01
                        positionY -= dy * 0.01
                        
                        updateTransform()
                        lastMousePosRight = Qt.point(mouse.x, mouse.y)
                    }
                }

                onWheel: function (wheel) {
                    if (!enabled) return
                    
                    var zoomFactor = 1 + wheel.angleDelta.y * 0.001
                    zoom *= zoomFactor
                    zoom = Math.max(0.1, Math.min(10.0, zoom))
                    zoomSlider.value = zoom
                    updateTransform()
                }
            }

            // 自动旋转逻辑
            Timer {
                interval: 50
                running: autoRotate && !isLoading && modelStatus === "已加载"
                repeat: true
                onTriggered: {
                    rotationY += 0.5
                    updateTransform()
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
                visible: !isLoading

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
            
            // 加载中提示
            Rectangle {
                anchors.centerIn: parent
                width: 200
                height: 100
                radius: 12
                color: "#000000"
                opacity: 0.8
                visible: isLoading
                
                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 10
                    
                    BusyIndicator {
                        Layout.alignment: Qt.AlignHCenter
                        running: true
                        width: 40
                        height: 40
                    }
                    
                    Label {
                        text: "正在加载模型..."
                        color: "#ffffff"
                        font.pixelSize: 14
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
                radius: 12
                opacity: 0.5
                visible: !isLoading

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

    // 文件对话框
    FileDialog {
        id: fileDialog
        title: "选择3D模型文件"
        nameFilters: ["3D模型文件 (*.stl *.ply *.obj *.gltf *.fbx)", "所有文件 (*)"]
        
        onAccepted: {
            console.log("QML: 文件对话框被接受，文件:", currentFile)
            handleFileSelected(currentFile)
        }
        
        onRejected: {
            console.log("QML: 文件对话框被取消")
        }
    }
    
    // 颜色选择对话框
    ColorDialog {
        id: colorDialog
        title: "选择模型颜色"
        
        onAccepted: {
            modelColor = color
            var color = Qt.color(color)
            viewer.setColor(color.r, color.g, color.b)
            console.log("QML: 颜色更改为:", color)
        }
    }
    
    // 错误提示弹窗
    MessageDialog {
        id: errorPopup
        title: "加载错误"
        
        text: ""
    }
    
    // 更新变换
    function updateTransform() {
        if (!viewer || isLoading) return
        
        try {
            var rotation = Qt.quaternion(0, rotationX * Math.PI / 180, rotationY * Math.PI / 180, 0)
            viewer.setTransform(Qt.vector3d(positionX, positionY, 0), rotation, zoom)
        } catch (e) {
            console.error("QML: 更新变换失败:", e)
        }
    }

    // 页面完成时添加调试信息
    Component.onCompleted: {
        console.log("=== ModelViewerOther 页面初始化完成 ===")
        console.log("3D查看器尺寸:", width, "x", height)
        
        // 初始化视图
        updateTransform()
        
        // 连接调试信号
        viewer.initialized.connect(function() {
            console.log("QML: 3D查看器初始化完成")
        })
    }
}