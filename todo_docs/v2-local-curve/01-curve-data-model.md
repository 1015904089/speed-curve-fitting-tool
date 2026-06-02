# 01 曲线数据模型

## 目标

建立正式数据模型，让绘画点、控制点、滑块采样点、拟合结果和运动预览可以共享同一套语义。

## TODO

- [ ] 定义采样点结构。
- [ ] 定义控制点结构。
- [ ] 定义曲线类型枚举。
- [ ] 定义拟合结果结构。
- [ ] 定义曲线有效时间范围。
- [ ] 定义曲线求值接口。
- [ ] 定义曲线导数接口。
- [ ] 定义曲线显示格式。

## 核心指导代码

```cpp
enum class CurveFamily {
    Polynomial,
    SmoothStep,
    SmootherStep,
    Logistic,
    ExponentialApproach
};

struct MotionSample {
    double timeSeconds;
    double position;
};

struct FitResult {
    CurveFamily family;
    std::vector<double> params;
    double validStartTime;
    double validEndTime;
    double rmse;
    QString displayFormula;
};
```

所有曲线在显示层都按 `position = f(time)` 理解。速度由 `f'(time)` 得到。

