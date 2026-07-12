---@meta

---实体类。
---@class Entity
Entity = {}

---判断实体是否合法。
---@param self Entity 实体类。
---@return boolean # 实体是否合法。
function Entity:valid() end

---销毁实体。
---@param self Entity 实体类。
function Entity:destroy() end

---为实体添加标签。
---@param self Entity 实体类。
---@param tag_id integer 标签编号。
---@return boolean # 添加是否成功。
function Entity:add_tag(tag_id) end

---从实体移除标签。
---@param self Entity 实体类。
---@param tag_id integer 标签编号。
---@return boolean # 移除是否成功。
function Entity:remove_tag(tag_id) end

---查询实体是否拥有指定标签。
---@param self Entity 实体类。
---@param tag_id integer 标签编号。
---@return boolean # 是否拥有。
function Entity:query_tag(tag_id) end
