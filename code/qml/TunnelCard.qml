import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// 单个隧道卡片
Rectangle {
    id: card

    required property var tunnel

    signal setupRequested()
    signal removeRequested()

    readonly property bool running: tunnel.running

    implicitHeight: content.implicitHeight + 28
    radius: Theme.radius
    color: Theme.cardBackground
    border.width: 1
    border.color: running ? Theme.success : Theme.border

    Behavior on border.color {
        ColorAnimation { duration: 120 }
    }

    ColumnLayout {
        id: content

        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Rectangle {
                width: 10
                height: 10
                radius: 5
                Layout.alignment: Qt.AlignVCenter
                color: card.running ? Theme.success : Theme.textMuted
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Label {
                    Layout.fillWidth: true
                    text: card.tunnel.title.length > 0 ? card.tunnel.title : qsTr("未配置隧道")
                    color: Theme.textPrimary
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                    elide: Text.ElideRight
                }

                Label {
                    text: card.running ? qsTr("运行中") : qsTr("已停止")
                    font.pixelSize: 11
                    color: card.running ? Theme.success : Theme.textSecondary
                }
            }

            AppButton {
                text: card.running ? qsTr("停止") : qsTr("启动")
                variant: card.running ? "danger" : "primary"
                enabled: card.running || card.tunnel.configured
                onClicked: card.tunnel.toggle()
            }

            AppButton {
                text: qsTr("设置")
                enabled: !card.running
                onClicked: card.setupRequested()
            }

            AppButton {
                text: qsTr("删除")
                variant: "ghost"
                enabled: !card.running
                onClicked: card.removeRequested()
            }
        }

        Label {
            Layout.fillWidth: true
            visible: text.length > 0
            text: {
                const lines = []
                const forwards = card.tunnel.forwards
                for (let i = 0; i < forwards.length; ++i) {
                    const forward = forwards[i]
                    lines.push("127.0.0.1:" + forward.localPort + "  →  " + forward.destIP + ":" + forward.destPort)
                }
                return lines.join("\n")
            }
            color: Theme.textSecondary
            font.pixelSize: 12
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
        }

        Label {
            Layout.fillWidth: true
            visible: !card.tunnel.configured
            text: qsTr("尚未完成配置，点击“设置”填写跳板机与转发规则")
            color: Theme.warning
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }

        Label {
            Layout.fillWidth: true
            visible: text.length > 0
            text: card.tunnel.lastError
            color: Theme.danger
            font.pixelSize: 12
            wrapMode: Text.WrapAnywhere
            maximumLineCount: 3
            elide: Text.ElideRight
        }
    }
}
