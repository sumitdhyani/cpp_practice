#include <functional>
#include <unordered_map>
template<class Yin, class Yang>
struct Agregator {
    Agregator(
        std::function<void(Yin)> yingFunc,
        std::function<void(Yang)> yangFunc,
        std::function<size_t(const Yin&)> keyFromYin,
        std::function<size_t(const Yang&)> keyFromYang
    ) : m_yingFunc(yingFunc),
        m_yangFunc(yangFunc),
        m_keyFromYin(keyFromYin),
        m_keyFromYang(keyFromYang) {}

    void onData(Yin yin) {
        //According to standard the default value of a scalar value type is 0,
        //so this line sleek and safe and cool! 
        size_t& count = m_countMap[m_keyFromYin(yin)];
        if (0 == count) {
            m_yingFunc(yin);
        }
        count++;
    }

    void onData(Yang yang) {
        if (auto it = m_countMap.find(m_keyFromYang(yang)); it != m_countMap.end()) {
            m_yangFunc(yang);
            size_t& count = *it
            if(--count == 0) {
                m_countMap.erase(it);
            }
            return true;
        } else {
            return false;
        }
    }

private:
    std::function<void(Yin)> m_yingFunc;
    std::function<void(Yang)> m_yangFunc;
    std::function<size_t(Yin)> m_keyFromYin;
    std::function<size_t(Yang)> m_keyFromYang;
    std::unordered_map<size_t, size_t> m_countMap;
};
