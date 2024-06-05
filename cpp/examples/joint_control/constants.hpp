/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <bosdyn/api/spot/spot_constants.pb.h>
#include <array>

struct SpotKin {
    /// Symbols for the dof of the robot
    enum Dof : int {
        FL_HX = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_FL_HX,
        FL_HY = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_FL_HY,
        FL_KN = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_FL_KN,
        FR_HX = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_FR_HX,
        FR_HY = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_FR_HY,
        FR_KN = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_FR_KN,
        HL_HX = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_HL_HX,
        HL_HY = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_HL_HY,
        HL_KN = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_HL_KN,
        HR_HX = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_HR_HX,
        HR_HY = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_HR_HY,
        HR_KN = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_HR_KN,

        // Arm
        A0_SH0 = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_A0_SH0,
        A0_SH1 = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_A0_SH1,
        A0_EL0 = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_A0_EL0,
        A0_EL1 = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_A0_EL1,
        A0_WR0 = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_A0_WR0,
        A0_WR1 = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_A0_WR1,
        // Hand
        A0_F1X = ::bosdyn::api::spot::JointIndex::JOINT_INDEX_A0_F1X,

        // DOF count for strictly the legs.
        N_DOF_LEGS = 12,
        // DOF count for all DOF on robot (arms and legs).
        N_DOF = 19,
    };

    enum LegIndex : int {
        FL = ::bosdyn::api::spot::LegIndex::LEG_INDEX_FL,
        FR = ::bosdyn::api::spot::LegIndex::LEG_INDEX_FR,
        HL = ::bosdyn::api::spot::LegIndex::LEG_INDEX_HL,
        HR = ::bosdyn::api::spot::LegIndex::LEG_INDEX_HR,

        // The number of legs
        N_LEGS = 4,
    };

    enum LegDofOrder : int {
        HX = ::bosdyn::api::spot::LegDofOrder::HX,
        HY = ::bosdyn::api::spot::LegDofOrder::HY,
        KN = ::bosdyn::api::spot::LegDofOrder::KN,

        // The number of leg dof
        N_LEG_DOF = 3,
    };

    constexpr static std::array<int, N_LEGS> legs = {FL, FR, HL, HR};
    constexpr static std::array<Dof, N_DOF_LEGS> legs_dof = {
        FL_HX, FL_HY, FL_KN, FR_HX, FR_HY, FR_KN, HL_HX, HL_HY, HL_KN, HR_HX, HR_HY, HR_KN};
    struct Gains {
        std::array<float, N_DOF> k_q_p{0};
        std::array<float, N_DOF> k_qd_p{0};

        std::array<float, N_DOF_LEGS> k_q_p_leg{0};
        std::array<float, N_DOF_LEGS> k_qd_p_leg{0};

        void set_default_position_control_gains() {
            // These gains are reasonable gains for position control in kinematics space. k_q_p is
            // the proportional gain for position control whereas k_qd_p is the proportional gain
            // for velocity conrol.
            constexpr static float hx_k_q_p = 624;
            constexpr static float hx_k_qd_p = 5.20;
            constexpr static float hy_k_q_p = 936;
            constexpr static float hy_k_qd_p = 5.20;
            constexpr static float kn_k_q_p = 286;
            constexpr static float kn_k_qd_p = 2.04;

            auto get_leg_dof = [](int leg, int dof) { return legs_dof[leg * N_LEG_DOF + dof]; };

            // Set leg gains
            for (auto leg : legs) {
                // Set HX gains
                k_q_p_leg[get_leg_dof(leg, HX)] = hx_k_q_p;
                k_qd_p_leg[get_leg_dof(leg, HX)] = hx_k_qd_p;

                // Set HY gains
                k_q_p_leg[get_leg_dof(leg, HY)] = hy_k_q_p;
                k_qd_p_leg[get_leg_dof(leg, HY)] = hy_k_qd_p;

                // Set KN gains
                k_q_p_leg[get_leg_dof(leg, KN)] = kn_k_q_p;
                k_qd_p_leg[get_leg_dof(leg, KN)] = kn_k_qd_p;
            }

            std::copy(k_q_p_leg.begin(), k_q_p_leg.end(), k_q_p.begin());
            std::copy(k_qd_p_leg.begin(), k_qd_p_leg.end(), k_qd_p.begin());

            // Set arm gains
            k_q_p[A0_SH0] = 1020;
            k_qd_p[A0_SH0] = 10.2;
            k_q_p[A0_SH1] = 255;
            k_qd_p[A0_SH1] = 15.3;
            k_q_p[A0_EL0] = 204;
            k_qd_p[A0_EL0] = 10.2;
            k_q_p[A0_EL1] = 102;
            k_qd_p[A0_EL1] = 2.04;
            k_q_p[A0_WR0] = 102;
            k_qd_p[A0_WR0] = 2.04;
            k_q_p[A0_WR1] = 102;
            k_qd_p[A0_WR1] = 2.04;
            k_q_p[A0_F1X] = 16.0;
            k_qd_p[A0_F1X] = 0.32;
        }
    } gains;

    SpotKin() { gains.set_default_position_control_gains(); }
} spot_kin;