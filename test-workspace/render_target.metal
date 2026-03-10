#include <metal_stdlib>
using namespace metal;

// =============================================================================
// Render target functions — tile-based deferred rendering, MSAA
// =============================================================================

// G-buffer layout for deferred rendering
struct GBufferData {
    float4 albedo;     // color attachment 0
    float4 normal;     // color attachment 1
    float4 position;   // color attachment 2
    float emission;    // color attachment 3.r
};

// Deferred lighting pass — reads from G-buffer color attachments
fragment float4 deferred_lighting(
    float4 frag_coord [[position]])
{
    // Read from G-buffer attachments
    float4 albedo = render_target_read<float4>(0);
    float4 normal_data = render_target_read<float4>(1);
    float4 world_pos = render_target_read<float4>(2);

    float3 N = normalize(normal_data.xyz * 2.0f - 1.0f);
    float3 P = world_pos.xyz;
    float3 color = albedo.rgb;

    // Simple directional light
    float3 L = normalize(float3(1.0f, 1.0f, 0.5f));
    float NdotL = max(dot(N, L), 0.0f);

    float3 diffuse = color * NdotL;
    float3 ambient = color * 0.1f;

    return float4(diffuse + ambient, albedo.a);
}

// MSAA resolve pass — reads per-sample and averages
fragment float4 msaa_resolve(
    float4 frag_coord [[position]])
{
    uint sample_count = get_render_target_sample_count();

    float4 resolved = float4(0.0f);
    for (uint i = 0; i < sample_count; ++i) {
        resolved += render_target_read<float4>(0, i);
    }
    resolved /= float(sample_count);

    return resolved;
}

// Write to multiple render targets (MRT) in a single pass
fragment void gbuffer_write(
    float4 frag_coord [[position]])
{
    float4 albedo = float4(0.8f, 0.2f, 0.1f, 1.0f);
    float4 normal = float4(0.5f, 0.5f, 1.0f, 0.0f);
    float4 position_data = float4(frag_coord.xyz, 1.0f);

    render_target_write(albedo, 0);
    render_target_write(normal, 1);
    render_target_write(position_data, 2);
}

// MSAA write — write to specific samples
fragment void msaa_write(
    float4 frag_coord [[position]])
{
    uint sample_count = get_render_target_sample_count();

    for (uint i = 0; i < sample_count; ++i) {
        float4 color = float4(float(i) / float(sample_count), 0.5f, 0.5f, 1.0f);
        render_target_write(color, 0, i);
    }
}

// Variable rasterization rate — uses fragment size
fragment float4 variable_rate_shading(
    float4 frag_coord [[position]])
{
    ushort2 frag_size = get_fragment_size();

    // Coarse fragments get simpler shading
    bool is_coarse = (frag_size.x > 1 || frag_size.y > 1);

    float4 color;
    if (is_coarse) {
        // Simplified shading for coarse fragments
        color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    } else {
        // Full-quality shading for fine fragments
        float2 uv = frag_coord.xy / float2(1920.0f, 1080.0f);
        color = float4(uv, 0.5f, 1.0f);
    }

    return color;
}

// Tile-based deferred rendering with derivative-based edge detection
fragment float4 edge_detection_deferred(
    float4 frag_coord [[position]])
{
    float4 center = render_target_read<float4>(0);
    float depth = center.w;

    // Use screen-space derivatives to detect edges
    float depth_dx = dfdx(depth);
    float depth_dy = dfdy(depth);
    float edge_strength = length(float2(depth_dx, depth_dy));

    uint num_attachments = get_num_color_attachments();

    if (edge_strength > 0.01f) {
        return float4(1.0f, 0.0f, 0.0f, 1.0f); // Red edges
    }

    return center;
}

// Fragment shader with front-face test and discard
fragment float4 two_sided_shading(
    float4 frag_coord [[position]])
{
    bool front = is_front_face();

    if (front) {
        return float4(0.2f, 0.6f, 1.0f, 1.0f); // Blue front face
    } else {
        return float4(1.0f, 0.3f, 0.2f, 0.5f); // Red translucent back face
    }
}

// Combined: read G-buffer, apply post-process with derivatives
fragment float4 post_process_pass(
    float4 frag_coord [[position]])
{
    // Read scene color and depth from G-buffer
    float4 scene_color = render_target_read<float4>(0);
    float4 depth_data = render_target_read<float4>(1);

    // Screen-space ambient occlusion approximation using derivatives
    float depth = depth_data.r;
    float dx = dfdx_fine(depth);
    float dy = dfdy_fine(depth);
    float curvature = abs(dx) + abs(dy);

    // Darken based on curvature (poor man's SSAO)
    float ao = saturate(1.0f - curvature * 10.0f);

    // Vignette using fragment size
    ushort2 fsize = get_fragment_size();
    float2 uv = frag_coord.xy / float2(1920.0f, 1080.0f);
    float vignette = 1.0f - dot(uv - 0.5f, uv - 0.5f) * 2.0f;

    return float4(scene_color.rgb * ao * vignette, scene_color.a);
}
