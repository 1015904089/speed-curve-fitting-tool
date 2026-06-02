# 04 曲线信息浮层

## 目标

在曲线拟合区域显示类似 Blender 的轻量数据信息浮层。

## TODO

- [ ] 在曲线拟合区域创建信息浮层。
- [ ] 支持折叠。
- [ ] 支持展开。
- [ ] 支持固定显示。
- [ ] 显示当前时间。
- [ ] 显示当前速度。
- [ ] 显示总路程。
- [ ] 显示有效范围。
- [ ] 缩放和平移时保持浮层位置稳定。
- [ ] 浮层不遮挡关键控制点。

## 显示字段

- 当前时间
- 当前速度
- 总路程
- 有效范围

## 不显示字段

- 速度范围

## 核心指导代码

```cpp
struct InfoOverlayData {
    double currentTime;
    double currentVelocity;
    double totalDistance;
    double validStartTime;
    double validEndTime;
};
```

浮层折叠时只显示一个简短标题，例如“曲线信息”。展开后再显示完整字段。

