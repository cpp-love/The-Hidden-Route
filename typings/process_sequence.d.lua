---@meta

---实体的进程序列类。
---@class ProcessSequence
ProcessSequence = {}

---添加文本淡入进程。
---@param self ProcessSequence 实体的进程序列类。
---@param duration_ms number 以毫秒为单位的时间。
---@return ProcessSequence # 对象本身，用于链式调用。
function ProcessSequence:text_fade_in(duration_ms) end

---添加文本淡出进程。
---@param self ProcessSequence 实体的进程序列类。
---@param duration_ms number 以毫秒为单位的时间。
---@return ProcessSequence # 对象本身，用于链式调用。
function ProcessSequence:text_fade_out(duration_ms) end

---添加销毁实体进程。
---@param self ProcessSequence 实体的进程序列类。
---@return ProcessSequence # 对象本身，用于链式调用。
function ProcessSequence:destroy_entity() end

---添加等待进程。
---@param self ProcessSequence 实体的进程序列类。
---@param duration_ms number 以毫秒为单位的时间。
---@return ProcessSequence # 对象本身，用于链式调用。
function ProcessSequence:wait(duration_ms) end
