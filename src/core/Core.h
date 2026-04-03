#ifndef CORE_H
#define CORE_H

#include <QObject>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    static Core* instance();

private:
    static Core* m_instance;
};

#endif // CORE_H
