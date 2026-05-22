#version 460 core

uniform sampler2D depthTexture;
uniform sampler2D screenTexture;
uniform sampler2D normalTexture;

uniform mat4 modelMat;
uniform mat4 invProjMat;

uniform float normalRepeat;
uniform float normalStrength;
uniform int normalFactor;
uniform bool hasNormalTex;

uniform float oceanRadius;
uniform float densityFalloff;
uniform vec4 oceanColor;
uniform vec4 fresnelColor;

uniform float camNearPlane;
uniform float FOVdeg;

uniform vec3 centre;
uniform vec3 camPos;
uniform vec3 sunPos;
uniform vec3 camUp;
uniform vec3 camForward;
uniform vec3 camRight;

uniform vec2 screenResolution;

uniform float time;
uniform float triplanarBlend;

in vec2 texCoord;
out vec4 fragColor;

////////////////////////////////////////////////////////////
// TRIPLANAR
////////////////////////////////////////////////////////////

vec3 getTriPlanarBlend(vec3 n)
{
    vec3 blending = max(abs(n), 0.00001);
    blending = normalize(blending);
    blending = pow(blending, vec3(triplanarBlend));
    blending /= (blending.x + blending.y + blending.z);

    return blending;
}

vec3 normalCalculation(vec3 localPos, float offset)
{
    vec3 spherePos = normalize(localPos);

    vec2 flow = vec2(sin(offset), cos(offset));

    mat3 normalMat = inverse(mat3(modelMat));

    vec3 vNorm = spherePos;
    vec3 vPos = normalMat * spherePos;

    vec3 blending = getTriPlanarBlend(vNorm);

    vec3 xaxis = texture(normalTexture, vPos.yz * normalRepeat + flow).rgb;
    vec3 yaxis = texture(normalTexture, vPos.xz * normalRepeat + flow).rgb;
    vec3 zaxis = texture(normalTexture, vPos.xy * normalRepeat + flow).rgb;

    vec3 normalTex =
        xaxis * blending.x +
        yaxis * blending.y +
        zaxis * blending.z;

    normalTex = normalTex * 2.0 - 1.0;
    normalTex.xy *= normalStrength;
    normalTex = normalize(normalTex);

    vec3 up = abs(vNorm.y) < 0.999
        ? vec3(0,1,0)
        : vec3(1,0,0);

    vec3 tangent = normalize(cross(up, vNorm));
    vec3 bitangent = cross(vNorm, tangent);

    mat3 tbn = mat3(
        normalize(tangent),
        normalize(bitangent),
        normalize(vNorm)
    );

    return normalize(tbn * normalTex);
}

////////////////////////////////////////////////////////////
// LIGHTING
////////////////////////////////////////////////////////////

