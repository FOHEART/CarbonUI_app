# CarbonUI_app

## 一键脚本（根目录 buildDeploy.bat + scripts/）

同一份 `.bat` 文件在 Windows（cmd）与 Linux（POSIX sh，经 WSL）下均可执行。

| 入口 | 作用 |
| --- | --- |
| `buildDeploy.bat`（根目录，双击） | 检测 VS 工具链（一套倒计时直跑 / 多套菜单选择）→ 清理 out/ → 构建 → 测试 → 文档 → 打包 |
| `scripts/build.bat [2022/2026] [Debug/Release]` | 配置 + 编译 + 部署（构建树 out/build/x64-*，运行时汇总 out/install/x64-*） |
| `scripts/deploy.bat [Debug/Release]` | 只做运行时汇总（exe + Qt DLL + fonts → out/install/x64-*） |
| `scripts/test.bat [Debug/Release] [ctest -R 正则]` | 单元/界面测试（独立构建树 out/build/tdd-tests；过滤命中 0 条会显式报错） |
| `scripts/gen_docs.bat` | Doxygen 文档（告警即失败，日志 out/doxygen/doxygen_warnings.log） |
| `scripts/package.bat [版本号]` | 打可分发源码 ZIP（out/pack/CarbonUI_app_v*.zip，泄漏校验 + robocopy/7z 双端回退） |
| `scripts/git_pull_latest.bat` | 拉取 origin/main 最新代码 |
| `scripts/open_in_vscode.bat` | 用 VSCode 打开工程根目录 |
| `scripts/ci/check_ci.bat` | 查询/等待 GitHub Actions 结果（`-Watch` 等待结束，`-RunId id` 看单次失败日志） |
| `scripts/ci/rerun_failed.bat` | 重跑失败 job（`-RunId id` 指定运行，`-DryRun` 只打印） |
| `scripts/ci/pull_artifacts.bat` | 拉取 CI 产物（默认 out/ci-artifacts） |
| `scripts/wsl/wsl.bat [sync/build/test/docs/verify]` | 本机 WSL 真 Linux 预验证（家目录副本内跑 POSIX 分支） |

环境变量：

| 变量 | 默认值 | 说明 |
| --- | --- | --- |
| `QT_PREFIX` | `D:/Qt5.15.2/5.15.2/msvc2019_64` | Windows 下 Qt 前缀（构建/测试/部署共用） |
| `DOXYGEN` | `D:/autotools/doxygen-1.11.0/doxygen.exe` | 文档生成用 doxygen 路径（Linux 用 PATH） |
| `WSL_DISTRO` / `WSL_DEST` / `WSL_QTDIR` | `Ubuntu-24.04` / `~/CarbonUI_app` / 系统 Qt5 | WSL 预验证相关 |

构建、测试与打包产物全部收敛在 `out/`（构建树、install、pack、doxygen、ci-artifacts）；
CMake 运行时输出目录为仓库根下的 `output`（deploy.bat 从该目录汇总），两者均已 gitignore。
