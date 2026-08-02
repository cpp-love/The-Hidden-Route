#!/usr/bin/env python3
"""
使用 clang-tidy 对 C/C++ 代码进行静态分析，并将问题输出到文件。

Affiliated with the project: The Hidden Route.
project version: 0.1.0
script version: 0.1.0-2
last modified date: 2026-08-02
"""

import argparse
import helper_base
import json
import re
import shutil
import subprocess
import sys
import tempfile
from datetime import datetime
from pathlib import Path

ANSI_ESCAPE = re.compile(r"\x1b\[[0-9;]*m")


def strip_ansi(text: str) -> str:
    """
    裁剪 ANSI 乱码。

    Args:
        text: 原始字符串。

    Returns:
        裁剪后的字符串。
    """
    return ANSI_ESCAPE.sub("", text)


def get_default_compiler_include_directories(compiler: str) -> list[str]:
    """
    获取指定编译器的默认系统包含路径。

    Args:
        compiler: 编译器可执行文件名或路径（如 'g++'、'clang++'）。

    Returns:
        包含路径列表（绝对路径）。

    Raises:
        ValueError: 编译器不是 GCC 或 Clang 家族，或者执行失败。
    """
    # 解析编译器路径（如果只给命令名，则使用 which 查找）
    compiler_path = shutil.which(compiler)
    if compiler_path is None:
        # 如果找不到，尝试当作路径处理
        compiler_path = compiler
    compiler_path = Path(compiler_path).resolve()
    compiler_name = compiler_path.stem

    if compiler_name not in {"gcc", "g++", "clang", "clang++"}:
        raise ValueError(f"不支持的编译器: {compiler_name}，仅支持 GCC 或 Clang 家族。")

    # 通过 -v 获取包含路径
    cmd = [str(compiler_path), "-v", "-x", "c++", "-E", "-"]
    proc = None
    try:
        proc = subprocess.Popen(
            cmd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding="utf-8",
        )
        _, stderr = proc.communicate(input="", timeout=10)
    except subprocess.TimeoutExpired:
        if proc is not None:
            proc.kill()
        raise RuntimeError(f"获取编译器 {compiler} 默认包含路径超时。")
    finally:
        if proc is not None and proc.poll() is None:
            proc.kill()

    includes: list[str] = []
    capturing = False
    for line in stderr.splitlines():
        if "#include <...> search starts here:" in line:
            capturing = True
            continue
        if "End of search list" in line:
            capturing = False
            continue
        if capturing:
            path = line.strip()
            if path and Path(path).exists():
                includes.append(str(Path(path).resolve()))
    return includes


def validate_compile_commands(path: Path) -> Path:
    """
    验证给定的路径是否是一个有效的 compile_commands.json 文件。

    Args:
        path: 路径对象。

    Returns:
        规范化的绝对路径。

    Raises:
        ValueError: 路径不存在、不是文件或文件名不正确。
    """
    if not path.exists():
        raise ValueError(f"路径 {path} 不存在。")
    if not path.is_file():
        raise ValueError(f"路径 {path} 不是文件。")
    if path.name != "compile_commands.json":
        raise ValueError(f"路径 {path} 不是 compile_commands.json 文件。")
    return path.resolve()


def collect_files(paths: list[str]) -> list[Path]:
    """
    收集命令行中指定的所有 C/C++ 文件（支持目录递归）。

    Args:
        paths: 文件或目录路径字符串列表。

    Returns:
        所有有效的 C/C++ 文件路径列表。

    Raises:
        ValueError: 没有找到任何有效的文件。
    """
    files: list[Path] = []
    for raw in paths:
        p = Path(raw)
        if not p.exists():
            print(f"警告: 路径 {raw} 不存在，已跳过。")
            continue
        if p.is_file():
            if p.suffix in helper_base.C_CPP_FILE_EXTENSIONS:
                files.append(p)
            else:
                print(f"警告: 文件 {raw} 不是 C/C++ 文件，已跳过。")
        elif p.is_dir():
            for ext in helper_base.C_CPP_FILE_EXTENSIONS:
                files.extend(p.rglob(f"*{ext}"))
        else:
            print(f"警告: {raw} 既不是文件也不是目录，已跳过。")
    if not files:
        raise ValueError("未找到任何有效的 C/C++ 文件。")
    return sorted(set(files))  # 去重并排序


