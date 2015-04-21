#include <d3d11.h>


struct BasicVertex
{
    float3 pos;  // position
    float3 norm; // surface normal vector
    float2 tex;  // texture coordinate
};

struct ConstantBuffer
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

// This class defines the application as a whole.
ref class Direct3DTutorialFrameworkView : public IFrameworkView
{
private:
    Platform::Agile<CoreWindow> m_window;
    ComPtr<IDXGISwapChain1> m_swapChain;
    ComPtr<ID3D11Device1> m_d3dDevice;
    ComPtr<ID3D11DeviceContext1> m_d3dDeviceContext;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11Buffer> m_constantBuffer;
    ConstantBuffer m_constantBufferData;

auto vertexShaderBytecode = reader->ReadData("SimpleVertexShader.cso");
        ComPtr<ID3D11VertexShader> vertexShader;
        DX::ThrowIfFailed(
            m_d3dDevice->CreateVertexShader(
                vertexShaderBytecode->Data,
                vertexShaderBytecode->Length,
                nullptr,
                &vertexShader
                )
            );

        // Create an input layout that matches the layout defined in the vertex shader code.
        // These correspond to the elements of the BasicVertex struct defined above.
        const D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        ComPtr<ID3D11InputLayout> inputLayout;
        DX::ThrowIfFailed(
            m_d3dDevice->CreateInputLayout(
                basicVertexLayoutDesc,
                ARRAYSIZE(basicVertexLayoutDesc),
                vertexShaderBytecode->Data,
                vertexShaderBytecode->Length,
                &inputLayout
                )
            );

        // Load the raw pixel shader bytecode from disk and create a pixel shader with it.
        auto pixelShaderBytecode = reader->ReadData("SimplePixelShader.cso");
        ComPtr<ID3D11PixelShader> pixelShader;
        DX::ThrowIfFailed(
            m_d3dDevice->CreatePixelShader(
                pixelShaderBytecode->Data,
                pixelShaderBytecode->Length,
                nullptr,
                &pixelShader
                )
            );

        // Create vertex and index buffers that define a simple unit cube.

        // In the array below, which will be used to initialize the cube vertex buffers,
        // multiple vertices are used for each corner to allow different normal vectors and
        // texture coordinates to be defined for each face.
        BasicVertex cubeVertices[] =
        {
            { float3(-0.5f, 0.5f, -0.5f), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 0.0f) }, // +Y (top face)
            { float3( 0.5f, 0.5f, -0.5f), float3(0.0f, 1.0f, 0.0f), float2(1.0f, 0.0f) },
            { float3( 0.5f, 0.5f,  0.5f), float3(0.0f, 1.0f, 0.0f), float2(1.0f, 1.0f) },
            { float3(-0.5f, 0.5f,  0.5f), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 1.0f) },

            { float3(-0.5f, -0.5f,  0.5f), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 0.0f) }, // -Y (bottom face)
            { float3( 0.5f, -0.5f,  0.5f), float3(0.0f, -1.0f, 0.0f), float2(1.0f, 0.0f) },
            { float3( 0.5f, -0.5f, -0.5f), float3(0.0f, -1.0f, 0.0f), float2(1.0f, 1.0f) },
            { float3(-0.5f, -0.5f, -0.5f), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 1.0f) },

            { float3(0.5f,  0.5f,  0.5f), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) }, // +X (right face)
            { float3(0.5f,  0.5f, -0.5f), float3(1.0f, 0.0f, 0.0f), float2(1.0f, 0.0f) },
            { float3(0.5f, -0.5f, -0.5f), float3(1.0f, 0.0f, 0.0f), float2(1.0f, 1.0f) },
            { float3(0.5f, -0.5f,  0.5f), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 1.0f) },

            { float3(-0.5f,  0.5f, -0.5f), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) }, // -X (left face)
            { float3(-0.5f,  0.5f,  0.5f), float3(-1.0f, 0.0f, 0.0f), float2(1.0f, 0.0f) },
            { float3(-0.5f, -0.5f,  0.5f), float3(-1.0f, 0.0f, 0.0f), float2(1.0f, 1.0f) },
            { float3(-0.5f, -0.5f, -0.5f), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 1.0f) },

            { float3(-0.5f,  0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 0.0f) }, // +Z (front face)
            { float3( 0.5f,  0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), float2(1.0f, 0.0f) },
            { float3( 0.5f, -0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), float2(1.0f, 1.0f) },
            { float3(-0.5f, -0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 1.0f) },

            { float3( 0.5f,  0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) }, // -Z (back face)
            { float3(-0.5f,  0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), float2(1.0f, 0.0f) },
            { float3(-0.5f, -0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), float2(1.0f, 1.0f) },
            { float3( 0.5f, -0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 1.0f) },
        };

        unsigned short cubeIndices[] =
        {
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7,

            8, 9, 10,
            8, 10, 11,

            12, 13, 14,
            12, 14, 15,

            16, 17, 18,
            16, 18, 19,

            20, 21, 22,
            20, 22, 23
        };

        D3D11_BUFFER_DESC vertexBufferDesc = {0};
        vertexBufferDesc.ByteWidth = sizeof(BasicVertex) * ARRAYSIZE(cubeVertices);
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA vertexBufferData;
        vertexBufferData.pSysMem = cubeVertices;
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        ComPtr<ID3D11Buffer> vertexBuffer;
        DX::ThrowIfFailed(
            m_d3dDevice->CreateBuffer(
                &vertexBufferDesc,
                &vertexBufferData,
                &vertexBuffer
                )
            );

        D3D11_BUFFER_DESC indexBufferDesc;
        indexBufferDesc.ByteWidth = sizeof(unsigned short) * ARRAYSIZE(cubeIndices);
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA indexBufferData;
        indexBufferData.pSysMem = cubeIndices;
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;

        ComPtr<ID3D11Buffer> indexBuffer;
        DX::ThrowIfFailed(
            m_d3dDevice->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &indexBuffer
                )
            );


        // Create a constant buffer for passing model, view, and projection matrices
        // to the vertex shader.  This will allow us to rotate the cube and apply
        // a perspective projection to it.

        D3D11_BUFFER_DESC constantBufferDesc = {0};
        constantBufferDesc.ByteWidth = sizeof(m_constantBufferData);
        constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.CPUAccessFlags = 0;
        constantBufferDesc.MiscFlags = 0;
        constantBufferDesc.StructureByteStride = 0;
        DX::ThrowIfFailed(
            m_d3dDevice->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
                )
            );

        // Specify the view transform corresponding to a camera position of
        // X = 0, Y = 1, Z = 2.  See Lesson 5 for a generalized camera class.

        m_constantBufferData.view = float4x4(
            -1.00000000f, 0.00000000f,  0.00000000f,  0.00000000f,
             0.00000000f, 0.89442718f,  0.44721359f,  0.00000000f,
             0.00000000f, 0.44721359f, -0.89442718f, -2.23606800f,
             0.00000000f, 0.00000000f,  0.00000000f,  1.00000000f
            );