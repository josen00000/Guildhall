MP2.A06 Friction
Project name : MP2Asg

## Checklist
- [x] Angular Update - Add members to `Rigidbody2D`
    - [x] Add `float m_rotationInRadians`
    - [x] Add `float m_angularVelocity`
    - [x] Add `float m_frameTorque`
    - [x] Add `float m_moment`
    - [x] Add appropriate getter/setter methods
- [x] `Rigidbody2D::CalculateMoment` to calculate moment based on attached collider and mass
    - [x] `virtual float Collider2D::CalculateMoment( float mass )`
    - [x] Implement `DiscCollider::CalculateMoment`
    - [x] Implement `PolygonCollider::CalculateMoment`
    - [x] When setting **mass** or **collider** on a rigidbody - recalculate your moment.
        - *Note: When setting mass, you can do less work by taking advantage of the ratio of new mass to old mass should match the ratios of the moments*.
- [x] Rotating a `Rigidbody2D` properly updates the **world shape** of the collider.
    - [x] Add controls to rotate a selected object
        - Suggest **R** and **F**
    - [x] Add controls to update the rotational velocity of an object
        - Suggest **T**, **G**, and **V** to increase, decrease, and reset
- [x] `Rigidbody2D` now do angular calculations during their update.
    - [x] Use torque to compute an angular acceleration
    - [x] Apply angular acceleration to update angular velocity
    - [x] Apply angular velocity to update rotation
- [x] Update `Rigidbody2D::GetImpactVelocity` to take into account rotational velocity.
- [x] Update `ApplyImpulseAt` to apply impulse to torque based on positions.
- [x] Update impulse calculations to take into account rotational forces, see PDF in lnks
- [x] Update tooltip to show...
    - [x] Moment of Inertia
    - [x] Current Rotation (degrees)
    - [x] Current Angular Velocity


------

## Resources

### Notes
- I suggest using radians as your unit of rotation within the system.  You can have
methods to take Degrees, such as `SetRotationDegrees( float deg )`, but when calculating
impact velocities, radians translate to linear space directly (since a radian is a relation to how are radius relates to distance traveled when rotating).

- Calculating moment is easier to do in local space for each shape.  Once calculated, you can use **Parallel Axis Theorem** to  get the moment of the rigidbody based on the colliders local offset.  This is useful if want to use a different center of mass.

- Calculating the moment of a polygon is the sum of the moments of its triangles, and then
translated to their corresponding position using the parallel axis theorem.




### Links
- http://chrishecker.com/images/e/e7/Gdmphys3.pdf : Link from previous article explaining the equations;
- Deriving Moments of Inertia (Ring/Rod/Disc/Cylindar): https://www.youtube.com/watch?v=9cuKlt15yCo
- Parallel Axis Theorem: https://en.wikipedia.org/wiki/Parallel_axis_theorem
- Moments for a Triangle: https://www.efunda.com/math/areas/triangle.cfm
- Deriving Moment of Inertia of a Triangle: https://www.youtube.com/watch?v=2SkQy-GVTRk
