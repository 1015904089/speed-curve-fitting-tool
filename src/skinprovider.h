#pragma once

#include <QIcon>
#include <QPixmap>
#include <QString>

class SkinProvider
{
public:
    virtual ~SkinProvider() = default;

    virtual QIcon icon(const QString &name) const = 0;
    virtual QPixmap image(const QString &name) const = 0;
    virtual QString styleSheet(const QString &componentName) const = 0;
};

class DefaultSkinProvider final : public SkinProvider
{
public:
    QIcon icon(const QString &name) const override;
    QPixmap image(const QString &name) const override;
    QString styleSheet(const QString &componentName) const override;
};
