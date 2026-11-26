# PetriRenamer C++ Prototype

Bản native Windows viết bằng C++17 và Qt 6 Widgets.

## Có sẵn
- Xem ảnh theo nhóm 1-6 ảnh.
- Nhập mã theo mẫu PetriRenamer.
- Sao chép sang `PetriRenamer_output`, không sửa ảnh gốc.
- Xác minh SHA-256 sau sao chép.
- `rename_manifest.csv` và thư mục `needs_review`.
- Đánh số Unicode liên tục khi trùng mã.

## Chưa có trong prototype
- OCR Tesseract/OpenCV.
- HEIC/HEIF (Qt mặc định không bảo đảm có plugin HEIF).
- Session JSON/autosave và rename journal.

## Build Windows
Cài Visual Studio 2022 Build Tools, CMake, Ninja và Qt 6.5+ MSVC 64-bit. Đặt biến `QTDIR`, ví dụ:

```bat
set QTDIR=C:\Qt\6.8.3\msvc2022_64
BUILD_WINDOWS.bat
```

Kết quả nằm trong `dist`. `windeployqt` sẽ chép DLL Qt để chạy trên máy không cài Qt.
