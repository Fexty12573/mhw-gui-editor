#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

/// The dimensions of a compressed block. Compressed block sizes are usually 4x4 pixels.
struct BlockDim {
  /// The width of the block in pixels.
  uint32_t width;
  /// The height of the block in pixels.
  uint32_t height;
  /// The depth of the block in pixels.
  uint32_t depth;
};

extern "C" {

/// See [crate::surface::swizzle_surface].
///
/// # Safety
/// `source` and `source_len` should refer to an array with at least as many bytes as the result of [deswizzled_surface_size].
/// Similarly, `destination` and `destination_len` should refer to an array with at least as many bytes as as the result of [swizzled_surface_size].
///
/// All the fields of `block_dim` must be non zero.
///
/// `block_height` must be one of the supported values in [BlockHeight].
void swizzle_surface(uint32_t width,
                     uint32_t height,
                     uint32_t depth,
                     const uint8_t *source,
                     uintptr_t source_len,
                     uint8_t *destination,
                     uintptr_t destination_len,
                     BlockDim block_dim,
                     uint32_t block_height_mip0,
                     uint32_t bytes_per_pixel,
                     uint32_t mipmap_count,
                     uint32_t array_count);

/// See [crate::surface::deswizzle_surface].
///
/// # Safety
/// `source` and `source_len` should refer to an array with at least as many bytes as the result of [swizzled_surface_size].
/// Similarly, `destination` and `destination_len` should refer to an array with at least as many bytes as as the result of [deswizzled_surface_size].
///
/// All the fields of `block_dim` must be non zero.
///
/// `block_height` must be one of the supported values in [BlockHeight].
void deswizzle_surface(uint32_t width,
                       uint32_t height,
                       uint32_t depth,
                       const uint8_t *source,
                       uintptr_t source_len,
                       uint8_t *destination,
                       uintptr_t destination_len,
                       BlockDim block_dim,
                       uint32_t block_height_mip0,
                       uint32_t bytes_per_pixel,
                       uint32_t mipmap_count,
                       uint32_t array_count);

/// See [crate::surface::swizzle_surface].
///
/// # Safety
/// All the fields of `block_dim` must be non zero.
/// `block_height_mip0` must be one of the supported values in [BlockHeight].
uintptr_t swizzled_surface_size(uint32_t width,
                                uint32_t height,
                                uint32_t depth,
                                BlockDim block_dim,
                                uint32_t block_height_mip0,
                                uint32_t bytes_per_pixel,
                                uint32_t mipmap_count,
                                uint32_t array_count);

/// See [crate::surface::swizzle_surface].
///
/// # Safety
/// All the fields of `block_dim` must be non zero.
uintptr_t deswizzled_surface_size(uint32_t width,
                                  uint32_t height,
                                  uint32_t depth,
                                  BlockDim block_dim,
                                  uint32_t bytes_per_pixel,
                                  uint32_t mipmap_count,
                                  uint32_t array_count);

/// See [crate::swizzle::swizzle_block_linear].
///
/// # Safety
/// `source` and `source_len` should refer to an array with at least as many bytes as the result of [deswizzled_mip_size].
/// Similarly, `destination` and `destination_len` should refer to an array with at least as many bytes as as the result of [swizzled_mip_size].
///
/// `block_height` must be one of the supported values in [BlockHeight].
void swizzle_block_linear(uint32_t width,
                          uint32_t height,
                          uint32_t depth,
                          const uint8_t *source,
                          uintptr_t source_len,
                          uint8_t *destination,
                          uintptr_t destination_len,
                          uint32_t block_height,
                          uint32_t bytes_per_pixel);

/// See [crate::swizzle::deswizzle_block_linear].
///
/// # Safety
/// `source` and `source_len` should refer to an array with at least as many bytes as the result of [swizzled_mip_size].
/// Similarly, `destination` and `destination_len` should refer to an array with at least as many bytes as as the result of [deswizzled_mip_size].
///
/// `block_height` must be one of the supported values in [BlockHeight].
void deswizzle_block_linear(uint32_t width,
                            uint32_t height,
                            uint32_t depth,
                            const uint8_t *source,
                            uintptr_t source_len,
                            uint8_t *destination,
                            uintptr_t destination_len,
                            uint32_t block_height,
                            uint32_t bytes_per_pixel);

/// See [crate::swizzle::swizzled_mip_size].
///
/// # Safety
/// `block_height` must be one of the supported values in [BlockHeight].
uintptr_t swizzled_mip_size(uint32_t width,
                            uint32_t height,
                            uint32_t depth,
                            uint32_t block_height,
                            uint32_t bytes_per_pixel);

/// See [crate::swizzle::deswizzled_mip_size].
uintptr_t deswizzled_mip_size(uint32_t width,
                              uint32_t height,
                              uint32_t depth,
                              uint32_t bytes_per_pixel);

/// See [crate::block_height_mip0].
uint32_t block_height_mip0(uint32_t height);

/// See [crate::mip_block_height].
///
/// # Safety
/// `block_height_mip0` must be one of the supported values in [BlockHeight].
uint32_t mip_block_height(uint32_t mip_height, uint32_t block_height_mip0);

}  // extern "C"
