#include "pch.h"
#include "Sampler.h"

#include "HrException.h"


Sampler::Sampler(ID3D11Device* device, SamplerMode mode) : m_mode(mode) {
    D3D11_SAMPLER_DESC desc = {};

    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = 1;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    switch (mode) {
    case SamplerMode::WrapLinear:
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        break;
    case SamplerMode::WrapPoint:
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        break;
    case SamplerMode::ClampLinear:
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        break;
    case SamplerMode::ClampPoint:
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        break;
    }

    HR_INIT(S_OK);
    HR_ASSERT(device->CreateSamplerState(&desc, &m_sampler));
}
