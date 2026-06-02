#include "skinprovider.h"

#include <QtGlobal>

QIcon DefaultSkinProvider::icon(const QString &name) const
{
    Q_UNUSED(name);
    return {};
}

QPixmap DefaultSkinProvider::image(const QString &name) const
{
    Q_UNUSED(name);
    return {};
}

QString DefaultSkinProvider::styleSheet(const QString &componentName) const
{
    Q_UNUSED(componentName);
    return {};
}
