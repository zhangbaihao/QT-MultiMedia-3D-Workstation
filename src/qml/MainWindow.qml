import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick.Dialogs

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    title: "QT-MultiMedia 3D Workstation"
    visible: true
    color: "#f8f9fa"
    

    // 顶部导航栏
    header: Rectangle {
        id: headerBackground
        height: 80
        color: "#f8f9fa"

        Rectangle {
            anchors {
                fill: parent
                topMargin: 10
                leftMargin: 10
                rightMargin: 10
                bottomMargin: 10
            }
            color: "#ffffff"
            border.color: "#e0e0e0"
            border.width: 1
            radius: 10

            TabBar {
                id: tabBar
                anchors.fill: parent
                background: Rectangle {
                    color: "transparent"
                }

                TabButton {
                    text: "首页"
                    onClicked: {
                        stackLayout.currentIndex = 0
                        tabBar.currentIndex = 0
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 0 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 0 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
                TabButton {
                    text: "3D模型 (Qt Quick 3D)"
                    onClicked: {
                        stackLayout.currentIndex = 1
                        tabBar.currentIndex = 1
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 1 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 1 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
                TabButton {
                    text: "3D模型 (Other)"
                    onClicked: {
                        stackLayout.currentIndex = 2
                        tabBar.currentIndex = 2
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 2 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 2 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
                TabButton {
                    text: "视频播放"
                    onClicked: {
                        stackLayout.currentIndex = 3
                        tabBar.currentIndex = 3
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 3 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 3 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
                TabButton {
                    text: "控件展示"
                    onClicked: {
                        stackLayout.currentIndex = 4
                        tabBar.currentIndex = 4
                    }
                    padding: 20
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    background: Rectangle {
                        color: tabBar.currentIndex === 4 ? "#0078d4" : "transparent"
                        radius: 8
                        anchors.fill: parent
                        anchors.margins: 8
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: tabBar.currentIndex === 4 ? "#ffffff" : "#333333"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        Behavior on color {
                            ColorAnimation { duration: 200 }
                        }
                    }
                }
            }
        }
    }

    // 主内容区域
    StackLayout {
        id: stackLayout
        anchors {
            fill: parent
            margins: 10
        }
        currentIndex: 0
        clip: true

        // 首页
        HomePage {
            clip: true
            onNavigateToPage: function(pageIndex) {
                stackLayout.currentIndex = pageIndex
                tabBar.currentIndex = pageIndex
            }
        }

        // 3D模型页面
        ModelViewerPage {
            
        }
        // 3D模型页面 (OpenGL)
        ModelViewerOther {
            clip: true
        }
        // 视频播放页面
        VideoPlayerPage {
            clip: true
        }
        // 控件展示页面
        ControlsPage {
            clip: true
        }
    }

    function showToast(message) {
        console.log("Toast:", message)
    }
}
