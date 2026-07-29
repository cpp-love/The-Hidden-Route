---关卡开始时的脚本。
function on_level_started()
    game:add_text("W/A/S/D 或 上下左右 移动\nCrtl+Z 撤回", Vector2.new(400, 500), 35)
    game:add_text("历经全部，有始有终，方可结束", Vector2.new(400, 400), 35)

    Logger.info("function on_level_started in level1.lua is done.")
end

---玩家走到特殊线段上时的脚本。
---@param entity Entity 特殊线段对应的实体。
function on_special_segment_walked(entity)
    if not triggered_tag_0 and entity:query_tag(0) then
        triggered_tag_0 = true

        if text_entity ~= nil and text_entity:valid() then
            text_entity:create_process_sequence()
                :text_fade_out(200)
                :destroy_entity()
        end

        text_entity = game:add_text("“断路亦是通路，\n平面也似立体。”", Vector2.new(160, 260), 20)
        text_entity:create_process_sequence()
            :text_fade_in(200)

        Logger.info("Tag 0 in level1 is triggered.")
    end

    if not triggered_tag_1 and entity:query_tag(1) then
        triggered_tag_1 = true

        if text_entity ~= nil and text_entity:valid() then
            text_entity:create_process_sequence()
                :text_fade_out(200)
                :destroy_entity()
        end

        text_entity = game:add_text("“或许，应该转个弯？”", Vector2.new(420, 20), 20)
        text_entity:create_process_sequence()
            :text_fade_in(200)

        Logger.info("Tag 1 in level1 is triggered.")
    end
end
