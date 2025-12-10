#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <cstddef>
#include <cstddef>
#include <concepts>
#include <functional>

// === Форвард-декларации шаблонных структур ===
template <typename T>
struct serializer;

template <typename T>
struct deserializer;

// === Концепты ===
template <typename T>
concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

template <typename T>
concept Serializable = requires(const T &obj, std::ostream &os) {
    serializer<T>::apply(obj, os);
};

template <typename T>
concept Deserializable = requires(T &obj, std::istream &is) {
    deserializer<T>::apply(obj, is);
};

template <TriviallyCopyable T>
struct serializer<T>
{
    static void apply(const T &obj, std::ostream &os)
    {
        os.write(reinterpret_cast<const char *>(&obj), sizeof(T));
    }
};

template <TriviallyCopyable T>
struct deserializer<T>
{
    static void apply(T &obj, std::istream &is)
    {
        is.read(reinterpret_cast<char *>(&obj), sizeof(T));
    }
};

template <>
struct serializer<std::string>
{
    static void apply(const std::string &str, std::ostream &os)
    {
        size_t size = str.size();
        serializer<size_t>::apply(size, os); // size_t - TriviallyCopyable
        if (size > 0)
        {
            os.write(str.data(), static_cast<std::streamsize>(size));
        }
    }
};

template <>
struct deserializer<std::string>
{
    static void apply(std::string &str, std::istream &is)
    {
        size_t size;
        deserializer<size_t>::apply(size, is);
        str.resize(size);
        if (size > 0)
        {
            is.read(str.data(), static_cast<std::streamsize>(size));
        }
    }
};

template <typename T>
    requires Serializable<T>
struct serializer<std::vector<T>>
{
    static void apply(const std::vector<T> &vec, std::ostream &os)
    {
        size_t size = vec.size();
        serializer<size_t>::apply(size, os);
        for (const auto &item : vec)
        {
            serializer<T>::apply(item, os);
        }
    }
};

template <typename T>
    requires Deserializable<T>
struct deserializer<std::vector<T>>
{
    static void apply(std::vector<T> &vec, std::istream &is)
    {
        size_t size;
        deserializer<size_t>::apply(size, is);
        vec.resize(size);
        for (auto &item : vec)
        {
            deserializer<T>::apply(item, is);
        }
    }
};

template <typename K, typename V>
    requires Serializable<K> && Serializable<V>
struct serializer<std::map<K, V>>
{
    static void apply(const std::map<K, V> &mp, std::ostream &os)
    {
        size_t size = mp.size();
        serializer<size_t>::apply(size, os);
        for (const auto &[key, value] : mp)
        {
            serializer<K>::apply(key, os);
            serializer<V>::apply(value, os);
        }
    }
};

template <typename K, typename V>
    requires Deserializable<K> && Deserializable<V>
struct deserializer<std::map<K, V>>
{
    static void apply(std::map<K, V> &mp, std::istream &is)
    {
        size_t size;
        deserializer<size_t>::apply(size, is);
        mp.clear();
        for (size_t i = 0; i < size; ++i)
        {
            K key;
            V value;
            deserializer<K>::apply(key, is);
            deserializer<V>::apply(value, is);
            mp.emplace(std::move(key), std::move(value));
        }
    }
};

template <typename T>
    requires Serializable<T>
void serialize(const T &obj, std::ostream &os)
{
    serializer<T>::apply(obj, os);
}

template <typename T>
    requires Deserializable<T>
void deserialize(T &obj, std::istream &is)
{
    deserializer<T>::apply(obj, is);
}