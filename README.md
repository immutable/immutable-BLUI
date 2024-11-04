# Immutable Integration Notes

This repository is a clone of [BLUI](https://github.com/getnamo/BLUI-Unreal/tree/4.2.0), modified to integrate with [unreal-immutable-sdk](https://github.com/immutable/unreal-immutable-sdk). It serves as a replacement for Epic's WebBrowserWidget plugin in Unreal Engine versions 4.26, 4.27, and 5.0, necessitated by the need for a more recent CEF version.

Credit for the original code belongs to the authors of the BLUI repository.

# Installation

### Prerequisites
- [git-lfs](https://git-lfs.com/)
- Unreal Engine (4.26, 4.27, or 5.0) installed via the Epic Games Launcher or built from source.
- Microsoft Visual Studio. Ensure you have a compatible version installed. Refer to the official Unreal Engine documentation for details.

### Clone the Repository

Open a terminal, navigate to your game's plugin folder, and run the following command:

```sh
git clone https://github.com/immutable/immutable-BLUI.git
```