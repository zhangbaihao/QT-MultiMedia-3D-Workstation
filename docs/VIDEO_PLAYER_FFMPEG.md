# 视频/音频播放器（FFmpeg）开发文档

## 1. 项目概述

QT-MultiMedia 3D Workstation 的视频播放器模块采用 **FFmpeg** 作为底层解码引擎，通过 **Qt Quick** 渲染视频帧，使用 **QAudioSink** 输出音频。整体架构遵循"解码-缓冲-渲染"三条流水线：视频帧由定时器驱动解码并推送到 `QQuickImageProvider`，音频由 `QAudioSink` 的回调机制拉取解码后的 PCM 数据。

## 2. 依赖

| 依赖 | 版本/说明 | 来源 |
|------|-----------|------|
| **FFmpeg** | 任意现代版本（含 avcodec, avformat, avutil, swscale, swresample） | vcpkg (`x64-windows`) |
| **Qt 6** | 6.5+（Core, Gui, Qml, Quick, Multimedia, Quick3D, Widgets） | 官方安装包 |
| **vcpkg** | 工具链 + 包管理 | `D:/code/vcpkg`（或自定义 `MY_VCPKG_ROOT`） |

### 2.1 FFmpeg 组件

| 库 | 用途 |
|----|------|
| `avformat` | 容器格式解析（打开/读取/seek 多媒体文件） |
| `avcodec` | 音视频解码 |
| `avutil` | 通用工具（帧管理、内存分配） |
| `swscale` | 视频帧格式转换（YUV → RGB24） |
| `swresample` | 音频重采样（将解码后的音频转换为 Qt 期望的格式） |

### 2.2 CMake 变量

| 变量 | 说明 |
|------|------|
| `MY_VCPKG_ROOT` | vcpkg 根目录（例如 `D:/code/vcpkg`）。`cmake/FFMPEGImports.cmake` 与 `src/CMakeLists.txt` 中 DLL/头文件路径均由此拼接 `installed/x64-windows`。 |
| `CMAKE_TOOLCHAIN_FILE` | 若未在命令行指定，根 `CMakeLists.txt` 会设为 `${MY_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake`。 |

与仓库外 SDK 示例（`F:/code/AIProject_Auto/SDK/CMakeLists.txt`）的思路一致：先设 vcpkg 根，再拼 `installed/x64-windows` 的 `include`/`lib`/`bin`。

### 2.3 运行时（Windows）

使用 vcpkg 工具链时，链接阶段会运行 **applocal.ps1**，将 FFmpeg、Assimp 等依赖 DLL 自动部署到可执行文件目录（例如 `avutil-60.dll`、`avcodec-62.dll`、`avformat-62.dll`、`swscale-9.dll`、`swresample-6.dll` 等，具体文件名随 FFmpeg 主版本变化）。若在未启用 applocal 的环境下构建，请手动将上述 DLL 从 `installed/x64-windows[/debug]/bin` 拷到 exe 同目录。

### 2.4 重要：Debug vs Release CRT 兼容性

**只有 `x64-windows`（Release）vcpkg triplet 被安装时，Debug 构建可能触发 CRT 断言。**

vcpkg 的默认 triplet `x64-windows` 使用 `/MD`（Release CRT）编译库。如果你的项目只安装了 `x64-windows`，而没有安装 `x64-windows-debug`（Debug CRT），则：

| 组件 | 构建类型 | CRT 链接 | CRT DLL |
|------|---------|---------|---------|
| FFmpeg DLL (vcpkg) | Release | `/MD` | `MSVCRT.dll` |
| 你的 EXE | **Debug** | **`/MDd`** | **`MSVCRTD.dll`** |
| 你的 EXE | **Release** | `/MD` | `MSVCRT.dll` |

Debug 构建时，由于 EXE 使用 Debug CRT（`/MDd`），而 FFmpeg DLL 使用 Release CRT（`/MD`），跨 DLL 边界的堆操作会导致 Visual C++ Debug CRT 触发 `_CtrlsValidHeapPointer(block)` 断言——**这是假阳性，实际没有内存损坏**。Release 构建因两边都用 `/MD`，完全兼容，无此问题。

**解决方案**：
- **推荐**：日常开发/测试直接用 **Release 构建**（`cmake --build . --config Release`）
- 如需 Debug 构建，安装 vcpkg Debug triplet：`vcpkg install ffmpeg[all]:x64-windows-debug`（编译较久）

## 3. 架构设计

### 3.1 整体架构

