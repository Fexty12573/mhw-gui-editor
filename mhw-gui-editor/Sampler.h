#pragma once
#include <d3d11.h>
#include <wrl.h>

#include "GUITypes.h"


class Sampler {
public:
    explicit Sampler(ID3D11Device* device, SamplerMode mode);
    Sampler() = default;

    [[nodiscard]] auto& get() const { return m_sampler; }
    [[nodiscard]] SamplerMode get_mode() const { return m_mode; }

private:
    SamplerMode m_mode;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
};
