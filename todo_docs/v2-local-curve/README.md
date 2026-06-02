# V2 本地曲线能力

## 目标

V2 在 V1 的 UI 和交互基础上，加入真实曲线数据、本地函数族拟合、运动预览和曲线分析。

验收目标：

```text
用户绘制曲线或拖动滑块后，可以得到本地拟合公式。
曲线区能显示当前时间、当前速度、总路程、有效范围。
运动预览能根据公式驱动物体移动。
```

## 文档索引

- [01 曲线数据模型](./01-curve-data-model.md)
- [02 滑块录制采样](./02-slider-recording-samples.md)
- [03 本地函数族拟合](./03-local-fit-functions.md)
- [04 曲线信息浮层](./04-curve-info-overlay.md)
- [05 运动预览与封闭区域](./05-motion-preview-and-area.md)
- [06 皮肤资源与预览解耦](./06-skinning-and-preview-assets.md)
- [07 V2 验收清单](./07-v2-acceptance.md)

## V2 边界

- 不接 PySR。
- 不做 Python Worker。
- 不做多候选符号回归。
