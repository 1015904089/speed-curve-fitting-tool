# 03 本地函数族拟合

## 目标

支持本地快速拟合，不依赖 PySR。

## TODO

- [ ] 支持低阶多项式拟合。
- [ ] 支持 SmoothStep。
- [ ] 支持 SmootherStep。
- [ ] 支持 Logistic。
- [ ] 支持指数趋近。
- [ ] 支持曲线类型选择。
- [ ] 支持拟合误差显示。
- [ ] 支持公式显示，小数最多 2 位。
- [ ] 支持拟合失败提示。

## 函数族

### 低阶多项式

用于通用曲线：

```text
f(t) = a0 + a1*t + a2*t^2 + ... + an*t^n
```

### SmoothStep

用于平滑起停：

```text
u = clamp(t / T, 0, 1)
f(t) = y0 + (y1 - y0) * (3u^2 - 2u^3)
```

### Logistic

用于 S 形曲线：

```text
f(t) = ymin + A / (1 + exp(-k * (t - t0)))
```

### 指数趋近

用于逐渐接近目标：

```text
f(t) = yinf - (yinf - y0) * exp(-k * t)
```

## 核心指导代码

统一求值接口建议如下：

```cpp
class CurveEvaluator {
public:
    virtual double valueAt(double timeSeconds) const = 0;
    virtual double velocityAt(double timeSeconds) const = 0;
    virtual QString formulaText(int maxDecimals) const = 0;
};
```

显示保留 2 位只影响 `formulaText`，不能影响内部计算精度。

