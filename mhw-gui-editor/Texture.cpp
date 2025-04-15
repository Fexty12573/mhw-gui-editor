#include "pch.h"
#include "Texture.h"
#include "DDSTextureLoader.h"
#include "HrException.h"
#include "Console.h"

#include "tegra_swizzle/tegra_swizzle.h"
#include <vector>

// 11 22 33 44
// 0x44332211
// 98 ab 7f 42 01
// 0x1427fab98

#define MAKEFOURCC_(a, b, c, d) (static_cast<u32>(a) | (static_cast<u32>(b) << 8) | (static_cast<u32>(c) << 16) | (static_cast<u32>(d) << 24))
#define MAKEFOURCC(fourcc_str) MAKEFOURCC_((fourcc_str)[0], (fourcc_str)[1], (fourcc_str)[2], (fourcc_str)[3])

enum DDS_FLAGS : u32 {
    DDSD_CAPS = 0x1,
    DDSD_HEIGHT = 0x2,
    DDSD_WIDTH = 0x4,
    DDSD_PITCH = 0x8,
    DDSD_PIXELFORMAT = 0x1000,
    DDSD_MIPMAPCOUNT = 0x20000,
    DDSD_LINEARSIZE = 0x80000,
    DDSD_DEPTH = 0x800000
};

enum DDS_PIXELFORMAT_FLAGS : u32 {
    DDPF_ALPHAPIXELS = 0x1,
    DDPF_ALPHA = 0x2,
    DDPF_FOURCC = 0x4,
    DDPF_RGB = 0x40,
    DDPF_YUV = 0x200,
    DDPF_LUMINANCE = 0x20000
};

enum DDS_PIXELDATA_COMPRESSION : u32 {
    DDSCAPS_COMPLEX = 0x8,
    DDSCAPS_TEXTURE = 0x1000,
    DDSCAPS_MIPMAP = 0x400000
};

struct TexHeaderMHGU {
    u32 magic;
    u32 version : 16;
    u32 attr : 8;
    u32 prebias : 4;
    u32 type : 4;
    u32 level_count : 6;
    u32 width : 13;
    u32 height : 13;
    u32 array_count : 8;
    u32 format : 8;
    u32 depth : 13;
    u32 auto_resize : 1;
    u32 render_target : 1;
    u32 use_vtf : 1;
};

struct DDS_EXTENDED_HEADER {
    DDS_HEADER hdr;
    DDS_HEADER_DXT10 ext;
};

DXGI_FORMAT Texture::convert_format(FORMAT fmt, bool mhgu) {
    if (mhgu) {
        switch (fmt) {
        case FORMAT::MHGU_UNKNOWN:          return DXGI_FORMAT_UNKNOWN;
        case FORMAT::MHGU_R8G8B8A8_UNORM:   return DXGI_FORMAT_R8G8B8A8_UNORM;
        case FORMAT::MHGU_BC1_UNORM:        return DXGI_FORMAT_BC1_UNORM;
        case FORMAT::MHGU_BC1_UNORM_SRGB:   return DXGI_FORMAT_BC1_UNORM_SRGB;
        case FORMAT::MHGU_BC3_UNORM:        return DXGI_FORMAT_BC3_UNORM;
        case FORMAT::MHGU_BCX_GRAYSCALE:    return DXGI_FORMAT_BC4_UNORM;
        case FORMAT::MHGU_BCX_NM2:          return DXGI_FORMAT_BC5_UNORM;
        case FORMAT::MHGU_BC7:              return DXGI_FORMAT_BC7_UNORM;
        default:                            break;
        }
    } else {
        switch (fmt) {
        case FORMAT::UNKNOWN:               return DXGI_FORMAT_UNKNOWN;
        case FORMAT::R8G8B8A8_UNORM:        return DXGI_FORMAT_R8G8B8A8_UNORM;
        case FORMAT::R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case FORMAT::R8G8_UNORM:            return DXGI_FORMAT_R8G8_UNORM;
        case FORMAT::BC1_UNORM:             return DXGI_FORMAT_BC1_UNORM;
        case FORMAT::BC1_UNORM_SRGB:        return DXGI_FORMAT_BC1_UNORM_SRGB;
        case FORMAT::BC4_UNORM:             return DXGI_FORMAT_BC4_UNORM;
        case FORMAT::BC5_UNORM:             return DXGI_FORMAT_BC5_UNORM;
        case FORMAT::BC6H_UF16:             return DXGI_FORMAT_BC6H_UF16;
        case FORMAT::BC7_UNORM:             return DXGI_FORMAT_BC7_UNORM;
        case FORMAT::BC7_UNORM_SRGB:        return DXGI_FORMAT_BC7_UNORM_SRGB;
        default:                            break;
        }
    }
    

    return DXGI_FORMAT_UNKNOWN;
}

