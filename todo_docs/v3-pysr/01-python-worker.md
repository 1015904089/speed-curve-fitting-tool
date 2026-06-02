# 01 Python Worker 架构

## 目标

通过独立 Python Worker 调用 PySR，避免 C++ 主界面阻塞。

## TODO

- [ ] 设计 Python Worker 启动方式。
- [ ] 设计 Worker 输入。
- [ ] 设计 Worker 输出。
- [ ] C++ 主程序异步启动 Worker。
- [ ] 主程序显示拟合中状态。
- [ ] Worker 完成后回传结果。
- [ ] Worker 失败后回传错误。
- [ ] 支持取消正在运行的拟合任务。
- [ ] 支持任务超时。

## 核心指导代码

C++ 侧以进程方式管理 Worker：

```cpp
// 推荐方向
// QProcess start: python fit_worker.py --input request.json --output result.json
// finished 后读取 result.json
```

不要在 UI 线程里等待 PySR 完成。界面只监听进程状态和结果文件。

