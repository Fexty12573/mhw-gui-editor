#include "pch.h"
#include "GUIEditor.h"
#include "App.h"
#include "RichText.h"

namespace utility {

static ImVec2 apply_control_point(ImVec2 pos, ImVec2 size, ControlPoint cp) {
    ImVec2 offset{ 0, 0 };

    switch (cp) {
    case ControlPoint::TL: break;
    case ControlPoint::TC: offset.x -= size.x * 0.5f; break;
    case ControlPoint::TR: offset.x -= size.x; break;
    case ControlPoint::CL: offset.y -= size.y * 0.5f; break;
    case ControlPoint::CC: offset.x -= size.x * 0.5f; offset.y -= size.y * 0.5f; break;
    case ControlPoint::CR: offset.x -= size.x; offset.y -= size.y * 0.5f; break;
    case ControlPoint::BL: offset.y -= size.y; break;
    case ControlPoint::BC: offset.x -= size.x * 0.5f; offset.y -= size.y; break;
    case ControlPoint::BR: offset.x -= size.x; offset.y -= size.y; break;
    default: break; // unknown values treated as TL
    }

    return pos + offset;
}

}


void GUIEditor::render_preview() {
    if (!m_file.is_mhgu()) {
        ImGui::Text("GUI preview is only available for MHGU files");
        return;
    }

    const auto text = m_preview_instance != -1
        ? m_file.m_instances[m_preview_instance].Name
        : "Select an instance to preview...";

    if (ImGui::BeginCombo("Instance", text.c_str())) {
        for (auto i = 0u; i < m_file.m_instances.size(); ++i) {
            ImGui::PushID(i);

            const auto& inst = m_file.m_instances[i];
            const bool selected = ImGui::Selectable("##Instance", m_preview_instance == i, ImGuiSelectableFlags_SpanAvailWidth);
            ImGui::SameLine();
            ImGui::RichText(inst.get_preview(i));

            if (selected) {
                m_preview_instance = i;
            }

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    if (m_preview_instance == -1) {
        return;
    }

    const ImVec2 size = {
        (float)m_file.get_width(),
        (float)m_file.get_height()
    };

    ImGui::SliderFloat("Offset X", &m_preview_offset.x, 0.0f, size.x);
    ImGui::SliderFloat("Offset Y", &m_preview_offset.y, 0.0f, size.y);

    const auto& inst = m_file.m_instances[m_preview_instance];

    const auto resourceParam = m_file.find_init_param(inst, "ResourceId");
    if (resourceParam && resourceParam->Value32 != 0xFFFFFFFF) {
        ImGui::Text("Instance must point to an animation within this file");
        return;
    }

    const auto animParam = m_file.find_init_param(inst, "AnimationId");
    if (!animParam || animParam->Value32 == 0xFFFFFFFF) {
        ImGui::Text("Instance must point to an animation within this file");
        return;
    }

    const AnimationPreviewContext ctx = {
        .Offset = m_preview_offset,
        .Scale = { 1, 1 },
        .Tint = { 1, 1, 1, 1 }
    };

    ImGui::BeginChild("Animation Preview", size, ImGuiChildFlags_FrameStyle);

    const auto& anim = std::ranges::find_if(m_file.m_animations, [animParam](const auto& anim) {
        return anim.ID == animParam->Value32;
    });

    if (anim == m_file.m_animations.end()) {
        ImGui::Text("Animation not found");
        ImGui::EndChild();
        return;
    }

    const auto& object = m_file.m_objects[anim->RootObjectIndex];

    render_object_preview(object, ctx);

    ImGui::EndChild();
}

void GUIEditor::render_object_preview(const GUIObject& obj, AnimationPreviewContext ctx) {
    const auto original_ctx = ctx;
    const auto draw_list = ImGui::GetWindowDrawList();

    switch (obj.Type) {
    default: [[fallthrough]];
    case ObjectType::None:
        return;

    case ObjectType::cGUIObjRoot:
        break;

    case ObjectType::cGUIObjColorAdjust: {
        const auto param = m_file.find_init_param(obj, "ColorScale");
        if (param) {
            ctx.Tint = ImVec4{
                param->ValueVector.x,
                param->ValueVector.y,
                param->ValueVector.z,
                param->ValueVector.w
            };
        }
    } break;

    case ObjectType::cGUIObjScissorMask:
        // TODO: Implement
        break;

    case ObjectType::cGUIObj2D: [[fallthrough]];
    case ObjectType::cGUIObjNull: { // Parent class
        const auto p_posx = m_file.find_init_param(obj, "PosX");
        const auto p_posy = m_file.find_init_param(obj, "PosY");
        const auto p_sclx = m_file.find_init_param(obj, "SclX");
        const auto p_scly = m_file.find_init_param(obj, "SclY");

        ctx.Offset.x += p_posx ? p_posx->ValueFloat : 0;
        ctx.Offset.y += p_posy ? p_posy->ValueFloat : 0;
        ctx.Scale.x *= p_sclx ? p_sclx->ValueFloat : 1;
        ctx.Scale.y *= p_scly ? p_scly->ValueFloat : 1;
    } break;

    case ObjectType::cGUIObjChildAnimationRoot:
        // TODO: Implement
        break;

    case ObjectType::cGUIObjPolygon: {
        const auto p_posx = m_file.find_init_param(obj, "PosX");
        const auto p_posy = m_file.find_init_param(obj, "PosY");
        const auto p_sclx = m_file.find_init_param(obj, "SclX");
        const auto p_scly = m_file.find_init_param(obj, "SclY");
        const auto p_size = m_file.find_init_param(obj, "Size");
        const auto p_color = m_file.find_init_param(obj, "VertexColor");
        const auto p_control_point = m_file.find_init_param(obj, "ControlPoint");

        if (!p_color || !p_size) {
            break;
        }

        ctx.Offset.x += p_posx ? p_posx->ValueFloat : 0;
        ctx.Offset.y += p_posy ? p_posy->ValueFloat : 0;
        ctx.Scale.x *= p_sclx ? p_sclx->ValueFloat : 1;
        ctx.Scale.y *= p_scly ? p_scly->ValueFloat : 1;

        if (p_color) {
            ctx.Tint.x *= p_color->ValueVector.x;
            ctx.Tint.y *= p_color->ValueVector.y;
            ctx.Tint.z *= p_color->ValueVector.z;
            ctx.Tint.w *= p_color->ValueVector.w;
        }

        const auto raw_pos = ImGui::GetCursorScreenPos() + ctx.Offset * ctx.Scale;
        const auto scaled_size = ImVec2{
            p_size->ValueVector.x * ctx.Scale.x,
            p_size->ValueVector.y * ctx.Scale.y
        };

        const auto control_point = p_control_point ? (ControlPoint)p_control_point->Value32 : ControlPoint::TL;
        const auto pos = utility::apply_control_point(raw_pos, scaled_size, control_point);

        const auto min = pos;
        const auto max = pos + scaled_size;

        draw_list->AddRectFilled(
            min, max,
            ImGui::ColorConvertFloat4ToU32(ctx.Tint)
        );
    } break;

    case ObjectType::cGUIObjTexWithParam: [[fallthrough]];
    case ObjectType::cGUIObjBlendTextureSample: [[fallthrough]];
    case ObjectType::cGUIObjTexture: {
        const auto p_texture = m_file.find_init_param(obj, "Texture");
        //const auto p_blend_texture = m_file.find_init_param(obj, "BlendTexture"); TODO: Implement
        const auto p_rect = m_file.find_init_param(obj, "mTextureRect");
        const auto p_color = m_file.find_init_param(obj, "VertexColor");
        const auto p_size = m_file.find_init_param(obj, "Size");
        const auto p_sampler = m_file.find_init_param(obj, "SamplerState");
        const auto p_posx = m_file.find_init_param(obj, "PosX");
        const auto p_posy = m_file.find_init_param(obj, "PosY");
        const auto p_sclx = m_file.find_init_param(obj, "SclX");
        const auto p_scly = m_file.find_init_param(obj, "SclY");
        const auto p_ctrl_point = m_file.find_init_param(obj, "ControlPoint");

        if (!p_texture || p_texture->Value32 == 0xFFFFFFFF) {
            break;
        }

        const auto texture = std::ranges::find_if(m_file.m_textures, [p_texture](const auto& tex) {
            return tex.ID == p_texture->Value32;
        });

        if (texture == m_file.m_textures.end()) {
            break;
        }

        const auto control_point = p_ctrl_point ? (ControlPoint)p_ctrl_point->Value32 : ControlPoint::TL;

        ctx.Offset.x += p_posx ? p_posx->ValueFloat : 0;
        ctx.Offset.y += p_posy ? p_posy->ValueFloat : 0;
        ctx.Scale.x *= p_sclx ? p_sclx->ValueFloat : 1;
        ctx.Scale.y *= p_scly ? p_scly->ValueFloat : 1;

        const ImVec2 uv0 = p_rect ? ImVec2{
            p_rect->ValueVector.x / texture->Width,
            p_rect->ValueVector.y / texture->Height
        } : ImVec2{ 0, 0 };

        const ImVec2 uv1 = p_rect ? ImVec2{
            p_rect->ValueVector.z / texture->Width,
            p_rect->ValueVector.w / texture->Height
        } : ImVec2{ 1, 1 };

        if (p_color) {
            ctx.Tint.x *= p_color->ValueVector.x;
            ctx.Tint.y *= p_color->ValueVector.y;
            ctx.Tint.z *= p_color->ValueVector.z;
            ctx.Tint.w *= p_color->ValueVector.w;
        }

        const auto raw_pos = ImGui::GetCursorScreenPos() + ctx.Offset * ctx.Scale;
        const auto scaled_size = p_size ? ImVec2{
            p_size->ValueVector.x * ctx.Scale.x,
            p_size->ValueVector.y * ctx.Scale.y
        } : p_rect ? ImVec2{
            (p_rect->ValueVector.z - p_rect->ValueVector.x) * ctx.Scale.x,
            (p_rect->ValueVector.z - p_rect->ValueVector.y)* ctx.Scale.y
        } : ImVec2{
            texture->Width * ctx.Scale.x,
            texture->Height * ctx.Scale.y
        };

        const auto pos = utility::apply_control_point(raw_pos, scaled_size, control_point);

        //const auto half_size = ImVec2{
        //    p_size->ValueVector.x * 0.5f * ctx.Scale.x,
        //    p_size->ValueVector.y * 0.5f * ctx.Scale.y
        //};

        const auto min = pos;
        const auto max = pos + scaled_size;

        if (p_sampler) {
            const auto& sampler = m_owner->get_samplers().at((SamplerMode)p_sampler->Value32);
            const auto& callback_data = m_draw_callback_data.emplace_back(
                std::make_unique<DrawCallbackData>(
                    m_app->get_context().Get(),
                    sampler.get(),
                    nullptr
                )
            );

            // Set custom sampler
            draw_list->AddCallback([](const ImDrawList*, const ImDrawCmd* cmd) {
                const auto data = (DrawCallbackData*)cmd->UserCallbackData;
                if (data) {
                    data->Context->PSGetSamplers(0, 1, &data->OriginalState);
                }
            }, callback_data.get());

            draw_list->AddImage(
                texture->RenderTexture.get_view().Get(),
                min, max,
                uv0, uv1,
                ImGui::ColorConvertFloat4ToU32(ctx.Tint)
            );

            draw_list->AddCallback([](const ImDrawList*, const ImDrawCmd* cmd) {
                const auto data = (DrawCallbackData*)cmd->UserCallbackData;
                if (data) {
                    data->Context->PSSetSamplers(0, 1, &data->OriginalState);
                }
            }, callback_data.get());
        } else {
            draw_list->AddImage(
                texture->RenderTexture.get_view().Get(),
                min, max,
                uv0, uv1,
                ImGui::ColorConvertFloat4ToU32(ctx.Tint)
            );
        }
    } break;

    case ObjectType::cGUIObjTextureSet: {
        const auto p_texture = m_file.find_init_param(obj, "Texture");
        const auto p_size = m_file.find_init_param(obj, "Size");
        const auto p_color = m_file.find_init_param(obj, "VertexColor");
        const auto p_sampler = m_file.find_init_param(obj, "SamplerState");
        const auto p_posx = m_file.find_init_param(obj, "PosX");
        const auto p_posy = m_file.find_init_param(obj, "PosY");
        const auto p_sclx = m_file.find_init_param(obj, "SclX");
        const auto p_scly = m_file.find_init_param(obj, "SclY");
        const auto p_ctrl_point = m_file.find_init_param(obj, "ControlPoint");

        if (!p_texture || p_texture->Value32 == 0xFFFFFFFF) {
            break;
        }

        ctx.Offset.x += p_posx ? p_posx->ValueFloat : 0;
        ctx.Offset.y += p_posy ? p_posy->ValueFloat : 0;
        ctx.Scale.x *= p_sclx ? p_sclx->ValueFloat : 1;
        ctx.Scale.y *= p_scly ? p_scly->ValueFloat : 1;

        if (p_color) {
            ctx.Tint.x *= p_color->ValueVector.x;
            ctx.Tint.y *= p_color->ValueVector.y;
            ctx.Tint.z *= p_color->ValueVector.z;
            ctx.Tint.w *= p_color->ValueVector.w;
        }

        const auto texture = std::ranges::find_if(m_file.m_textures, [p_texture](const auto& tex) {
            return tex.ID == p_texture->Value32;
        });

        if (texture == m_file.m_textures.end()) {
            break;
        }

        const auto control_point = p_ctrl_point ? (ControlPoint)p_ctrl_point->Value32 : ControlPoint::TL;

        for (const auto& uv : obj.ExtendData.UVs) {
            const ImVec2 uv0 = { uv.x, uv.y };
            const ImVec2 uv1 = { uv.w, uv.h };

            const auto raw_pos = ImGui::GetCursorScreenPos() + ctx.Offset * ctx.Scale;
            const auto scaled_size = p_size ? ImVec2{
                p_size->ValueVector.x * ctx.Scale.x,
                p_size->ValueVector.y * ctx.Scale.y
            } : ImVec2{
                (uv.w - uv.x) * ctx.Scale.x,
                (uv.h - uv.y) * ctx.Scale.y
            };

            const auto pos = utility::apply_control_point(raw_pos, scaled_size, control_point);
            const auto min = pos;
            const auto max = pos + scaled_size;

            if (p_sampler) {
                const auto& sampler = m_owner->get_samplers().at((SamplerMode)p_sampler->Value32);
                const auto& callback_data = m_draw_callback_data.emplace_back(
                    std::make_unique<DrawCallbackData>(
                        m_app->get_context().Get(),
                        sampler.get(),
                        nullptr
                    )
                );

                // Set custom sampler
                draw_list->AddCallback([](const ImDrawList*, const ImDrawCmd* cmd) {
                    const auto data = (DrawCallbackData*)cmd->UserCallbackData;
                    if (data) {
                        data->Context->PSGetSamplers(0, 1, &data->OriginalState);
                    }
                }, callback_data.get());

                draw_list->AddImage(
                    texture->RenderTexture.get_view().Get(),
                    min, max,
                    uv0, uv1,
                    ImGui::ColorConvertFloat4ToU32(ctx.Tint)
                );

                draw_list->AddCallback([](const ImDrawList*, const ImDrawCmd* cmd) {
                    const auto data = (DrawCallbackData*)cmd->UserCallbackData;
                    if (data) {
                        data->Context->PSSetSamplers(0, 1, &data->OriginalState);
                    }
                }, callback_data.get());
            } else {
                draw_list->AddImage(
                    texture->RenderTexture.get_view().Get(),
                    min, max,
                    uv0, uv1,
                    ImGui::ColorConvertFloat4ToU32(ctx.Tint)
                );
            }
        }
    } break;
    case ObjectType::cGUIObjMaterial:
        break;
    case ObjectType::cGUIObjMessage:
        break;
    case ObjectType::cGUIObjText:
        break;
    case ObjectType::cGUIObjFreePolygon:
        break;
    case ObjectType::cFestaGUIObjPolygon:
        break;
    case ObjectType::cFestaGUIObjTexture:
        break;
    }

    if (obj.NextIndex != -1) {
        // "Next" objects are not children of the current object, so they don't
        // inherit the context
        render_object_preview(m_file.m_objects[obj.NextIndex], original_ctx);
    }

    if (obj.ChildIndex != -1) {
        // Child objects inherit the context
        render_object_preview(m_file.m_objects[obj.ChildIndex], ctx);
    }
}