```
┌──────────────────────────────────────────────────────────────────┐
│                     QML UI (VideoPlayerPage.qml)                 │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌─────────────────┐  │
│  │ 播放按钮  │  │ 进度条    │  │ 音量控制  │  │ Image(videoframe)│  │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────────┬────────┘  │
│       │              │              │                  │          │
└───────┼──────────────┼──────────────┼──────────────────┼──────────┘
        │              │              │                  │
   ┌────▼──────────────▼──────────────▼──────────────────▼──────────┐
   │                    C++ Backend (QtMultiMedia3D)                 │
   │  ┌─────────────────────────────────────────────────────────┐   │
   │  │  VideoPlayer (QObject)                                   │   │
   │  │  - play()/pause()/stop()                                 │   │
   │  │  - setFilePath() / seek()                                │   │
   │  │  - QTimer 驱动定时解码                                    │   │
   │  └────────────┬────────────────────────────────────────────┘   │
   │               │                                                │
   │  ┌────────────▼────────────────────────────────────────────┐   │
   │  │  FFmpegDecoder                                          │   │
   │  │  - openFile() / decodeNextFrame() / seek()              │   │
   │  │  - 封装 FFmpeg avformat / avcodec / swscale             │   │
   │  └────────────┬────────────────────────────────────────────┘   │
   │               │                                                │
   │  ┌────────────▼────────────────────────────────────────────┐   │
   │  │  VideoFrameImageProvider (QQuickImageProvider)           │   │
   │  │  - 存储最新解码帧                                        │   │
   │  │  - QML Image 通过 image://videoframe/ 获取               │   │
   │  └─────────────────────────────────────────────────────────┘   │
   │                                                                │
   │  ┌─────────────────────────────────────────────────────────┐   │
   │  │  AudioPlayer (QObject + QAudioSink)                      │   │
   │  │  - FFmpeg 解码音频                                       │   │
   │  │  - SwrContext 重采样                                     │   │
   │  │  - QIODevice (AudioPullDevice) 被 QAudioSink 拉取        │   │
   │  └─────────────────────────────────────────────────────────┘   │
   └─────────────────────────────────────────────────────────────────┘
```

### 3.2 数据流

#### 视频流水线
```
File → avformat_open_input → avformat_find_stream_info
      → avcodec_find_decoder → avcodec_open2
      → [Timer] av_read_frame → avcodec_send_packet → avcodec_receive_frame
      → sws_scale (YUV→RGB24) → QImage → VideoFrameImageProvider
      → QML Image 渲染
```

#### 音频流水线
```
File → avformat_open_input → avformat_find_stream_info
      → avcodec_find_decoder → avcodec_open2
      → swr_alloc_set_opts2 → swr_init
      → [QAudioSink 回调] av_read_frame → avcodec_send_packet
      → avcodec_receive_frame → swr_convert → PCM Queue → QAudioSink
```

### 3.3 关键模块职责

| 模块 | 文件 | 类 | 职责 |
|------|------|-----|------|
| FFmpeg 解码器 | `FFmpegDecoder.h/.cpp` | `FFmpegDecoder` | 封装 FFmpeg 视频解码，输出 QImage |
| 视频播放器 | `VideoPlayer.h/.cpp` | `VideoPlayer` | 管理播放状态/定时器/属性绑定 |
| 帧图像提供器 | `VideoFrameImageProvider.h/.cpp` | `VideoFrameImageProvider` | QQuickImageProvider，桥接 C++ 帧到 QML |
| 音频播放器 | `AudioPlayer.h/.cpp` | `AudioPlayer` | FFmpeg 音频解码 + QAudioSink 输出 |
| QML 页面 | `VideoPlayerPage.qml` | — | 用户界面，绑定 C++ 属性 |

## 4. QML 类型契约（`import QtMultiMedia3D 1.0`）

### 4.1 VideoPlayer

| 属性 / 方法 | 类型 | 说明 |
|-------------|------|------|
| `filePath` | QString | 本地路径或 `file://` URL；C++ 侧用 `QUrl::fromUserInput` 规范化。 |
| `isPlaying` | bool (readonly) | 是否正在播放。 |
| `duration` | int (readonly) | 视频总时长（毫秒）。 |
| `position` | int | 当前播放位置（毫秒），可写用于 seek。 |
| `playbackRate` | qreal | 倍速（0.25～4.0），影响播放速度。 |
| `videoWidth` | int (readonly) | 视频宽度。 |
| `videoHeight` | int (readonly) | 视频高度。 |
| `frameSequence` | int (readonly) | 帧序号，每解码一帧递增，用于强制刷新 Image source。 |
| `play()` | method | 开始播放。 |
| `pause()` | method | 暂停播放。 |
| `stop()` | method | 停止播放并回到起点。 |
| `errorOccurred` | signal | 错误信号，`(string error)`。 |

### 4.2 AudioPlayer

