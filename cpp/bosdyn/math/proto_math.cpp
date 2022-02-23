/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/math/proto_math.h"
#include "bosdyn/math/api_common_frames.h"
#include "bosdyn/math/frame_helpers.h"

#define _USE_MATH_DEFINES

namespace bosdyn {

namespace api {

namespace {

const double kEqualityTolerance = .000001;  // 1e-6

::bosdyn::api::Vec2 make_vec(double x, double y) {
    ::bosdyn::api::Vec2 ret;
    ret.set_x(x);
    ret.set_y(y);
    return ret;
}

::bosdyn::api::Vec3 make_vec(double x, double y, double z) {
    ::bosdyn::api::Vec3 ret;
    ret.set_x(x);
    ret.set_y(y);
    ret.set_z(z);
    return ret;
}

::bosdyn::api::Quaternion make_quat(double w, double x, double y, double z) {
    ::bosdyn::api::Quaternion ret;
    ret.set_w(w);
    ret.set_x(x);
    ret.set_y(y);
    ret.set_z(z);
    return ret;
}

::bosdyn::api::SE3Pose make_pose(const ::bosdyn::api::Vec3& position,
                               const ::bosdyn::api::Quaternion& rotation) {
    ::bosdyn::api::SE3Pose ret;
    (*ret.mutable_position()) = position;
    (*ret.mutable_rotation()) = rotation;
    return ret;
}

::bosdyn::api::SE2Pose make_pose(const ::bosdyn::api::Vec2& position, const double& angle) {
    ::bosdyn::api::SE2Pose ret;
    (*ret.mutable_position()) = position;
    ret.set_angle(angle);
    return ret;
}

::bosdyn::api::SE3Covariance make_cov(const ::bosdyn::api::Vec3& r1, const ::bosdyn::api::Vec3& r2,
                                    const ::bosdyn::api::Vec3& r3, double yaw_variance) {
    Eigen::MatrixXd mat(6, 6);
    mat(3, 3) = r1.x();
    mat(4, 3) = r1.y();
    mat(5, 3) = r1.z();
    mat(3, 4) = r2.x();
    mat(4, 4) = r2.y();
    mat(4, 5) = r2.z();
    mat(3, 5) = r3.x();
    mat(4, 5) = r3.y();
    mat(5, 5) = r3.z();
    mat(2, 2) = yaw_variance;

    ::bosdyn::api::SE3Covariance ret;
    EigenToMatrix(mat, ret.mutable_matrix());
    SetDeprecatedFields(&ret);
    return ret;
}

inline double sqr(double x) { return x * x; }

const double kEpsilon = 1e-9;

}  // namespace

double Length(const ::bosdyn::api::Vec2& a) { return sqrt(a.x() * a.x() + a.y() * a.y()); }

double Length(const ::bosdyn::api::Vec3& a) {
    return sqrt(a.x() * a.x() + a.y() * a.y() + a.z() * a.z());
}

::bosdyn::api::Vec2 operator+(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b) {
    return make_vec(a.x() + b.x(), a.y() + b.y());
}

::bosdyn::api::Vec2 operator-(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b) {
    return make_vec(a.x() - b.x(), a.y() - b.y());
}

bool operator==(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b) {
    return ((fabs(a.x() - b.x()) < kEqualityTolerance) &&
            (fabs(a.y() - b.y()) < kEqualityTolerance));
}

::bosdyn::api::Vec2 operator*(const ::bosdyn::api::Vec2& a, double m) {
    return make_vec(m * a.x(), m * a.y());
}

::bosdyn::api::Vec2 operator*(double m, const ::bosdyn::api::Vec2& a) {
    return make_vec(m * a.x(), m * a.y());
}

::bosdyn::api::Vec2 operator/(const ::bosdyn::api::Vec2& a, double m) {
    return make_vec(a.x() / m, a.y() / m);
}

Eigen::Matrix<double, 2, 1> Skew(const ::bosdyn::api::Vec2& a) {
    Eigen::Matrix<double, 2, 1> skew_symmetric_a;
    skew_symmetric_a << a.y(), -a.x();
    return skew_symmetric_a;
}

Eigen::Matrix<double, 2, 1> EigenFromApiProto(const ::bosdyn::api::Vec2& a) {
    Eigen::Matrix<double, 2, 1> eigen_vec2_a;
    eigen_vec2_a << a.x(), a.y();
    return eigen_vec2_a;
}

::bosdyn::api::Vec2 EigenToApiProtoVector(const Eigen::Matrix<double, 2, 1>& a) {
    ::bosdyn::api::Vec2 proto_vec2_a;
    proto_vec2_a.set_x(a(0));
    proto_vec2_a.set_y(a(1));
    return proto_vec2_a;
}

bool Equals(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b, double tol /* = -1.0 */) {
    if (tol < 0.0) {
        tol = kEqualityTolerance;
    }
    return ((fabs(a.x() - b.x()) < tol) &&
            (fabs(a.y() - b.y()) < tol));
}

::bosdyn::api::Vec3 operator+(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b) {
    return make_vec(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}

::bosdyn::api::Vec3 operator-(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b) {
    return make_vec(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}

bool operator==(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b) {
    return ((fabs(a.x() - b.x()) < kEqualityTolerance) &&
            (fabs(a.y() - b.y()) < kEqualityTolerance) &&
            (fabs(a.z() - b.z()) < kEqualityTolerance));
}

::bosdyn::api::Vec3 operator*(const ::bosdyn::api::Vec3& a, double m) {
    return make_vec(a.x() * m, a.y() * m, a.z() * m);
}

::bosdyn::api::Vec3 operator*(double m, const ::bosdyn::api::Vec3& a) {
    return make_vec(a.x() * m, a.y() * m, a.z() * m);
}

::bosdyn::api::Vec3 operator/(const ::bosdyn::api::Vec3& a, double m) {
    return make_vec(a.x() / m, a.y() / m, a.z() / m);
}

Eigen::Matrix<double, 3, 3> Skew(const ::bosdyn::api::Vec3& a) {
    Eigen::Matrix<double, 3, 3> skew_symmetric_a;
    skew_symmetric_a << 0, -a.z(), a.y(), a.z(), 0, -a.x(), -a.y(), a.x(), 0;
    return skew_symmetric_a;
}

Eigen::Matrix<double, 3, 1> EigenFromApiProto(const ::bosdyn::api::Vec3& a) {
    Eigen::Matrix<double, 3, 1> eigen_vec3_a;
    eigen_vec3_a << a.x(), a.y(), a.z();
    return eigen_vec3_a;
}

::bosdyn::api::Vec3 EigenToApiProtoVector(const Eigen::Matrix<double, 3, 1>& a) {
    ::bosdyn::api::Vec3 proto_vec3_a;
    proto_vec3_a.set_x(a(0));
    proto_vec3_a.set_y(a(1));
    proto_vec3_a.set_z(a(2));
    return proto_vec3_a;
}

bool Equals(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b, double tol /* = -1.0 */) {
    if (tol < 0.0) {
        tol = kEqualityTolerance;
    }
    return ((fabs(a.x() - b.x()) < tol) &&
            (fabs(a.y() - b.y()) < tol) &&
            (fabs(a.z() - b.z()) < tol));
}

::bosdyn::api::Quaternion operator~(const ::bosdyn::api::Quaternion& q) {
    return make_quat(q.w(), -q.x(), -q.y(), -q.z());
}

::bosdyn::api::Quaternion operator*(const ::bosdyn::api::Quaternion& a,
                                  const ::bosdyn::api::Quaternion& b) {
    return make_quat(a.w() * b.w() - a.x() * b.x() - a.y() * b.y() - a.z() * b.z(),
                     a.w() * b.x() + a.x() * b.w() + a.y() * b.z() - a.z() * b.y(),
                     a.w() * b.y() - a.x() * b.z() + a.y() * b.w() + a.z() * b.x(),
                     a.w() * b.z() + a.x() * b.y() - a.y() * b.x() + a.z() * b.w());
}

::bosdyn::api::Quaternion operator+(const ::bosdyn::api::Quaternion& a,
                                  const ::bosdyn::api::Quaternion& b) {
    return make_quat(a.w() + b.w(), a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}
::bosdyn::api::Quaternion operator-(const ::bosdyn::api::Quaternion& a,
                                  const ::bosdyn::api::Quaternion& b) {
    return make_quat(a.w() - b.w(), a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}

::bosdyn::api::Quaternion operator-(const ::bosdyn::api::Quaternion& a) {
    return make_quat(-a.w(), -a.x(), -a.y(), -a.z());
}

bool operator==(const ::bosdyn::api::Quaternion& a, const ::bosdyn::api::Quaternion& b) {
    // Quaternion equality when (a == b) or (a == -b)
    bool a_eq_b =
        ((fabs(a.x() - b.x()) < kEqualityTolerance) && (fabs(a.y() - b.y()) < kEqualityTolerance) &&
         (fabs(a.z() - b.z()) < kEqualityTolerance) && (fabs(a.w() - b.w()) < kEqualityTolerance));
    bool a_eq_negative_b =
        ((fabs(a.x() + b.x()) < kEqualityTolerance) && (fabs(a.y() + b.y()) < kEqualityTolerance) &&
         (fabs(a.z() + b.z()) < kEqualityTolerance) && (fabs(a.w() + b.w()) < kEqualityTolerance));
    return (a_eq_b || a_eq_negative_b);
}

bool ToMatrix(const ::bosdyn::api::Quaternion& q, Eigen::Matrix<double, 3, 3>* rotation_matrix) {
    double norm = sqrt(sqr(q.x()) + sqr(q.y()) + sqr(q.z()) + sqr(q.w()));
    if (fabs(norm - 1.0) > 1e-3) {
        return false;
    }
    *rotation_matrix = EigenFromApiProto(q).toRotationMatrix();
    return true;
}

Eigen::Matrix<double, 2, 2> ToMatrix(const double& angle) {
    // Expects an angle in radians.
    double c = cos(angle);
    double s = sin(angle);
    Eigen::Matrix<double, 2, 2> rotation_matrix;
    rotation_matrix << c, -s, s, c;
    return rotation_matrix;
}

Eigen::Quaterniond EigenFromApiProto(const ::bosdyn::api::Quaternion& q) {
    return Eigen::Quaterniond(q.w(), q.x(), q.y(), q.z());
}

::bosdyn::api::Quaternion EigenToApiProto(const Eigen::Quaterniond& q) {
    return make_quat(q.w(), q.x(), q.y(), q.z());
}

::bosdyn::api::Vec3 operator*(const ::bosdyn::api::Quaternion& q, const ::bosdyn::api::Vec3& p) {
    ::bosdyn::api::Quaternion q2 = make_quat(0, p.x(), p.y(), p.z());
    q2 = q2 * ~q;
    q2 = q * q2;
    return make_vec(q2.x(), q2.y(), q2.z());
}

::bosdyn::api::Vec3 operator*(const ::bosdyn::api::SE3Pose& a_T_b, const ::bosdyn::api::Vec3& p) {
    return a_T_b.rotation() * p + a_T_b.position();
}

::bosdyn::api::SE3Pose operator~(const ::bosdyn::api::SE3Pose& a_T_b) {
    ::bosdyn::api::Quaternion inv_rot = ~a_T_b.rotation();
    ::bosdyn::api::Vec3 p = inv_rot * a_T_b.position();
    return make_pose(p * -1.0, inv_rot);
}

::bosdyn::api::SE2Pose operator~(const ::bosdyn::api::SE2Pose& a_T_b) {
    ::bosdyn::api::SE2Pose result;
    result.set_angle(-a_T_b.angle());
    double c = cos(a_T_b.angle());
    double s = sin(a_T_b.angle());
    result.mutable_position()->set_x(-c * a_T_b.position().x() - s * a_T_b.position().y());
    result.mutable_position()->set_y(s * a_T_b.position().x() - c * a_T_b.position().y());
    return result;
}

::bosdyn::api::SE3Pose operator*(const ::bosdyn::api::SE3Pose& a_T_b,
                               const ::bosdyn::api::SE3Pose& b_T_c) {
    ::bosdyn::api::Vec3 p = a_T_b.rotation() * b_T_c.position();
    return make_pose(a_T_b.position() + p, a_T_b.rotation() * b_T_c.rotation());
}

bool operator==(const ::bosdyn::api::SE3Pose& a_T_b, const ::bosdyn::api::SE3Pose& b_T_c) {
    return (a_T_b.position() == b_T_c.position() && a_T_b.rotation() == b_T_c.rotation());
}

bool Equals(const ::bosdyn::api::SE3Pose& a_T_b, const ::bosdyn::api::SE3Pose& b_T_c,
            double pos_tol /* = -1.0 */, double rot_tol /* = -1.0 */) {
    if (pos_tol < 0.0) {
        pos_tol = kEqualityTolerance;
    }
    if (rot_tol < 0.0) {
        rot_tol = kEqualityTolerance;
    }

    if (!Equals(a_T_b.position(), b_T_c.position(), pos_tol)) {
        return false;
    }

    // Quaternion equality when (a == b) or (a == -b)
    const ::bosdyn::api::Quaternion& a_R_b = a_T_b.rotation();
    const ::bosdyn::api::Quaternion& b_R_c = b_T_c.rotation();
    bool a_eq_b =
        ((fabs(a_R_b.x() - b_R_c.x()) < rot_tol) && (fabs(a_R_b.y() - b_R_c.y()) < rot_tol) &&
         (fabs(a_R_b.z() - b_R_c.z()) < rot_tol) && (fabs(a_R_b.w() - b_R_c.w()) < rot_tol));
    bool a_eq_negative_b =
        ((fabs(a_R_b.x() + b_R_c.x()) < rot_tol) && (fabs(a_R_b.y() + b_R_c.y()) < rot_tol) &&
         (fabs(a_R_b.z() + b_R_c.z()) < rot_tol) && (fabs(a_R_b.w() + b_R_c.w()) < rot_tol));
    return (a_eq_b || a_eq_negative_b);
}

::bosdyn::api::SE2Pose operator*(const ::bosdyn::api::SE2Pose& a_T_b,
                               const ::bosdyn::api::SE2Pose& b_T_c) {
    ::bosdyn::api::SE2Pose a_T_c;
    double c = cos(a_T_b.angle());
    double s = sin(a_T_b.angle());
    auto pos = a_T_c.mutable_position();
    pos->set_x(a_T_b.position().x() + c * b_T_c.position().x() - s * b_T_c.position().y());
    pos->set_y(a_T_b.position().y() + s * b_T_c.position().x() + c * b_T_c.position().y());
    a_T_c.set_angle(a_T_b.angle() + b_T_c.angle());
    return a_T_c;
}

bool operator==(const ::bosdyn::api::SE2Pose& a_T_b, const ::bosdyn::api::SE2Pose& b_T_c) {
    return ((fabs(a_T_b.angle() - b_T_c.angle()) < kEqualityTolerance) &&
            a_T_b.position() == b_T_c.position());
}

::bosdyn::api::Vec2 operator*(const ::bosdyn::api::SE2Pose& a_T_b, const ::bosdyn::api::Vec2& p) {
    ::bosdyn::api::Vec2 result;
    double c = cos(a_T_b.angle());
    double s = sin(a_T_b.angle());
    result.set_x(a_T_b.position().x() + c * p.x() - s * p.y());
    result.set_y(a_T_b.position().y() + s * p.x() + c * p.y());
    return result;
}

Eigen::Matrix<double, 3, 3> Adjoint(const ::bosdyn::api::SE2Pose& a_T_b) {
    // The adjoint matrix can be used to change reference frames for a SE(2) velocity twist vector.
    // For example, the vel_in_b = Adjoint(b_T_a) * vel_in_a
    Eigen::Matrix<double, 3, 3> a_adjoint_b;
    auto a_R_b = ToMatrix(a_T_b.angle());
    a_adjoint_b.block<2, 2>(0, 0) = a_R_b;
    a_adjoint_b.block<2, 1>(0, 2) = Skew(a_T_b.position());
    a_adjoint_b.block<1, 2>(2, 0) = Eigen::MatrixXd::Zero(1, 2);
    a_adjoint_b(2, 2) = 1.0;
    return a_adjoint_b;
}

Eigen::Matrix<double, 6, 6> Adjoint(const ::bosdyn::api::SE3Pose& a_T_b) {
    // The adjoint matrix can be used to change reference frames for a SE(3) velocity twist vector.
    // For example, the vel_in_b = Adjoint(b_T_a) * vel_in_a
    Eigen::Matrix<double, 6, 6> a_adjoint_b;
    Eigen::Matrix<double, 3, 3> a_R_b;
    ToMatrix(a_T_b.rotation(), &a_R_b);
    a_adjoint_b.block<3, 3>(0, 0) = a_R_b;
    a_adjoint_b.block<3, 3>(0, 3) = Skew(a_T_b.position()) * a_R_b;
    a_adjoint_b.block<3, 3>(3, 3) = a_R_b;
    a_adjoint_b.block<3, 3>(3, 0) = Eigen::MatrixXd::Zero(3, 3);
    return a_adjoint_b;
}

::bosdyn::api::SE2Velocity TransformVelocity(const Eigen::Matrix<double, 3, 3>& a_adjoint_b,
                                            const ::bosdyn::api::SE2Velocity& vel_in_b) {
    Eigen::Matrix<double, 3, 1> velocity_in_b = EigenFromApiProto(vel_in_b);
    Eigen::Matrix<double, 3, 1> velocity_in_a = a_adjoint_b * velocity_in_b;
    return EigenToApiProto(velocity_in_a);
}

::bosdyn::api::SE3Velocity TransformVelocity(const Eigen::Matrix<double, 6, 6>& a_adjoint_b,
                                            const ::bosdyn::api::SE3Velocity& vel_in_b) {
    Eigen::Matrix<double, 6, 1> velocity_in_b = EigenFromApiProto(vel_in_b);
    Eigen::Matrix<double, 6, 1> velocity_in_a = a_adjoint_b * velocity_in_b;
    return EigenToApiProto(velocity_in_a);
}

bool operator==(const ::bosdyn::api::SE2Velocity& vel_a, const ::bosdyn::api::SE2Velocity& vel_b) {
    return ((vel_a.linear() == vel_b.linear()) &&
            (fabs(vel_a.angular() - vel_b.angular()) < kEqualityTolerance));
}

bool operator==(const ::bosdyn::api::SE3Velocity& vel_a, const ::bosdyn::api::SE3Velocity& vel_b) {
    return ((vel_a.linear() == vel_b.linear()) && (vel_a.angular() == vel_b.angular()));
}

Eigen::Matrix<double, 3, 1> EigenFromApiProto(const ::bosdyn::api::SE2Velocity& vel_in_a) {
    Eigen::Matrix<double, 3, 1> eigen_vel_in_a;
    eigen_vel_in_a << vel_in_a.linear().x(), vel_in_a.linear().y(), vel_in_a.angular();
    return eigen_vel_in_a;
}

::bosdyn::api::SE2Velocity EigenToApiProto(const Eigen::Matrix<double, 3, 1>& vel_in_a) {
    ::bosdyn::api::SE2Velocity proto_vel_in_a;
    proto_vel_in_a.mutable_linear()->set_x(vel_in_a(0));
    proto_vel_in_a.mutable_linear()->set_y(vel_in_a(1));
    proto_vel_in_a.set_angular(vel_in_a(2));
    return proto_vel_in_a;
}

Eigen::Matrix<double, 6, 1> EigenFromApiProto(const ::bosdyn::api::SE3Velocity& vel_in_a) {
    Eigen::Matrix<double, 6, 1> eigen_vel_in_a;
    eigen_vel_in_a << vel_in_a.linear().x(), vel_in_a.linear().y(), vel_in_a.linear().z(),
        vel_in_a.angular().x(), vel_in_a.angular().y(), vel_in_a.angular().z();
    return eigen_vel_in_a;
}

::bosdyn::api::SE3Velocity EigenToApiProto(const Eigen::Matrix<double, 6, 1>& vel_in_a) {
    ::bosdyn::api::SE3Velocity proto_vel_in_a;
    proto_vel_in_a.mutable_linear()->set_x(vel_in_a(0));
    proto_vel_in_a.mutable_linear()->set_y(vel_in_a(1));
    proto_vel_in_a.mutable_linear()->set_z(vel_in_a(2));
    proto_vel_in_a.mutable_angular()->set_x(vel_in_a(3));
    proto_vel_in_a.mutable_angular()->set_y(vel_in_a(4));
    proto_vel_in_a.mutable_angular()->set_z(vel_in_a(5));
    return proto_vel_in_a;
}

::bosdyn::api::Vec2 Flatten(const ::bosdyn::api::Vec3& a) {
    ::bosdyn::api::Vec2 result;
    result.set_x(a.x());
    result.set_y(a.y());
    return result;
}


::bosdyn::api::SE2Pose FlattenToX(const SE3Pose& a) {
    ::bosdyn::api::SE2Pose result;
    result.mutable_position()->set_x(a.position().x());
    result.mutable_position()->set_y(a.position().y());
    // This projection is just making sure that the x-axis points the same direction.
    Vec3 x_axis;
    x_axis.set_x(1);
    x_axis = a.rotation() * x_axis;
    result.set_angle(atan2(x_axis.y(), x_axis.x()));
    return result;
}

::bosdyn::api::Vec3 Inflate(const ::bosdyn::api::Vec2& a, double z_height) {
    ::bosdyn::api::Vec3 result;
    result.set_x(a.x());
    result.set_y(a.y());
    result.set_z(z_height);
    return result;
}

::bosdyn::api::SE3Pose Inflate(const ::bosdyn::api::SE2Pose& a, double z_height) {
    ::bosdyn::api::SE3Pose out_se3_pose;
    out_se3_pose.mutable_position()->set_x(a.position().x());
    out_se3_pose.mutable_position()->set_y(a.position().y());
    out_se3_pose.mutable_position()->set_z(z_height);
    out_se3_pose.mutable_rotation()->CopyFrom(FromYaw(a.angle()));
    return out_se3_pose;
}

::bosdyn::api::SE2Pose Flatten(const ::bosdyn::api::SE3Pose& a) {
    ::bosdyn::api::SE2Pose out_se2_pose;
    out_se2_pose.mutable_position()->set_x(a.position().x());
    out_se2_pose.mutable_position()->set_y(a.position().y());
    out_se2_pose.set_angle(ToYaw(a.rotation()));
    return out_se2_pose;
}

bool SafeFlatten(const ::bosdyn::api::SE3Pose& in_se3_pose, const std::string& base_frame_name,
                  ::bosdyn::api::SE2Pose* out_se2_pose) {
    // For a transform a_T_b, the base_frame_name represents "frame A" and must be a frame name
    // for an SE(2) pose. This must be a gravity aligned frame, either "vision", "odom", or
    // "flat_body" to be safely converted from  an SE3Pose to an SE2Pose with minimal loss of
    // height information.
    if (!IsGravityAlignedFrameName(base_frame_name)) {
        return false;
    }
    *out_se2_pose = Flatten(in_se3_pose);
    return true;
}

bool SafeInflate(const ::bosdyn::api::SE2Pose& in_se2_pose, const std::string& base_frame_name,
                  ::bosdyn::api::SE3Pose* out_se3_pose, double z_height) {
    // For a transform a_T_b, the pose_frame_name represents "frame A" and must be a string frame
    // name for an SE(2) pose. This must be a gravity aligned frame, either "vision", "odom", or
    // "flat_body to be safely converted from an SE2Pose to an SE3Pose without creating incorrect
    // height information in the new pose.
    if (!IsGravityAlignedFrameName(base_frame_name)) {
        return false;
    }
    *out_se3_pose = Inflate(in_se2_pose);
    return true;
}

::bosdyn::api::Quaternion FromYaw(double angle) {
    return make_quat(cos(angle / 2.0), 0, 0, sin(angle / 2.0));
}

::bosdyn::api::Quaternion ClosestYawOnly(const ::bosdyn::api::Quaternion& q) {
    double mag = sqrt(q.w() * q.w() + q.z() * q.z());
    if (mag > 0) {
        return make_quat(q.w() / mag, 0, 0, q.z() / mag);
    } else {
        // If the problem is ill-posed, ie z-axis of quaternion is [0, 0, -1], then preserve old
        // behavior and always rotate 180 degrees around the y axis
        return EigenToApiProto(Eigen::Quaterniond(0, 0, 1, 0) * EigenFromApiProto(q));
    }
}

double ToYaw(const ::bosdyn::api::Quaternion& q) {
    // Get the closest yaw only quaternion: [qw, 0, 0, qz]. This can be computed:
    // q = [cos(th/2), n_x * sin(th/2), n_y * sin(th/2), n_z * sin(th/2)]
    // [cos(th/2), 0, 0, sin(th/2)] = [qw, 0, 0, qz]
    ::bosdyn::api::Quaternion yaw_only_quat = ClosestYawOnly(q);
    return 2 * atan2(yaw_only_quat.z(), yaw_only_quat.w());
}

double ToYaw(const ::bosdyn::api::SE3Pose& a_T_b) { return ToYaw(a_T_b.rotation()); }

double ToAngle(const ::bosdyn::api::Quaternion& q) {
    double norm = sqrt(sqr(q.x()) + sqr(q.y()) + sqr(q.z()));
    if (norm != 0.0) {
        double angle = 2.0 * std::atan2(norm, std::abs(q.w()));
        return std::abs(angle);
    } else {
        return 0.0;
    }
}

double ToAngle(const ::bosdyn::api::SE3Pose& a_T_b) { return ToAngle(a_T_b.rotation()); }

::bosdyn::api::SE3Pose Identity() { return make_pose(make_vec(0, 0, 0), make_quat(1, 0, 0, 0)); }
::bosdyn::api::SE2Pose SE2Identity() { return make_pose(make_vec(0, 0), 0.0); }
::bosdyn::api::SE3Covariance IdentityCovMatrix() {
    SE3Covariance out;
    Eigen::MatrixXd ident(6, 6);
    ident.setIdentity();
    EigenToMatrix(ident, out.mutable_matrix());
    SetDeprecatedFields(&out);
    return out;
}

::bosdyn::api::Vec3 CreateVec3(double x, double y, double z) {
    ::bosdyn::api::Vec3 v;
    v.set_x(x);
    v.set_y(y);
    v.set_z(z);
    return v;
}

::bosdyn::api::Quaternion CreateQuaternion(double w, double x, double y, double z) {
    ::bosdyn::api::Quaternion q;
    q.set_w(w);
    q.set_x(x);
    q.set_y(y);
    q.set_z(z);
    return q;
}

::bosdyn::api::Quaternion CreateRotationX(double angle) {
    ::bosdyn::api::Quaternion rotation;
    rotation.set_w(std::cos(angle/2.0));
    rotation.set_x(std::sin(angle/2.0));
    rotation.set_y(0);
    rotation.set_z(0);
    return rotation;
}

::bosdyn::api::Quaternion CreateRotationY(double angle) {
    ::bosdyn::api::Quaternion rotation;
    rotation.set_w(std::cos(angle/2.0));
    rotation.set_x(0);
    rotation.set_y(std::sin(angle/2.0));
    rotation.set_z(0);
    return rotation;
}

::bosdyn::api::Quaternion CreateRotationZ(double angle) {
    ::bosdyn::api::Quaternion rotation;
    rotation.set_w(std::cos(angle/2.0));
    rotation.set_x(0);
    rotation.set_y(0);
    rotation.set_z(std::sin(angle/2.0));
    return rotation;
}

::bosdyn::api::SE3Pose CreateSE3Pose(const ::bosdyn::api::Quaternion& q, const ::bosdyn::api::Vec3& v) {
    ::bosdyn::api::SE3Pose p;
    *p.mutable_rotation() = q;
    *p.mutable_position() = v;
    return p;
}

::bosdyn::api::Vec3 Cross(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b) {
    ::bosdyn::api::Vec3 c;
    c.set_x(a.y() * b.z() - a.z() * b.y());
    c.set_y(-a.x() * b.z() + a.z() * b.x());
    c.set_z(a.x() * b.y() - a.y() * b.x());

    return c;
}

double DotProduct(const ::bosdyn::api::Vec2& a, const ::bosdyn::api::Vec2& b) {
    return a.x() * b.x() + a.y() * b.y();
}

double DotProduct(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b) {
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

double DotProduct(const ::bosdyn::api::Quaternion& a, const ::bosdyn::api::Quaternion& b) {
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z() + a.w() * b.w();
}

double Dist3d(const ::bosdyn::api::SE3Pose& A_T_B, const ::bosdyn::api::SE3Pose& A_T_C) {
    return Length(A_T_B.position() - A_T_C.position());
}

::bosdyn::api::Quaternion operator*(const double& scalar, const ::bosdyn::api::Quaternion& a) {
    ::bosdyn::api::Quaternion out;
    out.set_x(a.x() * scalar);
    out.set_y(a.y() * scalar);
    out.set_z(a.z() * scalar);
    out.set_w(a.w() * scalar);
    return out;
}

::bosdyn::api::Quaternion Normalize(const ::bosdyn::api::Quaternion& q) {
    double len = sqrt(sqr(q.x()) + sqr(q.y()) + sqr(q.z()) + sqr(q.w()));
    return (1.0 / len) * q;
}

bool IsIdentity(const ::bosdyn::api::SE3Pose& pose) {
    if (Length(pose.position()) > kEpsilon) {
        return false;
    }
    return fabs(pose.rotation().w()) - 1.0 < kEpsilon &&  // +/- 1 fine
           fabs(pose.rotation().x()) < kEpsilon &&        // enforce 0
           fabs(pose.rotation().y()) < kEpsilon &&        // enforce 0
           fabs(pose.rotation().z()) < kEpsilon;          // enforce 0
}

void EigenToMatrix(const Eigen::MatrixXd& matrix, ::bosdyn::api::Matrix* matrix_out) {
    matrix_out->Clear();
    matrix_out->set_rows(matrix.rows());
    matrix_out->set_cols(matrix.cols());
    for (int r = 0; r < matrix.rows(); r++) {
        for (int c = 0; c < matrix.cols(); c++) {
            matrix_out->add_values(matrix(r, c));
        }
    }
}

void MatrixToEigen(const ::bosdyn::api::Matrix& matrix, Eigen::MatrixXd* matrix_out) {
    *matrix_out = Eigen::MatrixXd(matrix.rows(), matrix.cols());
    for (int r = 0; r < matrix.rows(); r++) {
        for (int c = 0; c < matrix.cols(); c++) {
            (*matrix_out)(r, c) = matrix.values(r * matrix.cols() + c);
        }
    }
}

void SetDeprecatedFields(::bosdyn::api::SE3Covariance* covariance) {
    Eigen::MatrixXd mat(6, 6);
    MatrixToEigen(covariance->matrix(), &mat);
    covariance->set_cov_xx(mat(3, 3));
    covariance->set_cov_xy(mat(3, 4));
    covariance->set_cov_xz(mat(3, 5));
    covariance->set_cov_yx(mat(4, 3));
    covariance->set_cov_yy(mat(4, 4));
    covariance->set_cov_yz(mat(4, 5));
    covariance->set_cov_zx(mat(5, 3));
    covariance->set_cov_zy(mat(5, 4));
    covariance->set_cov_zz(mat(5, 5));
    covariance->set_yaw_variance(mat(2, 2));
}

bool IntersectRayPlane(const bosdyn::api::Vec3& ray_point, const bosdyn::api::Vec3& ray_norm,
                       const bosdyn::api::Plane& plane, bosdyn::api::Vec3* intersection_point) {
    auto numerator = bosdyn::api::DotProduct(ray_point - plane.point(), plane.normal());
    auto denominator = bosdyn::api::DotProduct(ray_norm, plane.normal());
    if (abs(denominator) < kEqualityTolerance) {
        return false;
    }
    auto t = -1 * numerator / denominator;
    if (t < 0) {
        return false;
    }
    *intersection_point = ray_point + t * ray_norm;
    return true;
}

}  // namespace api

}  // namespace bosdyn
