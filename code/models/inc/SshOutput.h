#pragma once

#include <QByteArray>
#include <QString>

// Windows 版 OpenSSH 会把本地编码（如 GBK）的非 ASCII 输出转义成 \ooo，
// 这里负责把 ssh 的 stderr 还原成可读文本。
namespace SshOutput
{
QByteArray unescapeOctal(const QByteArray& output);
QString decode(const QByteArray& output);
}  // namespace SshOutput
