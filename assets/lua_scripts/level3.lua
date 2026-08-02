---关卡开始时的脚本。
function on_level_started()
    game:add_text("回头是岸；路中之路", Vector2.new(360, 500), 40)
    Logger.info("function on_level_started in level3.lua is done.")
end
