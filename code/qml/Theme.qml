pragma Singleton

import QtQuick

// 全局配色与尺寸，自动跟随系统浅色 / 深色主题
QtObject {
    readonly property bool dark: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark

    readonly property color background: dark ? "#1b1d21" : "#f4f5f7"
    readonly property color headerBackground: dark ? "#232529" : "#ffffff"
    readonly property color cardBackground: dark ? "#25272c" : "#ffffff"
    readonly property color dialogBackground: dark ? "#25272c" : "#ffffff"
    readonly property color inputBackground: dark ? "#2b2e34" : "#ffffff"
    readonly property color border: dark ? "#383b42" : "#e2e5ea"

    readonly property color textPrimary: dark ? "#e6e8ec" : "#1f2328"
    readonly property color textSecondary: dark ? "#a2a8b3" : "#5c6470"
    readonly property color textMuted: dark ? "#7a818c" : "#8b93a1"

    readonly property color primary: dark ? "#4c8dff" : "#2f6feb"
    readonly property color primaryHover: dark ? "#62a0ff" : "#3b7bf5"
    readonly property color primaryPressed: dark ? "#3a72d6" : "#2459c4"
    readonly property color primaryText: "#ffffff"

    readonly property color success: dark ? "#3ecf7a" : "#1f9d55"
    readonly property color warning: dark ? "#e6b34d" : "#b97700"
    readonly property color danger: dark ? "#ff7070" : "#d64545"
    readonly property color dangerSoft: dark ? "#3a2426" : "#fdecec"
    readonly property color dangerHover: dark ? "#472b2e" : "#fbe0e0"
    readonly property color dangerPressed: dark ? "#523034" : "#f7d0d0"

    readonly property color button: dark ? "#2b2e34" : "#f6f7f9"
    readonly property color buttonHover: dark ? "#33373e" : "#eef0f3"
    readonly property color buttonPressed: dark ? "#3a3f47" : "#e4e7eb"
    readonly property color buttonDisabled: dark ? "#26292e" : "#f2f3f5"
    readonly property color ghostHover: dark ? "#2e3238" : "#eef0f3"
    readonly property color ghostPressed: dark ? "#383d44" : "#e4e7eb"

    readonly property color toastBackground: dark ? "#e6e8ec" : "#24292f"
    readonly property color toastText: dark ? "#1b1d21" : "#ffffff"

    readonly property int radius: 10
    readonly property int spacing: 12
}
