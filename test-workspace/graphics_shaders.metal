#include <metal_stdlib>
using namespace metal;

// --- Shared structs ---

struct VertexIn {
    float3 position  [[attribute(0)]];
    float3 normal    [[attribute(1)]];
    float2 texCoord  [[attribute(2)]];
    float3 tangent   [[attribute(3)]];
    float3 bitangent [[attribute(4)]];
};

struct BasicVertexOut {
    float4 position [[position]];
    float3 color;
    float2 uv;
};

struct NormalMapVertexOut {
    float4 position [[position]];
    float2 uv;
    float3 worldPos;
    float3 tangentLightDir;
    float3 tangentViewDir;
};

struct ShadowVertexOut {
    float4 position [[position]];
    float3 worldPos;
    float3 normal;
    float4 shadowCoord;
    float2 uv;
};

struct PostProcessVertexOut { float4 position [[position]]; float2 uv; };
struct PBRVertexOut         { float4 position [[position]]; float3 worldPos; float3 normal; float2 uv; };
struct SkyboxVertexOut      { float4 position [[position]]; float3 uvw; };
struct InstanceVertexOut    { float4 position [[position]]; float3 color; float3 normal; };
struct VegetationVertexOut  { float4 position [[position]]; float2 uv; };

struct Uniforms {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float4x4 mvp;
    float4x4 lightSpaceMatrix;
    float3   cameraPos;
    float    time;
};

struct Light {
    float3 position;
    float3 direction;
    float3 color;
    float  intensity;
};

struct Material {
    float3 albedo;
    float  metallic;
    float  roughness;
    float  ao;
};

struct InstanceData {
    float4x4 modelMatrix;
    float4   color;
};

// --- 1. Basic vertex/fragment — MVP transform, texture sample ---

vertex BasicVertexOut basic_vertex(
    VertexIn in [[stage_in]],
    constant Uniforms& u [[buffer(0)]]
) {
    BasicVertexOut out;
    out.position = u.mvp * float4(in.position, 1.0);
    out.color = in.normal * 0.5 + 0.5;
    out.uv = in.texCoord;
    return out;
}

fragment float4 basic_fragment(
    BasicVertexOut in [[stage_in]],
    texture2d<float, access::sample> tex [[texture(0)]],
    sampler smp [[sampler(0)]]
) {
    float4 texColor = tex.sample(smp, in.uv);
    return float4(in.color * texColor.rgb, texColor.a);
}

// --- 2. Normal mapping — TBN matrix, Blinn-Phong ---

vertex NormalMapVertexOut normalmap_vertex(
    VertexIn in [[stage_in]],
    constant Uniforms& u [[buffer(0)]],
    constant Light& light [[buffer(1)]]
) {
    NormalMapVertexOut out;
    float4 worldPos4 = u.model * float4(in.position, 1.0);
    out.worldPos = worldPos4.xyz;
    out.position = u.projection * u.view * worldPos4;
    out.uv = in.texCoord;

    float3x3 normalMat = float3x3(u.model[0].xyz, u.model[1].xyz, u.model[2].xyz);
    float3 T = normalize(normalMat * in.tangent);
    float3 B = normalize(normalMat * in.bitangent);
    float3 N = normalize(normalMat * in.normal);
    float3x3 TBN = transpose(float3x3(T, B, N));

    out.tangentLightDir = TBN * (light.position - out.worldPos);
    out.tangentViewDir  = TBN * (u.cameraPos - out.worldPos);
    return out;
}

fragment float4 normalmap_fragment(
    NormalMapVertexOut in [[stage_in]],
    texture2d<float, access::sample> albedoTex [[texture(0)]],
    texture2d<float, access::sample> normalTex [[texture(1)]],
    sampler smp [[sampler(0)]],
    constant Light& light [[buffer(1)]]
) {
    float3 albedo = albedoTex.sample(smp, in.uv).rgb;
    float3 N = normalize(normalTex.sample(smp, in.uv).rgb * 2.0 - 1.0);
    float3 L = normalize(in.tangentLightDir);
    float3 V = normalize(in.tangentViewDir);
    float3 H = normalize(L + V);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, H), 0.0), 64.0);
    float3 color = albedo * (0.05 + diff * light.color * light.intensity)
                 + float3(spec) * light.color;
    return float4(clamp(color, 0.0, 1.0), 1.0);
}

