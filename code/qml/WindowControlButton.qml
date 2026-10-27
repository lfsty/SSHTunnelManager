import QtQuick
import QtQuick.Controls

// 无边框窗口的最小化 / 最大化 / 关闭按钮
AbstractButton {
    id: control

    required property string operation // minimize | maximize | restore | close

    readonly property bool isClose: operation === "close"

    implicitWidth: 42
    implicitHeight: 30
    focusPolicy: Qt.NoFocus
    hoverEnabled: true

    Accessible.name: {
        switch (operation) {
        case "minimize":
            return qsTr("最小化")
        case "maximize":
            return qsTr("最大化")
        case "restore":
            return qsTr("还原")
        default:
            return qsTr("关闭")
        }
    }

    background: Rectangle {
        color: {
            if (control.isClose) {
                if (control.pressed)
                    return "#b32a1b"
                return control.hovered ? "#e81123" : "transparent"
            }

            if (control.pressed)
                return Theme.buttonPressed
            return control.hovered ? Theme.buttonHover : "transparent"
        }
    }

    contentItem: Item {
        id: content

        readonly property color iconColor: control.isClose && control.hovered ? "#ffffff" : Theme.textSecondary

        Rectangle {
            anchors.centerIn: parent
            visible: control.operation === "minimize"
            width: 10
            height: 1
            color: content.iconColor
        }

        Rectangle {
            anchors.centerIn: parent
            visible: control.operation === "maximize"
            width: 9
            height: 9
            color: "transparent"
            border.width: 1
            border.color: content.iconColor
        }

        Item {
            anchors.centerIn: parent
            visible: control.operation === "restore"
            width: 11
            height: 11

            Rectangle {
                x: 0
                y: 2
                width: 8
                height: 8
                color: "transparent"
                border.width: 1
                border.color: content.iconColor
            }

            Rectangle {
                x: 2
                y: 0
                width: 8
                height: 8
                color: "transparent"
                border.width: 1
                border.color: content.iconColor
            }
        }

        Item {
            anchors.centerIn: parent
            visible: control.operation === "close"
            width: 10
            height: 10

            Rectangle {
                anchors.centerIn: parent
                width: 9
                height: 1
                color: content.iconColor
                rotation: 45
            }

            Rectangle {
                anchors.centerIn: parent
                width: 9
                height: 1
                color: content.iconColor
                rotation: -45
            }
        }
    }
}
