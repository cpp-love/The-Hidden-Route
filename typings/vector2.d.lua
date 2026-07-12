---@meta

---2D 向量类，多用来表示位置。
---@class Vector2
---x 坐标。
---@field x number
---y 坐标。
---@field y number
---加法运算。
---@operator add(Vector2):Vector2
---减法运算。
---@operator sub(Vector2):Vector2
---乘法运算。
---@operator mul(number):Vector2
---@operator mul(number):Vector2
---除法运算。
---@operator div(number):Vector2
---@operator div(number):unknown
---一元减法。
---@operator unm:Vector2
Vector2 = {}

---默认构造函数。
---@return Vector2 # 返回一个新的 Vector2 对象，值为 `(0, 0)`。
function Vector2.new() end

---构造函数。
---@param x number x 坐标。
---@param y number y 坐标。
---@return Vector2 # 返回一个新的 Vector2 对象。
function Vector2.new(x, y) end
