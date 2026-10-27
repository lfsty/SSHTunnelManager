# SSHTunnelManager

[![Windows](https://github.com/lfsty/SSHTunnelManager/actions/workflows/windows.yml/badge.svg?branch=main)](https://github.com/lfsty/SSHTunnelManager/actions/workflows/windows.yml)

用于管理 ssh 端口转发的小工具，界面完全由 QML 实现。

## 功能

- 管理多个 ssh 隧道，每个隧道可配置跳板机与多条本地端口转发规则
- 一键启动 / 停止单个隧道，卡片上实时显示运行状态与 ssh 错误信息
- 自动跟随系统浅色 / 深色主题
- 无系统标题栏，标题栏、窗口控制按钮与边缘缩放均由 QML 实现
- 系统托盘：单击 / 双击托盘图标显示主窗口，可从托盘退出或打开日志
- 单实例运行：重复启动时会把已运行的窗口带到前台
- 配置与日志均保存在程序当前工作目录

## 目录结构

```
code/
├── main.cpp                  # 程序入口、QML 类型注册、单实例
├── models/                   # C++ 后端（不依赖 QML）
│   ├── inc/
│   │   ├── ForwardData.h     # 单条端口转发规则
│   │   ├── LogManager.h      # 线程安全日志（含 Qt 消息重定向）
│   │   ├── Tunnel.h          # 隧道对象：ssh 进程生命周期
│   │   ├── TunnelListModel.h # 隧道列表模型
│   │   └── TunnelManager.h   # QML 入口：加载 / 保存配置
│   └── src/
├── qml/                      # QML 界面（仅 UI，无业务逻辑）
│   ├── Main.qml              # 主窗口、自绘标题栏、托盘
│   ├── Theme.qml             # 配色与尺寸（QML 单例）
│   ├── TunnelCard.qml        # 隧道卡片
│   ├── TunnelSetupDialog.qml # 隧道配置对话框
│   ├── ForwardRow.qml        # 转发规则编辑行
│   ├── AppButton.qml         # 统一风格按钮
│   └── WindowControlButton.qml
├── resources/img/icon.png
└── tests/                    # Qt Test 单元测试
```

## 构建

要求 Qt 6.8+（包含 Qt Quick、Qt Quick Controls 2、Qt Test）与 CMake 3.22+。

```bash
cmake -B build -S . -G "Visual Studio 17 2022" -A x64 \
      -DCMAKE_PREFIX_PATH=C:/Qt/Qt6.8.0/msvc2022_64
cmake --build build --config Release
```

生成的可执行文件位于 `build/Release/SSHTunnelManager.exe`。

## 测试

```bash
ctest --test-dir build -C Release --output-on-failure
```

## 数据文件

程序会把所有数据写在**当前工作目录**下：

| 文件 | 说明 |
| --- | --- |
| `tunnel.json` | 隧道配置，兼容旧版本字段名（`jumpserverUserName` / `forwardData` 等） |
| `log.txt` | ssh 输出、启停记录以及 Qt 警告 |

## 运行依赖

- Windows 10/11
- 已安装 OpenSSH 客户端，且 `ssh` 位于 `PATH` 中
- Microsoft Visual C++ 2015-2022 Redistributable（未随程序打包）
- 跳板机使用密钥认证（程序不提供交互式密码输入）
