import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick3D
import QtQuick3D.Helpers
import QtMultiMedia3D 1.0

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    title: "QT-MultiMedia 3D Workstation"
    visible: true
    color: "#f5f5f5"

    // 顶部导航栏
    header: TabBar {
        id: tabBar
        height: 60
        background: Rectangle {
            color: "#ffffff"
            border.color: "#e0e0e0"
            border.width: 1
        }

        TabButton {
            text: "首页"
            onClicked: swipeView.currentIndex = 0
        }
        TabButton {
            text: "3D模型"
            onClicked: swipeView.currentIndex = 1
        }
        TabButton {
            text: "视频播放"
            onClicked: swipeView.currentIndex = 2
        }
        TabButton {
            text: "控件展示"
            onClicked: swipeView.currentIndex = 3
        }
    }

    // 主内容区域
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: 0
        interactive: false  // 禁用滑动切换，只通过TabBar切换

        // 首页
        HomePage {
            onNavigateToPage: function(pageIndex) {
                swipeView.currentIndex = pageIndex
            }
        }

        // 3D模型页面
        ModelViewerPage {}

        // 视频播放页面
        VideoPlayerPage {}

        // 控件展示页面
        ControlPanel {}
    }

    function showToast(message) {
        console.log("Toast:", message)
    }
}
