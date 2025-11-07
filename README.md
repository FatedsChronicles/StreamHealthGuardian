# Stream Health Guardian

Dock-based health dashboard + rules-driven automation for OBS. Works on Windows/macOS/Linux.

- **Color-coded dock border**: Green (OK), Red (Video), Blue (Audio), Amber (Network)
- **Rules Engine**: Trigger actions on thresholds (overlay, scene switch, attempt bitrate/downscale)
- **Downshift Policy**: **Confirm (default)** or **Auto-apply** (best effort)

## Build via GitHub Actions
Push to GitHub; the workflows under `.github/workflows/` build and upload artifacts per-OS.
No local build required.

> Version and name are defined **only in `buildspec.json`**. CMake picks them up via `project(${_name} VERSION ${_version})`.