u32 Texture::format_to_fourcc(FORMAT fmt, bool mhgu) {
    if (mhgu) {
        switch (fmt) {
        case FORMAT::MHGU_UNKNOWN:          return MAKEFOURCC("UNKN");
        case FORMAT::MHGU_R8G8B8A8_UNORM:   return MAKEFOURCC("DX10");
        case FORMAT::MHGU_BC1_UNORM:        return MAKEFOURCC("DXT1");
        case FORMAT::MHGU_BC1_UNORM_SRGB:   return MAKEFOURCC("DXT1");
        case FORMAT::MHGU_BC3_UNORM:        return MAKEFOURCC("DXT5");
        case FORMAT::MHGU_BCX_GRAYSCALE:    return MAKEFOURCC("ATI1");
        case FORMAT::MHGU_BCX_NM2:          return MAKEFOURCC("ATI2");
        case FORMAT::MHGU_BC7:              return MAKEFOURCC("DX10");
        default:                            break;
        }
    } else {
        switch (fmt) {
        case FORMAT::UNKNOWN:             return MAKEFOURCC("UNKN");
        case FORMAT::R8G8B8A8_UNORM:      [[fallthrough]];
        case FORMAT::R8G8B8A8_UNORM_SRGB: [[fallthrough]];
        case FORMAT::BC6H_UF16:           [[fallthrough]];
        case FORMAT::BC7_UNORM:           [[fallthrough]];
        case FORMAT::R8G8_UNORM:          [[fallthrough]];
        case FORMAT::BC1_UNORM_SRGB:      [[fallthrough]];
        case FORMAT::BC7_UNORM_SRGB:      return MAKEFOURCC("DX10");
        case FORMAT::BC1_UNORM:           return MAKEFOURCC("DXT1");
        case FORMAT::BC4_UNORM:           return MAKEFOURCC("BC4U");
        case FORMAT::BC5_UNORM:           return MAKEFOURCC("BC5U");
        default:                          break;
        }
    }
    

    return MAKEFOURCC("UNKN");
}

bool Texture::is_4bpp(FORMAT fmt) {
    return fmt == FORMAT::BC1_UNORM || 
        fmt == FORMAT::BC1_UNORM_SRGB || 
        fmt == FORMAT::BC4_UNORM ||
        fmt == FORMAT::MHGU_BC1_UNORM_SRGB ||
        fmt == FORMAT::MHGU_BCX_GRAYSCALE;
}

bool Texture::is_16bpp(FORMAT fmt) {
    return fmt == FORMAT::R8G8_UNORM ||
        fmt == FORMAT::BC5_UNORM ||
        fmt == FORMAT::MHGU_BCX_NM2;
}

std::pair<size_t, size_t> Texture::get_block_size(FORMAT format) {
    switch (format) {
    case FORMAT::MHGU_BC1_UNORM_SRGB: [[fallthrough]];
    case FORMAT::MHGU_BC1_UNORM:
        return { 8, 4 };  // 4x4 block, 8 bytes per block (BC1)

    case FORMAT::MHGU_R8G8B8A8_UNORM:
        return { 4, 1 };  // 1x1 pixel, 4 bytes per pixel (RGBA8)

    case FORMAT::MHGU_BCX_GRAYSCALE:
        return { 8, 4 };  // 4x4 block, 8 bytes per block (BC4)

    case FORMAT::MHGU_BC3_UNORM: [[fallthrough]];
    case FORMAT::MHGU_BCX_NM2: [[fallthrough]];
    case FORMAT::MHGU_BC7:
        return { 16, 4 }; // 4x4 block, 16 bytes per block (BC3/BC5/BC7)

    case FORMAT::MHGU_UNKNOWN: [[fallthrough]];
    default:
        return { 1, 0 };  // Unknown format
    }
}

Texture::Texture(BinaryReader& reader, bool mhgu, ID3D11Device* dev, ID3D11DeviceContext* ctx) {
    load_from(reader, mhgu, dev, ctx);
}

Texture::Texture() = default;