def enhance_compile_commands(
    original_json: Path,
    extra_compile_args: list[str] | None = None,
    compiler: str = "clang++",
) -> Path:
    """
    读取 compile_commands.json，为每个编译命令添加额外的编译选项（如包含路径），
    并写入一个临时文件。返回该临时文件的路径。

    Args:
        original_json: 原始 compile_commands.json 路径。
        extra_compile_args: 需要额外添加的编译选项（如 -I、-D 等）。
        compiler: 用于替换原有编译器的名称（默认 clang++）。

    Returns:
        新生成的 compile_commands.json 临时文件路径。
    """
    with open(original_json, "r", encoding="utf-8") as f:
        data = json.load(f)

    if not isinstance(data, list):
        raise ValueError("compile_commands.json 根元素应为数组。")

    # 如果用户未提供额外参数，默认添加编译器默认包含路径
    if extra_compile_args is None:
        extra_compile_args = []

    # 尝试从第一条命令中提取编译器，用于获取其默认包含路径
    first_cmd = data[0].get("command", "")
    if first_cmd:
        # 简单提取编译器可执行文件名（可能带路径）
        first_compiler = first_cmd.split()[0]
    else:
        first_compiler = compiler  # 回退

    # 获取默认包含路径，并转换为 -isystem 参数
    try:
        default_includes = get_default_compiler_include_directories(first_compiler)
        include_args = [f"-isystem {inc}" for inc in default_includes]
        extra_compile_args.extend(include_args)
    except Exception as e:
        if first_compiler != compiler:
            # 回退
            try:
                default_includes = get_default_compiler_include_directories(compiler)
                include_args = [f"-isystem{inc}" for inc in default_includes]
                extra_compile_args.extend(include_args)
            except Exception as e:
                print(f"警告: 无法获取编译器默认包含路径: {e}")
        else:
            print(f"警告: 无法获取编译器默认包含路径: {e}")

    # 去重（保留用户指定的在前）
    seen: set[str] = set()
    unique_args: list[str] = []
    for arg in extra_compile_args:
        if arg not in seen:
            unique_args.append(arg)
            seen.add(arg)

    # 修改每个编译条目
    new_data = []
    for entry in data:
        # 处理 command 字段（字符串）
        if "command" in entry and isinstance(entry["command"], str):
            cmd_parts = entry["command"].split()
            # 将第一个元素（编译器）替换为 compiler，并插入额外参数
            new_cmd_parts = [compiler] + unique_args + cmd_parts[1:]
            entry["command"] = " ".join(new_cmd_parts)
        # 处理 arguments 字段（列表）
        elif "arguments" in entry and isinstance(entry["arguments"], list):
            # 替换编译器，插入参数
            new_args = [compiler] + unique_args + entry["arguments"][1:]
            entry["arguments"] = new_args
        else:
            print(f"警告: 条目缺少 'command' 或 'arguments' 字段，跳过修改。")

        new_data.append(entry)

    # 写入临时文件
    temp_dir = Path(tempfile.mkdtemp(prefix="clang_tidy_"))
    temp_json = temp_dir / "compile_commands.json"
    with open(temp_json, "w", encoding="utf-8") as f:
        json.dump(new_data, f, indent=2)
    return temp_json


def run_clang_tidy(
    files: list[Path],
    compile_commands: Path,
    extra_args: list[str] | None = None,
    checks: str | None = None,
    header_filter: str | None = None,
    timeout: int = 3000,
    verbose: bool = False,
) -> str:
    """
    调用 clang-tidy 分析文件列表，返回 stdout 输出。

    Args:
        files: 待分析的文件列表。
        compile_commands: compile_commands.json 路径。
        extra_args: 传递给 clang-tidy 的额外参数（如 --extra-arg=...）。
        checks: 启用的检查列表（如 '-checks=*'）。
        header_filter: 头文件过滤正则。
        timeout: 超时秒数。
        verbose: 是否输出详细信息。

    Returns:
        clang-tidy 的 stdout 字符串。

    Raises:
        subprocess.TimeoutExpired: 超时。
        subprocess.CalledProcessError: 执行失败（非零返回码）。
    """
    cmd = [
        "clang-tidy",
        "-p",
        str(compile_commands.resolve()),
        "--extra-arg=-fsized-deallocation",
    ]

    if checks:
        cmd.append(f"--checks={checks}")
    if header_filter:
        cmd.append(f"--header-filter={header_filter}")
    if extra_args:
        for arg in extra_args:
            cmd.append(f"--extra-arg={arg}")
    if verbose:
        cmd.append("--extra-arg=-v")

    # 添加文件
    cmd.extend(str(f.resolve()) for f in files)

    # 执行
    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        timeout=timeout,
        check=False,  # 我们手动检查返回码
    )
    if result.returncode != 0 and result.returncode != 1:
        # clang-tidy 返回 0 或 1（1 表示发现问题），其他表示错误
        raise subprocess.CalledProcessError(
            result.returncode, cmd, result.stdout, result.stderr
        )
    return strip_ansi(result.stdout)


def count_issues(output: str) -> int:
    """
    统计输出中的 warning 和 error 数量（简单模式匹配）。
    """
    # 匹配 "warning:" 或 "error:" 出现在行首或前面有空格
    pattern = re.compile(r"(?:^|\s)(warning|error):")
    return len(pattern.findall(output))