| 属性 / 方法 | 类型 | 说明 |
|-------------|------|------|
| `filePath` | QString | 与视频相同媒体源。 |
| `isPlaying` | bool (readonly) | 是否正在播放。 |
| `duration` | int (readonly) | 音频总时长（毫秒）。 |
| `position` | int | 当前播放位置（毫秒），可写用于 seek。 |
| `volume` | qreal | 音量 0.0～1.0。 |
| `muted` | bool | 静音。 |
| `play()` | method | 开始播放。 |
| `pause()` | method | 暂停播放。 |
| `stop()` | method | 停止播放。 |
| `errorOccurred` | signal | 错误信号，`(string error)`。 |

### 4.3 图像显示机制

引擎注册 `image://videoframe/` 前缀的 `QQuickImageProvider`，`VideoPlayer` 在解码后调用 `VideoFrameImageProvider::updateFrame()` 更新帧数据。QML 侧通过 `Image.source` 属性绑定图片源，使用 `frameSequence` 作为查询参数强制刷新缓存：

```qml
Image {
    source: "image://videoframe/frame?v=" + videoPlayer.frameSequence
}
```

`frameSequence` 每解码一帧递增，QML 引擎检测到 source URL 变化后重新请求图片。

## 5. 已知 Bug 与修复方案

### 5.1 Bug 1: FileDialog 兼容性问题

**症状**：在 Qt 6.5+ 环境中，`Qt.labs.platform` 的 `FileDialog` 在某些平台可能不可用或表现异常。

**根因**：`Qt.labs.platform` 在 Qt 6.5+ 中标记为 deprecated；早期代码同时检查了 `videoFileDialog.files`（复数）和 `videoFileDialog.file`（单数），但单个文件选择场景只有 `.file` 属性。

**修复方案**：只使用 `.file` 属性（单数），添加 try/catch 保护。或后续迁移到 `QtQuick.Dialogs`。

### 5.2 Bug 2: 音频 Player 在无声视频上崩溃

**症状**：打开仅有视频轨道的文件（如无音频的 MP4），点击播放按钮时 `AudioPlayer` 尝试打开音频解码器失败。

**根因**：`AudioPlayer::play()` 没有检查 `ensureDecoderOpen()` 的返回值，在无音频流的情况下继续操作音频设备。

**修复方案**：在 `play()` 中添加 `ensureDecoderOpen()` 返回检查，无音频流时不启动音频设备。QML 侧通过 `audioPlayer.duration > 0` 判断是否有音频流。

### 5.3 Bug 3: 音频 readAudioData 除零风险

**症状**：极端情况下（音频格式非标），`bytesForFrames(1)` 和 `bytesPerSample()` 都返回 0，导致 `bpf = 0`，计算 `n / bpf` 时触发除零。

**根因**：`m_outFormat` 从 `QAudioDevice::preferredFormat()` 获取，如果设备返回了不完整的格式信息，`bpf` 可能为 0。

**修复方案**：使用 `qMax(1, bpf)` 确保除数至少为 1，同时 `bpf` 回退到硬编码 4 字节。

### 5.4 Bug 4: VideoPlayer 帧率为零时的无限循环

**症状**：当 FFmpeg 无法从媒体文件中获取帧率（某些非标准文件）时，`m_frameRate` 为 0，导致 `startPlayback()` 中计算出的定时器间隔异常。

**根因**：`m_frameRate` 为 0 时，`1000.0 / fps` 除零，且 `qBound` 把间隔 clamp 到 10000ms。

**修复方案**：默认帧率设为 25.0fps，`openFile` 失败时保留默认值，`calculateFrameInterval()` 中使用 `qMax(0.5, ...)` 兜底。

### 5.5 Bug 5: 多页面切换时的资源竞争

**症状**：频繁切换 Tab 页时，`VideoPlayer` 和 `AudioPlayer` 可能在不同时间点被析构/创建，导致悬空指针调用。

**根因**：QML 中的 `VideoPlayer` 和 `AudioPlayer` 对象在页面切换时不会立刻销毁（`StackLayout` 默认缓存），但状态管理不当。

**修复方案**：添加页面切换时的生命周期管理，在页面不可见时暂停播放。

### 5.6 Bug 6: 无有效音频设备时的崩溃

**症状**：在没有音频输出设备的系统上（如某些服务器/CI 环境），`QMediaDevices::defaultAudioOutput()` 返回无效设备。

**根因**：没有检查 `QAudioDevice` 有效性或 `isFormatSupported()`。

**修复方案**：在创建 `QAudioSink` 之前检查音频设备有效性，使用 `dev.isFormatSupported(m_outFormat)` 验证格式。

### 5.7 Bug 7: 程序退出时的堆损坏 (_CtrlsValidHeapPointer)

**症状**：关闭程序（点击右上角 X）时，弹出 Visual C++ 运行时断言对话框：`_CtrlsValidHeapPointer(block)` 或类似堆损坏错误。

**根因 & 修复**：该问题由两个因素共同导致，分别修复：

#### 因素 A：FFmpeg 手动缓冲区管理

