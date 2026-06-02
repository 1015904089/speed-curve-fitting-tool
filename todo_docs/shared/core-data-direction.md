# 核心数据方向

## 数据方向

第一版只需要保存界面状态和用户交互轨迹。第二版开始再引入真实曲线数据模型。

核心物理含义固定为：

- 横轴：时间。
- 纵轴：位置。
- 滑块：一维位置输入。

## V1 临时数据

V1 的数据只为界面交互服务：

- 绘画轨迹点
- 控制点占位
- 滑块当前位置
- 滑块按下状态
- 布局分隔条比例
- 曲线区缩放和平移状态

## V2 正式数据

V2 开始引入正式数据：

- 采样点
- 控制点
- 曲线模型
- 拟合结果
- 运动预览状态
- 信息浮层状态

## 核心指导代码

以下结构用于统一开发方向，不要求 V1 全部实现。

```cpp
struct MotionSample {
    double timeSeconds;
    double position;
};

struct CurveViewport {
    double pixelsPerSecond;
    double pixelsPerUnit;
    double originX;
    double originY;
};

struct CurveInfoOverlayState {
    bool collapsed;
    double currentTime;
    double currentVelocity;
    double totalDistance;
    double validStartTime;
    double validEndTime;
};
```

V1 可以只实现 `CurveViewport` 和简单的绘画点集合。V2 再完善 `MotionSample` 和 `CurveInfoOverlayState`。

