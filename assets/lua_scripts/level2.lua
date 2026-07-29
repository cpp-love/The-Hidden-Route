---关卡开始时的脚本。
function on_level_started()
    game:add_text("铭记：\n路是有方向的；\n独树一帜，走出独特的路线", Vector2.new(400, 500), 35)
    game:add_text("03x1xxx7", Vector2.new(774, 595), 10)
    game:add_text("下面亦有一段路", Vector2.new(420, 265), 15)
    tag_sequence = {}
    is_tag_visited = {}
    Logger.info("function on_level_started in level2.lua is done.")
end

---玩家走到特殊线段上时的脚本。
---@param entity Entity 特殊线段对应的实体。
function on_special_segment_walked(entity)
    local tags = entity:get_tags()
    for index, tag in ipairs(tags) do
        if is_tag_visited[tag] == nil then
            is_tag_visited[tag] = true
            table.insert(tag_sequence, tag)
            Logger.info(string.format("Tag %d in level2 is triggered.", tag))
        end
    end
end

---关卡结束时的脚本。
function on_level_finished()
    Logger.info("function on_level_finished in level2.lua is done.")
    if tag_sequence[1] == 0 and tag_sequence[2] == 3 and tag_sequence[4] == 1 and tag_sequence[8] == 7 then
        Logger.info("special_level is locked by level2.lua.")
        return { "special_level" }
    end
end