`FFmpegDecoder` 使用 `av_malloc()` + `av_image_fill_arrays()` 手动管理 RGB 帧缓冲区，然后手动管理 `m_frameRGB->data[0]` 的生命周期。在 `closeFileUnlocked()` 中先设置 `data[0] = nullptr` 再 `av_frame_free()`，最后 `av_free(m_rgbBuffer)`。这种模式下 `AVFrame.buf[]` 从未被正确设置，`av_frame_free()` 无法通过 AVBufferRef 系统正确管理生命周期。当多次打开/关闭文件时，或者析构时序复杂时，可能发生 double-free 或悬空指针。

**修复**：改用 `av_frame_get_buffer()` 分配帧缓冲区。设置 `m_frameRGB->format`、`m_frameRGB->width`、`m_frameRGB->height` 后调用 `av_frame_get_buffer(m_frameRGB, 0)`。这会正确设置 `data[]`、`linesize[]` 和 `buf[]`，`av_frame_free()` 能自动通过 AVBufferRef 系统管理缓冲区生命周期。移除 `m_rgbBuffer` 成员变量和 `freeRgbBuffer()` 方法。

```cpp
// 旧代码（问题代码）
const int numBytes = av_image_get_buffer_size(targetFormat, w, h, 1);
m_rgbBuffer = static_cast<uint8_t *>(av_malloc(numBytes));
av_image_fill_arrays(m_frameRGB->data, m_frameRGB->linesize, m_rgbBuffer, targetFormat, w, h, 1);
// closeFileUnlocked 中需手动设置 data[0] = nullptr + av_frame_free + av_free

// 新代码（修复后）
m_frameRGB->format = targetFormat;
m_frameRGB->width = w;
m_frameRGB->height = h;
av_frame_get_buffer(m_frameRGB, 0);
// closeFileUnlocked 中只需 av_frame_free，自动管理
```

#### 因素 B：静态对象析构时日志处理器仍在运行

`main.cpp` 中的 `logMessageHandler()` 访问三个文件作用域静态对象：`s_logFile`、`s_logStream`、`s_logMutex`。这些对象在 `main()` 返回后的静态析构阶段被销毁。但 `qInstallMessageHandler(logMessageHandler)` 从未被恢复，导致在静态析构期间如果有任何 Qt 代码调用 `qDebug()`/`qWarning()`，我们的处理器访问已销毁的静态对象，产生未定义行为。

**修复**：保存旧的 message handler，在 `app.exec()` 返回后恢复：

```cpp
const auto prevLogHandler = qInstallMessageHandler(logMessageHandler);
// ... app.exec() ...
qInstallMessageHandler(prevLogHandler);
```

#### 其他安全措施

- `AudioPlayer::~AudioPlayer()` 中先 `m_sink->stop()` 同步停止音频输出，再关闭解码器，确保没有挂起的回调
- `VideoPlayer::~VideoPlayer()` 中先 `stopPlayback()` 停止定时器，再关闭解码器
- `AudioPlayer::readAudioData()` 添加 `m_destroying` 原子标志检查，防止析构后线程继续访问

#### 因素 C：Debug CRT 与 Release CRT 跨 DLL 不匹配（假阳性）

最终发现 Debug 模式下 `_CtrlsValidHeapPointer(block)` 断言的根本原因是 **vcpkg 只安装了三方库的 Release CRT 版本**（`x64-windows` triplet），而 EXE 在 Debug 构建时使用 Debug CRT（`/MDd`）。由于 FFmpeg DLL 使用 Release CRT（`/MD`）分配堆内存，Debug CRT 的堆追踪跨 DLL 检测时认为指针来源不匹配，触发断言。

**这个断言是假阳性，实际没有内存损坏**。Release 构建（EXE 和 FFmpeg DLL 都使用 `/MD`）完全正常。参见 §2.4。

## 6. 商业播放器开发标准参考

### 6.1 架构准则

| 原则 | 说明 |
|------|------|
| **分离解码与渲染** | 解码在独立线程进行，UI 线程仅负责帧推送和渲染 |
| **音视频同步** | 以音频时钟为基准（Audio Clock Master），视频帧根据 PTS 对齐 |
| **缓冲策略** | 至少要保证 500ms～2s 的音频缓冲，防止音频卡顿 |
| **硬件加速** | FFmpeg 支持 DXVA2 / D3D11VA / CUDA 等硬件解码器，应优先尝试 |
| **坑点处理** | 处理损坏文件、不标准码流、网络超时、音视频不同步等异常场景 |

### 6.2 标准播放器功能清单

