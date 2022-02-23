/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
// To get math constants (M_PI) in MSVC
#    define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <cmath>

#include <bosdyn/api/geometry.pb.h>

namespace bosdyn {

namespace api {

// Basic math operations directly on protobufs.
double Length(const ::bosdyn::api::Vec2& a);
double Length(const ::bosdyn::api::Vec3& a);

// Vec2 Operators
::bosdyn::api::Vec2 operator+(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b);
::bosdyn::api::Vec2 operator-(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b);
bool operator==(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b);
::bosdyn::api::Vec2 operator*(const ::bosdyn::api::Vec2& a, double m);
::bosdyn::api::Vec2 operator*(double m, const ::bosdyn::api::Vec2& a);
::bosdyn::api::Vec2 operator/(const ::bosdyn::api::Vec2& a, double m);
Eigen::Matrix<double, 2, 1> Skew(const ::bosdyn::api::Vec2& a);
Eigen::Matrix<double, 2, 1> EigenFromApiProto(const ::bosdyn::api::Vec2& a);
::bosdyn::api::Vec2 EigenToApiProtoVector(const Eigen::Matrix<double, 2, 1>& a);
bool Equals(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b, double tol = -1.0);

// Vec3 Operators
::bosdyn::api::Vec3 operator+(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b);
::bosdyn::api::Vec3 operator-(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b);
bool operator==(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b);
::bosdyn::api::Vec3 operator*(const ::bosdyn::api::Vec3& a, double m);
::bosdyn::api::Vec3 operator*(double m, const ::bosdyn::api::Vec3& a);
::bosdyn::api::Vec3 operator/(const ::bosdyn::api::Vec3& a, double m);
Eigen::Matrix<double, 3, 3> Skew(const ::bosdyn::api::Vec3& a);
Eigen::Matrix<double, 3, 1> EigenFromApiProto(const ::bosdyn::api::Vec3& a);
::bosdyn::api::Vec3 EigenToApiProtoVector(const Eigen::Matrix<double, 3, 1>& a);
bool Equals(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b, double tol = -1.0);

// Quaternion Operators
::bosdyn::api::Quaternion operator*(const ::bosdyn::api::Quaternion& a,
                                  const ::bosdyn::api::Quaternion& b);
bool operator==(const ::bosdyn::api::Quaternion& a, const ::bosdyn::api::Quaternion& b);
::bosdyn::api::Quaternion operator*(const double& scalar, const ::bosdyn::api::Quaternion& a);
::bosdyn::api::Vec3 operator*(const ::bosdyn::api::Quaternion& q, const ::bosdyn::api::Vec3& p);
::bosdyn::api::Quaternion operator+(const ::bosdyn::api::Quaternion& a,
                                  const ::bosdyn::api::Quaternion& b);
::bosdyn::api::Quaternion operator-(const ::bosdyn::api::Quaternion& a,
                                  const ::bosdyn::api::Quaternion& b);
::bosdyn::api::Quaternion operator-(const ::bosdyn::api::Quaternion& a);
bool ToMatrix(const ::bosdyn::api::Quaternion& q, Eigen::Matrix<double, 3, 3>* rotation_matrix);
Eigen::Matrix<double, 2, 2> ToMatrix(const double& angle);
Eigen::Quaterniond EigenFromApiProto(const ::bosdyn::api::Quaternion& q);
::bosdyn::api::Quaternion EigenToApiProto(const Eigen::Quaterniond& q);

// SE3Pose Operators
::bosdyn::api::Vec3 operator*(const ::bosdyn::api::SE3Pose& a_T_b, const ::bosdyn::api::Vec3& p);
::bosdyn::api::SE3Pose operator*(const ::bosdyn::api::SE3Pose& a_T_b,
                               const ::bosdyn::api::SE3Pose& b_T_c);
bool operator==(const ::bosdyn::api::SE3Pose& a_T_b, const ::bosdyn::api::SE3Pose& b_T_c);
bool Equals(const ::bosdyn::api::SE3Pose& a_T_b, const ::bosdyn::api::SE3Pose& b_T_c,
            double pos_tol = -1.0, double rot_tol = -1.0);

// SE2Pose Operators
::bosdyn::api::SE2Pose operator*(const ::bosdyn::api::SE2Pose& a_T_b,
                               const ::bosdyn::api::SE2Pose& b_T_c);
bool operator==(const ::bosdyn::api::SE2Pose& a_T_b, const ::bosdyn::api::SE2Pose& b_T_c);
::bosdyn::api::Vec2 operator*(const ::bosdyn::api::SE2Pose& a_T_b, const ::bosdyn::api::Vec2& p);

// SE(2) Velocity Helpers
bool operator==(const ::bosdyn::api::SE2Velocity& vel_a, const ::bosdyn::api::SE2Velocity& vel_b);
Eigen::Matrix<double, 3, 1> EigenFromApiProto(const ::bosdyn::api::SE2Velocity& vel_in_a);
::bosdyn::api::SE2Velocity EigenToApiProto(const Eigen::Matrix<double, 3, 1>& vel_in_a);
// The adjoint matrix can be used to change reference frames for a SE(2) velocity twist vector.
// For example, the vel_in_b = Adjoint(b_T_a) * vel_in_a
Eigen::Matrix<double, 3, 3> Adjoint(const ::bosdyn::api::SE2Pose& a_T_b);
// This function applies an adjoint matrix to a velocity twist vector to get the velocity in
// the reference frame a.
::bosdyn::api::SE2Velocity TransformVelocity(const Eigen::Matrix<double, 3, 3>& a_adjoint_b,
                                            const ::bosdyn::api::SE2Velocity& vel_in_b);
// SE(3) Velocity Helpers
bool operator==(const ::bosdyn::api::SE3Velocity& vel_a, const ::bosdyn::api::SE3Velocity& vel_b);
Eigen::Matrix<double, 6, 1> EigenFromApiProto(const ::bosdyn::api::SE3Velocity& vel_in_a);
::bosdyn::api::SE3Velocity EigenToApiProto(const Eigen::Matrix<double, 6, 1>& vel_in_a);
// The adjoint matrix can be used to change reference frames for a SE(3) velocity twist vector.
// For example, the vel_in_b = Adjoint(b_T_a) * vel_in_a
Eigen::Matrix<double, 6, 6> Adjoint(const ::bosdyn::api::SE3Pose& a_T_b);
// This function applies an adjoint matrix to a velocity twist vector to get the velocity in
// the reference frame a.
::bosdyn::api::SE3Velocity TransformVelocity(const Eigen::Matrix<double, 6, 6>& a_adjoint_b,
                                            const ::bosdyn::api::SE3Velocity& vel_in_b);

// Inverse.
::bosdyn::api::Quaternion operator~(const ::bosdyn::api::Quaternion& q);
::bosdyn::api::SE3Pose operator~(const ::bosdyn::api::SE3Pose& a_T_b);
::bosdyn::api::SE2Pose operator~(const ::bosdyn::api::SE2Pose& a_T_b);

// Generate a yaw rotation.
::bosdyn::api::Quaternion FromYaw(double angle);

// Create a yaw only quaternion.
::bosdyn::api::Quaternion ClosestYawOnly(const ::bosdyn::api::Quaternion& q);

// Get the yaw angle of a rotation or transform.
double ToYaw(const ::bosdyn::api::Quaternion& q);
double ToYaw(const ::bosdyn::api::SE3Pose& a_T_b);

// Get the angle of a rotation or transform.
double ToAngle(const ::bosdyn::api::Quaternion& q);
double ToAngle(const ::bosdyn::api::SE3Pose& a_T_b);

double DotProduct(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b);
double DotProduct(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b);
double DotProduct(const ::bosdyn::api::Quaternion& a, const ::bosdyn::api::Quaternion& b);
::bosdyn::api::Quaternion Normalize(const ::bosdyn::api::Quaternion& q);

::bosdyn::api::Vec3 CreateVec3(double x = 0, double y = 0, double z = 0);
::bosdyn::api::Quaternion CreateQuaternion(double w = 1, double x = 0, double y = 0, double z = 0);
::bosdyn::api::Quaternion CreateRotationX(double angle);
::bosdyn::api::Quaternion CreateRotationY(double angle);
::bosdyn::api::Quaternion CreateRotationZ(double angle);
::bosdyn::api::SE3Pose CreateSE3Pose(const ::bosdyn::api::Quaternion& q = CreateQuaternion(),
                                    const ::bosdyn::api::Vec3& v = CreateVec3());

// Get the identity transform.
::bosdyn::api::SE3Pose Identity();
::bosdyn::api::SE2Pose SE2Identity();

void EigenToMatrix(const Eigen::MatrixXd& matrix, ::bosdyn::api::Matrix* matrix_out);
void MatrixToEigen(const ::bosdyn::api::Matrix& matrix, Eigen::MatrixXd* matrix_out);

// Covariance operations.
void SetDeprecatedFields(::bosdyn::api::SE3Covariance* covariance);
::bosdyn::api::SE3Covariance IdentityCovMatrix();

::bosdyn::api::Vec3 Cross(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b);

// SE(3) to SE(2) conversions
::bosdyn::api::Vec2 Flatten(const ::bosdyn::api::Vec3& a);
::bosdyn::api::Vec3 Inflate(const ::bosdyn::api::Vec2& a, double z_height = 0.0);
::bosdyn::api::SE3Pose Inflate(const ::bosdyn::api::SE2Pose& a, double z_height = 0.0);
::bosdyn::api::SE2Pose Flatten(const ::bosdyn::api::SE3Pose& a);

// a special version of flatten that keeps the x vector of `a` pointing in the same direction.
::bosdyn::api::SE2Pose FlattenToX(const ::bosdyn::api::SE3Pose& a);

bool SafeFlatten(const ::bosdyn::api::SE3Pose& in_se3_pose, const std::string& base_frame_name,
                  ::bosdyn::api::SE2Pose* out_se2_pose);
bool SafeInflate(const ::bosdyn::api::SE2Pose& in_se2_pose, const std::string& base_frame_name,
                  ::bosdyn::api::SE3Pose* out_se3_pose, double z_height = 0.0);

// Returns the minimum angular distance between two angles (a) and (b) in radians.
template <typename T>
T AngleDiff(T a, T b) {
    T arg = std::fmod(a - b, 2 * M_PI);
    if (arg < 0) arg = arg + 2 * M_PI;
    if (arg > M_PI) arg = arg - 2 * M_PI;
    return -arg;
}

// Returns the length of the vector from frame B's origin to C's origin.
double Dist3d(const ::bosdyn::api::SE3Pose& A_T_B, const ::bosdyn::api::SE3Pose& A_T_C);

// Determine whether the given pose is near the identity.
bool IsIdentity(const ::bosdyn::api::SE3Pose& pose);

// Calculate intersection between ray and plane, false if no intersection.
bool IntersectRayPlane(const bosdyn::api::Vec3& ray_point, const bosdyn::api::Vec3& ray_norm,
                       const bosdyn::api::Plane& plane, bosdyn::api::Vec3* intersection_point);

}  // namespace api

}  // namespace bosdyn