float fresnelSchlick(vec3 viewDir, vec3 normal)
{
    float cosTheta = max(dot(viewDir, normal), 0.0);
    float f0 = 0.02;
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

vec3 directionalLight(vec3 norm, vec3 sunDir, vec3 rayDir, vec3 baseColor, float fresnel){
    vec3 lightDir = normalize(sunDir);

    float diffuse = max(dot(norm, lightDir), 0.0);
    vec3 ambient = baseColor * 0.05;
    vec3 reflectionDirection = reflect(-lightDir, norm);

    float specAmount = pow(max(dot(-rayDir, reflectionDirection), 0.0), normalFactor);

    float specularStrength = 0.5;
    vec3 specular = vec3(specAmount * specularStrength) * fresnel;

    return ambient + (baseColor * diffuse) + specular;
}

////////////////////////////////////////////////////////////
// SPHERE INTERSECTION
////////////////////////////////////////////////////////////

// for waves and movement
float wave(vec3 p)
{
    float t = time;

    float w1 = sin(p.x * 0.8 + t * 1.2);
    float w2 = sin(p.z * 1.3 + t * 0.9);
    float w3 = sin((p.x + p.z) * 0.6 + t * 1.5);

    return (w1 + w2 + w3) * 0.5;
}

vec2 raySphere(vec3 sphereCentre, vec3 rayOrigin, vec3 rayDir)
{
    vec3 offset = rayOrigin - sphereCentre;

    const float a = 1.0;
    float b = 2.0 * dot(offset, rayDir);

    float r = oceanRadius;

    float c = dot(offset, offset) - r * r;

    float discriminant = b * b - 4.0 * a * c;

    if(discriminant > 0.000001)
    {
        float s = sqrt(discriminant);
        float dstToSphereNear = max(0.0, (-b - s) / (2.0 * a));
        float dstToSphereFar = (-b + s) / (2.0 * a);

        if(dstToSphereFar >= 0.0) {
            return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
        }
    }

    return vec2(0.0);
}

////////////////////////////////////////////////////////////
// DEPTH
////////////////////////////////////////////////////////////

float LinearizeDepth(float d, float nearPlane)
{
    return nearPlane / d;
}

float DepthBufferFromLinear(float zLinear, float zNear)
{
    return zNear / zLinear;
}

vec3 ReconstructViewPos(vec2 uv, float depth)
{
    vec4 clip = vec4(
        uv * 2.0 - 1.0,
        depth,
        1.0
    );

    vec4 view = invProjMat * clip;
    view /= view.w;

    return view.xyz;
}


////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////

void main()
{
    vec4 screenCol = texture(screenTexture, texCoord);

    fragColor = screenCol;

    ////////////////////////////////////////////////////////
    // SCREEN RAY
    ////////////////////////////////////////////////////////

    vec2 rayCoord = texCoord * 2.0 - 1.0;

    float fov = radians(FOVdeg);
    float aspect = screenResolution.x / screenResolution.y;
    float scale = tan(fov * 0.5);

    vec3 rayDir = normalize(
        camForward +
        rayCoord.x * aspect * scale * camRight +
        rayCoord.y * scale * camUp
    );

    vec3 rayOrigin = vec3(0.0);
    vec3 localOrigin = centre - camPos;

    ////////////////////////////////////////////////////////
    // SCENE DEPTH
    ////////////////////////////////////////////////////////

    float depth = texture(depthTexture, texCoord).r;
	vec3 scenePos = ReconstructViewPos(texCoord, depth);
	float sceneDepthLinear = length(scenePos);


    ////////////////////////////////////////////////////////
    // OCEAN INTERSECTION
    ////////////////////////////////////////////////////////

    vec2 intersect = raySphere(localOrigin, rayOrigin, rayDir);

    float dstTo = intersect.x;
    float dstThrough = intersect.y;

    if(dstThrough <= 0.0)
        return;

    ////////////////////////////////////////////////////////
    // DEPTH CLAMP
    ////////////////////////////////////////////////////////

    dstThrough = max(0.0, min(dstThrough, sceneDepthLinear - dstTo));

    if(dstThrough <= 0.0)
        return;

    ////////////////////////////////////////////////////////
    // ENTRY POINT
    ////////////////////////////////////////////////////////

    const float epsilon = 0.001;
    vec3 entryPoint = rayOrigin + rayDir * (dstTo + 2.0 * epsilon);

    ////////////////////////////////////////////////////////
    // NORMALS
    ////////////////////////////////////////////////////////

    vec3 normal;

    if(hasNormalTex && dstTo > 0.0) {
        normal = normalCalculation(entryPoint - localOrigin, time / 50);
    }
    else {
        normal = normalize(entryPoint - localOrigin);
    }

    ////////////////////////////////////////////////////////
    // FRESNEL
    ////////////////////////////////////////////////////////

    vec3 viewDir = normalize(-entryPoint);
    float fresnel = fresnelSchlick(viewDir, normal);

    ////////////////////////////////////////////////////////
    // WATER ABSORPTION
    ////////////////////////////////////////////////////////

    float absorption = exp(-dstThrough * 0.2);
    vec3 waterColor = mix(oceanColor.rgb, screenCol.rgb, absorption);

    ////////////////////////////////////////////////////////
    // FRESNEL REFLECTION
    ////////////////////////////////////////////////////////

    // should be the atmosphere colour
    vec3 finalWaterColor = mix(waterColor, fresnelColor.rgb, fresnel);

    ////////////////////////////////////////////////////////
    // LIGHTING
    ////////////////////////////////////////////////////////

    vec3 litColor = directionalLight(normal, normalize(sunPos - (entryPoint + camPos)), rayDir, finalWaterColor, fresnel);
    fragColor.rgb = litColor;

    ////////////////////////////////////////////////////////
    // DEPTH WRITE
    ////////////////////////////////////////////////////////

    vec3 viewSpacePos = entryPoint;
    float camDepth = dot(viewSpacePos, camForward);

    gl_FragDepth = DepthBufferFromLinear(max(camDepth, camNearPlane), camNearPlane);
}