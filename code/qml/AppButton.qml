import QtQuick
import QtQuick.Controls

// 统一风格的按钮: primary / secondary / danger / ghost
Button {
    id: control

    property string variant: "secondary"

    readonly property bool isPrimary: variant === "primary"
    readonly property bool isDanger: variant === "danger"
    readonly property bool isGhost: variant === "ghost"

    implicitHeight: 32
    implicitWidth: Math.max(64, contentItem.implicitWidth + leftPadding + rightPadding)
    leftPadding: 14
    rightPadding: 14
    topPadding: 6
    bottomPadding: 6
    font.pixelSize: 13

    background: Rectangle {
        radius: 8
        border.width: control.isGhost || control.isPrimary || control.isDanger ? 0 : 1
        border.color: Theme.border
        color: {
            if (!control.enabled)
                return Theme.buttonDisabled

            if (control.isPrimary) {
                if (control.down)
                    return Theme.primaryPressed
                return control.hovered ? Theme.primaryHover : Theme.primary
            }

            if (control.isDanger) {
                if (control.down)
                    return Theme.dangerPressed
                return control.hovered ? Theme.dangerHover : Theme.dangerSoft
            }

            if (control.isGhost) {
                if (control.down)
                    return Theme.ghostPressed
                return control.hovered ? Theme.ghostHover : "transparent"
            }

            if (control.down)
                return Theme.buttonPressed
            return control.hovered ? Theme.buttonHover : Theme.button
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: {
            if (!control.enabled)
                return Theme.textMuted
            if (control.isPrimary)
                return Theme.primaryText
            if (control.isDanger)
                return Theme.danger
            if (control.isGhost)
                return Theme.textSecondary
            return Theme.textPrimary
        }
    }
}
