---关卡加载时的脚本。
function on_level_started()
    game:add_text("W/A/S/D 或 上下左右 移动，\nCrtl+Z 撤回。", Vector2.new(600, 500))

    Logger.info("The function `on_level_started` is done.")
end

---玩家走到特殊线段上时的脚本。
---@param entity Entity 特殊线段对应的实体。
function on_special_segment_walked(entity)
    if not triggered_tag_0 and entity:query_tag(0) then
        triggered_tag_0 = true

        if text_entity ~= nil and text_entity:valid() then
            text_entity:destroy()
        end

        text_entity = game:add_text("“断路亦是通路，\n平面也似立体。”", Vector2.new(240, 260), 20)

        Logger.info("The tag 0 is triggered.")
    end

    if not triggered_tag_1 and entity:query_tag(1) then
        triggered_tag_1 = true

        if text_entity ~= nil and text_entity:valid() then
            text_entity:destroy()
        end

        text_entity = game:add_text("“或许，应该转个弯？”", Vector2.new(500, 20), 20)

        Logger.info("The tag 1 is triggered.")
    end
end
