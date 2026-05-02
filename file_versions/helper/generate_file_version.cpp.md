\page file_versions generate_file_version.cpp 版本历史
\tableofcontent

# generate_file_version.cpp 版本历史

## \[0.1.0-1] - 2025-05-01 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加两种使用方法：
   1. `./generate_file_version <workspace_folder> <file> <authors>`
      - `<workspace_folder>` 是工作区目录
      - `<file>` 是文件（夹）路径
      - `<authors>` 是作者名称
      - `<file_relative>` 是文件或文件夹内的子文件相对工作区的路径（运行时根据 `<workspace_folder>` 和 `<file>` 推导出来的）
      - 生成的文件将保存在 `file_versions/<file_relative>`
   2. 直接运行 `./generate_file_version` 后根据提示操作
   - 生成的文件内容将基于 `<workspace_folder>/helper/file_version.template.md` 模板文件
