# C and C++ 代码检查（clang-format / clang-tidy，LLVM 23）

格式化与静态分析使用仓库根目录的 `.clang-format` 与 `.clang-tidy`，工具版本统一为 LLVM 23
（本地 `D:\autotools\clangTidy`、CI cpp-linter-action、pre-commit 三端一致）。
**不得为通过检查而修改这两个配置文件。**

在完成任何改动 C/C++ 代码（src/ inc/ tests/）的任务之前：

1. 编译数据库与检查一条龙（会自动生成 `out/build/lint/compile_commands.json`）：
   ```
   scripts\lint.bat
   ```
   默认只检查相对 `origin/main` 的**改动行**（需先 `git add` 暂存改动）；若 `origin/main`
   缺失，先执行 `git fetch origin main`。
2. 修复它报告的所有发现。clang-format 的修复可用 `--fix` 自动套用；clang-tidy 的发现手工修。
   除非任务明确要求，不要添加 `NOLINT` 注释。
3. 重复第 1 步。日志以 `0 checks-failed` 结尾才算完成。
4. 全量基线扫描（不受改动行限制，报告写入 `out/tidy/report.txt`，用于阶段性地清存量）：
   ```
   scripts\lint.bat all
   ```

退出码陷阱：`cpp-linter` 即使发现检查失败也退出 0，结果以日志末尾的 `N checks-failed`
计数为准；`scripts\lint.bat` 已代为解析并转换为非零退出码。
