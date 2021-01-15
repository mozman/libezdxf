// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_SIMPLE_SET_HPP
#define EZDXF_SIMPLE_SET_HPP

namespace ezdxf {
    template<typename T>
    class SimpleSet {
    public:
        SimpleSet(std::initializer_list <T> l) : data_{} {
            for (const T &d : l) add(d);
        }

        bool has(const T &v) const {
            for (const T &d : data_) if (d == v) return true;
            return false;
        }

        void add(const T &v) {
            if (!has(v)) data_.push_back(v);
        }

        [[nodiscard]] std::size_t size() const { return data_.size(); }

    private:
        std::vector <T> data_{};
    };
}

#endif //EZDXF_SIMPLE_SET_HPP
