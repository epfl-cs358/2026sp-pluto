"""
Inverse kinematics solver
"""
import numpy as np

def clamp(x):
    if (abs(x) > 1.0):
        return (1.0 if x > 0 else -1.0)
    else:
        return x

"""
Inverse kinematics for a single leg.

x,y,z              : coordinates of the foot
coxy, femur, tibia : lengths of coxa, femur and tibia

returns : the angles of the coxa, femur and tibia
"""
def IK(x, y, z, coxa, femur, tibia):
    # Angle for the coxa 
    C = np.sqrt(y**2 + z**2)
    D = np.sqrt(C**2 - coxa**2)
    coxa_angle = -np.arctan(y/z) + np.arctan(-coxa / D)
    
    # Angle for the tibia 
    G = np.sqrt(D**2 + x**2)
    n = (G**2 - femur**2 - tibia**2) / (2 * femur)
    tibia_angle = -np.arccos(clamp(n / tibia))

    # Angle for the femur
    femur_angle = -np.arctan(x / D) + np.arccos(clamp((femur + n) / G))
    
    return [coxa_angle, femur_angle, tibia_angle]

"""
Inverse kinematics for four legs.

xvals, yvals, zvals                        : 4d arrays of desired x,y,z values
femur, coxa, tibia                         : lengths of coxa, femur and tibia
hip_offsets                                : array of hip coordinate values, namely [xhipf, xhipb, yhipl] 

returns : the angles of the coxa, femur and tibia of each leg
"""
def robot_IK(xvals, yvals, zvals, coxa, femur, tibia, hip_offsets):
    fl_angles = IK(xvals[0] - hip_offsets[0], yvals[0] - hip_offsets[2], zvals[0], coxa, femur, tibia)
    fr_angles = IK(xvals[1] - hip_offsets[0], yvals[1] + hip_offsets[2], zvals[1], -coxa, femur, tibia)
    bl_angles = IK(xvals[2] - hip_offsets[1], yvals[2] - hip_offsets[2], zvals[2], coxa, femur, tibia)
    br_angles = IK(xvals[3] - hip_offsets[1], yvals[3] + hip_offsets[2], zvals[3], -coxa, femur, tibia)

    return [fl_angles, fr_angles, bl_angles, br_angles]