// --- 3. Shadow mapping — light-space coords, depth comparison ---

vertex ShadowVertexOut shadow_vertex(
    VertexIn in [[stage_in]],
    constant Uniforms& u [[buffer(0)]]
) {
    ShadowVertexOut out;
    float4 worldPos = u.model * float4(in.position, 1.0);
    out.worldPos = worldPos.xyz;
    out.position = u.projection * u.view * worldPos;
    out.normal = normalize((u.model * float4(in.normal, 0.0)).xyz);
    out.shadowCoord = u.lightSpaceMatrix * worldPos;
    out.uv = in.texCoord;
    return out;
}

fragment float4 shadow_fragment(
    ShadowVertexOut in [[stage_in]],
    depth2d<float, access::sample> shadowMap [[texture(0)]],
    texture2d<float, access::sample> albedoTex [[texture(1)]],
    sampler smp [[sampler(0)]],
    sampler shadowSmp [[sampler(1)]],
    constant Light& light [[buffer(1)]]
) {
    float3 projCoords = in.shadowCoord.xyz / in.shadowCoord.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = 1.0 - projCoords.y;
    float shadow = shadowMap.sample_compare(shadowSmp, projCoords.xy, projCoords.z);
    float3 L = normalize(light.position - in.worldPos);
    float diff = max(dot(in.normal, L), 0.0);
    float3 albedo = albedoTex.sample(smp, in.uv).rgb;
    float3 color = albedo * (0.1 + shadow * diff * light.color);
    return float4(color, 1.0);
}

// --- 4. Post-processing — fullscreen quad, edge detection via dfdx/dfdy ---

vertex PostProcessVertexOut postprocess_vertex(uint vid [[vertex_id]]) {
    float2 positions[] = { {-1, -1}, {3, -1}, {-1, 3} };
    float2 uvs[]       = { {0, 1}, {2, 1}, {0, -1} };
    PostProcessVertexOut out;
    out.position = float4(positions[vid], 0.0, 1.0);
    out.uv = uvs[vid];
    return out;
}

fragment float4 postprocess_fragment(
    PostProcessVertexOut in [[stage_in]],
    texture2d<float, access::sample> sceneTex [[texture(0)]],
    texture2d<float, access::sample> depthTex [[texture(1)]],
    sampler smp [[sampler(0)]]
) {
    float3 sceneColor = sceneTex.sample(smp, in.uv).rgb;
    float depth = depthTex.sample(smp, in.uv).r;
    float dx = dfdx(depth);
    float dy = dfdy(depth);
    float edge = sqrt(dx * dx + dy * dy);
    float edgeFactor = step(0.001, edge);
    float3 result = mix(sceneColor, float3(0.0), edgeFactor * 0.8);
    return float4(result, 1.0);
}

// --- 5. PBR metallic-roughness ---

static float distributionGGX(float3 N, float3 H, float roughness) {
    float a2 = roughness * roughness;
    a2 *= a2;
    float NdH = max(dot(N, H), 0.0);
    float denom = NdH * NdH * (a2 - 1.0) + 1.0;
    return a2 / (3.14159265 * denom * denom);
}

static float geometrySchlick(float NdV, float roughness) {
    float k = (roughness + 1.0);
    k = k * k / 8.0;
    return NdV / (NdV * (1.0 - k) + k);
}

static float3 fresnelSchlick(float cosTheta, float3 F0) {
    return F0 + (1.0 - F0) * pow(saturate(1.0 - cosTheta), 5.0);
}

vertex PBRVertexOut pbr_vertex(
    VertexIn in [[stage_in]],
    constant Uniforms& u [[buffer(0)]]
) {
    PBRVertexOut out;
    float4 world = u.model * float4(in.position, 1.0);
    out.worldPos = world.xyz;
    out.position = u.projection * u.view * world;
    out.normal = normalize((u.model * float4(in.normal, 0.0)).xyz);
    out.uv = in.texCoord;
    return out;
}

