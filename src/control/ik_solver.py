"""
Inverse kinematics solver
"""
import numpy as np

"""
Inverse kinematics for a single leg.
Refer to the calculation in the proposal

coords             : array of coordinates of the foot [x,y,z]
coxy, femur, tibia : lengths of coxa, femur and tibia

returns : the angles of the coxa, femur and tibia
"""
def IK(coords, coxa, femur, tibia):
    return 0

"""
Inverse kinematics for a four legs.

fr_coords, fl_coords, br_coords, bl_coords : array of coordinates of each foot
femur, coxa, tibia                         : lengths of coxa, femur and tibia

returns : the angles of the coxa, femur and tibia
"""
def leg_IK(fr_coords, fl_coords, br_coords, bl_coords, coxa, femur, tibia):
    return 0