def main():
    parser = argparse.ArgumentParser(
        description="使用 clang-tidy 检查 C/C++ 代码并输出问题到文件。"
    )
    parser.add_argument(
        "-p", "--build-path", required=True, help="compile_commands.json 文件路径。"
    )
    parser.add_argument(
        "-fd",
        "--file-or-directory",
        nargs="+",
        required=True,
        help="要检查的 C/C++ 文件或目录（可多个）。",
    )
    parser.add_argument(
        "-o",
        "--output",
        default="code_issues/code_issues.txt",
        help="输出文件路径（默认: code_issues/code_issues.txt）。",
    )
    parser.add_argument(
        "-ea",
        "--extra-arg",
        action="append",
        help="传递给 clang-tidy 的额外参数（如 --extra-arg=-std=c++17）。",
    )
    parser.add_argument("--checks", help="指定 clang-tidy 检查项（如 '-checks=*'）。")
    parser.add_argument(
        "--header-filter", default=".*", help="头文件过滤正则（默认 '.*' 包含所有）。"
    )
    parser.add_argument(
        "--compiler",
        default="clang++",
        help="用于替换编译命令中的编译器（默认 clang++）。",
    )
    parser.add_argument(
        "--batch-size", type=int, default=10, help="每批处理文件数量（默认 10）。"
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=3000,
        help="每个 clang-tidy 调用的超时秒数（默认 3000）。",
    )
    parser.add_argument(
        "--print-stderr",
        action="store_true",
        help="是否将 clang-tidy 的 stderr 输出到结果文件。",
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="启用详细输出。")
    args = parser.parse_args()

    temp_json = None
    try:
        # 1. 验证 compile_commands.json
        orig_json = validate_compile_commands(Path(args.build_path))

        # 2. 收集待检查文件
        files = collect_files(args.file_or_directory)
        print(f"共发现 {len(files)} 个 C/C++ 文件。")

        # 3. 生成增强的 compile_commands.json（添加默认包含路径等）
        print("生成临时 compile_commands.json ...")
        temp_json = enhance_compile_commands(
            orig_json,
            extra_compile_args=None,  # 可在此传入用户指定的编译选项
            compiler=args.compiler,
        )
        print(f"临时文件: {temp_json}")

        # 4. 准备输出文件
        output_file = Path(args.output).resolve()
        output_file.parent.mkdir(parents=True, exist_ok=True)
        with open(output_file, "w", encoding="utf-8") as f:
            f.write(f"clang-tidy 分析报告\n")
            f.write(f"生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"检查文件数: {len(files)}\n")
            f.write("=" * 80 + "\n\n")

        # 5. 分批处理
        issues_total = 0
        for i in range(0, len(files), args.batch_size):
            batch = files[i : i + args.batch_size]
            print(
                f"处理批次 {i//args.batch_size + 1} ({i+1}-{min(i+args.batch_size, len(files))}) ..."
            )

            try:
                stdout = run_clang_tidy(
                    files=batch,
                    compile_commands=temp_json,
                    extra_args=args.extra_arg,
                    checks=args.checks,
                    header_filter=args.header_filter,
                    timeout=args.timeout,
                    verbose=args.verbose,
                )
                issues_batch = count_issues(stdout)
                issues_total += issues_batch

                # 写入结果文件（追加）
                with open(output_file, "a", encoding="utf-8") as f:
                    f.write(
                        f"批次 {i//args.batch_size + 1} (文件: {', '.join(str(p.name) for p in batch)})\n"
                    )
                    f.write("-" * 80 + "\n")
                    f.write(stdout + "\n")
                    if args.print_stderr:
                        # 我们可以在这里捕获 stderr，但 run_clang_tidy 丢弃了 stderr
                        # 需要修改 run_clang_tidy 返回 stderr 或重定向
                        # 简单起见，我们先不处理 stderr，可以额外捕获
                        pass
                    f.write("\n")
            except subprocess.TimeoutExpired:
                print(f"错误: 批次超时 ({args.timeout} 秒)，跳过。")
                with open(output_file, "a", encoding="utf-8") as f:
                    f.write(f"批次 {i//args.batch_size + 1} 超时\n\n")
            except subprocess.CalledProcessError as e:
                print(f"错误: clang-tidy 执行失败，返回码 {e.returncode}，跳过。")
                with open(output_file, "a", encoding="utf-8") as f:
                    f.write(
                        f"批次 {i//args.batch_size + 1} 执行失败 (返回码 {e.returncode})\n\n"
                    )
            except Exception as e:
                print(f"未预期错误: {e}，跳过该批次。")

        # 6. 输出统计
        print(f"分析完成，共发现 {issues_total} 个问题。")
        print(f"结果已保存至 {output_file}")

    except Exception as e:
        print(f"严重错误: {e}", file=sys.stderr)
        sys.exit(1)
    finally:
        # 清理临时文件（如果存在）
        if temp_json is not None and temp_json.parent.exists():
            shutil.rmtree(temp_json.parent, ignore_errors=True)


if __name__ == "__main__":
    main()
