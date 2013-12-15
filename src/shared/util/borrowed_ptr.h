/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: borrowed_ptr.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

template <typename T>
class borrowed_ptr
{
  public:
    borrowed_ptr() = default;
    borrowed_ptr(borrowed_ptr<T> const &) = default;
    borrowed_ptr(T * const ptr)
      : m_ptr(ptr)
    { }
    borrowed_ptr(borrowed_ptr<T> &&ptr)
      : m_ptr(ptr.m_ptr)
    { ptr.m_ptr = nullptr; }
    borrowed_ptr(std::nullptr_t) : m_ptr(nullptr)
    { }

    ~borrowed_ptr() = default;

    T* get()
    { return m_ptr; }

    void reset(T * const ptr)
    { m_ptr = ptr; }
    void reset(std::nullptr_t)
    { m_ptr = nullptr; }
    void reset(borrowed_ptr<T> const &ptr)
    { m_ptr = ptr; }
    void reset(borrowed_ptr<T> &&ptr)
    { m_ptr = std::forward<borrowed_ptr<T>>(ptr); }

    operator bool() const
    { return m_ptr; }
    operator T*()
    { return m_ptr; }
    operator T*() const
    { return m_ptr; }

    T& operator *()
    { assert(m_ptr); return *m_ptr; }
    T& operator *() const
    { assert(m_ptr); return *m_ptr; }
    T* operator ->()
    { assert(m_ptr); return m_ptr; }
    T* operator ->() const
    { assert(m_ptr); return m_ptr; }

    borrowed_ptr<T>& operator =(T * const ptr)
    { m_ptr = ptr; return *this; }
    borrowed_ptr<T>& operator =(std::nullptr_t)
    { m_ptr = nullptr; return *this; }
    borrowed_ptr<T>& operator =(borrowed_ptr<T> const &ptr)
    { m_ptr = ptr.m_ptr; return *this; }
    borrowed_ptr<T>& operator =(borrowed_ptr<T> &&ptr)
    {
      if(this != &ptr)
      {
        m_ptr = ptr.m_ptr;
        ptr.m_ptr = nullptr;
      }
      return *this;
    }

    template <typename C>
    bool operator ==(C const &ptr) const
    { return m_ptr == ptr; }
    bool operator ==(borrowed_ptr<T> const &ptr) const
    { return m_ptr == ptr.m_ptr; }

    template <typename C>
    bool operator !=(C const &ptr) const
    { return m_ptr != ptr; }
    bool operator !=(borrowed_ptr<T> const &ptr) const
    { return m_ptr != ptr.m_ptr; }

  private:
    T *m_ptr{ nullptr };
};

template <typename T>
std::ostream& operator <<(std::ostream &stream, borrowed_ptr<T> const &ptr)
{ return stream << ptr.m_ptr; }
