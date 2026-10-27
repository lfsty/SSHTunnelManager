#include "SshOutput.h"

#include <QStringConverter>

namespace
{
bool isOctalDigit(char ch)
{
    return ch >= '0' && ch <= '7';
}
}  // namespace

QByteArray SshOutput::unescapeOctal(const QByteArray& output)
{
    QByteArray result;
    result.reserve(output.size());

    for (int i = 0; i < output.size(); ++i)
    {
        const char ch = output.at(i);

        if (ch == '\\' && i + 3 < output.size() && isOctalDigit(output.at(i + 1)) && isOctalDigit(output.at(i + 2)) && isOctalDigit(output.at(i + 3)))
        {
            const int value = (output.at(i + 1) - '0') * 64 + (output.at(i + 2) - '0') * 8 + (output.at(i + 3) - '0');
            if (value <= 0xFF)
            {
                result.append(static_cast<char>(value));
                i += 3;
                continue;
            }
        }

        result.append(ch);
    }

    return result;
}

QString SshOutput::decode(const QByteArray& output)
{
    const QByteArray unescaped = unescapeOctal(output);
    if (unescaped == output)
    {
        return QString::fromLocal8Bit(output);
    }

    // 还原后的字节使用系统本地编码（中文 Windows 上即 GBK）
    QStringDecoder decoder(QStringDecoder::System);
    return decoder.decode(unescaped);
}