| 功能 | 优先级 | 说明 |
|------|--------|------|
| 播放/暂停/停止 | P0 | 基础控制 |
| 进度拖拽（Seek） | P0 | 前后向搜索，精确到关键帧或任意帧 |
| 音量控制 | P0 | 包含静音、音量滑块 |
| 倍速播放 | P1 | 0.5x ~ 4.0x，保持音调不变（需 FFmpeg 的滤波或 SoundTouch） |
| 全屏模式 | P1 | 双击/按钮全屏 |
| 字幕支持 | P2 | SRT/ASS 字幕渲染 |
| 播放列表 | P2 | 连续播放、循环模式 |
| 画面调整 | P2 | 亮度/对比度/饱和度（swscale 或 shader） |
| 截图功能 | P2 | 捕获当前帧保存为图片 |
| 硬件解码 | P2 | DXVA2/D3D11VA 降低 CPU 占用 |

### 6.3 音视频同步方案

商业播放器通常采用以下几种同步策略：

| 方案 | 精度 | 复杂度 | 说明 |
|------|------|--------|------|
| **音频时钟主同步** | ★★★★★ | ★★★ | 以音频播放位置为标准，视频帧根据 PTS 等待或丢帧 |
| **外部时钟同步** | ★★★★ | ★★★★ | 使用独立时钟，音视频各自对齐到该时钟 |
| **时间戳差值同步** | ★★★ | ★ | 直接比较音视频帧的 PTS 差值调整速度 |

当前项目采用简化方案：视频用定时器按帧率推帧，音频独立推流，通过 `position` 属性粗略同步。

### 6.4 内存与性能优化

| 优化项 | 说明 |
|--------|------|
| **帧缓冲池** | 预分配 N 个 AVFrame 复用，避免重复 malloc/free |
| **零拷贝渲染** | 使用 GPU 直接处理 YUV 数据（NV12 纹理），跳过 RGB 转换 |
| **硬解** | DXVA2/D3D11 硬件加速解码，极大降低 CPU 占用 |
| **线程模型** | 解码线程 + 渲染线程分离，使用生产者-消费者队列 |

## 7. 测试策略

### 7.1 单元测试

对以下关键路径编写测试：

- `FFmpegDecoder::openFile()` — 正常/损坏/不存在的文件
- `FFmpegDecoder::decodeNextFrame()` — 解码验证、EOF 检测
- `FFmpegDecoder::seek()` — 前后 seek 精度验证
- `VideoPlayer::play/pause/stop` — 状态机转换
- `AudioPlayer::readAudioData()` — PCM 数据完整性

测试文件：`tests/test_multimedia.cpp`

### 7.2 集成测试

| 场景 | 验证内容 |
|------|----------|
| MP4 播放 | 视频画面渲染、音频输出、进度正确 |
| Seek 操作 | 拖拽后画面和声音同步更新 |
| 倍速播放 | 不同倍速下音视频基本同步 |
| 文件切换 | 切换文件时清理旧资源、加载新资源 |

### 7.3 QML 测试

```bash
# 使用 qmllint 检查 QML 语法
qmllint src/qml/VideoPlayerPage.qml

# 使用 qml 直接运行页面（独立测试）
qml src/qml/VideoPlayerPage.qml
```

## 8. 开发指南

### 8.1 本地开发

```bash
# 1. 配置 CMake
cmake -B build -DCMAKE_TOOLCHAIN_FILE="D:/code/vcpkg/scripts/buildsystems/vcpkg.cmake"

# 2. 编译
cmake --build build --config Debug

# 3. 运行
./build/bin/Debug/QT-MultiMedia-3D-Workstation.exe
```

### 8.2 添加新功能

1. **添加 FFmpeg 滤镜**：在 `FFmpegDecoder` 中添加 `AVFilterGraph` 管线
2. **支持硬件解码**：在 `FFmpegDecoder::setupVideoCodec()` 中尝试 `hw_device_ctx`
3. **添加帧缓冲**：在 `VideoPlayer` 中添加 `QQueue<QImage>` 作为帧缓冲队列
4. **添加字幕**：解析字幕流，在 QML 中叠加 `Text` 层

### 8.3 调试技巧

| 技巧 | 说明 |
|------|------|
| `qDebug()` 打印帧率 | 在 `onPlaybackTimer()` 中统计每秒实际解码帧数 |
| FFmpeg 日志 | 调用 `av_log_set_level(AV_LOG_DEBUG)` 查看详细日志 |
| QML 调试 | 使用 `qmlscene` 或 Qt Creator 的 QML 调试器 |
| 性能分析 | 使用 Qt Creator 的 CPU Profiler 或 Visual Studio 的性能工具 |

## 9. 参考资源

### 9.1 FFmpeg 文档

