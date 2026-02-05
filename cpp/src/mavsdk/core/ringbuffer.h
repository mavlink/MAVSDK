#pragma once

#include <iterator>
#include <cstddef>
#include <array>

// Inspired by:
// https://codereview.stackexchange.com/questions/189380/example-of-adding-stl-iterator-support-to-custom-collection-class

namespace mavsdk {

template<typename T, std::size_t N> class RingbufferIterator;
template<typename T, std::size_t N> class ConstRingbufferIterator;

template<typename T, std::size_t N> class Ringbuffer {
public:
    Ringbuffer() = default;
    ~Ringbuffer() = default;

    void push(const T& value)
    {
        if (_size < N) {
            ++_size;
        }
        _index = (_index + 1) % N;

        _storage[_index] = value;
    }

    T& operator[](int index) { return _storage[(_index + index + 1) % N]; }

    const T& operator[](int index) const { return _storage[(_index + index + 1) % N]; }

    using iterator = RingbufferIterator<T, N>;
    using const_iterator = ConstRingbufferIterator<T, N>;
    iterator begin() { return iterator(*this, 0); }
    iterator end() { return iterator(*this, _size); }
    const_iterator begin() const { return const_iterator(*this, 0); }
    const_iterator end() const { return const_iterator(*this, _size); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    std::size_t size() const { return _size; }

private:
    std::array<T, N> _storage{};
    std::size_t _index{0};
    std::size_t _size{0};
};

template<typename T, std::size_t N> class RingbufferIterator {
public:
    RingbufferIterator(Ringbuffer<T, N>& buf, std::size_t off) : _buf(buf), _off(off) {}

    bool operator==(const RingbufferIterator& i) { return &i._buf == &_buf && i._off == _off; }
    bool operator!=(const RingbufferIterator& i) { return !(*this == i); }
    RingbufferIterator& operator++()
    {
        ++_off;
        return *this;
    }
    RingbufferIterator operator++(int)
    {
        auto t = *this;
        ++_off;
        return t;
    }
    RingbufferIterator& operator--()
    {
        --_off;
        return *this;
    }
    RingbufferIterator operator--(int)
    {
        auto t = *this;
        --_off;
        return t;
    }
    std::ptrdiff_t operator-(RingbufferIterator const& sibling) const
    {
        return _off - sibling._off;
    }
    RingbufferIterator& operator+=(int amount)
    {
        _off += amount;
        return *this;
    }
    RingbufferIterator& operator-=(int amount)
    {
        _off -= amount;
        return *this;
    }
    bool operator<(RingbufferIterator const& sibling) const { return _off < sibling._off; }
    bool operator<=(RingbufferIterator const& sibling) const { return _off <= sibling._off; }
    bool operator>(RingbufferIterator const& sibling) const { return _off > sibling._off; }
    bool operator>=(RingbufferIterator const& sibling) const { return _off >= sibling._off; }
    T& operator[](int index) const { return _buf[index]; }
    T& operator*() const { return _buf[_off]; }

private:
    Ringbuffer<T, N>& _buf;
    std::size_t _off;
};

template<typename T, std::size_t N> class ConstRingbufferIterator {
public:
    ConstRingbufferIterator(const Ringbuffer<T, N>& buf, std::size_t off) : _buf(buf), _off(off) {}

    bool operator==(const ConstRingbufferIterator& i) { return &i._buf == &_buf && i._off == _off; }
    bool operator!=(const ConstRingbufferIterator& i) { return !(*this == i); }
    ConstRingbufferIterator& operator++()
    {
        ++_off;
        return *this;
    }
    ConstRingbufferIterator operator++(int)
    {
        auto t = *this;
        ++_off;
        return t;
    }
    ConstRingbufferIterator& operator--()
    {
        --_off;
        return *this;
    }
    ConstRingbufferIterator operator--(int)
    {
        auto t = *this;
        --_off;
        return t;
    }
    std::ptrdiff_t operator-(ConstRingbufferIterator const& sibling) const
    {
        return _off - sibling._off;
    }
    ConstRingbufferIterator& operator+=(int amount)
    {
        _off += amount;
        return *this;
    }
    ConstRingbufferIterator& operator-=(int amount)
    {
        _off -= amount;
        return *this;
    }
    bool operator<(ConstRingbufferIterator const& sibling) const { return _off < sibling._off; }
    bool operator<=(ConstRingbufferIterator const& sibling) const { return _off <= sibling._off; }
    bool operator>(ConstRingbufferIterator const& sibling) const { return _off > sibling._off; }
    bool operator>=(ConstRingbufferIterator const& sibling) const { return _off >= sibling._off; }
    const T& operator[](int index) const { return _buf[index]; }
    const T& operator*() const { return _buf[_off]; }

private:
    const Ringbuffer<T, N>& _buf;
    std::size_t _off;
};

} // namespace mavsdk