fragment float4 pbr_fragment(
    PBRVertexOut in [[stage_in]],
    texture2d<float, access::sample> albedoTex    [[texture(0)]],
    texture2d<float, access::sample> metallicTex  [[texture(1)]],
    texture2d<float, access::sample> roughnessTex [[texture(2)]],
    sampler smp [[sampler(0)]],
    constant Light& light [[buffer(1)]],
    constant Uniforms& u [[buffer(0)]]
) {
    float3 albedo    = albedoTex.sample(smp, in.uv).rgb;
    float  metallic  = metallicTex.sample(smp, in.uv).r;
    float  roughness = roughnessTex.sample(smp, in.uv).r;
    float3 N = normalize(in.normal);
    float3 V = normalize(u.cameraPos - in.worldPos);
    float3 L = normalize(light.position - in.worldPos);
    float3 H = normalize(V + L);
    float3 F0 = mix(float3(0.04), albedo, metallic);
    float NDF = distributionGGX(N, H, roughness);
    float G   = geometrySchlick(max(dot(N, V), 0.0), roughness)
              * geometrySchlick(max(dot(N, L), 0.0), roughness);
    float3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);
    float3 kD = (float3(1.0) - F) * (1.0 - metallic);
    float NdL = max(dot(N, L), 0.0);
    float3 specular = (NDF * G * F) / (4.0 * max(dot(N, V), 0.0) * NdL + 0.0001);
    float3 Lo = (kD * albedo / 3.14159265 + specular) * light.color * light.intensity * NdL;
    float3 color = float3(0.03) * albedo + Lo;
    // Reinhard tone map + gamma
    color = color / (color + float3(1.0));
    color = pow(color, float3(1.0 / 2.2));
    return float4(color, 1.0);
}

// --- 6. Skybox — cubemap sample ---

vertex SkyboxVertexOut skybox_vertex(
    uint vid [[vertex_id]],
    constant Uniforms& u [[buffer(0)]]
) {
    float2 positions[] = { {-1, -1}, {3, -1}, {-1, 3} };
    SkyboxVertexOut out;
    out.position = float4(positions[vid], 1.0, 1.0);
    // Strip translation from view, invert rotation to get world-space direction
    float4x4 viewRot = u.view;
    viewRot[3] = float4(0, 0, 0, 1);
    float3x3 invRot = transpose(float3x3(viewRot[0].xyz, viewRot[1].xyz, viewRot[2].xyz));
    out.uvw = invRot * float3(positions[vid], 1.0);
    return out;
}

fragment float4 skybox_fragment(
    SkyboxVertexOut in [[stage_in]],
    texturecube<float, access::sample> cubeMap [[texture(0)]],
    sampler smp [[sampler(0)]]
) {
    return cubeMap.sample(smp, normalize(in.uvw));
}

// --- 7. Instanced rendering — per-instance transforms from buffer ---

vertex InstanceVertexOut instanced_vertex(
    VertexIn in [[stage_in]],
    constant Uniforms& u [[buffer(0)]],
    const device InstanceData* instances [[buffer(2)]],
    uint iid [[instance_id]]
) {
    InstanceVertexOut out;
    float4x4 instanceModel = instances[iid].modelMatrix;
    float4 worldPos = instanceModel * float4(in.position, 1.0);
    out.position = u.projection * u.view * worldPos;
    out.normal = normalize((instanceModel * float4(in.normal, 0.0)).xyz);
    out.color = instances[iid].color.rgb;
    return out;
}

fragment float4 instanced_fragment(
    InstanceVertexOut in [[stage_in]],
    constant Light& light [[buffer(1)]]
) {
    float3 N = normalize(in.normal);
    float3 L = normalize(light.direction);
    float diff = max(dot(N, -L), 0.0);
    float3 color = in.color * (0.15 + diff * light.color * light.intensity);
    return float4(color, 1.0);
}

// --- 8. Alpha-tested vegetation — discard on alpha threshold ---

vertex VegetationVertexOut vegetation_vertex(
    VertexIn in [[stage_in]],
    constant Uniforms& u [[buffer(0)]]
) {
    VegetationVertexOut out;
    float3 pos = in.position;
    // Wind sway proportional to vertex height
    pos.x += sin(u.time * 2.0 + pos.x * 0.5) * pos.y * 0.03;
    out.position = u.mvp * float4(pos, 1.0);
    out.uv = in.texCoord;
    return out;
}

fragment float4 vegetation_fragment(
    VegetationVertexOut in [[stage_in]],
    texture2d<float, access::sample> tex [[texture(0)]],
    sampler smp [[sampler(0)]]
) {
    float4 color = tex.sample(smp, in.uv);
    if (color.a < 0.5) {
        discard_fragment();
    }
    return color;
}
