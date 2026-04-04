import QtQuick 2.15
import QtQuick3D

Item {
    id: cameraController
    anchors.fill: parent

    // 相机属性
    property Camera camera: null
    property vector3d cameraPosition: Qt.vector3d(0, 0, 20)
    property vector3d cameraRotation: Qt.vector3d(0, 0, 0)
    property real cameraZoom: 1.0
    property vector3d cameraTarget: Qt.vector3d(0, 0, 0)

    // 控制参数
    property real rotationSpeed: 0.5
    property real zoomSpeed: 0.1
    property real panSpeed: 0.01

    // 内部状态
    property point lastMousePos
    property bool isRotating: false
    property bool isPanning: false

    // 初始化相机
    function initCamera() {
        if (camera) {
            camera.position = cameraPosition
            camera.eulerRotation = cameraRotation
        }
    }

    // 重置相机
    function resetCamera() {
        cameraPosition = Qt.vector3d(0, 0, 20)
        cameraRotation = Qt.vector3d(0, 0, 0)
        cameraZoom = 1.0
        cameraTarget = Qt.vector3d(0, 0, 0)
        initCamera()
    }

    // 相机控制
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed: (mouse) => {
            lastMousePos = Qt.point(mouse.x, mouse.y)
            isRotating = (mouse.button === Qt.LeftButton)
            isPanning = (mouse.button === Qt.RightButton)
        }

        onPositionChanged: (mouse) => {
            var deltaX = mouse.x - lastMousePos.x
            var deltaY = mouse.y - lastMousePos.y

            if (isRotating) {
                // 旋转相机
                cameraRotation.y += deltaX * rotationSpeed
                cameraRotation.x += deltaY * rotationSpeed
                
                // 限制 X 轴旋转范围
                cameraRotation.x = Math.max(-89, Math.min(89, cameraRotation.x))
                
                if (camera) {
                    camera.eulerRotation = cameraRotation
                }
            } else if (isPanning) {
                // 平移相机
                cameraPosition.x -= deltaX * panSpeed * cameraZoom
                cameraPosition.y += deltaY * panSpeed * cameraZoom
                
                if (camera) {
                    camera.position = cameraPosition
                }
            }

            lastMousePos = Qt.point(mouse.x, mouse.y)
        }

        onWheel: (wheel) => {
            // 缩放相机
            var zoomFactor = 1 + wheel.angleDelta.y * zoomSpeed * 0.01
            cameraZoom *= zoomFactor
            cameraZoom = Math.max(0.1, Math.min(10.0, cameraZoom))
            
            cameraPosition.z /= zoomFactor
            
            if (camera) {
                camera.position = cameraPosition
            }
        }

        onReleased: {
            isRotating = false
            isPanning = false
        }
    }
}
