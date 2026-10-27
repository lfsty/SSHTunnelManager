pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// 隧道配置对话框: 跳板机信息 + 任意条端口转发规则
Dialog {
    id: dialog

    property var tunnel: null
    property bool isNewTunnel: false
    property string validationMessage: ""

    parent: Overlay.overlay
    anchors.centerIn: parent
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape
    width: Math.min(620, Overlay.overlay ? Overlay.overlay.width - 64 : 620)
    padding: 0

    background: Rectangle {
        radius: Theme.radius
        color: Theme.dialogBackground
        border.width: 1
        border.color: Theme.border
    }

    header: Item {
        implicitHeight: 58

        Label {
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("隧道设置")
            color: Theme.textPrimary
            font.pixelSize: 17
            font.weight: Font.DemiBold
        }

        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: Theme.border
        }
    }

    contentItem: Item {
        implicitWidth: 600
        implicitHeight: contentLayout.implicitHeight + 40

        ColumnLayout {
            id: contentLayout

            anchors.fill: parent
            anchors.margins: 20
            spacing: 16

            Label {
                text: qsTr("跳板机")
                color: Theme.textPrimary
                font.pixelSize: 13
                font.weight: Font.DemiBold
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Label {
                        text: qsTr("用户名")
                        color: Theme.textSecondary
                        font.pixelSize: 11
                    }

                    TextField {
                        id: userNameField
                        Layout.fillWidth: true
                        implicitHeight: 34
                        placeholderText: qsTr("登录跳板机的用户")
                        selectByMouse: true
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 240
                    spacing: 4

                    Label {
                        text: qsTr("地址")
                        color: Theme.textSecondary
                        font.pixelSize: 11
                    }

                    TextField {
                        id: serverIpField
                        Layout.fillWidth: true
                        implicitHeight: 34
                        placeholderText: qsTr("IP 或主机名")
                        selectByMouse: true
                    }
                }

                ColumnLayout {
                    Layout.preferredWidth: 110
                    spacing: 4

                    Label {
                        text: qsTr("端口")
                        color: Theme.textSecondary
                        font.pixelSize: 11
                    }

                    TextField {
                        id: serverPortField
                        Layout.fillWidth: true
                        implicitHeight: 34
                        placeholderText: qsTr("22")
                        selectByMouse: true
                        inputMethodHints: Qt.ImhDigitsOnly
                        validator: IntValidator { bottom: 1; top: 65535 }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: 4

                Label {
                    Layout.fillWidth: true
                    text: qsTr("端口转发规则")
                    color: Theme.textPrimary
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                }

                AppButton {
                    text: qsTr("添加规则")
                    onClicked: forwardModel.append({ "localPort": "", "destIP": "", "destPort": "" })
                }
            }

            ListView {
                id: forwardList

                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(Math.max(contentHeight, 34), 240)
                clip: true
                spacing: 8
                model: ListModel {
                    id: forwardModel
                }

                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                delegate: ForwardRow {
                    width: forwardList.width
                    onEdited: (row, role, value) => forwardModel.setProperty(row, role, value)
                    onRemoveRequested: {
                        if (forwardModel.count > 1) {
                            forwardModel.remove(index)
                        } else {
                            dialog.validationMessage = qsTr("至少需要一条端口转发规则")
                        }
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                visible: text.length > 0
                text: dialog.validationMessage
                color: Theme.danger
                font.pixelSize: 12
                wrapMode: Text.WordWrap
            }
        }
    }

    footer: Item {
        implicitHeight: 68

        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 1
            color: Theme.border
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 10

            Item { Layout.fillWidth: true }

            AppButton {
                text: qsTr("取消")
                onClicked: dialog.reject()
            }

            AppButton {
                text: qsTr("保存")
                variant: "primary"
                onClicked: dialog.saveConfiguration()
            }
        }
    }

    onOpened: {
        syncFromTunnel()
        userNameField.forceActiveFocus()
    }

    function syncFromTunnel() {
        forwardModel.clear()

        const forwards = dialog.tunnel ? dialog.tunnel.forwards : []
        for (let i = 0; i < forwards.length; ++i) {
            const forward = forwards[i]
            forwardModel.append({
                "localPort": forward.localPort,
                "destIP": forward.destIP,
                "destPort": forward.destPort
            })
        }

        if (forwardModel.count === 0) {
            forwardModel.append({ "localPort": "", "destIP": "", "destPort": "" })
        }

        userNameField.text = dialog.tunnel ? dialog.tunnel.userName : ""
        serverIpField.text = dialog.tunnel ? dialog.tunnel.serverIp : ""
        serverPortField.text = dialog.tunnel ? dialog.tunnel.serverPort : ""
        dialog.validationMessage = ""
    }

    function isValidPort(port) {
        if (!/^\d+$/.test(port)) {
            return false
        }
        const value = Number(port)
        return value >= 1 && value <= 65535
    }

    function validate() {
        if (!dialog.tunnel) {
            return qsTr("没有可保存的隧道")
        }

        if (userNameField.text.trim().length === 0) {
            return qsTr("请填写跳板机用户名")
        }

        if (serverIpField.text.trim().length === 0) {
            return qsTr("请填写跳板机地址")
        }

        if (!isValidPort(serverPortField.text.trim())) {
            return qsTr("跳板机端口必须是 1-65535 之间的数字")
        }

        if (forwardModel.count === 0) {
            return qsTr("至少需要一条端口转发规则")
        }

        const usedPorts = new Set()
        for (let i = 0; i < forwardModel.count; ++i) {
            const forward = forwardModel.get(i)
            const localPort = forward.localPort.trim()
            const destIP = forward.destIP.trim()
            const destPort = forward.destPort.trim()

            if (!isValidPort(localPort)) {
                return qsTr("第 %1 条规则的本地端口无效").arg(i + 1)
            }

            if (destIP.length === 0) {
                return qsTr("第 %1 条规则的目标地址不能为空").arg(i + 1)
            }

            if (!isValidPort(destPort)) {
                return qsTr("第 %1 条规则的目标端口无效").arg(i + 1)
            }

            if (usedPorts.has(localPort)) {
                return qsTr("本地端口 %1 重复").arg(localPort)
            }
            usedPorts.add(localPort)
        }

        return ""
    }

    function saveConfiguration() {
        const message = validate()
        if (message.length > 0) {
            dialog.validationMessage = message
            return
        }

        const forwards = []
        for (let i = 0; i < forwardModel.count; ++i) {
            const forward = forwardModel.get(i)
            forwards.push({
                "localPort": forward.localPort.trim(),
                "destIP": forward.destIP.trim(),
                "destPort": forward.destPort.trim()
            })
        }

        dialog.tunnel.applyConfiguration(userNameField.text.trim(), serverIpField.text.trim(), serverPortField.text.trim(), forwards)
        dialog.accept()
    }
}
