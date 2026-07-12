---关卡加载时的脚本。
function on_level_started()
    local entities = game:query_entities_with_tag(0)
    assert(#entities == 1, "错误：游戏关卡提供的带有 0 标签的实体不是仅有一个。")

    entity_with_tag_1 = entities[1]
    triggered_tag_0 = false
    assert(entity_with_tag_1:valid())
    assert(entity_with_tag_1:query_tag(0))

    print("message from lua in on_level_started!")
end

---玩家走到特殊线段上时的脚本。
---@param entity Entity 特殊线段对应的实体。
function on_special_segment_walked(entity)
    if not triggered_tag_0 and entity == entity_with_tag_1 then
        triggered_tag_0 = true
        text_entity = game:add_text("你好呀！", Vector2.new(200, 250))
        print("message from lua in on_special_segment_walked!")
    end
end
