---关卡开始时的脚本。
function on_level_started()
    game:add_text("多多益善", Vector2.new(130, 550), 40)
    Logger.info("function on_level_started in special_level.lua is done.")
end
