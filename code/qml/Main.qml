import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Qt.labs.platform as Platform
import SSHTunnelManager.Core

ApplicationWindow {
    id: window

    required property bool closeToTray

    width: 780
    height: 640
    minimumWidth: 560
    minimumHeight: 420
    visible: true
    title: qsTr("SSH 隧道管理")
    color: Theme.background

    // 去掉系统标题栏，改用 QML 绘制的标题栏
    flags: Qt.Window | Qt.FramelessWindowHint

    palette {
        window: Theme.background
        windowText: Theme.textPrimary
        base: Theme.inputBackground
        alternateBase: Theme.cardBackground
        text: Theme.textPrimary
        button: Theme.button
        buttonText: Theme.textPrimary
        highlight: Theme.primary
        highlightedText: Theme.primaryText
        mid: Theme.border
    }

    TunnelManager {
        id: manager
        onErrorOccurred: message => window.showToast(message)
    }

    Component.onCompleted: manager.load()

    function showWindow() {
        if (!window.visible)
            window.show()
        if (window.visibility === Window.Minimized)
            window.showNormal()
        window.raise()
        window.requestActivate()
    }

    function toggleMaximize() {
        if (window.visibility === Window.Maximized)
            window.showNormal()
        else
            window.showMaximized()
    }

    function showToast(message) {
        toastLabel.text = message
        toast.visible = true
        toastTimer.restart()
    }

    function createTunnel() {
        setupDialog.isNewTunnel = true
        setupDialog.tunnel = manager.addTunnel()
        setupDialog.open()
    }

    function openSetup(tunnel) {
        setupDialog.isNewTunnel = false
        setupDialog.tunnel = tunnel
        setupDialog.open()
    }

    onClosing: close => {
        manager.save()
        if (window.closeToTray) {
            close.accepted = false
            window.hide()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── 自绘标题栏 ──────────────────────────────────────────────
        Rectangle {
            id: titleBar

            Layout.fillWidth: true
            Layout.preferredHeight: 46
            color: Theme.headerBackground

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: Theme.border
            }

            // 拖动 / 双击最大化
            Item {
                anchors.fill: parent

                DragHandler {
                    target: null
                    onActiveChanged: {
                        if (active)
                            window.startSystemMove()
                    }
                }

                TapHandler {
                    acceptedButtons: Qt.LeftButton
                    onDoubleTapped: window.toggleMaximize()
                }
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 6
                spacing: 10

                Image {
                    source: "qrc:/img/icon.png"
                    sourceSize.width: 20
                    sourceSize.height: 20
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    text: qsTr("SSH 隧道管理")
                    color: Theme.textPrimary
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                }

                Item { Layout.fillWidth: true }

                AppButton {
                    text: qsTr("新建隧道")
                    variant: "primary"
                    implicitHeight: 28
                    topPadding: 4
                    bottomPadding: 4
                    onClicked: window.createTunnel()
                }

                Row {
                    spacing: 0

                    WindowControlButton {
                        operation: "minimize"
                        onClicked: window.showMinimized()
                    }

                    WindowControlButton {
                        operation: window.visibility === Window.Maximized ? "restore" : "maximize"
                        onClicked: window.toggleMaximize()
                    }

                    WindowControlButton {
                        operation: "close"
                        onClicked: window.close()
                    }
                }
            }
        }

        // ── 隧道列表 ────────────────────────────────────────────────
        Item {
            id: contentArea

            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: tunnelList

                anchors.fill: parent
                anchors.margins: 18
                topMargin: 0
                bottomMargin: 0
                clip: true
                spacing: 12
                model: manager.tunnels

                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                delegate: TunnelCard {
                    required property int index

                    width: tunnelList.width
                    onSetupRequested: window.openSetup(tunnel)
                    onRemoveRequested: manager.removeTunnel(index)
                }

                add: Transition {
                    NumberAnimation { properties: "opacity"; from: 0; to: 1; duration: 150 }
                }

                displaced: Transition {
                    NumberAnimation { properties: "y"; duration: 150; easing.type: Easing.OutCubic }
                }
            }

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 8
                visible: tunnelList.count === 0

                Image {
                    source: "qrc:/img/icon.png"
                    sourceSize.width: 56
                    sourceSize.height: 56
                    fillMode: Image.PreserveAspectFit
                    opacity: 0.85
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: qsTr("还没有配置任何隧道")
                    color: Theme.textPrimary
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 8
                }

                Label {
                    text: qsTr("点击“新建隧道”开始配置 ssh 端口转发")
                    color: Theme.textSecondary
                    font.pixelSize: 12
                    Layout.alignment: Qt.AlignHCenter
                }

                AppButton {
                    text: qsTr("新建隧道")
                    variant: "primary"
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 8
                    onClicked: window.createTunnel()
                }
            }

            Rectangle {
                id: toast

                z: 100
                visible: false
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 8
                width: Math.min(parent.width - 48, toastLabel.implicitWidth + 32)
                height: toastLabel.implicitHeight + 20
                radius: 8
                color: Theme.toastBackground

                Label {
                    id: toastLabel

                    anchors.centerIn: parent
                    width: parent.width - 32
                    color: Theme.toastText
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Timer {
                    id: toastTimer
                    interval: 5000
                    onTriggered: toast.visible = false
                }
            }
        }

        // ── 状态栏 ──────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 34
            color: Theme.headerBackground

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: Theme.border
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 18
                anchors.rightMargin: 12
                spacing: 12

                Label {
                    Layout.fillWidth: true
                    text: manager.storagePath
                    color: Theme.textMuted
                    font.pixelSize: 11
                    elide: Text.ElideMiddle
                }

                Label {
                    text: qsTr("%1 条隧道 · %2 个运行中").arg(tunnelList.count).arg(manager.runningCount)
                    color: Theme.textSecondary
                    font.pixelSize: 11
                }

                AppButton {
                    text: qsTr("打开日志")
                    variant: "ghost"
                    implicitHeight: 24
                    leftPadding: 8
                    rightPadding: 8
                    onClicked: manager.openLogFile()
                }
            }
        }
    }

    // 无边框窗口的边框
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.width: 1
        border.color: Theme.border
        visible: window.visibility !== Window.Maximized
        z: 900
    }

    // ── 边缘缩放 ──────────────────────────────────────────────────
    component ResizeHandle: MouseArea {
        property int edges

        acceptedButtons: Qt.LeftButton
        enabled: window.visibility !== Window.Maximized
        onPressed: window.startSystemResize(edges)
    }

    ResizeHandle {
        edges: Qt.LeftEdge
        width: 6
        height: parent.height
        anchors.left: parent.left
        cursorShape: Qt.SizeHorCursor
        z: 1000
    }

    ResizeHandle {
        edges: Qt.RightEdge
        width: 6
        height: parent.height
        anchors.right: parent.right
        cursorShape: Qt.SizeHorCursor
        z: 1000
    }

    ResizeHandle {
        edges: Qt.TopEdge
        width: parent.width
        height: 6
        anchors.top: parent.top
        cursorShape: Qt.SizeVerCursor
        z: 1000
    }

    ResizeHandle {
        edges: Qt.BottomEdge
        width: parent.width
        height: 6
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeVerCursor
        z: 1000
    }

    ResizeHandle {
        edges: Qt.LeftEdge | Qt.TopEdge
        width: 12
        height: 12
        anchors.left: parent.left
        anchors.top: parent.top
        cursorShape: Qt.SizeFDiagCursor
        z: 1001
    }

    ResizeHandle {
        edges: Qt.RightEdge | Qt.TopEdge
        width: 12
        height: 12
        anchors.right: parent.right
        anchors.top: parent.top
        cursorShape: Qt.SizeBDiagCursor
        z: 1001
    }

    ResizeHandle {
        edges: Qt.LeftEdge | Qt.BottomEdge
        width: 12
        height: 12
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeBDiagCursor
        z: 1001
    }

    ResizeHandle {
        edges: Qt.RightEdge | Qt.BottomEdge
        width: 12
        height: 12
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeFDiagCursor
        z: 1001
    }

    TunnelSetupDialog {
        id: setupDialog

        onAccepted: {
            isNewTunnel = false
            manager.save()
        }

        onRejected: {
            if (isNewTunnel && tunnel) {
                const index = manager.tunnels.indexOf(tunnel)
                if (index >= 0)
                    manager.removeTunnel(index)
            }
            isNewTunnel = false
        }
    }

    Platform.SystemTrayIcon {
        id: trayIcon

        visible: true
        icon.source: "qrc:/img/icon.png"
        tooltip: qsTr("SSH 隧道管理")

        menu: Platform.Menu {
            Platform.MenuItem {
                text: qsTr("显示主窗口")
                onTriggered: window.showWindow()
            }
            Platform.MenuSeparator {}
            Platform.MenuItem {
                text: qsTr("打开日志")
                onTriggered: manager.openLogFile()
            }
            Platform.MenuSeparator {}
            Platform.MenuItem {
                text: qsTr("退出")
                onTriggered: {
                    manager.save()
                    Qt.quit()
                }
            }
        }

        onActivated: reason => {
            if (reason === Platform.SystemTrayIcon.Trigger || reason === Platform.SystemTrayIcon.DoubleClick)
                window.showWindow()
        }
    }
}
