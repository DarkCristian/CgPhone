pragma Singleton
import QtQuick
import QtCore

QtObject {
    id: theme

    property Settings persisted: Settings {
        id: persisted
        category: "Appearance"
        property bool darkMode: false
    }

    property alias darkMode: persisted.darkMode

    readonly property color background: darkMode ? "#0B1220" : "#F6FAFF"
    readonly property color surface: darkMode ? "#111827" : "#FFFFFF"
    readonly property color surfaceAlt: darkMode ? "#172033" : "#EFF6FF"
    readonly property color hover: darkMode ? "#1E293B" : "#F1F7FF"
    readonly property color softBlue: darkMode ? "#172554" : "#EDF5FF"
    readonly property color text: darkMode ? "#F8FAFC" : "#111827"
    readonly property color textSecondary: darkMode ? "#CBD5E1" : "#334155"
    readonly property color secondary: darkMode ? "#CBD5E1" : "#475569"
    readonly property color muted: darkMode ? "#94A3B8" : "#64748B"
    readonly property color border: darkMode ? "#334155" : "#DDE6F2"
    readonly property color borderStrong: darkMode ? "#475569" : "#D8E1EE"
    readonly property color blueBorder: darkMode ? "#31558A" : "#C9DDF8"
    readonly property color track: darkMode ? "#334155" : "#DFE7F1"
    readonly property color disabled: darkMode ? "#475569" : "#CBD5E1"
    readonly property color accent: darkMode ? "#60A5FA" : "#2563EB"
    readonly property color onAccent: "#FFFFFF"
}
