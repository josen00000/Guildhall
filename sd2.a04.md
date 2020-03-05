C29.SD2.A04 : Perspective Camera
======

## Goal
By the end of this, we will have a 5-DoF (Degree of Freedom) camera, allowing movement in 3D using the keyboard/mouse.   This will take us over;

- Perspective Matrices
- Depth Buffers
- Matrix Inversion (orthonormal)
- Transforms
- model Matrices
- Rotation from Euler Angles
- Mouse Locking & Hiding
- Index Buffers


------

## Core Task List [85%]

**Reminder:  You can use previously uncompleted extras as well as extras in this assignment to fill out the points.**

- [x] `Camera::SetProjectionPerspective( float fovDegrees, float nearZClip, float farZClip )` implemented
    - [x] Set projection to `60 degrees`, and `-0.1` to `-100.0` for the clip planes.
- [x] Camera now has a `Transform`
    - [x] Create the `Transform` class
    - [x] `Transform::SetPosition` implemented
    - [x] `Transform::Translate` implemented
    - [x] `Transform::SetRotationFromPitchRollYawDegrees`
        - [x] When storing degrees, make sure they are stored in sane ranges...
            - [x] Roll & Yaw is `-180` to `180`
            - [x] Pitch is `-90` to `90`
- [x] Camera now calculates `view` matrix from their transform.
    - [x] `Transform::GetAsMatrix` implemented to calculate the camera's model matrix
    - [x] `MatrixInvertOrthoNormal` implemented to invert the camera's model into a view matrix
        - [x] `MatrixIsOrthoNormal` check added
        - [x] `MatrixTranspose` added
