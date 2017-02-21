//
// Created by c6s on 17-2-21.
//

#ifndef TINYNL_NONCOPYABLE_H
#define TINYNL_NONCOPYABLE_H
namespace tinyNL{
    class Noncopyable{
    protected:
        Noncopyable() = default;
        ~Noncopyable() = default;

    private:
        Noncopyable&operator=(const Noncopyable&) = delete;
        Noncopyable(const Noncopyable&) = delete;
    };
}
#endif //TINYNL_NONCOPYABLE_H
