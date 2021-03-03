#pragma once

#include <QString>

class CommonInterfaces {

public:
    virtual bool ping([[maybe_unused]] const QString& portName, [[maybe_unused]] int baud, [[maybe_unused]] int addr) = 0;
    virtual bool isConnected() const { return m_connected; }
    virtual void reset() { m_connected = false; }

    virtual void open(int mode) = 0;
    virtual void close() = 0;

protected:
    mutable bool m_connected = false;
};
