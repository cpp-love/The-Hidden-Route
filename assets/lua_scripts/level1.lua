---关卡加载时的脚本。
function on_level_started()
    local entities_with_tag_0 = game:query_entities_with_tag(0)
    assert(#entities_with_tag_0 == 1, "错误：游戏关卡提供的带有 0 标签的实体不是仅有一个。")

    entity_with_tag_0 = entities_with_tag_0[1]
    triggered_tag_0 = false
    assert(entity_with_tag_0:valid(), "错误：方法 `Game.query_entities_with_tag` 内部实现有问题。")
    assert(entity_with_tag_0:query_tag(0), "错误：方法 `Game.query_entities_with_tag` 内部实现有问题。")

    local entities_with_tag_1 = game:query_entities_with_tag(1)
    assert(#entities_with_tag_1 == 1, "错误：游戏关卡提供的带有 1 标签的实体不是仅有一个。")

    entity_with_tag_1 = entities_with_tag_1[1]
    triggered_tag_1 = false
    assert(entity_with_tag_1:valid(), "错误：方法 `Game.query_entities_with_tag` 内部实现有问题。")
    assert(entity_with_tag_1:query_tag(1), "错误：方法 `Game.query_entities_with_tag` 内部实现有问题。")

    Logger.info("The function `on_level_started` is done.")
end

---玩家走到特殊线段上时的脚本。
---@param entity Entity 特殊线段对应的实体。
function on_special_segment_walked(entity)
    if not triggered_tag_0 and entity == entity_with_tag_0 then
        triggered_tag_0 = true

        if text_entity ~= nil and text_entity:valid() then
            print("警告：文本实体已存在，可能是上一次触发的文本还未被销毁，或是关卡有 bug。")
            text_entity:destroy()
        end

        text_entity = game:add_text("“断路亦是通路，\n平面也似立体。”", Vector2.new(130, 230), 20)

        Logger.info("The tag 0 is triggered.")
    end

    if not triggered_tag_1 and entity == entity_with_tag_1 then
        triggered_tag_1 = true

        if text_entity ~= nil and text_entity:valid() then
            text_entity:destroy()
        end

        text_entity = game:add_text("“或许，应该转个弯？”", Vector2.new(400, 13), 20)

        Logger.info("The tag 1 is triggered.")
    end
end