- [x] Draw a Quad at `(0, 0, -10)`, or 10 units in front of the origin (should be visible when you start)
- [x] Allow player to move the camera by change the camera transform position
   - [x] `W` & `S`: Forward & Back (movement is relative to where you're looking)
   - [x] `A` & `D`: Left and Right (movement is relative to where you're looking) // left and right arrow
   - [x] `Space` & `C`: Up and Down Movement (movement is absolute (world up and world down) // up and down arrow
   - [x] `Left-Shift`: Move faster while held.
   - *Note:  If you want different controls, just make a note in your readme*
- [ ] Allow player to turn the camera using the mouse.
    - [ ] `InputSystem::HideSystemCursor` implemented
    - [ ] `InputSystem::ClipSystemCursor` implemented
    - [ ] `InputSystem::SetCursorMode` implemented
        - [ ] `ABSOLUTE` mode is what you currently have
        - [ ] `RELATIVE` move implemented
            - [ ] Move mouse to the center of the screen, and store off the cursor position
                - *Note:  Be sure to actually make the system call, not just assume where you moved it is where it went.  This can cause drifting.*
            - [ ] Each frame, get the cursor position, and calculate frame delta.
            - [ ] ...after which, reset to center of screen and reget the current position.
    - [ ] Game should be set to `RELATIVE` mode
        - [ ] `DevConsole` should unlock the mouse and set to `ABSOLUTE` mode
    - [ ] Associate `X` movement with `yaw`
    - [ ] Associate `Y` movement with `pitch`
        - [ ] Do not allow pitch above `85` degrees or below `95` degrees - no going upside down... yet...
        - *Note:  Up to you if you want inverted-y or not.*
- [ ] Support `RenderContext::SetModelMatrix`
    - [ ] Create a new uniform buffer for storing a model matrix (slot 2)
    - [ ] `SetModelMatrix` should update this uniform buffer
    - [ ] `BeginCamera` should `SetModelMatrix` to the `IDENTITY`, and be sure to bind the buffer.
- [ ] Be able to draw a cube mesh at `(1, 0.5, -12.0)`
    - [ ] Create a `GPUMesh` class
        - [ ] Implement `IndexBuffer`
        - [ ] Be able to construct a mesh from a vertex and index array
        - [ ] Add `RenderContext::BindIndexBuffer`
        - [ ] Add `RenderContext::DrawIndexed`
        - [ ] Add `RenderContext::DrawMesh`
            - This should bind the vertex buffer, index buffer, and then `DrawIndexed`
    - [ ] Game creates a `cube mesh` around the origin with 2 unit sides.
    - [ ] Game has a `Transform` for the cube set at `(1, 0.5, -12.0f)`,
    - [ ] Cube transform sets `yaw` rotation to current time each frame
    - [ ] Game should `SetModelMatrix` to the cube transform matrix
- [ ] Support a depth buffer
    - [ ] `Texture::CreateDepthStencilBuffer` added
    - [ ] `Camera::SetDepthStencilBuffer` added
    - [ ] `RenderContext` now automatcially creates a depth buffer during init matching the swap chain's size
    - [ ] `RenderContext::GetDefaultBackbuffer` implemented to return this
    - [ ] `RenderContext::BeginCamera`, now binds the camera's back buffer as well.
        - [ ] **IMPORANT:  Do not bind the default one automatically if the camera doesn't have one set.  There are reasons a camera may not want a depth buffer!**
    - [ ] Camera's clear options should now store off the `depth` and `stencil` clear values.
    - [ ] If camera has a depth buffer and says it should clear depth, also clear the depth buffer.
        - Use `ID3D11DeviceConext::ClearDepthStencilView` to clear if camera says to.
- [ ] Generate a **UV Sphere** mesh during `Game::Startup`
    - [ ] `MeshUtils.hpp` has a function
          `AddUVSphereToIndexedVertexArray( std::vector<VertexPCU>& verts, std::vector<uint>& indices, vec3 center, float radius, uint horizintalCuts, uint verticalCuts, RGBA color )`
    - [ ] Draw this UV sphere multiple times as a large moving ring in your game.  Each one rotating along a local axis as well as rotating along a global axis.  See demo.

------

## Extras
- [ ] *X04.10: 03%*: Mouse input, show, and clip options should use a stack to track state `InputSystem::PushMouseOptions`, see notes...
- [ ] *X04.11: 02%*: **Requires X04.10** - Mouse options are disabled when window loses focus, and re-applied when gaining focus.
- [ ] *X04.15: 04%*: CPU Mesh Subdivide (tesselation)
- [ ] *X04.20: 02%*: Cube Sphere Generation
- [ ] *X04.30: 04%*: IcoSphere Generation (No UV)
- [ ] *X04.31: 02%*: **Requiers X04.30** - IcoSphere UVs (Can use spherical projection - there's no clean seam so  will require a wrapping sampler to wo -k)
- [ ] *X04.40: 02%*: Plane Generation (with subdivision count)
- [ ] *X04.41: 04%*: **Requires X04.40** - Surface Generation for equations of the form "vec3 f(u, v)";
- [ ] *X04.42: 04%*: **Requires X04.40** - NURB Generation (can use previous)

------



## Documents

### Notes

- [Task Notes](./tasks.md)

------

### Links
- [Scratchapixel: Projection Matrices](https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/projection-matrix-introduction)  Walkthrough of various projection matrix construction
- [Cramer's Rule for Inverting a Matrix](http://pi.math.cornell.edu/~andreim/Lec17.pdf)
- [GLU's Inversion Code](matrixinvert.cpp)


------

## Extra Notes

### x04.10: `PushMouseOptions`
Have the input system use a stack, so that the current system, say, the dev console, can do things like this;

```cpp
void DevConsole::OnOpen()
{
   g_theInput->PushMouseOptions( MOUSE_ABSOLUTE, /*visible*/ true, /*clipped*/ false );
   // ...
}

void DevConsole::OnClose()
{
   g_theInput->PopMouseOptions();    
}
```

And it would then revert to the next highest state on the stack, which would likely be your game.  

### X04.11
Listen for `WM_ACTIVATE`, apply top most state if gaining focus, unapply state if losing focus;

// todo
- [ ] *X04.15: 04%*: CPU Mesh Subdivide (tesselation)
- [ ] *X04.20: 02%*: Cube Sphere Generation
- [ ] *X04.30: 04%*: IcoSphere Generation (No UV)
- [ ] *X04.31: 02%*: **Requiers X04.30** - IcoSphere UVs (Can use spherical projection - there's no clean seam so  will require a wrapping sampler to wo -k)
- [ ] *X04.40: 02%*: Plane Generation (with subdivision count)
- [ ] *X04.41: 04%*: **Requires X04.40** - Surface Generation for equations of the form "vec3 f(u, v)";
- [ ] *X04.42: 04%*: **Requires X04.40** - NURB Generation (can use previous)