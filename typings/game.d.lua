---@meta

---@class Game  -- 游戏的 API 类。
Game = {}

---创建实体。
---@return Entity # 创建的实体。
function Game:create_entity() end

---查询所有拥有指定标签的实体。
---@param tag_id integer 标签编号。
---@return Entity[] # 拥有指定标签的实体列表。
function Game:query_entities_with_tag(tag_id) end

---创建实体并为其添加文本。
---@param text string 文本内容。
---@param position Vector2 位置。
---@return Entity # 创建的文本实体。
function Game:add_text(text, position) end

---创建实体并为其添加文本。
---@param text string 文本内容。
---@param position Vector2 位置。
---@param character_size integer 字号。
---@return Entity # 创建的文本实体。
function Game:add_text(text, position, character_size) end

---@type Game
game = {}