void Texture::load_from(BinaryReader& reader, bool mhgu, ID3D11Device* dev, ID3D11DeviceContext* ctx) {
    if (mhgu) {
        const auto texHeader = reader.read<TexHeaderMHGU>();
        if (texHeader.magic != MAKEFOURCC("TEX\0") || texHeader.version != 0x80A0) {
            return spdlog::error("Invalid TEX file");
        }

        m_mip_count = 1;
        m_width = texHeader.width;
        m_height = texHeader.height;
        
        const auto length = reader.read<u32>();
        reader.seek_relative(sizeof(u32) * texHeader.level_count);

        std::vector<u8> data;
        data.resize(length);
        reader.read_bytes(data);

        const auto format = static_cast<FORMAT>(texHeader.format);
        const auto dxformat = convert_format(format, true);

        if (dxformat == DXGI_FORMAT_UNKNOWN) {
            return spdlog::error("Unknown format: {}", static_cast<u32>(format));
        }

        const auto [block_size, block_dim] = get_block_size(format);

        const BlockDim dims = {
            .width = static_cast<uint32_t>(block_dim),
            .height = static_cast<uint32_t>(block_dim),
            .depth = 1
        };

        const uint32_t block_height = block_height_mip0(m_height);
        const uint32_t bytes_per_block = static_cast<uint32_t>(block_size);
        const uint32_t bytes_per_pixel = std::max(1u, static_cast<uint32_t>(block_size / (block_dim * block_dim)));

        const size_t block_w = (m_width + block_dim - 1) / block_dim;
        const size_t block_h = (m_height + block_dim - 1) / block_dim;
        const size_t unswizzled_size = block_w * block_h * block_size;

        //std::vector<u8> unswizzled(deswizzled_surface_size(
        //    m_width,
        //    m_height,
        //    1,
        //    dims,
        //    bytes_per_pixel,
        //    m_mip_count,
        //    1
        //));

        std::vector<u8> unswizzled(unswizzled_size);

        deswizzle_surface(
            m_width,
            m_height,
            1,
            data.data(),
            data.size(),
            unswizzled.data(),
            unswizzled.size(),
            dims,
            block_height,
            bytes_per_block,
            m_mip_count,
            1
        );

        DDS_EXTENDED_HEADER header{};
        header.hdr.size = sizeof(DDS_HEADER);
        header.hdr.flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;
        header.hdr.height = m_height;
        header.hdr.width = m_width;
        header.hdr.depth = 1;
        header.hdr.mipMapCount = 1;

        header.hdr.pitchOrLinearSize = static_cast<uint32_t>(unswizzled.size() / m_mip_count);

        header.hdr.ddspf.size = sizeof(DDS_PIXELFORMAT);
        header.hdr.ddspf.flags = DDPF_FOURCC;
        header.hdr.ddspf.fourCC = format_to_fourcc(format, true);
        header.hdr.caps = DDSCAPS_TEXTURE;

        if (header.hdr.ddspf.fourCC == MAKEFOURCC("DX10")) {
            header.ext.dxgiFormat = dxformat;
            header.ext.resourceDimension = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
            header.ext.arraySize = 1;
            header.ext.miscFlag = 0;
            header.ext.miscFlags2 = 0;
        }

        HR_INIT(S_OK);
        HR_ASSERT(DirectX::CreateTextureFromDDS(
            dev,
            ctx,
            &header.hdr,
            unswizzled.data(),
            unswizzled.size(),
            0,
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            D3D11_CPU_ACCESS_WRITE,
            0,
            DirectX::DDS_LOADER_DEFAULT,
            &m_texture,
            &m_view
        ));
    } else {
        if (reader.read<u32>() != MAKEFOURCC("TEX\0") || reader.read<u32>() != 0x10) {
            return spdlog::error("Invalid TEX file");
        }

        reader.seek_absolute(0x14);
        m_mip_count = reader.read<u32>();
        m_width = reader.read<u32>();
        m_height = reader.read<u32>();
        const auto format = reader.abs_offset_read<FORMAT>(0x24, false);
        const auto dxformat = convert_format(format);
        const auto offset = reader.abs_offset_read<s64>(0xB8, false);

        std::vector<u8> data;
        data.resize(reader.size() - offset);

        reader.seek_absolute(offset);
        reader.read_bytes(data);

        if (dxformat == DXGI_FORMAT_UNKNOWN) {
            return spdlog::error("Unknown format: {}", static_cast<u32>(format));
        }

        DDS_EXTENDED_HEADER header{};
        header.hdr.size = sizeof(DDS_HEADER);
        header.hdr.flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT | DDSD_LINEARSIZE;
        header.hdr.height = m_height;
        header.hdr.width = m_width;

        if (is_4bpp(format)) {
            header.hdr.pitchOrLinearSize = m_width * m_height / 2;
        } else if (is_16bpp(format)) {
            header.hdr.pitchOrLinearSize = m_width * m_height * 2;
        } else {
            header.hdr.pitchOrLinearSize = m_width * m_height;
        }

        header.hdr.depth = 1;
        header.hdr.mipMapCount = m_mip_count;
        header.hdr.ddspf.size = sizeof(DDS_PIXELFORMAT);
        header.hdr.ddspf.flags = DDPF_FOURCC;
        header.hdr.ddspf.fourCC = format_to_fourcc(format);
        header.hdr.caps = DDSCAPS_COMPLEX | DDSCAPS_MIPMAP | DDSCAPS_TEXTURE;

        if (header.hdr.ddspf.fourCC == MAKEFOURCC("DX10")) {
            header.ext.dxgiFormat = dxformat;
            header.ext.resourceDimension = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
            header.ext.arraySize = 1;
        }

        HR_INIT(S_OK);
        HR_ASSERT(DirectX::CreateTextureFromDDS(
            dev,
            ctx,
            &header.hdr,
            data.data(),
            data.size(),
            0,
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            D3D11_CPU_ACCESS_WRITE,
            0,
            DirectX::DDS_LOADER_DEFAULT,
            &m_texture,
            &m_view
        ));
    }
}

#undef MAKEFOURCC
