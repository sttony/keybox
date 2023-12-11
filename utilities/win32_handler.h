//
// Created by tongl on 12/11/2023.
//

#ifndef KEYBOX_WIN32_HANDLER_H
#define KEYBOX_WIN32_HANDLER_H

#include <functional>

template<typename T_Handler>
class Win32Handler {
private:
    T_Handler _handler;
    std::function<void(T_Handler)> _deleter;
public:
    T_Handler *ptr() { return &_handler; }

    operator T_Handler() { return _handler; }

    Win32Handler(T_Handler h, std::function<void(T_Handler)> deleter) : _handler(h), _deleter(std::move(deleter)) {}

    ~Win32Handler() {
        _deleter(_handler);
    }
};

#endif //KEYBOX_WIN32_HANDLER_H
