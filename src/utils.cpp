// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "ezdxf/utils.hpp"
#include "ezdxf/tag/tag.hpp"
#include <exception>

using namespace ezdxf::tag;

namespace ezdxf::utils {

    // Source: https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
    void ltrim(std::string &s) {
        // trim from start (in place)
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
    }

    void rtrim(std::string &s) {
        // trim from end (in place)
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    void trim(std::string &s) {
        // trim from both ends (in place)
        ltrim(s);
        rtrim(s);
    }

    void rtrim_endl(std::string &s) {
        // trim <CR><LF> from end (in place)
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return ch != 10 && ch != 13;
        }).base(), s.end());
    }

    std::optional<Real> safe_str_to_real(const String &s) {
        try {
            return stod(s);
        }
        catch (...) {
            return {};
        }
    }

    std::optional<int64_t> safe_str_to_int64(const String &s) {
        try {
            return stoll(s);
        }
        catch (...) {
            return {};
        }
    }

    int safe_group_code(const String &s) {
        // Returns kError for invalid group codes.
        // valid group codes are in the range [0 .. 1071]
        try {
            auto code = stoi(s);
            return is_valid_group_code(code) ? code : GroupCode::kError;
        }
        catch (...) {
            return GroupCode::kError;
        }
    }

    inline static unsigned char _nibble_to_char(const unsigned char nibble) {
        // Convert a nibble (0-15) into a char '0'-'9', 'A'-'F'
        return nibble + (nibble < 10 ? 0x30 : 0x37); // '0' : 'A' - 10
    }

    String hexlify(const Bytes &data) {
        // Convert Binary data into a continuous hex string
        // e.g. {0xfe, 0xfe, ...} to "FEFE..."
        //
        // Returns uppercase hex chars.

        auto buffer = String();
        buffer.reserve(data.size() * 2);

        for (const unsigned char byte: data) {
            buffer.push_back(_nibble_to_char(byte >> 4));
            buffer.push_back(_nibble_to_char(byte & 0x0f));
        }
        return buffer;
    }

    inline static char _char_to_nibble(const char c) {
        // Convert an ascii hex char into a number e.g. 'A' -> 10.
        // Valid chars '0'-'9', 'A'-'F', 'a'-'f'
        //
        // Returns -1 for invalid chars.

        if (c >= '0' && c <= '9')
            return c - 0x30;
        if (c >= 'A' && c <= 'F')
            return c - 0x37;
        if (c >= 'a' && c <= 'f')
            return c - 0x57;
        return -1;  // error
    }

    std::optional<Bytes> unhexlify(String s) {
        // Convert a continuous hex string into binary data
        // e.g. "FEFE..." to {0xfe, 0xfe, ...}.
        //
        // If argument `s` contains an uneven count of chars, the last char is
        // ignored!

        trim(s); // trim white space on both sides inplace
        Bytes bytes{};
        bytes.reserve(s.size() >> 1);
        bool high_nibble = true;
        unsigned char byte;
        signed char nibble;

        for (const char c : s) {
            nibble = _char_to_nibble(c);
            if (nibble < 0) return {};  // string contains invalid chars
            if (high_nibble) {
                byte = nibble << 4;
            } else {
                bytes.push_back(byte | nibble);
            }
            high_nibble = !high_nibble;
        }
        return bytes;
    }

    Bytes concatenate_bytes(const std::vector<Bytes> &data) {
        Bytes merged = Bytes{};
        if (!data.empty()) {
            std::size_t sum = 0;
            for (const auto &bytes : data) {
                sum += bytes.size();
            }
            merged.reserve(sum);
            for (const auto &bytes : data) {
                merged.insert(merged.end(), bytes.begin(), bytes.end());
            }
        }
        return merged;
    }

    String dxf_version_to_str(Version v) {
        switch (v) {
            case Version::R9:
                return "AC1004";
            case Version::R10:
                return "AC1006";
            case Version::R12:
                return "AC1009";
            case Version::R13:
                return "AC1012";
            case Version::R14:
                return "AC1014";
            case Version::R2000:
                return "AC1015";
            case Version::R2004:
                return "AC1018";
            case Version::R2007:
                return "AC1021";
            case Version::R2010:
                return "AC1024";
            case Version::R2013:
                return "AC1027";
            case Version::R2018:
                return "AC1032";
            default:
                return "AC1009";
        }
    }

    Version str_to_dxf_version(const String s) { // NOLINT(performance-unnecessary-value-param)
        if (s == "AC1004") return Version::R9;
        if (s == "AC1006") return Version::R10;
        if (s == "AC1009") return Version::R12;
        if (s == "AC1012") return Version::R13;
        if (s == "AC1014") return Version::R14;
        if (s == "AC1015") return Version::R2000;
        if (s == "AC1018") return Version::R2004;
        if (s == "AC1021") return Version::R2007;
        if (s == "AC1024") return Version::R2010;
        if (s == "AC1027") return Version::R2013;
        if (s == "AC1032") return Version::R2018;
        return Version::R12;  // Default
    }

    const SimpleSet<Version> DXFExportVersions{
            Version::R12,
            Version::R2000,
            Version::R2004,
            Version::R2007,
            Version::R2010,
            Version::R2013,
            Version::R2018,
    };

    class EntityLookupTable {
        // Goal: compact and fast enough DXFEntity lookup by handle.
        // Relationship between Handle and DXFEntity is fixed and does not
        // change over the lifetime of a document and entities will also not
        // destroyed, therefore deleting entries is not required.
        // Table size is fixed and will not grow over runtime.

    private:
        using Data = struct {
            uint64_t handle;
            void *data;
        };

        uint64_t hash_mask_ = 0;
        std::vector<std::vector<Data>> data_{};

        void init_data(int size) {
            data_.clear();
            data_.reserve(size);
            for (int i = 0; i < size; ++i) {
                data_.emplace_back();
            }
        }

        auto get_bucket(uint64_t handle) {
            return data_[handle & hash_mask_];
        }

    public:
        EntityLookupTable() = delete;

        // 2^12 = 4096 buckets by default:
        explicit EntityLookupTable(int hash_bits = 12) {
            if (hash_bits < 8 || hash_bits > 24)
                throw std::invalid_argument("invalid hash bit count");
            hash_mask_ = (2 << hash_bits) - 1;
            init_data(2 << hash_bits);
        }

        // "get()" is the most important function here:
        void *get(uint64_t handle, void *default_ = nullptr) {
            // Linear search is fast for small vectors!
            for (auto[h, data] : get_bucket(handle)) {
                if (h == handle) return data;
            }
            return default_;
        }

        inline bool has(uint64_t handle) {
            return get(handle) != nullptr;
        }

        void add(uint64_t handle, void *data) {
            if (!has(handle)) {
                get_bucket(handle).emplace_back(handle, data);
            } else throw(std::invalid_argument("handle already exist"));
        }
    };
}
