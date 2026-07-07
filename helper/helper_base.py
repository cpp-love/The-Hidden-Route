#!/usr/bin/env python3
"""
一些帮助组件。

Affiliated with the project: The Hidden Route.
project version: 0.1.0
file version: 0.1.0-1
last modified date: 2026-05-01
"""

from pathlib import Path
from enum import IntFlag, auto
from typing import Final


class PathType(IntFlag):
    NOT_EXIST = auto()  # 不存在
    DIRECTORY = auto()  # 文件夹
    FILE = auto()  # 文件
    SYMLINK = auto()  # 链接
    ALL = DIRECTORY | FILE | SYMLINK | NOT_EXIST  # 都可能


def filter_invalid_path(
    path: Path, path_type_should_be: PathType = PathType.ALL
) -> Path:
    """
    过滤非法的路径，并展开为绝对路径。

    若路径非法，则抛出异常。

    Args:
        path: 路径。
        path_type_should_be: 路径应该成为的类型的类型。

    Returns:
        保证合法的路径。

    Raises:
        ValueError: 路径非法。
    """

    if (
        ((PathType.NOT_EXIST in path_type_should_be) and (not path.exists()))
        or ((PathType.DIRECTORY in path_type_should_be) and path.is_dir())
        or ((PathType.FILE in path_type_should_be) and path.is_file())
        or ((PathType.SYMLINK in path_type_should_be) and path.is_symlink())
    ):
        return path.resolve()
    raise ValueError(f"路径 {path} 不符合 {path_type_should_be} 的要求。")


C_CPP_FILE_EXTENSION: Final[list[str]] = [
    ".cpp",
    ".hpp",
    ".c",
    ".h",
    ".cc",
    ".hh",
    ".cxx",
    ".hxx",
]  # C/C++ 文件扩展名
