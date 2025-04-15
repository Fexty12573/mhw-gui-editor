#pragma once

#include "BinaryReader.h"
#include "dti_types.h"

#include <d3d11.h>
#include <dxgiformat.h>
#include <wrl.h>

class Texture {
    enum class FORMAT {
        UNKNOWN = 0,
        R8G8B8A8_UNORM = 7,
        R8G8B8A8_UNORM_SRGB = 9,
        R8G8_UNORM = 19,
        BC1_UNORM = 22,
        BC1_UNORM_SRGB = 23,
        BC4_UNORM = 24,
        BC5_UNORM = 26,
        BC6H_UF16 = 28,
        BC7_UNORM = 30,
        BC7_UNORM_SRGB = 31,

        MHGU_UNKNOWN = 0,
        MHGU_R8G8B8A8_UNORM = 7,
        MHGU_BC1_UNORM = 19,
        MHGU_BC1_UNORM_SRGB = 20,
        MHGU_BC3_UNORM = 23,
        MHGU_BCX_GRAYSCALE = 25,
        MHGU_BCX_NM2 = 31,
        MHGU_BC7 = 42,
    };

    static DXGI_FORMAT convert_format(FORMAT fmt, bool mhgu = false);
    static u32 format_to_fourcc(FORMAT fmt, bool mhgu = false);

    static bool is_4bpp(FORMAT fmt);
    static bool is_16bpp(FORMAT fmt);

    // MHGU Only
    static std::pair<size_t, size_t> get_block_size(FORMAT format);

public:
    explicit Texture(BinaryReader& reader, bool mhgu, ID3D11Device* dev, ID3D11DeviceContext* ctx);
    Texture();

    void load_from(BinaryReader& reader, bool mhgu, ID3D11Device* dev, ID3D11DeviceContext* ctx);

    [[nodiscard]] bool is_valid() const { return m_texture && m_view; }
    [[nodiscard]] auto& get_view() const { return m_view; }

    [[nodiscard]] u32 get_width() const { return m_width; }
    [[nodiscard]] u32 get_height() const { return m_height; }
    [[nodiscard]] u32 get_mip_count() const { return m_mip_count; }

private:
    Microsoft::WRL::ComPtr<ID3D11Resource> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_view;

    u32 m_width;
    u32 m_height;
    u32 m_mip_count;
};

