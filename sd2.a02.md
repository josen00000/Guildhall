## Task List

- [x] *05pts*: When creating a texture from a handle (`ID3D11Texture2D*`), use `ID3D11Texture2D::GetDesc` to cache off the texure dimensions.
- [x] *05pts*: `Camera::SetColorTarget( Texture* tex )` implemented.
    - [x] If color target is `nullptr` when `BeginCamera` is called, default to the swapchain's backbuffer.
- [x] *20pts*: Be able to create a `Shader` by compiling *Vertex* and *Pixel* shader stages.
    - [x] Can compile a shader stage
    - [x] Properly print out compilation errors.
    - [x] Create a raster state
- [x] *10pts*: Implement `RenderContext::BindShader( Shader* shader )` to set the active shader.
    - [x] Assert that you are currently drawing (between a `BeginCamera` and `EndCamera` call)
    - [x] Binds the vertex and pixel shader stages to the correct place in the pipeline.
    - [x] Bind the raster state
- [X] *10pts*: Implement `RenderContext::GetOrCreateShader( char const* filename )`
- [x] *05pts*: Implement `RenderContext::BindShader( char const* filename )` helper method so we can set by string.
- [X] *05pts*: Implement `RenderContext::Draw( int vertexCount, int startIndex = 0 )` to RenderContext
- [x] *10pts*: Implement `RenderBuffer` class for allocating GPU memory
- [x] *10pts*: Implement `VertexBuffer` specialization of `RenderBuffer` for handling vertex data
    - [ ] Add `buffer_attribute_t` description of your `VertexPCT`
- [ ] *05pts*: Implement `RenderContext::BindVertexBuffer( VertexBuffer* vbo )`
    - [ ] Keep track of currently bound vertex buffer and don't rebind if no change is made.
- [ ] *15pts*: Implement `RenderContext::DrawVertexArray`
    - [ ] `RenderContext` should own a vertex buffer (`RenderBuffer* m_immediateVBO`) to use for `DrawVertexArray`
    - [ ] Copy vertices to immediate vbo.
    - [ ] Bind the vertex buffer to input slot 0 (i)
    - [ ] `GetOrCreate` an input layout from current shader/vertex buffer.  Bind if different.
    - [ ] Call `Draw`


------

## Extras
- [ ] *X02.00 : 04pts*:  **Requires X01.00 -or- X01.01** - Allow user to change resolution at runtime and properly recreate the swapchains.
- [ ] *X02.01 : 04pts*:  Hook into DirectX output feed and forward it through your own `DebuggerPrintf` or `ConsolePrintf`
- [ ] *X02.10 : 02pts*:  Shader Reloading.  On key press (`F10` suggested), reload all shaders in your shader database
- [ ] *X02.11 : 04pts*:  Directory Watch.  Write a system that prints when a file changes (is added, removed, or changed) in your data folder during run-time.
- [ ] *x02.12 : 02pts*:  **Requires X02.10 and X02.11**:  Auto reload shaders when changed on disc.

------
