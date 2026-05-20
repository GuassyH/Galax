#version 460 core

uniform sampler2D depthTexture;
uniform sampler2D screenTexture;
uniform sampler2D normalTexture;

uniform mat4 modelMat;

uniform float normalRepeat;
uniform float normalStrength;
uniform int normalFactor;
uniform bool hasNormalTex;

uniform float oceanRadius;
uniform float densityFalloff;
uniform vec4 oceanColor;

uniform float camFarPlane;
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

vec2 raySphere(vec3 sphereCentre, vec3 rayOrigin, vec3 rayDir)
{
    vec3 offset = rayOrigin - sphereCentre;

    const float a = 1.0;
    float b = 2.0 * dot(offset, rayDir);
    float c = dot(offset, offset) - oceanRadius * oceanRadius;

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

float LinearizeDepth(float d, float zNear, float zFar){
    float z_n = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}

float DepthBufferFromLinear(float zLinear, float zNear, float zFar){
    float z_n = (zLinear * (zFar + zNear) - 2.0 * zNear * zFar) / (zLinear * (zFar - zNear));
    return 0.5 * (z_n + 1.0);
}

////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////

void main()
{
    vec4 screenCol = texture(screenTexture, texCoord);

    fragColor = screenCol;

    gl_FragDepth = texture(depthTexture, texCoord).r;

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

    vec3 rayOrigin = camPos;

    ////////////////////////////////////////////////////////
    // SCENE DEPTH
    ////////////////////////////////////////////////////////

    float sceneDepthLinear = LinearizeDepth(texture(depthTexture, texCoord).r, camNearPlane, camFarPlane);

    ////////////////////////////////////////////////////////
    // OCEAN INTERSECTION
    ////////////////////////////////////////////////////////

    vec2 intersect = raySphere(centre, rayOrigin, rayDir);

    float dstTo = intersect.x;
    float dstThrough = intersect.y;

    if(dstThrough <= 0.0)
        return;

    ////////////////////////////////////////////////////////
    // DEPTH CLAMP
    ////////////////////////////////////////////////////////

    float denom = max(dot(rayDir, camForward), 0.1);

    float distanceAlongRayToScene = sceneDepthLinear / denom;
    dstThrough = max(0.0, min(dstThrough, distanceAlongRayToScene - dstTo));


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
        normal = normalCalculation(entryPoint - centre, time / 50);
    }
    else {
        normal = normalize(entryPoint - centre);
    }

    ////////////////////////////////////////////////////////
    // FRESNEL
    ////////////////////////////////////////////////////////

    vec3 viewDir = normalize(camPos - entryPoint);
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
    vec3 skyColor = vec3(0.35, 0.55, 0.85);
    vec3 finalWaterColor = mix(waterColor, skyColor, fresnel);

    ////////////////////////////////////////////////////////
    // LIGHTING
    ////////////////////////////////////////////////////////

    vec3 litColor = directionalLight(normal, normalize(sunPos - entryPoint), rayDir, finalWaterColor, fresnel);
    fragColor.rgb = litColor;

    ////////////////////////////////////////////////////////
    // DEPTH WRITE
    ////////////////////////////////////////////////////////

    vec3 viewSpacePos = entryPoint - camPos;
    float camDepth = dot(viewSpacePos, camForward);

    gl_FragDepth = DepthBufferFromLinear(max(camDepth, camNearPlane), camNearPlane, camFarPlane);
}