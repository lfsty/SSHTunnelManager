pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// 编辑单条端口转发规则
RowLayout {
    id: row

    required property int index
    required property string localPort
    required property string destIP
    required property string destPort

    signal edited(int row, string role, string value)
    signal removeRequested()

    spacing: 8

    TextField {
        Layout.fillWidth: true
        Layout.preferredWidth: 130
        implicitHeight: 34
        placeholderText: qsTr("本地端口")
        text: row.localPort
        selectByMouse: true
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator { bottom: 1; top: 65535 }
        onTextEdited: row.edited(row.index, "localPort", text)
    }

    TextField {
        Layout.fillWidth: true
        Layout.preferredWidth: 220
        implicitHeight: 34
        placeholderText: qsTr("目标地址")
        text: row.destIP
        selectByMouse: true
        onTextEdited: row.edited(row.index, "destIP", text)
    }

    TextField {
        Layout.preferredWidth: 110
        implicitHeight: 34
        placeholderText: qsTr("目标端口")
        text: row.destPort
        selectByMouse: true
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator { bottom: 1; top: 65535 }
        onTextEdited: row.edited(row.index, "destPort", text)
    }

    AppButton {
        text: "✕"
        variant: "ghost"
        implicitWidth: 34
        implicitHeight: 34
        onClicked: row.removeRequested()
    }
}
