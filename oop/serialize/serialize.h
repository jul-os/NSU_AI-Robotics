#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <cstddef>

// Форвард-декларации
template <typename T>
void serialize(const T &obj, std::ostream &os);

template <typename T>
void deserialize(T &obj, std::istream &is);

// POD-типы
//  Указатели здесь используются ТОЛЬКО как временные reinterpret-касты,
//  необходимые для бинарного ввода-вывода. Они не владеют памятью,
//  поэтому умные указатели не применимы
template <typename T>
struct serializer
{
    static void apply(const T &obj, std::ostream &os)
    {
        os.write(reinterpret_cast<const char *>(&obj), sizeof(T));
        // запись sizeof байтов
    }
};

template <typename T>
struct deserializer
{
    static void apply(T &obj, std::istream &is)
    {
        is.read(reinterpret_cast<char *>(&obj), sizeof(T));
        // читает их же
    }
};

// std::string
template <>
struct serializer<std::string>
{
    static void apply(const std::string &str, std::ostream &os)
    {
        size_t size = str.size();
        serialize(size, os);
        if (size > 0)
        {
            os.write(str.data(), static_cast<std::streamsize>(size));
        }
        // записать длину size_t, птом саму строку
    }
};

template <>
struct deserializer<std::string>
{
    static void apply(std::string &str, std::istream &is)
    {
        size_t size;
        deserialize(size, is);
        str.resize(size);
        if (size > 0)
        {
            is.read(str.data(), static_cast<std::streamsize>(size));
        }
    }
};

// std::vector<T>
template <typename T>
struct serializer<std::vector<T>>
{
    static void apply(const std::vector<T> &vec, std::ostream &os)
    {
        size_t size = vec.size();
        serialize(size, os);
        for (const auto &item : vec)
        {
            serialize(item, os); // рекурсивно сериализовать каждый элемент
        }
    }
};

template <typename T>
struct deserializer<std::vector<T>>
{
    static void apply(std::vector<T> &vec, std::istream &is)
    {
        size_t size;
        deserialize(size, is);
        vec.resize(size);
        for (auto &item : vec)
        {
            deserialize(item, is);
        }
    }
};

// std::map<K, V>
template <typename K, typename V>
struct serializer<std::map<K, V>>
{
    static void apply(const std::map<K, V> &mp, std::ostream &os)
    {
        size_t size = mp.size();
        serialize(size, os);
        for (const auto &pair : mp)
        {
            serialize(pair.first, os); // ключ и значние по порядку
            serialize(pair.second, os);
        }
    }
};

template <typename K, typename V>
struct deserializer<std::map<K, V>>
{
    static void apply(std::map<K, V> &mp, std::istream &is)
    {
        size_t size;
        deserialize(size, is);
        mp.clear();
        for (size_t i = 0; i < size; ++i)
        {
            K key;
            V value;
            deserialize(key, is);
            deserialize(value, is);
            mp.emplace(std::move(key), std::move(value));
        }
    }
};

template <typename T>
void serialize(const T &obj, std::ostream &os)
{
    serializer<T>::apply(obj, os);
}

template <typename T>
void deserialize(T &obj, std::istream &is)
{
    deserializer<T>::apply(obj, is);
}