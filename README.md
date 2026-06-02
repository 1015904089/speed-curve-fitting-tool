# Speed Curve Fitting Tool

一个基于 Qt/C++ 的一维运动曲线绘制与拟合工具，支持可拖拽界面分区、画布绘制、滑块录制、本地曲线模型，并计划接入 PySR 符号回归。

## 当前版本

当前代码是 V1 白板 UI 原型，重点验证界面和交互：

- 三块主区域：曲线拟合区域、滑块区域、结果展示区域。
- 左右、上下区域可拖拽调整尺寸。
- 曲线区域支持常驻网格、缩放、平移和鼠标绘画。
- 曲线区域包含可折叠信息浮层占位。
- 滑块区域支持一维拖动和录制时间显示。
- 结果展示区域包含公式、拟合类型和运动预览占位。
- 非网格图片资源已预留换皮接口，默认使用 Qt 默认皮肤。

V1 不包含真实拟合、速度计算、总路程计算、有效范围计算或 PySR 接入。

## 本地路径

- 项目目录：`D:\drawplot`
- Qt 目录：`D:\qt6`
- Qt Kit：`D:\qt6\6.11.1\mingw_64`
- CMake：`D:\qt6\Tools\CMake_64\bin\cmake.exe`
- Ninja：`D:\qt6\Tools\Ninja\ninja.exe`
- MinGW：`D:\qt6\Tools\mingw1310_64`

## 构建

```powershell
D:\qt6\Tools\CMake_64\bin\cmake.exe -S D:\drawplot -B D:\drawplot\build -G Ninja -DCMAKE_MAKE_PROGRAM=D:\qt6\Tools\Ninja\ninja.exe -DCMAKE_PREFIX_PATH=D:\qt6\6.11.1\mingw_64 -DCMAKE_CXX_COMPILER=D:\qt6\Tools\mingw1310_64\bin\g++.exe -DCMAKE_C_COMPILER=D:\qt6\Tools\mingw1310_64\bin\gcc.exe
D:\qt6\Tools\CMake_64\bin\cmake.exe --build D:\drawplot\build
```

构建产物：

```text
D:\drawplot\build\speed_curve_fitting_tool.exe
```

## 文档

- [指导大纲](./curve_fitting_tool_guideline.md)
- [TODO 文档树](./todo_docs/README.md)