- [FFmpeg Documentation](https://ffmpeg.org/documentation.html)
- [FFmpeg API 示例 (doc/examples)](https://github.com/FFmpeg/FFmpeg/tree/master/doc/examples)
- `avcodec_send_packet` / `avcodec_receive_frame` 新 API 文档

### 9.2 Qt 文档

- [QAudioSink Class](https://doc.qt.io/qt-6/qaudiosink.html)
- [QQuickImageProvider Class](https://doc.qt.io/qt-6/qquickimageprovider.html)
- [Qt Quick Dialog FileDialog](https://doc.qt.io/qt-6/qml-qtquick-dialogs-filedialog.html)

### 9.3 商业播放器参考

- **VLC**：开源播放器标杆，使用 FFmpeg + 自有输出框架
- **MPC-HC/BE**：Windows 平台经典播放器，LAV Filters 为分离器/解码器
- **IINA**：macOS 平台现代播放器，基于 mpv
- **PotPlayer**：Windows 平台高性能播放器，内置 DXVA 硬解
- **FFplay**：FFmpeg 自带的简易播放器，参考其 `av_sync_type` 同步逻辑

### 9.4 关键技术文章

- [How to write a video player in less than 1000 lines](https://github.com/rambodrahmati/FFmpeg-Video-Player)
- [Dranger's FFmpeg Tutorial](http://dranger.com/ffmpeg/)
- [Qt FFmpeg Player Implementation](https://github.com/ARMinel/Qt6-QML-Video-Player)

## 10. 当前已知限制

- **音画同步**：目前音视频通过各自独立的时钟驱动（视频=QTimer 帧率定时器，音频=QAudioSink 回调）。seek 时同步设置两端 position，但长时间播放可能出现漂移。后续应以音频时钟为主轴（Audio Clock Master）或使用外部参考时钟。
- **解码在 UI 线程**：`VideoPlayer` 的帧解码由 `QTimer` 驱动在主线程执行。高码率视频（4K/8K）可能导致 UI 卡顿。后续应移入独立解码线程 + 生产者-消费者队列。
- **仅软解**：未启用 DXVA2/D3D11VA 硬件加速。FFmpeg 7.x 支持通过 `hw_device_ctx` 配置硬件解码器。对于高端 GPU，硬解可大幅降低 CPU 占用并提升功耗比。
- **无字幕支持**：视频字幕流被忽略，未渲染。后续可解析字幕流并在 QML 中叠加 Text 层。
- **无播放列表**：仅支持单文件播放。
- **Qt.labs.platform 已弃用**：FileDialog 依赖 `Qt.labs.platform`（Qt 6.5+ 标记为 deprecated），后续应迁移到 `QtQuick.Dialogs`。

## 11. 验证建议

- 配置并编译主目标 `QT-MultiMedia-3D-Workstation`。
- 对 `src/qml/VideoPlayerPage.qml` 运行 `qmllint`（需与 `qt_add_qml_module` 的 import 路径一致）。
- 手动测试流程：
  1. 启动应用 → 切换到视频播放 tab
  2. 点击 📂 选择 MP4 文件 → 查看视频信息是否显示
  3. 点击 ▶ 播放 → 画面和声音是否正常
  4. 拖动进度条 → 画面和声音是否同步
  5. 音量/静音切换
  6. 倍速切换
  7. 快进/快退按钮
  8. 切换到其他 tab 再切回

## 12. 实现历程与关键问题记录

本节记录了音视频播放器从零到可用的完整实现历程，包括每个阶段遇到的核心问题、调试过程和解决方案。

### 12.1 第一阶段：视频解码 + QML 渲染

#### 实现方案

- `FFmpegDecoder` 封装 FFmpeg 的 `avformat` / `avcodec` / `swscale`，解码视频帧为 `QImage`
- `VideoPlayer` 管理播放状态，用 `QTimer` 按帧率定时触发解码
- `VideoFrameImageProvider` 继承 `QQuickImageProvider`，将解码帧桥接到 QML `Image` 组件
- QML 侧使用**双 Image 交替**（double buffering）消除帧切换闪烁

#### 双缓冲渲染机制

```qml
Image {
    id: videoImageA
    visible: videoFrameContainer.activeIndex === 0
}
Image {
    id: videoImageB
    visible: videoFrameContainer.activeIndex === 1
}
// 每次新帧到达：
// 1. 更新当前不可见图层的 source
// 2. 切换 activeIndex 使其可见
```

#### 关键点：QImage 格式选择

`AV_PIX_FMT_BGRA`（小端：B,G,R,A）与 `QImage::Format_ARGB32` 在 x86/x64 上的内存布局完全一致，无需额外的像素格式转换，`sws_scale` 直接输出到 `m_frameRGB->data[0]`，`QImage` 直接包装该内存：

```cpp
QImage image(m_frameRGB->data[0], w, h, m_frameRGB->linesize[0], QImage::Format_ARGB32);
return image.copy();  // 深拷贝，释放原始缓冲区
```

### 12.2 第二阶段：音频解码 + QAudioSink 输出

#### 实现方案

- 在 `AudioPlayer` 中复用 FFmpeg 的 `avformat` / `avcodec` 进行音频解码
- `SwrContext` 将解码后的音频从原始格式（可能是 int16/float、单声道/立体声、任意采样率）重采样到 QAudioSink 期望的格式（Float 48kHz 立体声）
- `QAudioSink` + `QIODevice`（`AudioPullDevice`）的 pull 模型输出 PCM 数据

#### 核心问题：QAudioSink 无声

**最关键的 Bug：`QIODevice::bytesAvailable()` 必须返回正值**

`QAudioSink` 在从 `QIODevice` 读取数据前会先检查 `bytesAvailable()`：

```
QAudioSink → bytesAvailable() == 0 → IdleState（永不调用 readData）
QAudioSink → bytesAvailable() > 0 → readData() → 播放
```

默认的 `QIODevice::bytesAvailable()` 返回 0。必须重写 `bytesAvailable()`，在队列为空但仍在播放时返回一个"诱饵"正值（4096），强制 `QAudioSink` 调用 `readData()`。一旦 `readData()` 被调用，`fillAudioQueue()` 就会解码更多数据填充队列。

```cpp
qint64 AudioPullDevice::bytesAvailable() const
{
    qint64 queueSize = m_player ? m_player->pcmQueueSize() : 0;
    if (queueSize > 0)
        return queueSize + QIODevice::bytesAvailable();
    // 播放中但队列空：告诉 sink 还有数据要来，触发 readData
    if (m_player && m_player->isPlaying() && !m_player->isAtEnd())
        return 4096;
    return QIODevice::bytesAvailable();
}
```

这个 Bug 耗时最长才定位——之前一直以为 `QAudioSink.start(device)` 会自动拉取，但实际必须先通过 `bytesAvailable` 触发。

#### 第二个关键点：预填音频队列

在调用 `m_sink->start(m_pull)` **之前**，先解码约 50ms（~24576 字节）的音频数据放入 `m_pcmQueue`。否则 `bytesAvailable()` 立即返回 4096，但 `readAudioData()` 中的 `fillAudioQueue()` 还来不及解码，导致 `readData()` 返回 0，QAudioSink 立即进入 IdleState。

```cpp
// play() 中：
{
    QMutexLocker lock(&m_mutex);
    const int prefillSize = qMax(m_outFormat.bytesForDuration(50000), 16384);
    fillAudioQueue(prefillSize);
}
m_sink->start(m_pull);  // 预填了数据再启动
```

#### 第三个关键点：音频设备选择与格式协商

- `setAudioDevice(description)` 允许用户通过 ComboBox 选择音频输出设备
- `findAudioDevice()` 在指定设备不可用时回退到默认设备
- QAudioSink 实际使用的格式可能与设置格式不一致（驱动层协商），此时必须重新配置 `SwrContext`

```cpp
const QAudioFormat actualFmt = m_sink->format();
if (actualFmt != m_outFormat) {
    m_outFormat = actualFmt;
    // 释放旧的 swr，用新格式重新 setupSwr
}
```

### 12.3 第三阶段：SoundTouch 倍速播放（计划中）

- 当前倍速通过调整 `QTimer` 间隔实现（跳帧/慢放），不改变音调
- 商业方案需要 SoundTouch 库或 FFmpeg 的 `atempo` 滤镜

### 12.4 第四阶段：日志系统

#### 实现：文件日志 + stderr 双输出

- 使用 `qInstallMessageHandler()` 安装自定义消息处理器
- 日志输出到 exe 同目录的 `audiolog.txt`，如果不可写则回退到 temp 目录
- 每条日志包含时间戳和类型标记 `[D]`/`[W]`/`[C]`/`[F]`/`[I]`
- `QMutex` 保护文件写入，支持多线程 `qDebug()` 调用

#### 容易忽略的问题：退出时清理日志处理器

静态对象（`s_logFile`、`s_logStream`、`s_logMutex`）会在 main() 返回后的静态析构阶段销毁。如果日志处理器仍被 `qInstallMessageHandler` 安装着，任何在静态析构期间发生的 `qDebug()` 调用都会访问已销毁的静态对象，导致未定义行为甚至崩溃。

**修复**：在 `main()` 返回前恢复旧的 message handler：

```cpp
const auto prevHandler = qInstallMessageHandler(logMessageHandler);
int ret = app.exec();
qInstallMessageHandler(prevHandler);  // 恢复，避免静态析构时访问已销毁的静态对象
return ret;
```

### 12.5 已知局限与后续优化方向

#### 音视频同步

当前方案：视频用 QTimer 按帧率推帧，音频独立由 QAudioSink 回调驱动。seek 时同时设置 `videoPlayer.position` 和 `audioPlayer.position` 来对齐两端。长时间播放可能出现漂移。

**商业方案**：以音频时钟为主轴（Audio Clock Master），视频帧根据 PTS 与音频时钟的差值决定等待或丢帧。

#### 解码在 UI 线程

`VideoPlayer` 的帧解码由 `QTimer` 驱动在主线程执行。高码率视频（4K/8K）可能导致 UI 卡顿。

**后续优化**：将解码移入独立线程，使用生产者-消费者队列（`QQueue<QImage>` + `QWaitCondition`）传递帧数据。

#### 仅软解

未启用 DXVA2/D3D11VA 硬件加速。FFmpeg 7.x 支持通过 `hw_device_ctx` 配置硬件解码器。

#### FFmpeg 缓冲区管理

**关键教训**：不要混合使用手动 `av_malloc`/`av_image_fill_arrays` 和 `av_frame_free`。

正确做法：使用 `av_frame_get_buffer()` 让 FFmpeg 自己管理帧缓冲区生命周期。`av_frame_free()` 通过 `AVBufferRef` 系统自动管理引用计数和释放。

```cpp
// ✅ 正确做法
m_frameRGB->format = AV_PIX_FMT_BGRA;
m_frameRGB->width = w;
m_frameRGB->height = h;
av_frame_get_buffer(m_frameRGB, 0);
// 之后只需 av_frame_free，自动释放缓冲区

// ❌ 错误做法（旧代码）
av_malloc + av_image_fill_arrays → 手动管理 data[] 生命周期 → 容易 double-free
```

### 12.6 QML + C++ 集成模式总结

```
┌──────────────────────────────────────────────────────────────────┐
│  QML 层                                                         │
│                                                                  │
│  import QtMultiMedia3D 1.0                                       │
│                                                                  │
│  VideoPlayer { id: videoPlayer }                                 │
│  AudioPlayer { id: audioPlayer }                                 │
│                                                                  │
│  直接读写属性：videoPlayer.position / volume / muted             │
│  调用方法：play() / pause() / stop()                            │
│  信号连接：Connections { onErrorOccurred: ... }                 │
│  图像桥接：Image { source: "image://videoframe/..." }           │
│  设备枚举：audioPlayer.availableAudioDevices()                  │
└───────────────────────────────┬──────────────────────────────────┘
                                │
                                ▼
┌──────────────────────────────────────────────────────────────────┐
│  C++ 层                                                         │
│                                                                  │
│  qmlRegisterType<XX>("QtMultiMedia3D", 1, 0, "XX")              │
│                                                                  │
│  VideoPlayer (QObject) : FFmpegDecoder → QImage                 │
│  AudioPlayer (QObject) : FFmpeg → Swr → PCM Queue → QAudioSink  │
│  VideoFrameImageProvider (QQuickImageProvider) : QImage cache    │
└──────────────────────────────────────────────────────────────────┘
```

**QML 与 C++ 的通信方式**：
- **属性绑定**：`Q_PROPERTY` → QML 可直接读写
- **方法调用**：`Q_INVOKABLE` 或 `public slots` → QML 可直接调用
- **信号**：`signals` → QML 用 `Connections` 或 `onXxxChanged` 处理
- **图像桥接**：`addImageProvider("videoframe", provider)` → QML 用 `image://videoframe/` 获取
- **上下文属性**：`setContextProperty("modelLoader", ...)` → QML 全局变量

### 12.7 当前状态总结

| 功能 | 状态 | 备注 |
|------|------|------|
| 视频解码 + 渲染 | ✅ 可用 | FFmpeg 软解，QImage 桥接到 QML |
| 音频解码 + 输出 | ✅ 可用 | QAudioSink pull 模型，Swr 重采样 |
| 播放/暂停/停止 | ✅ 可用 | 音视频联动 |
| Seek（进度拖拽） | ✅ 可用 | 自定义 Item+MouseArea 实现 |
| 音量控制 | ✅ 可用 | 自定义音量滑块 |
| 静音切换 | ✅ 可用 | |
| 倍速播放 | ✅ 可用 | 通过 QTimer 间隔调节 |
| 快进/快退 10s | ✅ 可用 | |
| 音频设备选择 | ✅ 可用 | ComboBox + availableAudioDevices() |
| 文件选择 | ✅ 可用 | Qt.labs.platform FileDialog |
| 自动播放 | ✅ 可用 | Timer 重试机制 |
| 日志系统 | ✅ 可用 | 文件 + stderr 双输出 |
| 退出堆损坏 | ✅ 已修复 | av_frame_get_buffer + log handler 清理 |
| 格式协商 | ✅ 已修复 | QAudioSink 实际格式与 swr 同步 |
| 无音轨文件处理 | ✅ 已修复 | hasAudioStream 标志保护 |
| 硬件解码 | ❌ 未实现 | 后续可添加 DXVA2/D3D11VA |
| 字幕支持 | ❌ 未实现 | |
| 播放列表 | ❌ 未实现 | |
| 多线程解码 | ❌ 未实现 | 当前在 UI 线程解码 |
