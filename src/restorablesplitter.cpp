#include "restorablesplitter.h"

#include <QMouseEvent>
#include <QSplitterHandle>

class RestorableSplitterHandle final : public QSplitterHandle
{
public:
    RestorableSplitterHandle(Qt::Orientation orientation, RestorableSplitter *parent)
        : QSplitterHandle(orientation, parent)
        , m_splitter(parent)
    {
    }

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override
    {
        QSplitterHandle::mouseDoubleClickEvent(event);
        emit m_splitter->handleDoubleClicked();
    }

private:
    RestorableSplitter *m_splitter = nullptr;
};

RestorableSplitter::RestorableSplitter(Qt::Orientation orientation, QWidget *parent)
    : QSplitter(orientation, parent)
{
}

QSplitterHandle *RestorableSplitter::createHandle()
{
    return new RestorableSplitterHandle(orientation(), this);
}
