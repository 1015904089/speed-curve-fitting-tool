# 02 JSON 通信协议

## 目标

定义 C++ 与 Python Worker 的稳定通信格式。

## 请求字段

- requestId
- curveFamily
- samples
- maxComplexity
- timeoutSeconds
- operators

## 返回字段

- requestId
- ok
- bestExpression
- displayFormula
- rmse
- complexity
- candidates
- errorMessage

## 核心指导代码

请求示例：

```json
{
  "requestId": "fit-001",
  "curveFamily": "AutoPySR",
  "samples": [
    { "timeSeconds": 0.0, "position": 0.0 },
    { "timeSeconds": 0.5, "position": 12.4 },
    { "timeSeconds": 1.0, "position": 20.0 }
  ],
  "maxComplexity": 12,
  "timeoutSeconds": 30,
  "operators": {
    "binary": ["+", "-", "*"],
    "unary": ["exp", "square", "cube"]
  }
}
```

返回示例：

```json
{
  "requestId": "fit-001",
  "ok": true,
  "bestExpression": "20.0 * (1 - exp(-3.2 * t))",
  "displayFormula": "f(t) = 20.00 * (1 - exp(-3.20t))",
  "rmse": 0.12,
  "complexity": 8,
  "candidates": [],
  "errorMessage": ""
}
```

