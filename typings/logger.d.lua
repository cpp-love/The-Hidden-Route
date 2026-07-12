---@meta

---日志类。
---@class Logger
Logger = {}

---@param ... any 要打印的参数。
function Logger.trace(...) end

---@param ... any 要打印的参数。
function Logger.debug(...) end

---@param ... any 要打印的参数。
function Logger.info(...) end

---@param ... any 要打印的参数。
function Logger.warn(...) end

---@param ... any 要打印的参数。
function Logger.error(...) end

---@param ... any 要打印的参数。
function Logger.critical(...) end
