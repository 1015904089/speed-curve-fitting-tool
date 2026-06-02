# V3 PySR 自动拟合

## 目标

V3 在 V2 的稳定曲线能力上接入 PySR，让工具支持自动符号回归。

验收目标：

```text
用户选择“自动 PySR”后，软件能把采样点发送给 PySR。
PySR 在后台生成候选公式。
主程序不卡顿，并能显示最佳公式、误差、复杂度和运动预览。
```

## 文档索引

- [01 Python Worker 架构](./01-python-worker.md)
- [02 JSON 通信协议](./02-json-protocol.md)
- [03 PySR 拟合策略](./03-pysr-fit-strategy.md)
- [04 结果展示与失败处理](./04-results-and-errors.md)
- [05 V3 验收清单](./05-v3-acceptance.md)

## V3 边界

- PySR 只在绘制完成、录制完成或点击重新拟合时运行。
- PySR 不用于鼠标拖动中的实时预览。
- PySR 不用于每帧刷新。

