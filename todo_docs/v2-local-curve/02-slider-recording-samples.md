# 02 滑块录制采样

## 目标

将滑块拖动转换为一维位置随时间变化的采样点。

## TODO

- [ ] 按下滑块时清空或准备采样缓冲。
- [ ] 启动录制计时器。
- [ ] 拖动时记录时间和位置。
- [ ] 松开滑块时结束录制。
- [ ] 将归一化滑块位置映射为真实位置。
- [ ] 支持直线区域总长度设置。
- [ ] 对采样点进行降采样。
- [ ] 对异常抖动点进行过滤。
- [ ] 将采样轨迹同步显示到曲线拟合区域。

## 采样规则

- 滑块是一维运动。
- 滑块位置映射到纵轴位置。
- 时间从鼠标按下开始。
- 录制结束时间为鼠标松开时刻。

## 核心指导代码

```cpp
double positionFromSlider(double normalized, double lineLength) {
    return normalized * lineLength;
}
```

若后续需要支持中心原点运动，可以改成：

```cpp
double centeredPositionFromSlider(double normalized, double lineLength) {
    return (normalized - 0.5) * lineLength;
}
```

