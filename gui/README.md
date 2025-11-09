# TRTS Lab GUI

This directory contains a Qt5 desktop shell for the TRTS symbolic laboratory. The interface wraps the
existing command-line utilities and exposes them through a dockable, tab-oriented workflow covering
configuration, execution, visualization, analysis, and theory exploration.

## Building

```bash
mkdir -p build && cd build
cmake .. -DQt5_DIR=/path/to/Qt5
cmake --build .
```

The resulting executable `trts_lab_gui` links against Qt5 Core and Widgets.

## Layout Overview

* **Engine configuration** – parameter form with presets and advanced toggles.
* **Execution** – live engine controls, tick state, and event log.
* **ψ rhythm** – streaming visualization of ψ firings.
* **Stack** – koppa stack exploration.
* **Analyzer** – metallic constant analyzer output.
* **Fingerprint** – barcode and export controls.
* **Phase map** – region explorer with rerun affordances.
* **Theorist** – symbolic hypothesis browser.
* **Output table** – raw CSV-style capture with export.

The GUI expects the TRTS engine binary (`trts_go_time`) to be present at the repository root or
provided via the `TRTS_ENGINE_EXECUTABLE` environment variable.
