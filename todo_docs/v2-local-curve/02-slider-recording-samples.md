# 02 滑块录制采样

## 目标

将滑块拖动转换为一维位置随时间变化的采样点。

## TODO

- [ ] 按下滑块时清空或准备采样缓冲。
- [ ] 启动录制计时器。
- [ ] 拖动时记录时间和位置。
- [ ] 松开滑块时结束录制。
- [ ] 将归一化滑块位置映射为真实位置。
- [ ] 支持位置最小值和最大值设置。
- [ ] 对采样点进行降采样。
- [ ] 对异常抖动点进行过滤。
- [ ] 将采样轨迹同步显示到曲线拟合区域。

## 采样规则

- 滑块是一维运动。
- 滑块位置映射为一维位置采样。
- 曲线拟合区域的 Y 轴表示速度，后续由位置采样差分或拟合导数得到。
- 时间从鼠标按下开始。
- 录制结束时间为鼠标松开时刻。

## 核心指导代码

```cpp
double positionFromSlider(double normalized, double minPosition, double maxPosition) {
    return minPosition + normalized * (maxPosition - minPosition);
}
```
