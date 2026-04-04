// TestPage.qml - 语法修正版
import QtQuick
import QtQuick3D
import QtQuick.Controls

Window {
    width: 800
    height: 600
    visible: true
    title: "3D轨道相机查看器"
    
    View3D {
        id: view3D
        anchors.fill: parent
        
        environment: SceneEnvironment {
            clearColor: "#2c3e50"
            backgroundMode: SceneEnvironment.Color
        }
        
        // 相机控制参数
        property vector3d targetPoint: Qt.vector3d(0, 0, 0)  // 模型中心点
        property real cameraDistance: 600
        property real cameraYaw: 0      // 水平旋转
        property real cameraPitch: 0    // 垂直旋转
        
        // 计算相机位置（使用球坐标系）
        function calculateCameraPosition() {
            // 转换为弧度
            var pitchRad = cameraPitch * Math.PI / 180
            var yawRad = cameraYaw * Math.PI / 180
            
            // 球坐标转直角坐标
            var x = cameraDistance * Math.cos(pitchRad) * Math.sin(yawRad)
            var y = cameraDistance * Math.sin(pitchRad)
            var z = cameraDistance * Math.cos(pitchRad) * Math.cos(yawRad)
            
            // 从目标点偏移
            return Qt.vector3d(x, y, z).plus(targetPoint)
        }
        
        // 轨道相机
        PerspectiveCamera {
            id: camera
            position: calculateCameraPosition()
            
            Component.onCompleted: {
                // 初始化时看向目标点
                camera.lookAt(targetPoint)
            }
        }
        
        // 目标节点（模型中心）
        Node {
            id: targetNode
            position: targetPoint
            
            // 模型
            Model {
                source: "#Cube"
                position: Qt.vector3d(0, 0, 0)
                scale: Qt.vector3d(2, 2, 2)
                materials: [
                    PrincipledMaterial {
                        baseColor: "#e74c3c"
                        roughness: 0.3
                    }
                ]
            }
        }
        
        // 光源
        DirectionalLight {
            eulerRotation.x: -30
            eulerRotation.y: 30
        }
        
        // 坐标轴
        Node {
            visible: true
            
            // X轴 - 红色
            Model {
                source: "#Cylinder"
                position: Qt.vector3d(50, 0, 0)
                scale: Qt.vector3d(0.05, 50, 0.05)
                eulerRotation.z: -90
                materials: [ DefaultMaterial { diffuseColor: "red" } ]
            }
            
            // Y轴 - 绿色
            Model {
                source: "#Cylinder"
                position: Qt.vector3d(0, 50, 0)
                scale: Qt.vector3d(0.05, 50, 0.05)
                materials: [ DefaultMaterial { diffuseColor: "green" } ]
            }
            
            // Z轴 - 蓝色
            Model {
                source: "#Cylinder"
                position: Qt.vector3d(0, 0, 50)
                scale: Qt.vector3d(0.05, 50, 0.05)
                eulerRotation.x: 90
                materials: [ DefaultMaterial { diffuseColor: "blue" } ]
            }
        }
        
        // 自定义鼠标控制器
        MouseArea {
            id: mouseController
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            
            property point lastMousePos
            property real rotationSpeed: 0.5
            property real panSpeed: 0.1
            property real zoomSpeed: 0.001
            
            onPressed: (mouse) => {
                lastMousePos = Qt.point(mouse.x, mouse.y)
            }
            
            onPositionChanged: (mouse) => {
                var deltaX = mouse.x - lastMousePos.x
                var deltaY = mouse.y - lastMousePos.y
                
                if (mouse.buttons & Qt.LeftButton) {
                    // 左键：围绕模型旋转
                    view3D.cameraYaw += deltaX * rotationSpeed
                    view3D.cameraPitch -= deltaY * rotationSpeed
                    
                    // 限制俯仰角
                    if (view3D.cameraPitch > 89) view3D.cameraPitch = 89
                    if (view3D.cameraPitch < -89) view3D.cameraPitch = -89
                    
                    // 更新相机
                    updateCamera()
                } 
                else if (mouse.buttons & Qt.RightButton) {
                    // 右键：平移模型
                    var yawRad = view3D.cameraYaw * Math.PI / 180
                    var pitchRad = view3D.cameraPitch * Math.PI / 180
                    
                    // 相机朝向
                    var forward = Qt.vector3d(
                        Math.cos(pitchRad) * Math.sin(yawRad),
                        Math.sin(pitchRad),
                        Math.cos(pitchRad) * Math.cos(yawRad)
                    ).normalized()
                    
                    // 世界上方向
                    var worldUp = Qt.vector3d(0, 1, 0)
                    
                    // 右方向
                    var right = forward.crossProduct(worldUp).normalized()
                    
                    // 上方向
                    var up = right.crossProduct(forward).normalized()
                    
                    // 更新目标点
                    view3D.targetPoint = view3D.targetPoint.plus(
                        right.times(-deltaX * panSpeed)
                    ).plus(
                        up.times(deltaY * panSpeed)
                    )
                    
                    // 更新相机
                    updateCamera()
                }
                
                lastMousePos = Qt.point(mouse.x, mouse.y)
            }
            
            onWheel: (wheel) => {
                // 滚轮：缩放
                var zoomFactor = 1.0 + wheel.angleDelta.y * zoomSpeed
                view3D.cameraDistance *= zoomFactor
                
                // 限制缩放范围
                if (view3D.cameraDistance < 10) view3D.cameraDistance = 10
                if (view3D.cameraDistance > 2000) view3D.cameraDistance = 2000
                
                // 更新相机
                updateCamera()
            }
            
            // 更新相机位置和朝向
            function updateCamera() {
                camera.position = view3D.calculateCameraPosition()
                camera.lookAt(view3D.targetPoint)
            }
        }
    }
    
    // 顶部信息栏
    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        height: 40
        color: "#55000000"
        radius: 8
        
        Row {
            anchors.centerIn: parent
            spacing: 20
            
            Text {
                text: "相机位置: (" + 
                      camera.position.x.toFixed(1) + ", " +
                      camera.position.y.toFixed(1) + ", " +
                      camera.position.z.toFixed(1) + ")"
                color: "white"
                font.pixelSize: 12
            }
            
            Text {
                text: "模型中心: (" + 
                      view3D.targetPoint.x.toFixed(1) + ", " +
                      view3D.targetPoint.y.toFixed(1) + ", " +
                      view3D.targetPoint.z.toFixed(1) + ")"
                color: "white"
                font.pixelSize: 12
            }
        }
    }
    
    // 底部控制面板
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        width: 400
        height: 100
        color: "#55000000"
        radius: 8
        
        Column {
            anchors.centerIn: parent
            spacing: 8
            
            Row {
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter
                
                Button {
                    text: "前视图"
                    onClicked: {
                        view3D.cameraYaw = 0
                        view3D.cameraPitch = 0
                        updateCameraView()
                    }
                }
                
                Button {
                    text: "顶视图"
                    onClicked: {
                        view3D.cameraYaw = 0
                        view3D.cameraPitch = 89
                        updateCameraView()
                    }
                }
                
                Button {
                    text: "侧视图"
                    onClicked: {
                        view3D.cameraYaw = 90
                        view3D.cameraPitch = 0
                        updateCameraView()
                    }
                }
                
                Button {
                    text: "重置"
                    onClicked: {
                        view3D.cameraDistance = 600
                        view3D.cameraYaw = 0
                        view3D.cameraPitch = 0
                        view3D.targetPoint = Qt.vector3d(0, 0, 0)
                        updateCameraView()
                    }
                }
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "左键旋转 · 右键平移 · 滚轮缩放"
                color: "white"
                font.pixelSize: 12
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "距离: " + view3D.cameraDistance.toFixed(1) + 
                      "  Yaw: " + view3D.cameraYaw.toFixed(1) + "°" +
                      "  Pitch: " + view3D.cameraPitch.toFixed(1) + "°"
                color: "#ffcc00"
                font.pixelSize: 12
            }
        }
    }
    
    // 更新相机视图的函数
    function updateCameraView() {
        camera.position = view3D.calculateCameraPosition()
        camera.lookAt(view3D.targetPoint)
    }
    
    // 调试信息
    Component.onCompleted: {
        console.log("=== 3D查看器初始化完成 ===")
        console.log("初始相机位置:", camera.position)
        console.log("模型中心:", view3D.targetPoint)
    }
}