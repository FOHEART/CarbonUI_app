# D:\autotools 命令行工具清单

> 来源：`D:\autotools\autotools.md`（2026-11 快照）。多数工具**不在 PATH**，
> 调用时使用"位置"列的绝对路径。本清单供工程脚本、CI 与日常按需取用。

| 工具 | 用途 | 位置 |
| --- | --- | --- |
| pandoc | 文档格式转换（Markdown ↔ Word/HTML/PDF 等） | D:\autotools\Pandoc |
| wkhtmltopdf | HTML 转 PDF / 图片 | D:\autotools\wkhtmltopdf\bin |
| ffmpeg | 音视频转码、剪辑与流处理 | D:\autotools\ffmpeg\bin |
| python | Python 运行环境 | C:\python |
| Gost (GO Simple Tunnel) | 端口转发 / 代理隧道 | D:\autotools\gost |
| wget | 命令行文件下载 | D:\autotools\wget |
| aria2 (aria2c) | 多协议多线程命令行下载工具（HTTP/HTTPS/FTP/BT/磁力） | D:\autotools\aria2-1.37.0-win-64bit-build1 |
| MuPDF (mutool) | PDF 查看与处理（mutool 子命令） | D:\autotools\mupdf |
| iperf3 | 网络带宽与性能测试 | D:\autotools\iperf |
| pdftohtml (Poppler) | PDF 转换工具集（pdftotext / pdftoppm / pdftocairo / pdfimages 等） | D:\autotools\pdftohtml |
| claude-skills | Claude 技能包（pdf / pptx / video-downloader） | D:\autotools\claude-skills |
| 7-Zip Extra (7za) | 命令行压缩 / 解压（7z / zip / gz / xz / tar 等），ARM64 版本见 arm64 子目录 | D:\autotools\7z |
| Doxygen (doxygen / doxywizard) | 源码文档生成（C/C++ 等，本工程 Doxyfile 零告警验收用 1.11.0） | D:\autotools\doxygen-1.11.0 |
| RhinoAI 配置包 | Rhino 8 三维建模的 MCP 连接 + 技能包 | D:\autotools\rhinoai |
| gallery-dl | 命令行图集下载（1000+ 站点），独立版 v1.32.18 | D:\autotools\gallery_dl |
| Crawl4AI | 网页爬取与数据提取技能包（Crawl4AI v0.7.4+） | D:\autotools\crawl4ai |
| clangTidy (LLVM/Clang 23) | C/C++ 静态分析、格式化与依赖扫描（clang-tidy / clang-format 等，带 `_23_windows-amd64` 后缀，需绝对路径） | D:\autotools\clangTidy |
| GitHub CLI (gh) | GitHub 命令行工具，完整路径调用；未登录时可用 `git credential fill` 取令牌设 `GH_TOKEN` | D:\autotools\gh_windows_amd64\bin\gh.exe |
| skills | 通用技能与工程实践资料包（TDD / Qt / CMake / Git / Doxygen 等指南） | D:\autotools\skills |
| [WIN]需要安装的库 | Windows 离线安装包合集（VC2022 redist / VS2022 / cmake / python / Git 等） | D:\autotools\[WIN]需要安装的库 |

## 本工程的直接使用者

| 环节 | 工具 |
| --- | --- |
| Doxygen 文档（`Doxyfile` 零告警） | `D:\autotools\doxygen-1.11.0\doxygen.exe`（`scripts\gen_docs.bat` 已内置该路径，也可用 `DOXYGEN` 环境变量覆盖） |
| GitHub CI 结果查询/重跑/拉产物 | `D:\autotools\gh_windows_amd64\bin\gh.exe`（`scripts\ci\ci_common.ps1` 已内置该默认路径） |
| 源码打包压缩 | `D:\autotools\7z\7za.exe`（`tools\package_source.ps1` 回退链中的一环） |
