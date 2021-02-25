#pragma once

#include <QString>

class CommonInterfaces {

public:
    virtual bool ping(const QString& portName = QString(), int baud = 9600, int addr = 0) = 0;
    virtual bool isConnected() const { return m_connected; }
    virtual void reset() { m_connected = false; }

protected:
    bool m_connected = false;
};
